#include "jsondataparse.h"

JsonDataParse::JsonDataParse()
{

}

JsonDataParse* JsonDataParse::m_pInstence = nullptr;

JsonDataParse* JsonDataParse::getInstence()
{
    if(m_pInstence == nullptr)
    {
        m_pInstence = new JsonDataParse;
    }
    return m_pInstence;
}

bool JsonDataParse::startParseJson(QString &jsonContent)
{
    clearStJsonData();
    QJsonParseError jsonError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonContent.toUtf8(), &jsonError);
    if (jsonError.error == QJsonParseError::NoError)
    {
        if (jsonDoc.isObject())
        {
            QJsonObject jsonObj = jsonDoc.object();
            QJsonArray jsonArray;
            if(jsonObj.contains("_ProjectName") && jsonObj.value("_ProjectName").isString())
            {// 工程名
                qDebug() << jsonObj.value("_ProjectName").toString();
                m_stJsonData._mapJsonData.insert("_ProjectName", jsonObj.value("_ProjectName").toString());
            }
            if(jsonObj.contains("_UserName") && jsonObj.value("_UserName").isString())
            {// 用户名
                qDebug() << jsonObj.value("_UserName").toString();
                m_stJsonData._mapJsonData.insert("_UserName", jsonObj.value("_UserName").toString());
            }
            if(jsonObj.contains("_Password") && jsonObj.value("_Password").isString())
            {// 密码
                qDebug() << jsonObj.value("_Password").toString();
                m_stJsonData._mapJsonData.insert("_Password", jsonObj.value("_Password").toString());
            }
            if(jsonObj.contains("_CommitEmail") && jsonObj.value("_CommitEmail").isString())
            {// 提交者邮箱
                qDebug() << jsonObj.value("_CommitEmail").toString();
                m_stJsonData._mapJsonData.insert("_CommitEmail", jsonObj.value("_CommitEmail").toString());
            }
            if(jsonObj.contains("_CommitContent") && jsonObj.value("_CommitContent").isString())
            {// 提交说明内容
                qDebug() << jsonObj.value("_CommitContent").toString();
                m_stJsonData._mapJsonData.insert("_CommitContent", jsonObj.value("_CommitContent").toString());
            }
            if(jsonObj.contains("_CloneToLocal") && jsonObj.value("_CloneToLocal").isString())
            {// 克隆的pc机位置
                qDebug() << jsonObj.value("_CloneToLocal").toString();
                m_stJsonData._mapJsonData.insert("_CloneToLocal", jsonObj.value("_CloneToLocal").toString());
            }
            if(jsonObj.contains("_CloneFromRemote") && jsonObj.value("_CloneFromRemote").isString())
            {// 克隆的远端地址
                qDebug() << jsonObj.value("_CloneFromRemote").toString();
                m_stJsonData._mapJsonData.insert("_CloneFromRemote", jsonObj.value("_CloneFromRemote").toString());
            }
            if(jsonObj.contains("_CheckoutVersion") && jsonObj.value("_CheckoutVersion").isString())
            {// 回退版本
                qDebug() << jsonObj.value("_CheckoutVersion").toString();
                m_stJsonData._mapJsonData.insert("_CheckoutVersion", jsonObj.value("_CheckoutVersion").toString());
            }
            if(jsonObj.contains("_FilePostUrl") && jsonObj.value("_FilePostUrl").isString())
            {// 文件上传的服务地址
                qDebug() << jsonObj.value("_FilePostUrl").toString();
                m_stJsonData._mapJsonData.insert("_FilePostUrl", jsonObj.value("_FilePostUrl").toString());
            }
            if(jsonObj.contains("_FileList") && jsonObj.value("_FileList").isArray())
            {// 文件列表
                jsonArray = jsonObj.value("_FileList").toArray();
                for (int i = 0; i < jsonArray.size(); ++i)
                {
                    QJsonObject jsonObjSon1 = jsonArray[i].toObject();
                    if(jsonObjSon1.contains("_File") && jsonObjSon1.value("_File").isString())
                    {// 文件
                        qDebug() << jsonObjSon1.value("_File").toString();
                        m_stJsonData._FileList.append(jsonObjSon1.value("_File").toString());
                    }
                }
            }
        }
    }
    return true;
}

void JsonDataParse::clearStJsonData()
{
    m_stJsonData._FileList.clear();
    m_stJsonData._mapJsonData.clear();
}

