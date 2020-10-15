#include "gsppull.h"

GspPull::GspPull()
{

}

GspPull::~GspPull()
{

}

void GspPull::Pull(QString prjName,
                   QString username,
                   QString password,
                   QString commitEmail,
                   QString remoteUrl, bool isRequest)
{
    g_userName = username;
    g_password = password;
    m_commitEmail = commitEmail;
    RW_ConfigFile::getInstence()->reReadRepositoryPath();
    QString tmpLocal = RW_ConfigFile::getInstence()->getRepositoryPath() + prjName;
    QByteArray ba_repPath = tmpLocal.toUtf8();
    m_stResult.content = "拉取成功！"; m_stResult.statusCode = "0";
    QByteArray ba_fetchUrl = remoteUrl.toUtf8();

    int error = 0;
    git_repository* rep = nullptr;
    char* pullArgv[3] = { "merge", "master" };//, "--no-commit"
    char* fetchArgv[2] = { "fetch", "origin" };//ba_fetchUrl.data()
    git_libgit2_init();
    error = git_repository_open(&rep, ba_repPath.data());
    if (error < 0) {
        emit signal_Error(git2_ExecuteError(error));
        goto SHUTDOWN;
    }
    // get a remote
    error = lg2_fetch(rep, 2, fetchArgv);
    if (error < 0) {
        emit signal_Error(git2_ExecuteError(error));
        goto SHUTDOWN;
    }
    // merger
    error = lg2_merge(rep, 2, pullArgv);
    if (error < 0) {
        emit signal_Error(git2_ExecuteError(error));
    }
SHUTDOWN:
    if(rep) {
        git_repository_free(rep);
        git_libgit2_shutdown();
        if (isRequest && error >= 0)
            emit signal_PullResult(m_stResult);
    }
}

struct merge_options {
    const char **heads;
    size_t heads_count;

    git_annotated_commit **annotated;
    size_t annotated_count;

    int no_commit : 1;
};

static void print_usage(void)
{
    fprintf(stderr, "usage: merge [--no-commit] <commit...>\n");
    exit(1);
}

static void merge_options_init(struct merge_options *opts)
{
    memset(opts, 0, sizeof(*opts));

    opts->heads = NULL;
    opts->heads_count = 0;
    opts->annotated = NULL;
    opts->annotated_count = 0;
}

static void opts_add_refish(struct merge_options *opts, const char *refish)
{
    size_t sz;

    assert(opts != NULL);

    sz = ++opts->heads_count * sizeof(opts->heads[0]);
    opts->heads = (const char**)xrealloc((void *) opts->heads, sz);
    opts->heads[opts->heads_count - 1] = refish;
}

static void parse_options(const char **repo_path, struct merge_options *opts, int argc, char **argv)
{
    struct args_info args = ARGS_INFO_INIT;

    if (argc <= 1)
        print_usage();

    for (args.pos = 1; args.pos < argc; ++args.pos) {
        const char *curr = argv[args.pos];

        if (curr[0] != '-') {
            opts_add_refish(opts, curr);
        } else if (!strcmp(curr, "--no-commit")) {
            opts->no_commit = 1;
        } else if (match_str_arg(repo_path, &args, "--git-dir")) {
            continue;
        } else {
            print_usage();
        }
    }
}

static int resolve_heads(git_repository *repo, struct merge_options *opts)
{
    git_annotated_commit **annotated = (git_annotated_commit **)calloc(opts->heads_count, sizeof(git_annotated_commit *));
    size_t annotated_count = 0, i;
    int err = 0;

    for (i = 0; i < opts->heads_count; i++) {
        err = resolve_refish(&annotated[annotated_count++], repo, opts->heads[i]);
        if (err != 0) {
            fprintf(stderr, "failed to resolve refish %s: %s\n", opts->heads[i], git_error_last()->message);
            annotated_count--;
            continue;
        }
    }

    if (annotated_count != opts->heads_count) {
        fprintf(stderr, "unable to parse some refish\n");
        free(annotated);
        return -1;
    }

    opts->annotated = annotated;
    opts->annotated_count = annotated_count;
    return 0;
}

