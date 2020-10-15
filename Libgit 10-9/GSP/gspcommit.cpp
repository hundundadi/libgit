#include "gspcommit.h"

GspCommit::GspCommit()
{

}

GspCommit::~GspCommit()
{

}

void GspCommit::CommitFileToRemote(QString prjName, QString username, QString password, QString commitEmail, QString commitContent, QStringList fileList)
{
    g_userName = username;
    g_password = password;
    m_userInfo = username;
    m_userEmail = commitEmail;
    m_prjName = prjName;
    m_commitLog = commitContent;

    RW_ConfigFile::getInstence()->reReadRepositoryPath();
    QString tmpLocal = RW_ConfigFile::getInstence()->getRepositoryPath() + prjName;
    QByteArray ba_repPath = tmpLocal.toUtf8();
    git_repository* rep = nullptr;
    ExecuteResult stResult; stResult.content = "提交成功！"; stResult.statusCode = "0";
    int error = 0;
    char* addArgv[2] = {nullptr};
    QByteArray ba_Content = commitContent.toUtf8();
    char* commitArgv[3] = {"commit", "-m", ba_Content.data()};
    git_libgit2_init();
    error = git_repository_open(&rep, ba_repPath.constData());
    if (error < 0) {
        goto DOWN;
    }
    // add xxx
    addArgv[0] = "add";
    for (int index = 0; index < fileList.size(); index++) {
        QString tmpFile = fileList.at(index);
        QByteArray ba_file = tmpFile.toUtf8();
        addArgv[1] = ba_file.data();
        error = lg2_add(rep, 2, addArgv);
        if (error < 0) {
            goto DOWN;
        }
    }
    // commit -m "content"
    error = lg2_commit(rep, 3, commitArgv);
    if (error < 0) {
        goto DOWN;
    }
    // push origin master
    error = lg2_push(rep, 0, nullptr, fileList);
    if (error) {
        emit signal_Error(git2_ExecuteError(error));
    }
DOWN:
    if (rep) {
        git_repository_free(rep);
        git_libgit2_shutdown();
        if (error >= 0)
            emit signal_CommitResult(stResult);
    }
}

enum index_mode {
    INDEX_NONE,
    INDEX_ADD,
};

struct index_options {
    int dry_run;
    int verbose;
    git_repository *repo;
    enum index_mode mode;
    int add_update;
};

/* Forward declarations for helpers */
static void parse_opts(const char **repo_path, struct index_options *opts, struct args_info *args);
int print_matched_cb(const char *path, const char *matched_pathspec, void *payload);

int GspCommit::lg2_add(git_repository *repo, int argc, char **argv)
{
    git_index_matched_path_cb matched_cb = NULL;
    git_index *index;
    git_strarray array = {0};
    struct index_options options = {0};
    struct args_info args = ARGS_INFO_INIT;

    options.mode = INDEX_ADD;

    /* Parse the options & arguments. */
    parse_opts(NULL, &options, &args);
    strarray_from_args(&array, &args);

    /* Grab the repository's index. */
    check_lg2(git_repository_index(&index, repo), "Could not open repository index", NULL);

    /* Setup a callback if the requested options need it */
    if (options.verbose || options.dry_run) {
        matched_cb = &print_matched_cb;
    }

    options.repo = repo;

    /* Perform the requested action with the index and files */
    if (options.add_update) {
        git_index_update_all(index, &array, matched_cb, &options);
    } else {
        git_index_add_all(index, &array, 0, matched_cb, &options);
    }

    /* Cleanup memory */
    git_index_write(index);
    git_index_free(index);

    return 0;
}

/*
 * This callback is called for each file under consideration by
 * git_index_(update|add)_all above.
 * It makes uses of the callback's ability to abort the action.
 */
int print_matched_cb(const char *path, const char *matched_pathspec, void *payload)
{
    struct index_options *opts = (struct index_options *)(payload);
    int ret;
    unsigned status;
    (void)matched_pathspec;

    /* Get the file status */
    if (git_status_file(&status, opts->repo, path) < 0)
        return -1;

    if ((status & GIT_STATUS_WT_MODIFIED) || (status & GIT_STATUS_WT_NEW)) {
        printf("add '%s'\n", path);
        ret = 0;
    } else {
        ret = 1;
    }

    if (opts->dry_run)
        ret = 1;

    return ret;
}

void init_array(git_strarray *array, int argc, char **argv)
{
    unsigned int i;

    array->count = argc;
    array->strings = (char**)calloc(array->count, sizeof(char *));
    assert(array->strings != NULL);

    for (i = 0; i < array->count; i++) {
        array->strings[i] = argv[i];
    }

    return;
}

void print_usage(void)
{
    fprintf(stderr, "usage: add [options] [--] file-spec [file-spec] [...]\n\n");
    fprintf(stderr, "\t-n, --dry-run    dry run\n");
    fprintf(stderr, "\t-v, --verbose    be verbose\n");
    fprintf(stderr, "\t-u, --update     update tracked files\n");
    exit(1);
}

