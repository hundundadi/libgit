#include "gspbasecommon.h"

QString g_userName;
QString g_password;
CommitLog g_commitLog;

GspBaseCommon::GspBaseCommon(QObject *parent) : QObject(parent)
{
    connect(this, SIGNAL(signal_FileStatus(FileStatus)), GitCmdHandle::getInstence(),SLOT(slot_statusResult(FileStatus)));
    connect(this, SIGNAL(signal_CloneResult(ExecuteResult)), GitCmdHandle::getInstence(), SLOT(slot_cloneResult(ExecuteResult)));
    connect(this, SIGNAL(signal_CheckoutResult(ExecuteResult)), GitCmdHandle::getInstence(), SLOT(slot_checkoutResult(ExecuteResult)));
    connect(this, SIGNAL(signal_CommitResult(ExecuteResult)), GitCmdHandle::getInstence(), SLOT(slot_commitPushResult(ExecuteResult)));
    connect(this, SIGNAL(signal_LogResult(QList<CommitLog>)), GitCmdHandle::getInstence(), SLOT(slot_logResult(QList<CommitLog>)));
    connect(this, SIGNAL(signal_Error(ExecuteResult)), GitCmdHandle::getInstence(), SLOT(slot_Error(ExecuteResult)));
    connect(this, SIGNAL(signal_PullResult(ExecuteResult)), GitCmdHandle::getInstence(), SLOT(slot_pullResult(ExecuteResult)));
}

void GspBaseCommon::check_lg2(int error, const char *message, const char *extra)
{
    const git_error *lg2err;
    const char *lg2msg = "", *lg2spacer = "";
    char buffer[512];

    if (!error)
        return;

    if ((lg2err = git_error_last()) != NULL && lg2err->message != NULL) {
        lg2msg = lg2err->message;
        lg2spacer = " - ";
    }

    if (extra)
        sprintf(buffer, "%s '%s' [%d]%s%s",
            message, extra, error, lg2spacer, lg2msg);
    else
        sprintf(buffer, "%s [%d]%s%s",
            message, error, lg2spacer, lg2msg);
    ExecuteResult stResult;
    stResult.statusCode = QString::number(error);
    stResult.content = QString(buffer);
    emit signal_Error(stResult);
}

ExecuteResult GspBaseCommon::git2_ExecuteError(int error)
{
    QString strError; ExecuteResult stResult;
    const git_error *e = giterr_last();
    strError = "Error: " + QString::number(error) + " / " + QString::number(e->klass) + " : " + e->message;
    stResult.content = strError;
    stResult.statusCode = QString::number(error);
    return stResult;
}

// clone验证、拉取验证 用户名、密码验证
int cred_acquire_cb(git_credential **out,
        const char *url,
        const char *username_from_url,
        unsigned int allowed_types,
        void *payload)
{
    int error = 1;
    UNUSED(url);
    UNUSED(payload);
    UNUSED(username_from_url);
    UNUSED(allowed_types);
    QByteArray ba_user = g_userName.toUtf8();
    QByteArray ba_pass = g_password.toUtf8();
    // 1.ssh密钥验证 2.用户名、密码验证 3.用户名验证
    error = git_credential_userpass_plaintext_new(out, ba_user.constData(), ba_pass.constData());
    return error;
}

int resolve_refish(git_annotated_commit **commit, git_repository *repo, const char *refish)
{
    git_reference *ref;
    git_object *obj;
    int err = 0;

    assert(commit != NULL);

    err = git_reference_dwim(&ref, repo, refish);
    if (err == GIT_OK) {
        git_annotated_commit_from_ref(commit, repo, ref);
        git_reference_free(ref);
        return 0;
    }

    err = git_revparse_single(&obj, repo, refish);
    if (err == GIT_OK) {
        err = git_annotated_commit_lookup(commit, repo, git_object_id(obj));
        git_object_free(obj);
    }

    return err;
}

