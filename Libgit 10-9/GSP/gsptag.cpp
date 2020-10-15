#include "gsptag.h"

GspTag::GspTag()
{

}

GspTag::~GspTag()
{

}

void GspTag::Tag(QString prjName, QString username, QString password, QString commitEmail, QString opt)
{
    g_userName = username;
    g_password = password;
    m_userEmail = commitEmail;
    RW_ConfigFile::getInstence()->reReadRepositoryPath();
    QString tmpLocal = RW_ConfigFile::getInstence()->getRepositoryPath() + prjName;
    QByteArray ba_repPath = tmpLocal.toUtf8();

    int error = 0;
    git_repository* rep = nullptr;
    char* tagArgv[16] = {nullptr};
    tagArgv[0] = "tag";
    m_stResult.content = "标签操作成功！"; m_stResult.statusCode = "0";
    git_libgit2_init();
    error = git_repository_open(&rep, ba_repPath.data());
    if (error < 0) {
        goto SHUTDOWN;
    }
    if (opt.compare("delete")) { // 删除标签
        tagArgv[1] = "-d";
        tagArgv[2] = "v1.0.2";
        error = lg2_tag(rep, 3, tagArgv);
    }
    else if (opt.compare("create")) { // 创建标签
        tagArgv[1] = "-a";
        tagArgv[2] = "v1.0.1";
        error = lg2_tag(rep, 3, tagArgv);
    }
    else if (opt.compare("list")) { // 列出所有标签
        tagArgv[1] = "-l"; // --list
        error = lg2_tag(rep, 2, tagArgv);
    }else {
        // 后期为某个commit打标签
    }
SHUTDOWN:
    if (error < 0) {
        emit signal_Error(git2_ExecuteError(error)); //失败
    }
    if (rep) {
        git_repository_free(rep);
        git_libgit2_shutdown();
        //emit signal_TagResult(m_stResult);
        // printf+打印信息
    }
}

/** tag_options represents the parsed command line options */
struct tag_options {
    const char *message;
    const char *pattern;
    const char *tag_name;
    const char *target;
    int num_lines;
    int force;
};

/** tag_state represents the current program state for dragging around */
typedef struct {
    git_repository *repo;
    struct tag_options *opts;
} tag_state;

/** An action to execute based on the command line arguments */
typedef void (*tag_action)(tag_state *state);
typedef struct args_info args_info;

static void check(int result, const char *message)
{
    if (result) fatal(message, NULL);
}

/** Tag listing: Print individual message lines */
static void print_list_lines(const char *message, const tag_state *state)
{
    const char *msg = message;
    int num = state->opts->num_lines - 1;

    if (!msg) return;

    /** first line - headline */
    while(*msg && *msg != '\n') printf("%c", *msg++);

    /** skip over new lines */
    while(*msg && *msg == '\n') msg++;

    printf("\n");

    /** print just headline? */
    if (num == 0) return;
    if (*msg && msg[1]) printf("\n");

    /** print individual commit/tag lines */
    while (*msg && num-- >= 2) {
        printf("    ");

        while (*msg && *msg != '\n') printf("%c", *msg++);

        /** handle consecutive new lines */
        if (*msg && *msg == '\n' && msg[1] == '\n') {
            num--;
            printf("\n");
        }
        while(*msg && *msg == '\n') msg++;

        printf("\n");
    }
}

/** Tag listing: Print an actual tag object */
static void print_tag(git_tag *tag, const tag_state *state)
{
    printf("%-16s", git_tag_name(tag));

    if (state->opts->num_lines) {
        const char *msg = git_tag_message(tag);
        print_list_lines(msg, state);
    } else {
        printf("\n");
    }
}

/** Tag listing: Print a commit (target of a lightweight tag) */
static void print_commit(git_commit *commit, const char *name,
        const tag_state *state)
{
    printf("%-16s", name);

    if (state->opts->num_lines) {
        const char *msg = git_commit_message(commit);
        print_list_lines(msg, state);
    } else {
        printf("\n");
    }
}

/** Tag listing: Fallback, should not happen */
static void print_name(const char *name)
{
    printf("%s\n", name);
}

/** Tag listing: Lookup tags based on ref name and dispatch to print */
static int each_tag(const char *name, tag_state *state)
{
    git_repository *repo = state->repo;
    git_object *obj;
git_revparse_single(&obj, repo, name);
    //check_lg2(, "Failed to lookup rev", name);

    switch (git_object_type(obj)) {
        case GIT_OBJECT_TAG:
            print_tag((git_tag *) obj, state);
            break;
        case GIT_OBJECT_COMMIT:
            print_commit((git_commit *) obj, name, state);
            break;
        default:
            print_name(name);
    }

    git_object_free(obj);
    return 0;
}