static void parse_opts(const char **repo_path, struct index_options *opts, struct args_info *args)
{
    if (args->argc <= 1)
        print_usage();

    for (args->pos = 1; args->pos < args->argc; ++args->pos) {
        const char *curr = args->argv[args->pos];

        if (curr[0] != '-') {
            if (!strcmp("add", curr)) {
                opts->mode = INDEX_ADD;
                continue;
            } else if (opts->mode == INDEX_NONE) {
                fprintf(stderr, "missing command: %s", curr);
                print_usage();
                break;
            } else {
                /* We might be looking at a filename */
                break;
            }
        } else if (match_bool_arg(&opts->verbose, args, "--verbose") ||
                   match_bool_arg(&opts->dry_run, args, "--dry-run") ||
                   match_str_arg(repo_path, args, "--git-dir") ||
                   (opts->mode == INDEX_ADD && match_bool_arg(&opts->add_update, args, "--update"))) {
            continue;
        } else if (match_bool_arg(NULL, args, "--help")) {
            print_usage();
            break;
        } else if (match_arg_separator(args)) {
            break;
        } else {
            fprintf(stderr, "Unsupported option %s.\n", curr);
            print_usage();
        }
    }
}

int GspCommit::lg2_commit(git_repository *repo, int argc, char **argv)
{
    const char *opt = argv[1];
    const char *comment = argv[2];
    int error;

    git_oid tree_oid;
    git_tree *tree;
    git_index *index;
    git_object *parent = NULL;
    git_reference *ref = NULL;
    git_signature *signature;

    /* Validate args */
    if (argc < 3 || strcmp(opt, "-m") != 0) {
        printf ("USAGE: %s -m <comment>\n", argv[0]);
        return -1;
    }

    error = git_revparse_ext(&parent, &ref, repo, "HEAD");
    if (error == GIT_ENOTFOUND) {
        printf("HEAD not found. Creating first commit\n");
        error = 0;
    } else if (error != 0) {
        const git_error *err = git_error_last();
        if (err) printf("ERROR %d: %s\n", err->klass, err->message);
        else printf("ERROR %d: no detailed info\n", error);
    }

    check_lg2(git_repository_index(&index, repo), "Could not open repository index", NULL);
    check_lg2(git_index_write_tree(&tree_oid, index), "Could not write tree", NULL);;
    check_lg2(git_index_write(index), "Could not write index", NULL);;

    check_lg2(git_tree_lookup(&tree, repo, &tree_oid), "Error looking up tree", NULL);

    QByteArray ba_userInfo = m_userInfo.toUtf8();
    QByteArray ba_userEmail = m_userEmail.toUtf8();
    check_lg2(git_signature_now(&signature, ba_userInfo.constData(), ba_userEmail.constData()), "Error creating signature", NULL);

    check_lg2(git_commit_create_v(
        &m_commit_oid,
        repo,
        "HEAD",
        signature,
        signature,
        NULL,
        comment,
        tree,
        parent ? 1 : 0, parent), "Error creating commit", NULL);

    git_index_free(index);
    git_signature_free(signature);
    git_tree_free(tree);

    return error;
}

int GspCommit::lg2_push(git_repository *repo, int argc, char **argv, QStringList fileList)
{
    int error = 0;
    git_push_options options = GIT_PUSH_OPTIONS_INIT;
    git_remote* remote = NULL;
    char *refspec = "refs/heads/master";
    const git_strarray refspecs = {
        &refspec,
        1
    };

    /* Validate args */
    if (argc > 1) {
        printf ("USAGE: %s\n\nsorry, no arguments supported yet\n", argv[0]);
        return -1;
    }

    check_lg2(git_remote_lookup(&remote, repo, "origin" ), "Unable to lookup remote", NULL);

    //check_lg2(git_push_options_init(&options, GIT_PUSH_OPTIONS_VERSION ), "Error initializing push", NULL);

    options.callbacks.credentials = cred_acquire_cb;
    error = git_remote_push(remote, &refspecs, &options);
    /* 本地仓库不是最新的pull，需要先拉取；若有冲突则解决后；再提交  */
    if (error == -11){
        qDebug() << "本地仓库不是最新的pull，需要先拉取；若有冲突则解决后；再提交";
        //git_checkout_options gitCheckoutOpt = GIT_CHECKOUT_OPTIONS_INIT;
        // 删除已commit的未push的空提交
        //git_reset(repo, (git_object*)&m_commit_oid, GIT_RESET_MIXED, &gitCheckoutOpt);
        //GitApiHandle::getInstence()->m_scommitLog = m_commitLog;
        //if(!GitApiHandle::getInstence()->gitPull(m_prjName, g_userName, g_password, m_userEmail, fileList)) {
        //    qDebug() << "冲突";// 冲突
        //}
        //else {
        //    qDebug() << "合并后的提交。";
        //    error = git_remote_push(remote, &refspecs, &options); // 合并后无冲突再次提交
        //}
    }
    return error;
}
