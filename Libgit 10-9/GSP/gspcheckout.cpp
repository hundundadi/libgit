#include "gspcheckout.h"

GspCheckout::GspCheckout()
{

}

GspCheckout::~GspCheckout()
{

}

void GspCheckout::CheckoutVersion(QString userName, QString password, QString prjName, QString checkoutVersion, QStringList fileList)
{
    g_userName = userName;
    g_password = password;

    RW_ConfigFile::getInstence()->reReadRepositoryPath();
    QString tmpLocal = RW_ConfigFile::getInstence()->getRepositoryPath() + prjName;
    QByteArray ba_path = tmpLocal.toUtf8();
    git_repository* rep = nullptr;
    ExecuteResult stResult; stResult.content = "回退成功！"; stResult.statusCode = "0";
    int error = 0;
    git_libgit2_init();
    error = git_repository_open(&rep, ba_path.constData());
    if (error < 0) {
        emit signal_Error(git2_ExecuteError(error));
        qDebug() << "本地存储库打开失败";
    }
    QByteArray ba_version = checkoutVersion.toUtf8();
    if (fileList.isEmpty()) {
        qDebug() << "整个版本回退。";
        char* argc[2] = {"checkout", ba_version.data()};
        error = lg2_checkout(rep, 2, argc);
    }
    else {
        qDebug() << "回退版本中的某些文件";
        char* argc[3] = {nullptr};
        argc[0] = "checkout";
        argc[1] = ba_version.data();
        for(int index = 0; index < fileList.size(); index++) {
            QString str_file = fileList.at(index);
            QByteArray ba_file = str_file.toUtf8();
            argc[2] = ba_file.data();
            error = lg2_checkout(rep, 3, argc);
            if (error < 0)
                break;
        }
    }
    if (error < 0) {
        emit signal_Error(git2_ExecuteError(error));
        qDebug() << "版本回退失败。";
    }
    else if (rep) {
        emit signal_CheckoutResult(stResult);
        qDebug() << "版本回退成功。";
    }
    git_repository_free(rep);
    git_libgit2_shutdown();
}

/* Define the printf format specifer to use for size_t output */
#if defined(_MSC_VER) || defined(__MINGW32__)
#	define PRIuZ "Iu"
#	define PRIxZ "Ix"
#	define PRIdZ "Id"
#else
#	define PRIuZ "zu"
#	define PRIxZ "zx"
#	define PRIdZ "zd"
#endif

/**
 * The following example demonstrates how to do checkouts with libgit2.
 *
 * Recognized options are :
 *  --force: force the checkout to happen.
 *  --[no-]progress: show checkout progress, on by default.
 *  --perf: show performance data.
 */

typedef struct {
    int force : 1;
    int progress : 1;
    int perf : 1;
} checkout_options;

static void print_usage(void)
{
    fprintf(stderr, "usage: checkout [options] <branch>\n"
        "Options are :\n"
        "  --git-dir: use the following git repository.\n"
        "  --force: force the checkout.\n"
        "  --[no-]progress: show checkout progress.\n"
        "  --perf: show performance data.\n");
    exit(1);
}

static void parse_options(const char **repo_path, checkout_options *opts, struct args_info *args)
{
    if (args->argc <= 1)
        print_usage();

    memset(opts, 0, sizeof(*opts));

    /* Default values */
    opts->progress = 1;

    for (args->pos = 1; args->pos < args->argc; ++args->pos) {
        const char *curr = args->argv[args->pos];
        int bool_arg;

        if (match_arg_separator(args)) {
            break;
        } else if (!strcmp(curr, "--force")) {
            opts->force = 1;
        } else if (match_bool_arg(&bool_arg, args, "--progress")) {
            opts->progress = bool_arg;
        } else if (match_bool_arg(&bool_arg, args, "--perf")) {
            opts->perf = bool_arg;
        } else if (match_str_arg(repo_path, args, "--git-dir")) {
            continue;
        } else {
            break;
        }
    }
}

/**
 * This function is called to report progression, ie. it's called once with
 * a NULL path and the number of total steps, then for each subsequent path,
 * the current completed_step value.
 */
static void print_checkout_progress(const char *path, size_t completed_steps, size_t total_steps, void *payload)
{
    (void)payload;
    if (path == NULL) {
        printf("checkout started: %" PRIuZ " steps\n", total_steps);
    } else {
        printf("checkout: %s %" PRIuZ "/%" PRIuZ "\n", path, completed_steps, total_steps);
    }
}

/**
 * This function is called when the checkout completes, and is used to report the
 * number of syscalls performed.
 */
static void print_perf_data(const git_checkout_perfdata *perfdata, void *payload)
{
    (void)payload;
    printf("perf: stat: %" PRIuZ " mkdir: %" PRIuZ " chmod: %" PRIuZ "\n",
           perfdata->stat_calls, perfdata->mkdir_calls, perfdata->chmod_calls);
}

/**
 * This is the main "checkout <branch>" function, responsible for performing
 * a branch-based checkout.
 */
