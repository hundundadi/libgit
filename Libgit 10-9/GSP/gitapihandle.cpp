#include "gitapihandle.h"

GitApiHandle::GitApiHandle(QObject *parent) : QObject(parent)
{
    connect(this, SIGNAL(signal_FileStatus(FileStatus)), GitCmdHandle::getInstence(),SLOT(slot_statusResult(FileStatus)));
    connect(this, SIGNAL(signal_CloneResult(ExecuteResult)), GitCmdHandle::getInstence(), SLOT(slot_cloneResult(ExecuteResult)));
    connect(this, SIGNAL(signal_CheckoutResult(ExecuteResult)), GitCmdHandle::getInstence(), SLOT(slot_checkoutResult(ExecuteResult)));
    connect(this, SIGNAL(signal_CommitResult(ExecuteResult)), GitCmdHandle::getInstence(), SLOT(slot_commitPushResult(ExecuteResult)));
    connect(this, SIGNAL(signal_LogResult(QList<CommitLog>)), GitCmdHandle::getInstence(), SLOT(slot_logResult(QList<CommitLog>)));
    connect(this, SIGNAL(signal_Error(ExecuteResult)), GitCmdHandle::getInstence(), SLOT(slot_Error(ExecuteResult)));
    m_pGitRepository = nullptr;
    m_iGitParentCount = 0;
    m_pParents[0] = {nullptr};
}

GitApiHandle::~GitApiHandle()
{
}

GitApiHandle* GitApiHandle::m_pInstence = nullptr;

GitApiHandle* GitApiHandle::getInstence()
{
    if(m_pInstence == nullptr)
    {// 懒汉式 多线程不安全啊
        m_pInstence = new GitApiHandle();
    }
    return m_pInstence;
}

QString fileStatus(unsigned int status_flags)
{
    if (GIT_STATUS_INDEX_NEW == status_flags){
        return "index_ new.";
    }// 相对于本地索引->新建
    else if (GIT_STATUS_INDEX_MODIFIED == status_flags){
        return "index_ modified.";
    }// 相对于本地索引->修改
    else if (GIT_STATUS_INDEX_DELETED == status_flags){
        return "index_ type deleted.";
    }// 相对于本地索引->删除
    else if (GIT_STATUS_INDEX_RENAMED == status_flags){
        return "index_ rename.";
    }// 相对于本地索引->重命名
    else if (GIT_STATUS_INDEX_TYPECHANGE == status_flags){
        return "index_ type change.";
    }// 相对于本地索引->类型修改
    else if (GIT_STATUS_WT_NEW == status_flags){
        return "new file.";
    }// 相对于git服务器->新建
    else if (GIT_STATUS_WT_MODIFIED == status_flags){
        return "modified file.";
    }// 相对于git服务器->修改
    else if (GIT_STATUS_WT_DELETED == status_flags){
        return "deleted file.";
    }// 相对于git服务器->删除
    else if (GIT_STATUS_WT_TYPECHANGE == status_flags){
        return "type change file.";
    }// 相对于git服务器->类型修改
    else if (GIT_STATUS_WT_RENAMED == status_flags){
        return "rename file.";
    }// 相对于git服务器->重命名
    else if (GIT_STATUS_WT_UNREADABLE == status_flags){
        return "unreadable file.";
    }// 相对于git服务器->不可读
    else if (GIT_STATUS_IGNORED == status_flags){
        return "ignored file.";
    }// .git中忽略的文件
    else if (GIT_STATUS_CONFLICTED == status_flags){
        return "conflicted file.";
    }// 冲突文件
    else//GIT_STATUS_CURRENT
    {
        return "unknow status.";
    }//未知状态
}

void GitApiHandle::OptionsRepository()
{
    RW_ConfigFile::getInstence()->reReadRepositoryPath();
    m_sLocalRep = RW_ConfigFile::getInstence()->getRepositoryPath() + m_sPrjName;
    if (m_sLocalRep.contains("\\\\")) {
        m_sLocalRep.replace("\\\\", "\\");
    }
    QDir dir(m_sLocalRep);
    if(!dir.exists())
    {
        dir.mkpath(m_sLocalRep);
    }
}

ExecuteResult GitApiHandle::git2_ExecuteError(int error)
{
    QString strError; ExecuteResult stResult;
    const git_error *e = giterr_last();
    strError = "Error: " + QString::number(error) + " / " + QString::number(e->klass) + " : " + e->message;
    stResult.content = strError;
    stResult.statusCode = QString::number(error);
    return stResult;
}

int status_cb(const char *path, unsigned int status_flags, void *payload)
{
    (void)payload;
    FileStatus stFileStatus;
    stFileStatus.file = QString(path);
    stFileStatus.status = fileStatus(status_flags);
    qDebug() << stFileStatus.file;
    qDebug() << stFileStatus.status;
    GitApiHandle::getInstence()->m_FileList.append(stFileStatus);
    return 0;
}

