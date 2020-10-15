#include "gspclone.h"

GspClone::GspClone()
{

}

GspClone::~GspClone()
{

}

typedef struct progress_data {
    git_indexer_progress fetch_progress;
    size_t completed_steps;
    size_t total_steps;
    const char *path;
} progress_data;

static void print_progress(const progress_data *pd)
{
    int network_percent = pd->fetch_progress.total_objects > 0 ?
        (100*pd->fetch_progress.received_objects) / pd->fetch_progress.total_objects :
        0;
    int index_percent = pd->fetch_progress.total_objects > 0 ?
        (100*pd->fetch_progress.indexed_objects) / pd->fetch_progress.total_objects :
        0;

    int checkout_percent = pd->total_steps > 0
        ? (int)((100 * pd->completed_steps) / pd->total_steps)
        : 0;
    size_t kbytes = pd->fetch_progress.received_bytes / 1024;

    if (pd->fetch_progress.total_objects &&
        pd->fetch_progress.received_objects == pd->fetch_progress.total_objects) {
        printf("Resolving deltas %u/%u\r",
               pd->fetch_progress.indexed_deltas,
               pd->fetch_progress.total_deltas);
    } else {
        printf("net %3d%% (%4" PRIuZ " kb, %5u/%5u)  /  idx %3d%% (%5u/%5u)  /  chk %3d%% (%4" PRIuZ "/%4" PRIuZ")%s\n",
           network_percent, kbytes,
           pd->fetch_progress.received_objects, pd->fetch_progress.total_objects,
           index_percent, pd->fetch_progress.indexed_objects, pd->fetch_progress.total_objects,
           checkout_percent,
           pd->completed_steps, pd->total_steps,
           pd->path);
    }
}

static int sideband_progress(const char *str, int len, void *payload)
{
    (void)payload; /* unused */

    printf("remote: %.*s", len, str);
    fflush(stdout);
    return 0;
}

static int fetch_progress(const git_indexer_progress *stats, void *payload)
{
    progress_data *pd = (progress_data*)payload;
    pd->fetch_progress = *stats;
    print_progress(pd);
    return 0;
}
static void checkout_progress(const char *path, size_t cur, size_t tot, void *payload)
{
    progress_data *pd = (progress_data*)payload;
    pd->completed_steps = cur;
    pd->total_steps = tot;
    pd->path = path;
    print_progress(pd);
}

void GspClone::Clone(QString userName, QString password, QString remote, QString prjName)
{
    g_userName = userName;
    g_password = password;

    RW_ConfigFile::getInstence()->reReadRepositoryPath();
    QString tmpLocal = RW_ConfigFile::getInstence()->getRepositoryPath() + prjName;
    QByteArray ba_CloneUrl = remote.toUtf8();
    QByteArray ba_ClonePath = tmpLocal.toUtf8();
    char* argv[3] = {"clone", ba_CloneUrl.data(), ba_ClonePath.data()};
    lg2_clone(3, argv);
}

int GspClone::lg2_clone(int argc, char **argv)
{
    git_libgit2_init();
    progress_data pd = {{0}};
    git_repository *cloned_repo = NULL;
    git_clone_options clone_opts = GIT_CLONE_OPTIONS_INIT;
    git_checkout_options checkout_opts = GIT_CHECKOUT_OPTIONS_INIT;
    const char *url = argv[1];
    const char *path = argv[2];
    int error;
    ExecuteResult stResult; stResult.content = "克隆成功！"; stResult.statusCode = "0";
    qDebug() << "开始克隆。";

    /* Set up options */
    checkout_opts.checkout_strategy = GIT_CHECKOUT_SAFE;
    checkout_opts.progress_cb = checkout_progress;
    checkout_opts.progress_payload = &pd;
    clone_opts.checkout_opts = checkout_opts;
    clone_opts.fetch_opts.callbacks.sideband_progress = sideband_progress;
    clone_opts.fetch_opts.callbacks.transfer_progress = &fetch_progress;
    clone_opts.fetch_opts.callbacks.credentials = cred_acquire_cb;
    clone_opts.fetch_opts.callbacks.payload = &pd;

    /* Do the clone */
    error = git_clone(&cloned_repo, url, path, &clone_opts);
    if (error != 0) {
        emit signal_Error(git2_ExecuteError(error));
        qDebug() << "克隆失败。";
    }
    else if (cloned_repo) {
        git_repository_free(cloned_repo);
        git_libgit2_shutdown();
        emit signal_CloneResult(stResult);
        qDebug() << "克隆成功。";
    }
    return error;
}