QString JsonDataParse::getValue(QString mapKey)
{
    if(m_stJsonData._mapJsonData.contains(mapKey))
        return m_stJsonData._mapJsonData.value(mapKey);
    else
        return "";
}

QStringList JsonDataParse::getFileList()
{
    return m_stJsonData._FileList;
}
QJsonArray buildJsonDataTree(FileStatus fileTreeData)
{
    QJsonArray jsonArray;
    foreach (FileStatus fileInfo, fileTreeData.children) {
        QJsonObject jsonArrayObj;
        jsonArrayObj.insert("filename", fileInfo.file);
        jsonArrayObj.insert("grade", fileInfo.grade);
        jsonArrayObj.insert("deptid", fileInfo.deptid);
        jsonArrayObj.insert("belongto", fileInfo.belongto);
        jsonArrayObj.insert("update_user", fileInfo.user);
        jsonArrayObj.insert("update_date", fileInfo.date);
        jsonArrayObj.insert("ver_desc", fileInfo.version);
        jsonArrayObj.insert("file_size", fileInfo.size);
        jsonArrayObj.insert("relativepath", fileInfo.relativepath);
        if(!fileInfo.children.isEmpty()) {
            jsonArrayObj.insert("children", QJsonValue(buildJsonDataTree(fileInfo)));
            jsonArrayObj.insert("status", fileInfo.status);
        }
        else {
            jsonArrayObj.insert("children", "");
            if (fileInfo.status.isEmpty())
                jsonArrayObj.insert("status", "正常");
            else
                jsonArrayObj.insert("status", fileInfo.status);
        }
        jsonArray.push_back(QJsonValue(jsonArrayObj));
    }
    return jsonArray;
}
/* {
    "data": [{
        "deptid": "",
        "memo": "",
        "isdeprelated": "",
        "sequence": -1,
        "children": [{
            "deptid": "",
            "memo": "",
            "isdeprelated": "",
            "sequence": 1,
            "children": [],
            "grade": 1,
            "name": "123",
            "belongto": "Z0000",
            "typeid": "",
            "leve": 1,
            "id": "123",
            "text": "123",
            "iconcls": "",
            "open": true
        }],
        "grade": 0,
        "name": "相关领导",
        "belongto": "",
        "typeid": "",
        "leve": 0,
        "id": "Z0000",
        "text": "相关领导",
        "iconcls": "",
        "open": true
    }, {
        "deptid": null,
        "memo": "",
        "isdeprelated": null,
        "sequence": 0,
        "children": [],
        "grade": 0,
        "name": "超级管理员",
        "belongto": "",
        "typeid": null,
        "leve": 0,
        "id": "Z0001",
        "text": "超级管理员",
        "iconcls": null,
        "open": true
    }, {
        "deptid": null,
        "memo": "",
        "isdeprelated": null,
        "sequence": 1,
        "children": [],
        "grade": 0,
        "name": "主任设计师",
        "belongto": "",
        "typeid": null,
        "leve": 0,
        "id": "Z0002",
        "text": "主任设计师",
        "iconcls": null,
        "open": true
    }, {
        "deptid": null,
        "memo": "",
        "isdeprelated": null,
        "sequence": 2,
        "children": [],
        "grade": 0,
        "name": "软件项目管理员",
        "belongto": "",
        "typeid": null,
        "leve": 0,
        "id": "Z0003",
        "text": "软件项目管理员",
        "iconcls": null,
        "open": true
    }, {
        "deptid": "",
        "memo": "",
        "isdeprelated": "",
        "sequence": 2,
        "children": [],
        "grade": 0,
        "name": "目录树角色",
        "belongto": "",
        "typeid": "",
        "leve": 0,
        "id": "Catalog",
        "text": "目录树角色",
        "iconcls": "",
        "open": true
    }, {
        "deptid": "",
        "memo": "",
        "isdeprelated": "",
        "sequence": 3,
        "children": [],
        "grade": 0,
        "name": "工程责任人",
        "belongto": "",
        "typeid": "",
        "leve": 0,
        "id": "Z0004",
        "text": "工程责任人",
        "iconcls": "",
        "open": true
    }, {
        "deptid": "",
        "memo": "",
        "isdeprelated": "",
        "sequence": 4,
        "children": [],
        "grade": 0,
        "name": "产品责任人",
        "belongto": "",
        "typeid": "",
        "leve": 0,
        "id": "Z0005",
        "text": "产品责任人",
        "iconcls": "",
        "open": true
    }, {
        "deptid": "",
        "memo": "",
        "isdeprelated": "",
        "sequence": 5,
        "children": [],
        "grade": 0,
        "name": "集成人员",
        "belongto": "",
        "typeid": "",
        "leve": 0,
        "id": "Z0006",
        "text": "集成人员",
        "iconcls": "",
        "open": true
    }],
    "retmsg": "查询成功",
    "retcode": 0
}
*/
QString JsonDataParse::generateFileStatusJsonString(FileStatus fileList)
{
    QString jsonString = "";
    QJsonObject jsonBodyObj;
    QJsonArray jsonArray = buildJsonDataTree(fileList);
    jsonBodyObj.insert("data", QJsonValue(jsonArray));
    jsonBodyObj.insert("retmsg", "获取成功");
    jsonBodyObj.insert("retcode","0");
    QJsonDocument jsonDoc;
    jsonDoc.setObject(jsonBodyObj);
    jsonString= jsonDoc.toJson(QJsonDocument::Compact);
    return jsonString;
}