static int perform_checkout_ref(git_repository *repo, git_annotated_commit *target, const char *target_ref, checkout_options *opts, char **path)
{
    git_checkout_options checkout_opts = GIT_CHECKOUT_OPTIONS_INIT;
    git_reference *ref = NULL, *branch = NULL;
    git_commit *target_commit = NULL;
    int err;

    /** Setup our checkout options from the parsed options */
    checkout_opts.checkout_strategy = GIT_CHECKOUT_SAFE;
    if (opts->force)
        checkout_opts.checkout_strategy = GIT_CHECKOUT_FORCE;

    if (opts->progress)
        checkout_opts.progress_cb = print_checkout_progress;

    if (opts->perf)
        checkout_opts.perfdata_cb = print_perf_data;
    checkout_opts.paths.strings = path;
    checkout_opts.paths.count = 1;
    checkout_opts.checkout_strategy |= GIT_CHECKOUT_FORCE;
    checkout_opts.checkout_strategy |= GIT_CHECKOUT_DONT_WRITE_INDEX;
    /** Grab the commit we're interested to move to */
    err = git_commit_lookup(&target_commit, repo, git_annotated_commit_id(target));
    if (err != 0) {
        fprintf(stderr, "failed to lookup commit: %s\n", git_error_last()->message);
        goto cleanup;
    }

    /**
     * Perform the checkout so the workdir corresponds to what target_commit
     * contains.
     *
     * Note that it's okay to pass a git_commit here, because it will be
     * peeled to a tree.
     */
    err = git_checkout_tree(repo, (const git_object *)target_commit, &checkout_opts);
    if (err != 0) {
        fprintf(stderr, "failed to checkout tree: %s\n", git_error_last()->message);
        goto cleanup;
    }

    /**
     * Now that the checkout has completed, we have to update HEAD.
     *
     * Depending on the "origin" of target (ie. it's an OID or a branch name),
     * we might need to detach HEAD.
     */ // 不改变head得指向，依旧指向master
    //if (git_annotated_commit_ref(target)) {
    //    const char *target_head;
    //
    //    if ((err = git_reference_lookup(&ref, repo, git_annotated_commit_ref(target))) < 0)
    //        goto error;
    //
    //    if (git_reference_is_remote(ref)) {
    //        if ((err = git_branch_create_from_annotated(&branch, repo, target_ref, target, 0)) < 0)
    //            goto error;
    //        target_head = git_reference_name(branch);
    //    } else {
    //        target_head = git_annotated_commit_ref(target);
    //    }
    //
    //    err = git_repository_set_head(repo, target_head);
    //} else {
    //    err = git_repository_set_head_detached_from_annotated(repo, target);
    //}

error:
    if (err != 0) {
        fprintf(stderr, "failed to update HEAD reference: %s\n", git_error_last()->message);
        goto cleanup;
    }

cleanup:
    git_commit_free(target_commit);
    git_reference_free(branch);
    git_reference_free(ref);

    return err;
}

/**
 * This corresponds to `git switch --guess`: if a given ref does
 * not exist, git will by default try to guess the reference by
 * seeing whether any remote has a branch called <ref>. If there
 * is a single remote only that has it, then it is assumed to be
 * the desired reference and a local branch is created for it.
 *
 * The following is a simplified implementation. It will not try
 * to check whether the ref is unique across all remotes.
 */
static int guess_refish(git_annotated_commit **out, git_repository *repo, const char *ref)
{
    git_strarray remotes = { NULL, 0 };
    git_reference *remote_ref = NULL;
    int error;
    size_t i;

    if ((error = git_remote_list(&remotes, repo)) < 0)
        goto out;

    for (i = 0; i < remotes.count; i++) {
        char *refname = NULL;
        size_t reflen;

        reflen = snprintf(refname, 0, "refs/remotes/%s/%s", remotes.strings[i], ref);
        if ((refname = (char*)malloc(reflen + 1)) == NULL) {
            error = -1;
            goto next;
        }
        snprintf(refname, reflen + 1, "refs/remotes/%s/%s", remotes.strings[i], ref);

        if ((error = git_reference_lookup(&remote_ref, repo, refname)) < 0)
            goto next;

        break;
next:
        free(refname);
        if (error < 0 && error != GIT_ENOTFOUND)
            break;
    }

    if (!remote_ref) {
        error = GIT_ENOTFOUND;
        goto out;
    }

    if ((error = git_annotated_commit_from_ref(out, repo, remote_ref)) < 0)
        goto out;

out:
    git_reference_free(remote_ref);
    git_strarray_dispose(&remotes);
    return error;
}

/** That example's entry point */
int GspCheckout::lg2_checkout(git_repository *repo, int argc, char **argv)
{
    struct args_info args = ARGS_INFO_INIT;
    checkout_options opts;
    int state;
    git_annotated_commit *checkout_target = NULL;
    int err = 0;
    const char *path = ".";

    /** Parse our command line options */
    parse_options(&path, &opts, &args);

    /** Make sure we're not about to checkout while something else is going on */
    state = git_repository_state(repo);
    if (state != GIT_REPOSITORY_STATE_NONE) {
        fprintf(stderr, "repository is in unexpected state %d\n", state);
        goto cleanup;
    }

    if (match_arg_separator(&args)) {
        /**
         * Try to checkout the given path
         */

        fprintf(stderr, "unhandled path-based checkout\n");
        err = 1;
        goto cleanup;
    } else {
        /**
         * Try to resolve a "refish" argument to a target libgit2 can use
         */
        if ((err = resolve_refish(&checkout_target, repo, args.argv[args.pos])) < 0 &&
            (err = guess_refish(&checkout_target, repo, args.argv[args.pos])) < 0) {
            fprintf(stderr, "failed to resolve %s: %s\n", args.argv[args.pos], git_error_last()->message);
            goto cleanup;
        }
        err = perform_checkout_ref(repo, checkout_target, args.argv[args.pos], &opts, &argv[2]);
    }

cleanup:
    git_annotated_commit_free(checkout_target);

    return err;
}