// 调用查询本地git仓库所有文件状态
bool GitApiHandle::gitStatus(QString projectName, QString username, QString password)
{
    m_sUserName = username;
    m_sPassword = password;
    m_sPrjName = projectName;
    m_FileList.clear();
    ExecuteResult stResult; stResult.content = "仓库状态正常"; stResult.statusCode = "0";
    qDebug() << "开始仓库文件状态查询。";
    OptionsRepository();
    int error = 0;
    git_index* index = nullptr;
    git_index_conflict_iterator* conflict_iterator = nullptr;
    // init and open repository.
    error = git2_InitAndOpen();
    if(error < 0)
    {
        emit signal_Error(git2_ExecuteError(error));
        goto SHUTDOWN;
    }
    git_repository_index(&index, m_pGitRepository);
    if (git_index_has_conflicts(index))
    {
        const git_index_entry* ancestor_out = nullptr;
        const git_index_entry* our_out = nullptr;
        const git_index_entry* their_out = nullptr;

        git_index_conflict_iterator_new(&conflict_iterator, index);

        while (git_index_conflict_next(&ancestor_out, &our_out, &their_out, conflict_iterator) != GIT_ITEROVER)
        {
            if (ancestor_out->path){
                qDebug() << ancestor_out->path;
                error = git_index_conflict_remove(index, ancestor_out->path);
            }
            else if (our_out->path){
                qDebug() << our_out->path;
                error = git_index_conflict_remove(index, our_out->path);
            }
            else if (their_out->path) {
                qDebug() << their_out->path;
                error = git_index_conflict_remove(index, their_out->path);
            }
            else
                qDebug() << "冲突文件信息无法获取，冲突标志无法取消！";
            if (error < 0)
            {
                emit signal_Error(git2_ExecuteError(error));
            }
        }

        // git checkout --theirs <file>
        git_checkout_options opt = GIT_CHECKOUT_OPTIONS_INIT;
        opt.checkout_strategy |= GIT_CHECKOUT_USE_OURS;
        git_checkout_index(m_pGitRepository, index, &opt);

        git_index_conflict_iterator_free(conflict_iterator);
    }
    git_index_free(index);
    // simple
    error = git_status_foreach(m_pGitRepository, status_cb, nullptr);
    if (error < 0)
    {
        emit signal_Error(git2_ExecuteError(error));
    }
SHUTDOWN:
    git_repository_free(m_pGitRepository);
    git_libgit2_shutdown();
    if (!(error < 0)) {
        updateWebPage();
        qDebug() << "仓库状态查询成功。";
    }
    return true;
}
// 获取一个文件下的所有文件
FileStatus GitApiHandle::getRepostoryFileAndDir(QString path, FileStatus &pramfile, int &iGrade)
{
    QDir dir(path);
    dir.setSorting(QDir::Size | QDir::Reversed);
    QFileInfoList file_list = dir.entryInfoList(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    QFileInfoList folder_list = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);

    foreach(QFileInfo folder, folder_list)
    {
        FileStatus dirInfo;
        QUuid uuid = QUuid::createUuid();
        if (!pramfile.file.isEmpty())
            dirInfo.belongto = folder.absolutePath().split('/').last();// 父文件夹
        dirInfo.file = folder.fileName();
        dirInfo.grade = QString::number(iGrade);
        dirInfo.deptid = uuid.toString();
        dirInfo.date = folder.lastModified().toString("yyyy-MM-dd HH:mm:ss");
        //dirInfo.size = caculateSuitFileSizeUnit(folder.size());
        dirInfo.relativepath = folder.filePath().remove(0, m_sLocalRep.size() + 1);
        iGrade++;
        pramfile.children.append(getRepostoryFileAndDir(folder.absoluteFilePath(), dirInfo, iGrade));
        iGrade--;
    }
    foreach(QFileInfo file, file_list)
    {
        FileStatus fileInfo;
        QUuid uuid = QUuid::createUuid();
        if (!pramfile.file.isEmpty())
            fileInfo.belongto = file.absolutePath().split('/').last();// 父文件夹
        fileInfo.file = file.fileName();
        fileInfo.grade = QString::number(iGrade);
        fileInfo.deptid = uuid.toString();
        fileInfo.date = file.lastModified().toString("yyyy-MM-dd HH:mm:ss");
        fileInfo.size = caculateSuitFileSizeUnit(file.size());
        fileInfo.relativepath = file.filePath().remove(0, m_sLocalRep.size() + 1);
        GspLog* logObj = new GspLog;
        logObj->Log(m_sPrjName, m_sUserName, m_sPassword, QStringList(fileInfo.relativepath));
        fileInfo.user = g_commitLog.author;
        fileInfo.version = g_commitLog.commit_info;
        g_commitLog.author.clear();
        g_commitLog.commit_info.clear();
        g_commitLog.commit_sha.clear();
        g_commitLog.dateTime.clear();
        g_commitLog.email.clear();
        g_commitLog.mergeFrom.clear();
        delete logObj;
        pramfile.children.append(fileInfo);
    }
    return pramfile;
}
// 计算出适合的文件大小 及单位
QString GitApiHandle::caculateSuitFileSizeUnit(qint64 size)
{
    QString fileSize;
    int i_TB = (size / (float)byteToGB) / 1024;
    int i_GB = size / byteToGB;
    int i_MB = size / byteToMB;
    int i_KB = size / byteToKB;
    if (i_TB > 0)
        fileSize = QString::number((size / (float)byteToGB) / 1024.0) + "TB";
    else if (i_GB)
        fileSize = QString::number(size / (float)byteToGB) + "GB";
    else if (i_MB)
        fileSize = QString::number(size / (float)byteToMB) + "MB";
    else if (i_KB)
        fileSize = QString::number(size / (float)byteToKB) + "KB";
    else
        fileSize = QString::number(size) + "Bytes";
    return fileSize;
}

bool GitApiHandle::updateFileStatus(QStringList repFileList, FileStatus &fileList)
{
    foreach (QString strFileName, repFileList) {
        QStringList tmpFileList = repFileList;
        tmpFileList.removeFirst();
        for (int i = 0; i < fileList.children.size(); i ++) {
            if (!tmpFileList.isEmpty()) { // 文件夹
                m_bRepSamePath = false; // 路径未必一致
                if (!fileList.children[i].children.isEmpty()) {
                    if (strFileName == fileList.children[i].file) {
                        m_bRepSamePath = true; // 路径一致
                        m_strDir_QUUID = fileList.children[i].deptid;
                        if(updateFileStatus(tmpFileList, fileList.children[i]))
                        return true;
                    }
                }
            }
            else { // 文件
                if (strFileName == fileList.children[i].file) {
                    fileList.children[i].status = m_strCurrentStatus;
                    return true;
                }
                else {
                    if (m_bRepDeletedFile && m_bRepSamePath && i == (fileList.children.size()-1)) {
                        FileStatus stRepDeletedFile;
                        stRepDeletedFile.file = strFileName;
                        stRepDeletedFile.grade = fileList.children[i].grade;
                        stRepDeletedFile.deptid = QUuid::createUuid().toString();
                        stRepDeletedFile.belongto = fileList.children[i].belongto;
                        stRepDeletedFile.date = "";
                        stRepDeletedFile.size = "";
                        QString tmpPath = fileList.children[i].relativepath;
                        tmpPath.replace(fileList.children[i].file, strFileName);
                        stRepDeletedFile.relativepath = tmpPath;
                        GspLog* logObj = new GspLog;
                        logObj->Log(m_sPrjName, m_sUserName, m_sPassword, QStringList(stRepDeletedFile.relativepath));
                        stRepDeletedFile.user = g_commitLog.author;
                        stRepDeletedFile.version = g_commitLog.commit_info;
                        g_commitLog.author.clear();
                        g_commitLog.commit_info.clear();
                        g_commitLog.commit_sha.clear();
                        g_commitLog.dateTime.clear();
                        g_commitLog.email.clear();
                        g_commitLog.mergeFrom.clear();
                        delete logObj;
                        stRepDeletedFile.status = m_strCurrentStatus;
                        fileList.children.append(stRepDeletedFile);
                        m_strDir_QUUID.clear();// 当被删除的找到文件，并添加后，应清空uuid
                    }
                    continue;
                }
            }
        }
    }
    return false;
}
//仓库顶层文件夹或文件夹下文件 添加信息
// 文件夹存储 新建 apped + 文件存储 新建 apped到文件夹
void GitApiHandle::AddRepoDeletedFile(QStringList repFileList, FileStatus &fileList, int &iGrade) {
    QString strFileName = repFileList.first();
    QStringList tmpFileList = repFileList;
    tmpFileList.removeFirst();
    if (!tmpFileList.isEmpty()) {
        FileStatus tmpDirStatus;
        if (m_strRelativePath.endsWith('/'))
            tmpDirStatus.belongto = m_strRelativePath.split('/').at(m_strRelativePath.split('/').size() - 2);
        else
            tmpDirStatus.belongto = fileList.file;
        m_strRelativePath += strFileName + "/";

        tmpDirStatus.file = strFileName;
        tmpDirStatus.grade = QString::number(iGrade);
        tmpDirStatus.deptid = QUuid::createUuid().toString();
        tmpDirStatus.date = "";
        tmpDirStatus.size = "";
        tmpDirStatus.relativepath = m_strRelativePath;
        iGrade++;
        AddRepoDeletedFile(tmpFileList, tmpDirStatus, iGrade);
        iGrade--;
        fileList.children.append(tmpDirStatus);
    }
    else {
        FileStatus tmpFileStatus;
        tmpFileStatus.belongto = m_strRelativePath.split('/').at(m_strRelativePath.split('/').size() - 2);
        m_strRelativePath += strFileName;

        tmpFileStatus.file = strFileName;
        tmpFileStatus.grade = QString::number(iGrade);
        tmpFileStatus.deptid = QUuid::createUuid().toString();
        tmpFileStatus.date = "";
        tmpFileStatus.size = "";
        tmpFileStatus.relativepath = m_strRelativePath;
        GspLog* logObj = new GspLog;
        logObj->Log(m_sPrjName, m_sUserName, m_sPassword, QStringList(tmpFileStatus.relativepath));
        tmpFileStatus.user = g_commitLog.author;
        tmpFileStatus.version = g_commitLog.commit_info;
        g_commitLog.author.clear();
        g_commitLog.commit_info.clear();
        g_commitLog.commit_sha.clear();
        g_commitLog.dateTime.clear();
        g_commitLog.email.clear();
        g_commitLog.mergeFrom.clear();
        delete logObj;
        tmpFileStatus.status = m_strCurrentStatus;
        fileList.children.append(tmpFileStatus);
        m_strRelativePath.clear();
    }
}
// uuid对应的文件夹下文件 添加信息
void GitApiHandle::AddRepoDeletedDir(QStringList repFileList, FileStatus &fileList){
    if (!m_strDir_QUUID.isEmpty()) {
        for (int index = 0; index < fileList.children.size(); index++) {
            if (!m_strDir_QUUID.compare(fileList.children[index].deptid)) {
                int iGrade = fileList.children[index].deptid.toInt();
                iGrade++;
                m_strRelativePath += repFileList.first() + "/";
                repFileList.removeFirst();
                AddRepoDeletedFile(repFileList, fileList.children[index], iGrade);
                m_strDir_QUUID.clear();
            }
            else if (!fileList.children[index].children.isEmpty()) {
                AddRepoDeletedDir(repFileList, fileList.children[index]);
            }
        }
    }
}

