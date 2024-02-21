#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <QtGlobal>

typedef unsigned int uint;

#define REG_OK  "register ok"
#define REG_FAIL  "register fail"
#define LOG_OK "login in ok"
#define LOG_FAIL "login in fail"
#define QUERY_OK "query ok"
#define QUERY_FAIL "query fail"
#define CREATE_OK "create ok"
#define CREATE_FAIL "create fail"
#define RENAME_OK "rename ok"
#define RENAME_FAIL "rename fail"
#define REMOVE_OK "remove ok"
#define REMOVE_FAIL "remove fail"
#define UPLOAD_OK "upload ok"
#define UPLOAD_FAIL "upload fail"
#define DOWNLOAD_OK "download ok"
#define DOWNLOAD_FIAL "download fail"
#define SEND_PRIVATE_MSG_OK "private send ok"
#define SEND_PRIVATE_MSG_FAIL "private send fail"
#define ADD_FRIEND_QUEST_OK "add friend quest ok"
#define ADD_FRIEND_QUEST_FAIL "add friend quest fail"
#define ADD_FRIEND_ACCEPT_OK "add friend accept ok"
#define ADD_FRIEND_ACCEPT_FAIL "add friend accept fail"
#define ADD_FRIEND_REFUSE_OK "add friend refuse ok"
#define ADD_FRIEND_REFUSE_FAIL "add friend refuse fail"
#define SHARE_FILE_OK "share file ok"
#define SHARE_FILE_FAIL "share file fail"
#define SHARE_REFUSE_OK "share refuse ok"
#define SHARE_REFUSE_FAIL "share refuse fail"

enum ENUM_MSG_TYPE{
    ENUM_MSG_TYPE_MIN = 0,

    ENUM_MSG_TYPE_REGISTER_REQUEST,             //注册
    ENUM_MSG_TYPE_REGISTER_RESPOND,

    ENUM_MSG_TYPE_LOGIN_IN_REQUEST,             //登陆
    ENUM_MSG_TYPE_LOGIN_IN_RESPOND,

    ENUM_MSG_TYPE_QUERY_USER_REQUEST,           //查询用户
    ENUM_MSG_TYPE_QUERY_USER_RESPOND,

    ENUM_MSG_TYPE_REFRESH_DIR_REQUEST,          //刷新当前目录文件
    ENUM_MSG_TYPE_REFRESH_DIR_RESPOND,

    ENUM_MSG_TYPE_CREATE_DIR_REQUEST,           //创建新文件夹
    ENUM_MSG_TYPE_CREATE_DIR_RESPOND,

    ENUM_MSG_TYPE_RENAME_DIR_REQUEST,           //重命名
    ENUM_MSG_TYPE_RENAME_DIR_RESPOND,

    ENUM_MSG_TYPE_REMOVE_FILE_REQUEST,          //删除文件请求
    ENUM_MSG_TYPE_REMOVE_DIR_REQUEST,           //删除目录请求
    ENUM_MSG_TYPE_REMOVE_RESPOND,

    ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST,          //上传文件
    ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND,

    ENUM_MSG_TYPE_DOWNLOAD_FILE_REQUEST,        //下载文件
    ENUM_MSG_TYPE_DOWNLOAD_FILE_RESPOND,

    ENUM_MSG_TYPE_PRIVATE_MESSAGE_REQUEST,      //私聊消息
    ENUM_MSG_TYPE_PRIVATE_MESSAGE_RESPOND,
    ENUM_MSG_TYPE_PRIVATE_MESSAGE_TRANSPOND,    //消息转发

    ENUM_MSG_TYPE_ADD_FRIEND_REQUEST,           //添加好友
    ENUM_MSG_TYPE_ADD_FRIEND_RESPOND,

    ENUM_MSG_TYPE_ADD_FRIEND_ACCEPT,            //添加好友接受消息
    ENUM_MSG_TYPE_ADD_FRIEND_REFUSE,            //添加好友拒绝消息
    ENUM_MSG_TYPE_ADD_FRIEND_COMMON_RESPOND,

    ENUM_MSG_TYPE_QUERY_ALL_ADD_FRIEND_REQUEST,   //查询所有添加好友请求
    ENUM_MSG_TYPE_QUERY_ALL_ADD_FRIEND_RESPOND,

    ENUM_MSG_TYPE_QUERY_ALL_FRIEND_REQUEST,     //查询所有好友
    ENUM_MSG_TYPE_QUERY_ALL_FRIEND_RESPOND,

    ENUM_MSG_TYPE_SHARE_FILE_REQUEST,           //分享文件
    ENUM_MSG_TYPE_SHARE_FILE_RESPOND,
    ENUM_MSG_TYPE_SHARE_REFUSE_REQUEST,         //拒绝接收分享
    ENUM_MSG_TYPE_SHARE_REFUSE_RESPOND,

    ENUM_MSG_TYPE_QUERY_ALL_SHARE_FILE_REQUEST, //查询所有分享请求
    ENUM_MSG_TYPE_QUERY_ALL_SHARE_FILE_RESPOND,

    ENUM_MSG_TYPE_MAX = 0X00FFFFFF
};

//协议传输单元
typedef struct PDU{
    uint uiPDULen;          //总的消息大小
    uint uiMsgType;         //消息类型
    char caData[256];        //消息数据
    uint uiMsgLen;          //实际消息长度
    int caMsg[];            //实际消息

}PDU;

//文件信息结构体
typedef struct FileInfo{
    char fileName[128];
    char fileChangeTime[32];
    char fileType[16];
    qint64 fileSize;
    int isFile;

}FileInfo;

//分享信息结构体
typedef struct ShareFileInfo{
    char fileName[128];
    char sender[128];
    qint64 fileSize;
}ShareFileInfo;

typedef struct FriendInfo{
    char friendName[128];
    int isOnline;
}FriendInfo;



PDU *mk_pdu(uint uiMsgLen);

#endif // PROTOCOL_H