QJsonArray buildConflictFileData(QStringList list)
{
    QJsonArray jsonArray;
    foreach (QString fileInfo, list) {
        QJsonObject jsonArrayObj;
        jsonArrayObj.insert("filename", fileInfo);
        jsonArray.push_back(QJsonValue(jsonArrayObj));
    }
    return jsonArray;
}

QString JsonDataParse::generateNormalResultJsonString(ExecuteResult stResult)
{
    QString jsonString = "";
    QJsonObject jsonBodyObj;
    if (!stResult.conflictFileList.isEmpty()) {
        QJsonArray jsonArray = buildConflictFileData(stResult.conflictFileList);
        jsonBodyObj.insert("data", QJsonValue(jsonArray));
    }
    jsonBodyObj.insert("retmsg", stResult.content);
    jsonBodyObj.insert("retcode", stResult.statusCode);
    QJsonDocument jsonDoc;
    jsonDoc.setObject(jsonBodyObj);
    jsonString= jsonDoc.toJson(QJsonDocument::Compact);
    return jsonString;
}

QJsonArray buildLogJsonData(QList<CommitLog> loglist)
{
    QJsonArray jsonArray;
    foreach (CommitLog loginfo, loglist) {
        QJsonObject jsonArrayObj;
        jsonArrayObj.insert("author", loginfo.author);
        jsonArrayObj.insert("datetime", loginfo.dateTime);
        jsonArrayObj.insert("commit_sha", loginfo.commit_sha);
        jsonArrayObj.insert("commit_info", loginfo.commit_info);
        if(!loginfo.mergeFrom.isEmpty()) {
            QJsonArray mergeArray;
            foreach (QString merge, loginfo.mergeFrom) {
                QJsonObject mergeObj;
                mergeObj.insert("mergefromparent", merge);
                mergeArray.push_back(QJsonValue(mergeObj));
            }
            jsonArrayObj.insert("mergefrom", QJsonValue(mergeArray));
        }
        else {
            jsonArrayObj.insert("mergefrom", "");
        }
        jsonArray.push_back(QJsonValue(jsonArrayObj));
    }
    return jsonArray;
}

QString JsonDataParse::generateCommitLogJsonString(QList<CommitLog> loglist)
{
    QString jsonString = "";
    QJsonObject jsonBodyObj;
    QJsonArray jsonArray = buildLogJsonData(loglist);
    jsonBodyObj.insert("data", QJsonValue(jsonArray));
    jsonBodyObj.insert("retmsg", "获取成功");
    jsonBodyObj.insert("retcode","0");
    QJsonDocument jsonDoc;
    jsonDoc.setObject(jsonBodyObj);
    jsonString= jsonDoc.toJson(QJsonDocument::Compact);
    return jsonString;
}

QString JsonDataParse::generateUploadFileContentJsonString(FileData fileData)
{
    QString jsonString = "";
    QJsonObject jsonBodyObj;
    jsonBodyObj.insert("file", fileData.file);
    jsonBodyObj.insert("size", fileData.size);
    jsonBodyObj.insert("content", fileData.content);
    jsonBodyObj.insert("retmsg", fileData.retmsg);
    jsonBodyObj.insert("retcode",fileData.retcode);
    QJsonDocument jsonDoc;
    jsonDoc.setObject(jsonBodyObj);
    jsonString= jsonDoc.toJson(QJsonDocument::Compact);
    return jsonString;
}