bool GitApiHandle::updateWebPage()
{
    FileStatus fileAndDirInfo;
    int grade = 0;
    m_bRepDeletedFile = false;
    m_bRepSamePath = false;
    m_strDir_QUUID.clear();
    getRepostoryFileAndDir(m_sLocalRep, fileAndDirInfo, grade);
    foreach (FileStatus tmpVar, m_FileList) { // 只有相对于仓库有变化的文件
        QStringList strList = tmpVar.file.split('/');
        m_strCurrentStatus = tmpVar.status;
        if (m_strCurrentStatus == "deleted file.") {
            m_bRepSamePath = true;
            m_bRepDeletedFile = true;
        }
        bool isSec = updateFileStatus(strList, fileAndDirInfo);
        if(!isSec && !m_strDir_QUUID.isEmpty()) {
            // 不同级 需要查找到对应的uuid文件夹
            AddRepoDeletedDir(strList, fileAndDirInfo);
        }
        else if (!isSec) {
            // 与仓库同级文件夹被删除 直接新建父级别0
            grade = 0;
            AddRepoDeletedFile(strList, fileAndDirInfo, grade);
        }
        m_bRepDeletedFile = false;
        m_bRepSamePath = false;
    }
    emit this->signal_FileStatus(fileAndDirInfo);
    return true;
}

int GitApiHandle::git2_InitAndOpen()
{
    int error = 0;
    // 初始化
    error = git_libgit2_init();
    if (error < 0)
        return error;
    error = git_repository_open(&m_pGitRepository, m_sLocalRep.toUtf8().constData());//m_pConfigData->getRepositoryPath().toUtf8().constData()
    if (error < 0)
        return error;
    return error;
}

int GitApiHandle::git2_GetHead(git_commit** pParentCommit)
{
    int error = 0;
    git_reference* ref_head = nullptr;
    error = git_repository_head(&ref_head, m_pGitRepository);

    // has parent
    if (error == GIT_OK)
    {
        // get parent commit
        git_commit_lookup(pParentCommit, m_pGitRepository, git_reference_target(ref_head));
        m_pParents[0] = *pParentCommit;
        m_iGitParentCount = 1;
        git_reference_free(ref_head);
    }
    else if (error != GIT_EUNBORNBRANCH)
    {
        return error;
    }
    return error;
}

int GitApiHandle::git2_AddFile(git_index** pIndex, QStringList fileList)
{
    int error = 0;
    git_strarray paths = {nullptr, 0};
    if (!fileList.isEmpty()) {
        paths.strings = (char**)calloc(fileList.size(), sizeof(char*));
        for (int i = 0; i < fileList.size(); i++) {
            QString tmpFile = fileList.at(i);
            QByteArray ba = tmpFile.toUtf8();
            paths.strings[i] = ba.data();
        }
        paths.count = fileList.size();
    }
    error = git_index_add_all(*pIndex, &paths, GIT_INDEX_ADD_DEFAULT, nullptr, nullptr);
    return error;
}

int GitApiHandle::git2_AddToTree(git_tree **pNewTree, QStringList fileList)
{
    int error = 0;
    git_oid new_tree_id;
    git_index* pIndex = nullptr;

    // get index
    error = git_repository_index(&pIndex, m_pGitRepository);
    if (error < 0)
    {
        return error;
    }
    // remove conflict index
    //error = git_index_conflict_cleanup(pIndex);
    //if (error < 0) {
    //    return error;
    //}
    error = git2_AddFile(&pIndex, fileList);
    if (error < 0)
    {
        git_index_free(pIndex);
        return error;
    }
    else
    {
        /* Write the in-memory index to disk */
        error = git_index_write(pIndex);
        if (error < 0) {
            return error;
        }
    }

    // write index to tree
    error = git_index_write_tree(&new_tree_id, pIndex);
    if (error < 0)
    {
        return error;
    }
    else
    {
        git_index_free(pIndex);
    }
    error = git_tree_lookup(pNewTree, m_pGitRepository, &new_tree_id);
    return error;
}