static int perform_fastforward(git_repository *repo, const git_oid *target_oid, int is_unborn)
{
    git_checkout_options ff_checkout_options = GIT_CHECKOUT_OPTIONS_INIT;
    git_reference *target_ref;
    git_reference *new_target_ref;
    git_object *target = NULL;
    int err = 0;

    if (is_unborn) {
        const char *symbolic_ref;
        git_reference *head_ref;

        /* HEAD reference is unborn, lookup manually so we don't try to resolve it */
        err = git_reference_lookup(&head_ref, repo, "HEAD");
        if (err != 0) {
            fprintf(stderr, "failed to lookup HEAD ref\n");
            return -1;
        }

        /* Grab the reference HEAD should be pointing to */
        symbolic_ref = git_reference_symbolic_target(head_ref);

        /* Create our master reference on the target OID */
        err = git_reference_create(&target_ref, repo, symbolic_ref, target_oid, 0, NULL);
        if (err != 0) {
            fprintf(stderr, "failed to create master reference\n");
            return -1;
        }

        git_reference_free(head_ref);
    } else {
        /* HEAD exists, just lookup and resolve */
        err = git_repository_head(&target_ref, repo);
        if (err != 0) {
            fprintf(stderr, "failed to get HEAD reference\n");
            return -1;
        }
    }

    /* Lookup the target object */
    err = git_object_lookup(&target, repo, target_oid, GIT_OBJECT_COMMIT);
    if (err != 0) {
        fprintf(stderr, "failed to lookup OID %s\n", git_oid_tostr_s(target_oid));
        return -1;
    }

    /* Checkout the result so the workdir is in the expected state */
    ff_checkout_options.checkout_strategy = GIT_CHECKOUT_SAFE;
    err = git_checkout_tree(repo, target, &ff_checkout_options);
    if (err != 0) {
        fprintf(stderr, "failed to checkout HEAD reference\n");
        return -1;
    }

    /* Move the target reference to the target OID */
    err = git_reference_set_target(&new_target_ref, target_ref, target_oid, NULL);
    if (err != 0) {
        fprintf(stderr, "failed to move HEAD reference\n");
        return -1;
    }

    git_reference_free(target_ref);
    git_reference_free(new_target_ref);
    git_object_free(target);

    return 0;
}

void GspPull::output_conflicts(git_index *index)
{
    git_index_conflict_iterator *conflicts;
    const git_index_entry *ancestor;
    const git_index_entry *our;
    const git_index_entry *their;
    int err = 0;

    check_lg2(git_index_conflict_iterator_new(&conflicts, index), "failed to create conflict iterator", NULL);

    while ((err = git_index_conflict_next(&ancestor, &our, &their, conflicts)) == 0) {
        if (ancestor->path){
            m_stResult.conflictFileList.append(QString(ancestor->path));
        }
        else if (our->path) {
            m_stResult.conflictFileList.append(QString(our->path));
        }
        else if (their->path) {
            m_stResult.conflictFileList.append(QString(their->path));
        }
    }

    if (err != GIT_ITEROVER) {
        fprintf(stderr, "error iterating conflicts\n");
    }

    git_index_conflict_iterator_free(conflicts);
}