static void action_list_tags(tag_state *state)
{
    const char *pattern = state->opts->pattern;
    git_strarray tag_names = {0};
    size_t i;
git_tag_list_match(&tag_names, pattern ? pattern : "*", state->repo);
    //check_lg2(, "Unable to get list of tags", NULL);

    for(i = 0; i < tag_names.count; i++) {
        each_tag(tag_names.strings[i], state);
    }

    git_strarray_dispose(&tag_names);
}

static void action_delete_tag(tag_state *state)
{
    struct tag_options *opts = state->opts;
    git_object *obj;
    git_buf abbrev_oid = {0};

    check(!opts->tag_name, "Name required");
git_revparse_single(&obj, state->repo, opts->tag_name);
    //check_lg2(, "Failed to lookup rev", opts->tag_name);
git_object_short_id(&abbrev_oid, obj);
    //check_lg2(, "Unable to get abbreviated OID", opts->tag_name);
git_tag_delete(state->repo, opts->tag_name);
    //check_lg2(, "Unable to delete tag", opts->tag_name);

    printf("Deleted tag '%s' (was %s)\n", opts->tag_name, abbrev_oid.ptr);

    git_buf_dispose(&abbrev_oid);
    git_object_free(obj);
}

static void action_create_lighweight_tag(tag_state *state)
{
    git_repository *repo = state->repo;
    struct tag_options *opts = state->opts;
    git_oid oid;
    git_object *target;

    check(!opts->tag_name, "Name required");

    if (!opts->target) opts->target = "HEAD";

    check(!opts->target, "Target required");
git_revparse_single(&target, repo, opts->target);
    //check_lg2(, "Unable to resolve spec", opts->target);
git_tag_create_lightweight(&oid, repo, opts->tag_name, target, opts->force);
    //check_lg2(, "Unable to create tag", NULL);

    git_object_free(target);
}

static void action_create_tag(tag_state *state)
{
    git_repository *repo = state->repo;
    struct tag_options *opts = state->opts;
    git_signature *tagger;
    git_oid oid;
    git_object *target;

    check(!opts->tag_name, "Name required");
    check(!opts->message, "Message required");

    if (!opts->target) opts->target = "HEAD";
git_revparse_single(&target, repo, opts->target);
    //check_lg2(, "Unable to resolve spec", opts->target);
git_signature_default(&tagger, repo); // 打标签者
    //check_lg2(, "Unable to create signature", NULL);
git_tag_create(&oid, repo, opts->tag_name, target, tagger, opts->message, opts->force);
    //check_lg2(, "Unable to create tag", NULL);

    git_object_free(target);
    git_signature_free(tagger);
}

static void print_usage(void)
{
    fprintf(stderr, "usage: see `git help tag`\n");
    //exit(1);
}

/** Parse command line arguments and choose action to run when done */
static void parse_options(tag_action *action, struct tag_options *opts, int argc, char **argv)
{
    args_info args = ARGS_INFO_INIT;
    *action = &action_list_tags;

    for (args.pos = 1; args.pos < argc; ++args.pos) {
        const char *curr = argv[args.pos];

        if (curr[0] != '-') {
            if (!opts->tag_name)
                opts->tag_name = curr;
            else if (!opts->target)
                opts->target = curr;
            else
                print_usage();

            if (*action != &action_create_tag)
                *action = &action_create_lighweight_tag;
        } else if (!strcmp(curr, "-n")) {
            opts->num_lines = 1;
            *action = &action_list_tags;
        } else if (!strcmp(curr, "-a")) {
            *action = &action_create_tag;
        } else if (!strcmp(curr, "-f")) {
            opts->force = 1;
        } else if (match_int_arg(&opts->num_lines, &args, "-n", 0)) {
            *action = &action_list_tags;
        } else if (match_str_arg(&opts->pattern, &args, "-l")) {
            *action = &action_list_tags;
        } else if (match_str_arg(&opts->tag_name, &args, "-d")) {
            *action = &action_delete_tag;
        } else if (match_str_arg(&opts->message, &args, "-m")) {
            *action = &action_create_tag;
        }
    }
}

/** Initialize tag_options struct */
static void tag_options_init(struct tag_options *opts)
{
    memset(opts, 0, sizeof(*opts));

    opts->message   = NULL;
    opts->pattern   = NULL;
    opts->tag_name  = NULL;
    opts->target    = NULL;
    opts->num_lines = 0;
    opts->force     = 0;
}

int GspTag::lg2_tag(git_repository *repo, int argc, char **argv)
{
    struct tag_options opts;
    tag_action action;
    tag_state state;

    tag_options_init(&opts);
    parse_options(&action, &opts, argc, argv);

    state.repo = repo;
    state.opts = &opts;
    action(&state);

    return 0;
}