int cred_acquire_cb_push(git_cred **cred,
                    const char *url,
                    const char *username_from_url,
                    unsigned int allowed_types,
                    void *payload)
{

    //"C:\\Users\\YeHua\\.ssh\\id_rsa.pub"
    //int error = git_cred_ssh_key_new(cred, username_from_url, nullptr, "C:\\Users\\YeHua\\.ssh\\id_rsa", "123456");
    //int error = git_cred_ssh_key_memory_new(cred, username_from_url, nullptr, nullptr, nullptr);
    //int error = git_cred_ssh_key_from_agent(cred,username_from_url);
    //int error = git_cred_username_new(cred, username_from_url);
    //git_cred_default_new(cred);
    int error = git_cred_userpass_plaintext_new(cred, GitApiHandle::getInstence()->m_sUserName.toUtf8().constData(),
                                                      GitApiHandle::getInstence()->m_sPassword.toUtf8().constData());
    if (error < 0)
    {
        QString strError; ExecuteResult stResult;
        const git_error *e = giterr_last();
        strError = "Error: " + QString::number(error) + " / " + QString::number(e->klass) + " : " + e->message;
        stResult.content = strError;
        stResult.statusCode = QString::number(error);
        emit GitApiHandle::getInstence()->signal_Error(stResult);
    }
    return 0;
}
int GitApiHandle::git2_PushRemote()
{
    int error = 0;
    git_remote* pRemote = nullptr;
    error = git_remote_lookup(&pRemote, m_pGitRepository, "origin");
    if (error < 0)
        return error;
    const char *refs[] = {"refs/heads/master:refs/heads/master"};
    git_strarray strarr = {(char**)refs, 1};
    git_push_options opts = GIT_PUSH_OPTIONS_INIT;
    //git_push_options_init(&opts, GIT_PUSH_OPTIONS_VERSION);

    opts.callbacks.credentials = cred_acquire_cb_push;
    error = git_remote_push(pRemote, &strarr, &opts);
    git_remote_free(pRemote);
    return error;
}
// 提交
bool GitApiHandle::gitCommit(QString prjName, QString username, QString password, QString commitEmail, QString commitContent, QStringList fileList)
{
    int error = 0;
    m_sPrjName = prjName;
    m_sUserName = username;
    m_sPassword = password;
    OptionsRepository();
    ExecuteResult stResult; stResult.content = "提交成功！"; stResult.statusCode = "0";
    git_signature* me = nullptr;
    git_tree* pNewTree = nullptr;
    git_commit* pParentCommit = nullptr;
    git_oid new_commit;
    git_checkout_options gitCheckoutOpt = GIT_CHECKOUT_OPTIONS_INIT;
    qDebug() << "开始提交操作。";
    error = git2_InitAndOpen();
    if(error < 0) {
        emit signal_Error(git2_ExecuteError(error));
        goto SHUTDOWN;
    }
    error = git2_GetHead(&pParentCommit);
    if(error < 0)
    {
        emit signal_Error(git2_ExecuteError(error));
        goto SHUTDOWN;
    }
    error = git2_AddToTree(&pNewTree, fileList);
    if(error < 0)
    {
        emit signal_Error(git2_ExecuteError(error));
        goto SHUTDOWN;
    }
    // signature
    error = git_signature_now(&me, username.toUtf8().constData(), commitEmail.toUtf8().constData());
    if (error < 0) {
        emit signal_Error(git2_ExecuteError(error));
        goto SHUTDOWN;
    }
    // new commit
    error = git_commit_create(&new_commit,
                              m_pGitRepository,
                              "HEAD",
                              me, me,
                              "UTF-8",
                              commitContent.toUtf8().constData(),
                              pNewTree, m_iGitParentCount, m_pParents);
    if (error < 0)
    {
        emit signal_Error(git2_ExecuteError(error));
        goto SHUTDOWN;
    }
    // push remote
    error = git2_PushRemote();
    if(error < 0)
    {
        // 删除已commit的未push的空提交
        git_reset(m_pGitRepository, (git_object*)&new_commit, GIT_RESET_MIXED, &gitCheckoutOpt);
        /* 本地仓库不是最新的pull，需要先拉取；若有冲突则解决后；再提交  */
        if (error == -11){
            if(!gitPull(prjName, username, password, commitEmail, fileList)) {
                stResult.conflictFileList = m_confFileList;
                stResult.content = "提交文件有冲突.";
                stResult.statusCode = "1";
                m_confFileList.clear();
                emit signal_CommitResult(stResult);//gitStatus(prjName);// 冲突
            }
            else {
                error = git2_PushRemote(); // 合并后无冲突再次提交
                if (error < 0)
                    emit signal_Error(git2_ExecuteError(error));
            }
        }
    }
    git_commit_free(pParentCommit);
    git_tree_free(pNewTree);
    git_signature_free(me);

SHUTDOWN:
    git_repository_free(m_pGitRepository);
    git_libgit2_shutdown();
    if (!(error < 0)) {
        emit signal_CommitResult(stResult);
        qDebug() << "提交成功。";
     }
    return true;
}

int cred_acquire_cb_clone(git_cred **cred,
                          const char *url,
                          const char *username_from_url,
                          unsigned int allowed_types,
                          void *payload)
{
    int error = git_cred_userpass_plaintext_new(cred, GitApiHandle::getInstence()->m_sUserName.toUtf8().constData(),
                                                GitApiHandle::getInstence()->m_sPassword.toUtf8().constData());
    if (error < 0)
    {
        QString strError; ExecuteResult stResult;
        const git_error *e = giterr_last();
        strError = "Error: " + QString::number(error) + " / " + QString::number(e->klass) + " : " + e->message;
        stResult.content = strError;
        stResult.statusCode = QString::number(error);
        emit GitApiHandle::getInstence()->signal_Error(stResult);
    }
    return 0;
}
// 远程仓库克隆
bool GitApiHandle::gitClone(QString userName, QString password, QString remote, QString prjName)
{
    m_sUserName = userName;
    m_sPassword = password;
    m_sPrjName = prjName;
    OptionsRepository();
    ExecuteResult stResult; stResult.content = "克隆成功！"; stResult.statusCode = "0";
    int error = 0;
    git_repository* rep = nullptr;
    git_clone_options opt = GIT_CLONE_OPTIONS_INIT;
    git_checkout_options checkout_opts = GIT_CHECKOUT_OPTIONS_INIT;
    checkout_opts.checkout_strategy = GIT_CHECKOUT_SAFE;
    //opt.checkout_branch = "master";
    opt.checkout_opts = checkout_opts;
    opt.fetch_opts.callbacks.credentials = cred_acquire_cb_clone;
    qDebug() << "开始克隆操作。";
    git_libgit2_init();
    error = git_clone(&rep, remote.toUtf8().constData(), m_sLocalRep.toUtf8().constData(), &opt);
    if (error < 0) {
        emit signal_Error(git2_ExecuteError(error));
    }
    if (rep)
        git_repository_free(rep);
    git_libgit2_shutdown();
    if (!(error < 0)) {
        emit signal_CloneResult(stResult);
        qDebug() << "克隆成功。";
    }
    return true;
}