int GspPull::create_merge_commit(git_repository *repo, git_index *index, struct merge_options *opts)
{
    git_oid tree_oid, commit_oid;
    git_tree *tree;
    git_signature *sign;
    git_reference *merge_ref = NULL;
    git_annotated_commit *merge_commit;
    git_reference *head_ref;
    git_commit **parents = (git_commit **)calloc(opts->annotated_count + 1, sizeof(git_commit *));
    const char *msg_target = NULL;
    size_t msglen = 0;
    char *msg;
    size_t i;
    int err;

    /* Grab our needed references */
    check_lg2(git_repository_head(&head_ref, repo), "failed to get repo HEAD", NULL);
    if (resolve_refish(&merge_commit, repo, opts->heads[0])) {
        fprintf(stderr, "failed to resolve refish %s", opts->heads[0]);
        free(parents);
        return -1;
    }

    /* Maybe that's a ref, so DWIM it */
    err = git_reference_dwim(&merge_ref, repo, opts->heads[0]);
    check_lg2(err, "failed to DWIM reference", git_error_last()->message);

    QByteArray ba_user = g_userName.toUtf8();
    QByteArray ba_email = m_commitEmail.toUtf8();
    /* Grab a signature */
    check_lg2(git_signature_now(&sign, ba_user.constData(), ba_email.constData()), "failed to create signature", NULL);

#define MERGE_COMMIT_MSG "Merge %s '%s'"
    /* Prepare a standard merge commit message */
    if (merge_ref != NULL) {
        check_lg2(git_branch_name(&msg_target, merge_ref), "failed to get branch name of merged ref", NULL);
    } else {
        msg_target = git_oid_tostr_s(git_annotated_commit_id(merge_commit));
    }

    msglen = snprintf(NULL, 0, MERGE_COMMIT_MSG, (merge_ref ? "branch" : "commit"), msg_target);
    if (msglen > 0) msglen++;
    msg = (char*)malloc(msglen);
    err = snprintf(msg, msglen, MERGE_COMMIT_MSG, (merge_ref ? "branch" : "commit"), msg_target);

    /* This is only to silence the compiler */
    if (err < 0) goto cleanup;

    /* Setup our parent commits */
    err = git_reference_peel((git_object **)&parents[0], head_ref, GIT_OBJECT_COMMIT);
    check_lg2(err, "failed to peel head reference", NULL);
    for (i = 0; i < opts->annotated_count; i++) {
        git_commit_lookup(&parents[i + 1], repo, git_annotated_commit_id(opts->annotated[i]));
    }

    /* Prepare our commit tree */
    check_lg2(git_index_write_tree(&tree_oid, index), "failed to write merged tree", NULL);
    check_lg2(git_tree_lookup(&tree, repo, &tree_oid), "failed to lookup tree", NULL);

    /* Commit time ! */
    err = git_commit_create(&commit_oid,
                            repo, git_reference_name(head_ref),
                            sign, sign,
                            NULL, msg,
                            tree,
                            opts->annotated_count + 1, (const git_commit **)parents);
    check_lg2(err, "failed to create commit", NULL);

    /* We're done merging, cleanup the repository state */
    git_repository_state_cleanup(repo);

cleanup:
    free(parents);
    return err;
}

int GspPull::lg2_merge(git_repository *repo, int argc, char **argv)
{
    struct merge_options opts;
    git_index *index;
    int state;//git_repository_state_t
    git_merge_analysis_t analysis;
    git_merge_preference_t preference;
    const char *path = ".";
    int err = 0;

    merge_options_init(&opts);
    parse_options(&path, &opts, argc, argv);

    state = git_repository_state(repo);
    if (state != GIT_REPOSITORY_STATE_NONE) {
        fprintf(stderr, "repository is in unexpected state %d\n", state);
        goto cleanup;
    }

    err = resolve_heads(repo, &opts);
    if (err != 0)
        goto cleanup;

    err = git_merge_analysis(&analysis, &preference,
                             repo,
                             (const git_annotated_commit **)opts.annotated,
                             opts.annotated_count);
    check_lg2(err, "merge analysis failed", NULL);

    if (analysis & GIT_MERGE_ANALYSIS_UP_TO_DATE) {
        printf("Already up-to-date\n");
        return 0;
    }
    else if (analysis & GIT_MERGE_ANALYSIS_UNBORN ||
              (analysis & GIT_MERGE_ANALYSIS_FASTFORWARD &&
              !(preference & GIT_MERGE_PREFERENCE_NO_FASTFORWARD))) {
        const git_oid *target_oid;
        if (analysis & GIT_MERGE_ANALYSIS_UNBORN) {
            printf("Unborn\n");
        } else {
            printf("Fast-forward\n");
        }

        /* Since this is a fast-forward, there can be only one merge head */
        target_oid = git_annotated_commit_id(opts.annotated[0]);
        assert(opts.annotated_count == 1);

        return perform_fastforward(repo, target_oid, (analysis & GIT_MERGE_ANALYSIS_UNBORN));
    }
    else if (analysis & GIT_MERGE_ANALYSIS_NORMAL) {
        git_merge_options merge_opts = GIT_MERGE_OPTIONS_INIT;
        git_checkout_options checkout_opts = GIT_CHECKOUT_OPTIONS_INIT;

        merge_opts.flags = 0;
        merge_opts.file_flags = GIT_MERGE_FILE_STYLE_DIFF3;

        checkout_opts.checkout_strategy = GIT_CHECKOUT_FORCE|GIT_CHECKOUT_ALLOW_CONFLICTS;

        if (preference & GIT_MERGE_PREFERENCE_FASTFORWARD_ONLY) {
            printf("Fast-forward is preferred, but only a merge is possible\n");
            return -1;
        }

        err = git_merge(repo,
                        (const git_annotated_commit **)opts.annotated, opts.annotated_count,
                        &merge_opts, &checkout_opts);
        check_lg2(err, "merge failed", NULL);
    }

    /* If we get here, we actually performed the merge above */
    m_stResult.conflictFileList.clear();
    check_lg2(git_repository_index(&index, repo), "failed to get repository index", NULL);

    if (git_index_has_conflicts(index)) {
        /* Handle conflicts */
        output_conflicts(index);
        m_stResult.content = "文件有冲突，请解决冲突。";
        m_stResult.statusCode = "-1";
    } else if (!opts.no_commit) { // 不提交合并，可能会影响下次提交
        create_merge_commit(repo, index, &opts);
        printf("Merge made\n");
    }
cleanup:
    free((char **)opts.heads);
    free(opts.annotated);
    return 0;
}