void fatal(const char *message, const char *extra)
{
    if (extra)
        fprintf(stderr, "%s %s\n", message, extra);
    else
        fprintf(stderr, "%s\n", message);

    exit(1);
}

void *xrealloc(void *oldp, size_t newsz)
{
    void *p = realloc(oldp, newsz);
    if (p == NULL) {
        fprintf(stderr, "Cannot allocate memory, exiting.\n");
        exit(1);
    }
    return p;
}

int diff_output(
    const git_diff_delta *d,
    const git_diff_hunk *h,
    const git_diff_line *l,
    void *p)
{
    FILE *fp = (FILE*)p;

    (void)d; (void)h;

    if (!fp)
        fp = stdout;

    if (l->origin == GIT_DIFF_LINE_CONTEXT ||
        l->origin == GIT_DIFF_LINE_ADDITION ||
        l->origin == GIT_DIFF_LINE_DELETION)
        fputc(l->origin, fp);

    fwrite(l->content, 1, l->content_len, fp);

    return 0;
}

size_t is_prefixed(const char *str, const char *pfx)
{
    size_t len = strlen(pfx);
    return strncmp(str, pfx, len) ? 0 : len;
}

int match_bool_arg(int *out, struct args_info *args, const char *opt)
{
    const char *found = args->argv[args->pos];

    if (!strcmp(found, opt)) {
        *out = 1;
        return 1;
    }

    if (!strncmp(found, "--no-", strlen("--no-")) &&
        !strcmp(found + strlen("--no-"), opt + 2)) {
        *out = 0;
        return 1;
    }

    *out = -1;
    return 0;
}

int match_arg_separator(struct args_info *args)
{
    if (args->opts_done)
        return 1;

    if (strcmp(args->argv[args->pos], "--") != 0)
        return 0;

    args->opts_done = 1;
    args->pos++;
    return 1;
}

int match_str_arg(
    const char **out, struct args_info *args, const char *opt)
{
    const char *found = args->argv[args->pos];
    size_t len = is_prefixed(found, opt);

    if (!len)
        return 0;

    if (!found[len]) {
        if (args->pos + 1 == args->argc)
            fatal("expected value following argument", opt);
        args->pos += 1;
        *out = args->argv[args->pos];
        return 1;
    }

    if (found[len] == '=') {
        *out = found + len + 1;
        return 1;
    }

    return 0;
}

void strarray_from_args(git_strarray *array, struct args_info *args)
{
    size_t i;

    array->count = args->argc - args->pos;
    array->strings = (char**)calloc(array->count, sizeof(char *));
    assert(array->strings != NULL);

    for (i = 0; args->pos < args->argc; ++args->pos) {
        array->strings[i++] = args->argv[args->pos];
    }
    args->pos = args->argc;
}

static const char *match_numeric_arg(struct args_info *args, const char *opt)
{
    const char *found = args->argv[args->pos];
    size_t len = is_prefixed(found, opt);

    if (!len)
        return NULL;

    if (!found[len]) {
        if (args->pos + 1 == args->argc)
            fatal("expected numeric value following argument", opt);
        args->pos += 1;
        found = args->argv[args->pos];
    } else {
        found = found + len;
        if (*found == '=')
            found++;
    }

    return found;
}

static int match_int_internal(
    int *out, const char *str, int allow_negative, const char *opt)
{
    char *endptr = NULL;
    int	  val = (int)strtol(str, &endptr, 10);

    if (!endptr || *endptr != '\0')
        fatal("expected number", opt);
    else if (val < 0 && !allow_negative)
        fatal("negative values are not allowed", opt);

    if (out)
        *out = val;

    return 1;
}

int match_int_arg(
    int *out, struct args_info *args, const char *opt, int allow_negative)
{
    const char *found = match_numeric_arg(args, opt);
    if (!found)
        return 0;
    return match_int_internal(out, found, allow_negative, opt);
}

int is_integer(int *out, const char *str, int allow_negative)
{
    return match_int_internal(out, str, allow_negative, NULL);
}