int guess_refish(git_annotated_commit **out, git_repository *repo, const char *ref)
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

int resolve_refish_checkout(git_annotated_commit **commit, git_repository *repo, const char *refish)
{
    git_reference *ref;
    git_object *obj;
    int err = 0;
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

// 版本回退
bool GitApiHandle::gitCheckout(QString userName, QString password, QString prjName, QString checkoutVersion, QStringList fileList)
{
    int error = 0;
    m_sUserName = userName;
    m_sPassword = password;
    m_sPrjName = prjName;
    OptionsRepository();
    git_commit* gitCommit = nullptr;
    git_repository* pRepository = nullptr;
    git_annotated_commit *checkout_target = nullptr;
    git_reference *ref = nullptr, *branch = nullptr;
    git_checkout_options opts = GIT_CHECKOUT_OPTIONS_INIT;
    qDebug() << "开始版本回退操作。";
    ExecuteResult stResult; stResult.content = "回退成功！"; stResult.statusCode = "0";
    git_libgit2_init();
    error = git_repository_open_ext(&pRepository, m_sLocalRep.toUtf8().constData(), 0, nullptr); // 初始化并打开存储库
    if ( error < 0) {
        emit signal_Error(git2_ExecuteError(error));
        goto SHUTDOWN;
    }
    error = git_repository_state(pRepository);
    if (error != GIT_REPOSITORY_STATE_NONE) {
        emit signal_Error(git2_ExecuteError(error));
        goto SHUTDOWN;
    }
    if ((error = resolve_refish_checkout(&checkout_target, pRepository, checkoutVersion.toUtf8().constData())) < 0 &&
        (error = guess_refish(&checkout_target, pRepository, checkoutVersion.toUtf8().constData())) < 0) {
        emit signal_Error(git2_ExecuteError(error));
        goto SHUTDOWN;
    }
    error = git_commit_lookup(&gitCommit, pRepository, git_annotated_commit_id(checkout_target));
    if (error < 0) {
        emit signal_Error(git2_ExecuteError(error));
        goto SHUTDOWN;
    }
    opts.checkout_strategy = GIT_CHECKOUT_FORCE;
    if (!fileList.isEmpty()) {
        opts.paths.strings = (char**)calloc(fileList.size(), sizeof(char *));
        if (opts.paths.strings == NULL)
            goto SHUTDOWN;
        for (int i = 0; i < fileList.size(); i++)
        { // 多个文件回退 or 若为空，则回退整个commit
            QString file = fileList.at(i);
            QByteArray str = file.toUtf8();
            opts.paths.strings[i] = str.data();
            qDebug() << "回退文件：" << file;
        }
        opts.paths.count = fileList.size();
    }

    error = git_checkout_tree(pRepository, (const git_object *)(gitCommit), &opts);
    if (error < 0) {
        emit signal_Error(git2_ExecuteError(error));
        goto SHUTDOWN;
    }
    // 将head以及index设置到切换的版本
    //if (git_annotated_commit_ref(checkout_target)) {
    //    const char *target_head;
    //
    //    if ((error = git_reference_lookup(&ref, pRepository, git_annotated_commit_ref(checkout_target))) < 0){
    //        emit signal_Error(git2_ExecuteError(error));
    //        goto SHUTDOWN;
    //    }
    //    if (git_reference_is_remote(ref)) {
    //        if ((error = git_branch_create_from_annotated(&branch, pRepository, checkoutVersion.toUtf8().constData(), checkout_target, 0)) < 0){
    //            emit signal_Error(git2_ExecuteError(error));
    //            goto SHUTDOWN;
    //        }
    //        target_head = git_reference_name(branch);
    //    } else {
    //        target_head = git_annotated_commit_ref(checkout_target);
    //    }
    //
    //    error = git_repository_set_head(pRepository, target_head);
    //    if (error < 0){
    //        emit signal_Error(git2_ExecuteError(error));
    //    }
    //} else {
    //    error = git_repository_set_head_detached_from_annotated(pRepository, checkout_target);
    //    if (error < 0){
    //        emit signal_Error(git2_ExecuteError(error));
    //    }
    //}
SHUTDOWN:
    if (checkout_target)
        git_annotated_commit_free(checkout_target);
    if (gitCommit)
        git_commit_free(gitCommit);
    if (pRepository)
        git_repository_free(pRepository);
    if (branch)
        git_reference_free(branch);
    if (ref)
        git_reference_free(ref);
    git_libgit2_shutdown();
    if (!( error < 0)) {
        emit signal_CheckoutResult(stResult);
        qDebug() << "版本回退已成功。";
    }
    return true;
}

/** Helper to format a git_time value like Git. */
QString GitApiHandle::print_time(const git_time *intime)
{
    char sign, out[32], buffer[256];
    struct tm *intm;
    int offset, hours, minutes;
    time_t t;

    offset = intime->offset;
    if (offset < 0) {
        sign = '-';
        offset = -offset;
    } else {
        sign = '+';
    }

    hours   = offset / 60;
    minutes = offset % 60;

    t = (time_t)intime->time + (intime->offset * 60);

    intm = gmtime(&t);
    strftime(out, 32, "%a %b %e %T %Y", intm);
    sprintf(buffer, "%s %c%02d%02d", out, sign, hours, minutes);
    return QString(buffer);
}

/** Helper to print a commit object. */
void GitApiHandle::print_commit(git_commit *commit)
{
    char buf[GIT_OID_HEXSZ + 1];
    int i, count;
    const git_signature *sig;
    const char *scan, *eol;
    CommitLog commitLog;

    git_oid_tostr(buf, sizeof(buf), git_commit_id(commit));
    commitLog.commit_sha = QString(buf);

    //printf("log size %d\n", (int)strlen(git_commit_message(commit)));

    if ((count = (int)git_commit_parentcount(commit)) > 1) {
        for (i = 0; i < count; ++i) {
            git_oid_tostr(buf, 8, git_commit_parent_id(commit, i));
            commitLog.mergeFrom.append(QString(buf));
        }
    }

    if ((sig = git_commit_author(commit)) != NULL) {
        commitLog.author = QString("%1 <%2>").arg(QString(sig->name)).arg(QString(sig->email));
        commitLog.dateTime = print_time(&sig->when);
    }

    for (scan = git_commit_message(commit); scan && *scan; ) {
        for (eol = scan; *eol && *eol != '\n'; ++eol) /* find eol */;
        commitLog.commit_info += QString(scan);
        scan = *eol ? eol + 1 : NULL;
    }
    m_listCommitlog.append(commitLog);
}
/** Helper to find how many files in a commit changed from its nth parent. */
int GitApiHandle::match_with_parent(git_commit *commit, int i, git_diff_options *opts)
{
    int error = 0;
    git_commit *parent;
    git_tree *a, *b;
    git_diff *diff;
    int ndeltas;

    error = git_commit_parent(&parent, commit, (size_t)i);
    if ( error < 0) {
        emit signal_Error(git2_ExecuteError(error));
    }
    error = git_commit_tree(&a, parent);
    if ( error < 0) {
        emit signal_Error(git2_ExecuteError(error));
    }
    error = git_commit_tree(&b, commit);
    if ( error < 0) {
        emit signal_Error(git2_ExecuteError(error));
    }
    error = git_diff_tree_to_tree(&diff, git_commit_owner(commit), a, b, opts);
    if ( error < 0) {
        emit signal_Error(git2_ExecuteError(error));
    }
    ndeltas = (int)git_diff_num_deltas(diff);

    git_diff_free(diff);
    git_tree_free(a);
    git_tree_free(b);
    git_commit_free(parent);

    return ndeltas > 0;
}

// 日志清单 -- 获取log的不完整、日期不对
bool GitApiHandle::gitLog(QString prjName, QString username, QString password, QStringList fileList)
{
    int error = 0;
    int i, parents;
    git_oid oid;
    git_commit *commit = NULL;
    git_revwalk *walker = nullptr;
    git_diff_options diffopts = GIT_DIFF_OPTIONS_INIT;
    git_pathspec *ps = NULL;
    m_listCommitlog.clear();

    m_sUserName = username;
    m_sPassword = password;
    m_sPrjName = prjName;
    OptionsRepository();
    char* paths[fileList.size()] = {nullptr};
    for (int i = 0; i < fileList.size(); i++)
    { // 多个文件提交日志 or 若为空，则查看所有提交日志
        QString file = fileList.at(i);
        paths[i] = file.toUtf8().data();
    }// 本地仓库最新的head
    ExecuteResult stResult; stResult.content = "查看日志成功！"; stResult.statusCode = "0";
    error = git2_InitAndOpen(); // 初始化并打开存储库
    if ( error < 0) {
        emit signal_Error(git2_ExecuteError(error));
        goto SHUTDOWN;
    }

    error = git_revwalk_new(&walker, m_pGitRepository);
    if ( error < 0) {// 新建遍历器
        emit signal_Error(git2_ExecuteError(error));
        goto SHUTDOWN;
    }
    error = git_revwalk_sorting(walker, GIT_SORT_TIME);
    if ( error < 0) {// 对遍历器中的提交记录排序方式设置
        emit signal_Error(git2_ExecuteError(error));
        goto SHUTDOWN;
    }
    error = git_revwalk_push_head(walker);
    if ( error < 0) {// 获取本地push的head最新历史
        emit signal_Error(git2_ExecuteError(error));
        goto SHUTDOWN;
    }

    diffopts.pathspec.strings = paths;
    diffopts.pathspec.count	  = fileList.size();
    if (diffopts.pathspec.count > 0) {
        error = git_pathspec_new(&ps, &diffopts.pathspec);
        if ( error < 0) {//
            emit signal_Error(git2_ExecuteError(error));
            goto SHUTDOWN;
        }
    }
    for (; !git_revwalk_next(&oid, walker); git_commit_free(commit)) {
        error = git_commit_lookup(&commit, m_pGitRepository, &oid);

        parents = (int)git_commit_parentcount(commit);
        if (diffopts.pathspec.count > 0) {
            int unmatched = parents;

            if (parents == 0) {
                git_tree *tree;
                error = git_commit_tree(&tree, commit);
                if (git_pathspec_match_tree(
                        NULL, tree, GIT_PATHSPEC_NO_MATCH_ERROR, ps) != 0)
                    unmatched = 1;
                git_tree_free(tree);
            } else if (parents == 1) {
                unmatched = match_with_parent(commit, 0, &diffopts) ? 0 : 1;
            } else {
                for (i = 0; i < parents; ++i) {
                    if (match_with_parent(commit, i, &diffopts))
                        unmatched--;
                }
            }

            if (unmatched > 0)
                continue;
        }
        print_commit(commit);
    }
    git_pathspec_free(ps);
    git_revwalk_free(walker);
SHUTDOWN:
    git_repository_free(m_pGitRepository);
    m_pGitRepository = nullptr;
    git_libgit2_shutdown();
    //if (!( error < 0))
    //    emit signal_LogResult(m_listCommitlog);
    return true;
}

// 获取单个文件的日志版本
bool GitApiHandle::catchSignalFileLogVersion(QString fileRelativePath)
{
    int error = 0;
    int i, parents;
    git_oid oid;
    git_commit *commit = NULL;
    git_revwalk *walker = nullptr;
    git_libgit2_init();
    git_repository* rep = nullptr;
    git_diff_options diffopts = GIT_DIFF_OPTIONS_INIT;
    git_pathspec *ps = NULL;
    m_listCommitlog.clear();
    char* paths[1] = {nullptr};
    paths[0] = fileRelativePath.toUtf8().data();
    error = git_repository_open(&rep, m_sLocalRep.toUtf8().constData());
    if (error < 0)
    {
        emit signal_Error(git2_ExecuteError(error));
        goto SHUTDOWN;
    }
    error = git_revwalk_new(&walker, rep);
    if ( error < 0) {// 新建遍历器
        emit signal_Error(git2_ExecuteError(error));
        goto SHUTDOWN;
    }
    error = git_revwalk_sorting(walker, GIT_SORT_TIME);
    if ( error < 0) {// 对遍历器中的提交记录排序方式设置
        emit signal_Error(git2_ExecuteError(error));
        goto SHUTDOWN;
    }
    error = git_revwalk_push_head(walker);
    if ( error < 0) {// 获取本地push的head最新历史
        emit signal_Error(git2_ExecuteError(error));
        goto SHUTDOWN;
    }

    diffopts.pathspec.strings = paths;
    diffopts.pathspec.count	  = 1;
    if (diffopts.pathspec.count > 0) {
        error = git_pathspec_new(&ps, &diffopts.pathspec);
        if ( error < 0) {//
            emit signal_Error(git2_ExecuteError(error));
            goto SHUTDOWN;
        }
    }
    for (; !git_revwalk_next(&oid, walker); git_commit_free(commit)) {
        error = git_commit_lookup(&commit, rep, &oid);

        parents = (int)git_commit_parentcount(commit);
        if (diffopts.pathspec.count > 0) {
            int unmatched = parents;

            if (parents == 0) {
                git_tree *tree;
                error = git_commit_tree(&tree, commit);
                if (git_pathspec_match_tree(
                        NULL, tree, GIT_PATHSPEC_NO_MATCH_ERROR, ps) != 0)
                    unmatched = 1;
                git_tree_free(tree);
            } else if (parents == 1) {
                unmatched = match_with_parent(commit, 0, &diffopts) ? 0 : 1;
            } else {
                for (i = 0; i < parents; ++i) {
                    if (match_with_parent(commit, i, &diffopts))
                        unmatched--;
                }
            }

            if (unmatched > 0)
                continue;
        }
        print_commit(commit);
    }
    git_pathspec_free(ps);
    git_revwalk_free(walker);
SHUTDOWN:
    git_repository_free(rep);
    git_libgit2_shutdown();
    if (!m_listCommitlog.isEmpty()) {
        return true;
    }
    else {
        return false;
    }
}

// 拉取远程仓库 并 合并
bool GitApiHandle::gitPull(QString prjName, QString username, QString password, QString commitEmail, QStringList fileList, bool isWeb)
{
    int error = 0;bool isConflict = false;
    git_fetch_options fetch_opts = GIT_FETCH_OPTIONS_INIT;
    git_merge_options merge_opt = GIT_MERGE_OPTIONS_INIT;
    git_checkout_options checkout_opt = GIT_CHECKOUT_OPTIONS_INIT;
    git_repository *repo = nullptr;
    git_index* index = nullptr;
    git_index_conflict_iterator* conflict_iterator = nullptr;
    git_commit* their_commit = nullptr;
    git_commit* our_commit = nullptr;
    git_tree* new_tree = nullptr;
    git_signature* me = nullptr;
    git_remote* remote = nullptr;
    git_reference* origin_master = nullptr;
    git_reference* local_master = nullptr;
    const git_annotated_commit* their_head[10];
    git_oid new_tree_id;
    git_oid commit_id;
    //char *refspec = "refs/heads/master";
    //const git_strarray refspecs = { &refspec, 1 };
    m_sPrjName = prjName;
    m_sUserName = username;
    m_sPassword = password;
    ExecuteResult stResult;stResult.content = "拉取成功！"; stResult.statusCode = "0";
    OptionsRepository();
    QByteArray ba_repoLocal = m_sLocalRep.toUtf8();
    git_libgit2_init();
    error = git_repository_open(&repo, ba_repoLocal.constData());
    if (error < 0) {
        goto SHUTDOWN;
    }
    // get a remote
    error = git_remote_lookup(&remote, repo, "origin");//
    if (error < 0) {
            goto SHUTDOWN;
    }
    fetch_opts.callbacks.credentials = cred_acquire_cb_clone;
    error = git_remote_fetch(remote, nullptr, &fetch_opts, "fetch");//refspecs
    if (error < 0) {
        goto SHUTDOWN;
    }
    error = git_branch_lookup(&origin_master, repo, "origin/master", GIT_BRANCH_REMOTE);
    if (error < 0) {
        goto SHUTDOWN;
    }
    error = git_branch_lookup(&local_master, repo, "master", GIT_BRANCH_LOCAL);
    if (error < 0) {
        goto SHUTDOWN;
    }

    error = git_repository_set_head(repo, git_reference_name(local_master));
    if (error < 0) {
        goto SHUTDOWN;
    }
    error = git_annotated_commit_from_ref((git_annotated_commit **)&their_head[0], repo, origin_master);
    if (error < 0) {
        goto SHUTDOWN;
    }
    merge_opt.flags = 0;
    merge_opt.file_flags = GIT_MERGE_FILE_STYLE_DIFF3;
    checkout_opt.checkout_strategy = GIT_CHECKOUT_FORCE|GIT_CHECKOUT_ALLOW_CONFLICTS;

    error = git_merge(repo, their_head, 1, &merge_opt, &checkout_opt);
    if (error < 0)
    {
        if (error != GIT_EMERGECONFLICT){
            goto SHUTDOWN;
        }
    }

    // reslove conflicts
    error = git_repository_index(&index, repo);
    if (error < 0) {
        goto SHUTDOWN;
    }
    if (git_index_has_conflicts(index))
    {
        const git_index_entry* ancestor_out = nullptr;
        const git_index_entry* our_out = nullptr;
        const git_index_entry* their_out = nullptr;

        git_index_conflict_iterator_new(&conflict_iterator, index);

        while (git_index_conflict_next(&ancestor_out, &our_out, &their_out, conflict_iterator) != GIT_ITEROVER)
        {
            isConflict = true;
            if (ancestor_out){
                stResult.conflictFileList.append(QString(ancestor_out->path));
                m_confFileList.append(QString(ancestor_out->path));
            }
            else if (our_out->path) {
                stResult.conflictFileList.append(QString(our_out->path));
                m_confFileList.append(QString(our_out->path));
            }
            else if (their_out->path) {
                stResult.conflictFileList.append(QString(their_out->path));
                m_confFileList.append(QString(their_out->path));
            }
        }

        // git checkout --theirs <file>
        git_checkout_options opt = GIT_CHECKOUT_OPTIONS_INIT;
        opt.checkout_strategy |= GIT_CHECKOUT_USE_OURS;
        git_checkout_index(repo, index, &opt);

        git_index_conflict_iterator_free(conflict_iterator);
    }
    git_commit_lookup(&their_commit, repo, git_reference_target(origin_master));
    git_commit_lookup(&our_commit, repo, git_reference_target(local_master));
    git_signature_now(&me, username.toUtf8().constData(), commitEmail.toUtf8().constData());
    // add and commit 添加工程下的变动到索引（除新建的）
    if (fileList.isEmpty()) {
        error = git_index_update_all(index, nullptr, nullptr, nullptr); //可能会导致仓库下所有的变动，新增成为提交
        if (error < 0) {
            goto SHUTDOWN;
        }
        error = git_index_write(index);
        if (error < 0) {
            goto SHUTDOWN;
        }
        error = git_index_write_tree(&new_tree_id, index);
        if (error < 0) {
            goto SHUTDOWN;
        }
        error = git_tree_lookup(&new_tree, repo, &new_tree_id);
        if (error < 0) {
            goto SHUTDOWN;
        }
        error = git_commit_create_v(&commit_id, repo, git_reference_name(local_master), me, me, "UTF-8", "Merge branch origin master", new_tree, 2, our_commit, their_commit);
    }
    else {
        m_pGitRepository = repo;
        error = git2_AddToTree(&new_tree, fileList); // 添加当前 需同步的文件到索引
        if (error < 0) {
            goto SHUTDOWN;
        }
        QString tmp = "merge and " + m_scommitLog;
        error = git_commit_create_v(&commit_id, repo, git_reference_name(local_master), me, me, "UTF-8", tmp.toUtf8().constData(), new_tree, 2, our_commit, their_commit);
    }
SHUTDOWN:
    if (error < 0) {
        emit signal_Error(git2_ExecuteError(error));
    }
    git_repository_state_cleanup(repo);
    git_index_free(index);
    git_tree_free(new_tree);
    git_commit_free(their_commit);
    git_commit_free(our_commit);
    git_remote_free(remote);
    git_reference_free(origin_master);
    git_reference_free(local_master);
    git_repository_free(repo);
    git_libgit2_shutdown();
    if (isConflict) {
        stResult.content = "文件有冲突，请解决！";
        stResult.statusCode = "-1";
        emit signal_CommitResult(stResult);
        return false;
    }
    else if (!isConflict && error >= 0) {
        emit signal_CommitResult(stResult);
    }
    return true;
}


// 拉取远程仓库 并 合并
bool GitApiHandle::gitPull_1(QString prjName, QString username, QString password, QString commitEmail, QStringList fileList, bool isWeb)
{
    int error = 0;bool isConflict = false;
    git_fetch_options fetch_opts = GIT_FETCH_OPTIONS_INIT;
    git_merge_options merge_opt = GIT_MERGE_OPTIONS_INIT;
    git_checkout_options checkout_opt = GIT_CHECKOUT_OPTIONS_INIT;
    git_repository *repo = nullptr;
    git_index* index = nullptr;
    git_index_conflict_iterator* conflict_iterator = nullptr;
    git_commit* their_commit = nullptr;
    git_commit* our_commit = nullptr;
    git_tree* new_tree = nullptr;
    git_signature* me = nullptr;
    git_remote* remote = nullptr;
    git_reference* origin_master = nullptr;
    git_reference* local_master = nullptr;
    const git_annotated_commit* their_head[10];
    git_oid new_tree_id;
    git_oid commit_id;
    //char *refspec = "refs/heads/master";
    //const git_strarray refspecs = { &refspec, 1 };
    m_sPrjName = prjName;
    m_sUserName = username;
    m_sPassword = password;
    ExecuteResult stResult;stResult.content = "拉取成功！"; stResult.statusCode = "0";
    OptionsRepository();
    QByteArray ba_repoLocal = m_sLocalRep.toUtf8();
    git_libgit2_init();
    error = git_repository_open(&repo, ba_repoLocal.constData());
    if (error < 0) {
        goto SHUTDOWN;
    }
    // get a remote
    error = git_remote_lookup(&remote, repo, "origin");//
    if (error < 0) {
            goto SHUTDOWN;
    }
    fetch_opts.callbacks.credentials = cred_acquire_cb_clone;
    error = git_remote_fetch(remote, nullptr, &fetch_opts, "fetch");//refspecs
    if (error < 0) {
        goto SHUTDOWN;
    }
    error = git_branch_lookup(&origin_master, repo, "origin/master", GIT_BRANCH_REMOTE);
    if (error < 0) {
        goto SHUTDOWN;
    }
    error = git_branch_lookup(&local_master, repo, "master", GIT_BRANCH_LOCAL);
    if (error < 0) {
        goto SHUTDOWN;
    }

    error = git_repository_set_head(repo, git_reference_name(local_master));
    if (error < 0) {
        goto SHUTDOWN;
    }
    error = git_annotated_commit_from_ref((git_annotated_commit **)&their_head[0], repo, origin_master);
    if (error < 0) {
        goto SHUTDOWN;
    }
    merge_opt.flags = 0;
    merge_opt.file_flags = GIT_MERGE_FILE_STYLE_DIFF3;
    //checkout_opt.checkout_strategy = GIT_CHECKOUT_FORCE|GIT_CHECKOUT_ALLOW_CONFLICTS;
    checkout_opt.checkout_strategy =GIT_CHECKOUT_FORCE ;//| GIT_CHECKOUT_USE_THEIRS;

    error = git_merge(repo, their_head, 1, &merge_opt, &checkout_opt);
    if (error < 0)
    {
        if (error != GIT_EMERGECONFLICT){
            goto SHUTDOWN;
        }
    }

    // reslove conflicts
    error = git_repository_index(&index, repo);
    if (error < 0) {
        goto SHUTDOWN;
    }
    if (git_index_has_conflicts(index))
    {
        const git_index_entry* ancestor_out = nullptr;
        const git_index_entry* our_out = nullptr;
        const git_index_entry* their_out = nullptr;

        git_index_conflict_iterator_new(&conflict_iterator, index);

        while (git_index_conflict_next(&ancestor_out, &our_out, &their_out, conflict_iterator) != GIT_ITEROVER)
        {
            isConflict = true;
            if (ancestor_out){
                stResult.conflictFileList.append(QString(ancestor_out->path));
                m_confFileList.append(QString(ancestor_out->path));
            }
            else if (our_out->path) {
                stResult.conflictFileList.append(QString(our_out->path));
                m_confFileList.append(QString(our_out->path));
            }
            else if (their_out->path) {
                stResult.conflictFileList.append(QString(their_out->path));
                m_confFileList.append(QString(their_out->path));
            }
        }

        // git checkout --theirs <file>
        git_checkout_options opt = GIT_CHECKOUT_OPTIONS_INIT;
        opt.checkout_strategy |= GIT_CHECKOUT_USE_OURS;
        git_checkout_index(repo, index, &opt);

        git_index_conflict_iterator_free(conflict_iterator);
    }
    git_commit_lookup(&their_commit, repo, git_reference_target(origin_master));
    git_commit_lookup(&our_commit, repo, git_reference_target(local_master));
    git_signature_now(&me, username.toUtf8().constData(), commitEmail.toUtf8().constData());
    // add and commit 添加工程下的变动到索引（除新建的）
    if (fileList.isEmpty()) {
        error = git_index_update_all(index, nullptr, nullptr, nullptr); //可能会导致仓库下所有的变动，新增成为提交
        if (error < 0) {
            goto SHUTDOWN;
        }
        error = git_index_write(index);
        if (error < 0) {
            goto SHUTDOWN;
        }
        error = git_index_write_tree(&new_tree_id, index);
        if (error < 0) {
            goto SHUTDOWN;
        }
        error = git_tree_lookup(&new_tree, repo, &new_tree_id);
        if (error < 0) {
            goto SHUTDOWN;
        }
        error = git_commit_create_v(&commit_id, repo, git_reference_name(local_master), me, me, "UTF-8", "Merge branch origin master", new_tree, 2, our_commit, their_commit);
    }
    else {
        m_pGitRepository = repo;
        error = git2_AddToTree(&new_tree, fileList); // 添加当前 需同步的文件到索引
        if (error < 0) {
            goto SHUTDOWN;
        }
        QString tmp = "merge and " + m_scommitLog;
        error = git_commit_create_v(&commit_id, repo, git_reference_name(local_master), me, me, "UTF-8", tmp.toUtf8().constData(), new_tree, 2, our_commit, their_commit);
    }
SHUTDOWN:
    if (error < 0) {
        emit signal_Error(git2_ExecuteError(error));
    }
    git_repository_state_cleanup(repo);
    git_index_free(index);
    git_tree_free(new_tree);
    git_commit_free(their_commit);
    git_commit_free(our_commit);
    git_remote_free(remote);
    git_reference_free(origin_master);
    git_reference_free(local_master);
    git_repository_free(repo);
    git_libgit2_shutdown();
    if (isConflict) {
        stResult.content = "文件有冲突，请解决！";
        stResult.statusCode = "-1";
        emit signal_CommitResult(stResult);
        return false;
    }
    else if (!isConflict && error >= 0) {
        emit signal_CommitResult(stResult);
    }
    return true;
}