static int progress_cb(const char *str, int len, void *data)
{
    (void)data;
    printf("remote: %.*s", len, str);
    fflush(stdout); /* We don't have the \n to force the flush */
    return 0;
}

/**
 * This function gets called for each remote-tracking branch that gets
 * updated. The message we output depends on whether it's a new one or
 * an update.
 */
static int update_cb(const char *refname, const git_oid *a, const git_oid *b, void *data)
{
    char a_str[GIT_OID_HEXSZ+1], b_str[GIT_OID_HEXSZ+1];
    (void)data;

    git_oid_fmt(b_str, b);
    b_str[GIT_OID_HEXSZ] = '\0';

    if (git_oid_is_zero(a)) {
        printf("[new]     %.20s %s\n", b_str, refname);
    } else {
        git_oid_fmt(a_str, a);
        a_str[GIT_OID_HEXSZ] = '\0';
        printf("[updated] %.10s..%.10s %s\n", a_str, b_str, refname);
    }

    return 0;
}

/**
 * This gets called during the download and indexing. Here we show
 * processed and total objects in the pack and the amount of received
 * data. Most frontends will probably want to show a percentage and
 * the download rate.
 */
static int transfer_progress_cb(const git_indexer_progress *stats, void *payload)
{
    (void)payload;

    if (stats->received_objects == stats->total_objects) {
        printf("Resolving deltas %u/%u\r",
               stats->indexed_deltas, stats->total_deltas);
    } else if (stats->total_objects > 0) {
        printf("Received %u/%u objects (%u) in %" PRIuZ " bytes\r",
               stats->received_objects, stats->total_objects,
               stats->indexed_objects, stats->received_bytes);
    }
    return 0;
}

/** Entry point for this command */
int GspPull::lg2_fetch(git_repository *repo, int argc, char **argv)
{
    git_remote *remote = NULL;
    const git_indexer_progress *stats;
    git_fetch_options fetch_opts = GIT_FETCH_OPTIONS_INIT;

    if (argc < 2) {
        fprintf(stderr, "usage: %s fetch <repo>\n", argv[-1]);
        return EXIT_FAILURE;
    }

    /* Figure out whether it's a named remote or a URL */
    printf("Fetching %s for repo %p\n", argv[1], repo);
    if (git_remote_lookup(&remote, repo, argv[1]) < 0)
        if (git_remote_create_anonymous(&remote, repo, argv[1]) < 0)
            goto on_error;

    /* Set up the callbacks (only update_tips for now) */
    fetch_opts.callbacks.update_tips = &update_cb;
    fetch_opts.callbacks.sideband_progress = &progress_cb;
    fetch_opts.callbacks.transfer_progress = transfer_progress_cb;
    fetch_opts.callbacks.credentials = cred_acquire_cb;

    /**
     * Perform the fetch with the configured refspecs from the
     * config. Update the reflog for the updated references with
     * "fetch".
     */
    if (git_remote_fetch(remote, NULL, &fetch_opts, "fetch") < 0)
        goto on_error;

    /**
     * If there are local objects (we got a thin pack), then tell
     * the user how many objects we saved from having to cross the
     * network.
     */
    stats = git_remote_stats(remote);
    if (stats->local_objects > 0) {
        printf("\rReceived %u/%u objects in %" PRIuZ " bytes (used %u local objects)\n",
               stats->indexed_objects, stats->total_objects, stats->received_bytes, stats->local_objects);
    } else{
        printf("\rReceived %u/%u objects in %" PRIuZ "bytes\n",
            stats->indexed_objects, stats->total_objects, stats->received_bytes);
    }

    git_remote_free(remote);

    return 0;

 on_error:
    git_remote_free(remote);
    return -1;
}
