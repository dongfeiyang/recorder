/* 该文件用于描述访问数据库的接口声明 */
#ifndef _OPERATE_DB_H_
#define _OPERATE_DB_H_
#include <mysql.h>
#include "status.h"

/*
 * 访问数据库的字符串的最大长度
 */
#define QUERY_MAX_SIZE 1024

/*
 * 设置访问数据库参数
 * _localhost -- 访问数据库的主机名
 * _userName -- 访问数据库的用户名
 * _password -- 对应的密码
 * _databaseName -- 要操作的数据库名字
 * 返回结果状态值，如果没有异常返回OK；否则返回ERROR
 */
STATUS
InitDbParms( char const *_localhost, char const *_userName, char const *_password, char const *_databaseName );

/*
 * 操作数据库
 * commandString -- 存放SQL操作命令
 * 如果操作成功返回OK；否则返回ERROR
 */
STATUS
UpdateDatabase( char const *commandString );

/*
 * 查询数据库
 * commandString -- 查询sql命令
 * resultp -- 指向存放查询结果的双指针(如果传递给该参数NULL，则函数不会返回查询结果)
 * 返回结果状态值，如果操作成功返回OK；否则返回ERROR
 */
STATUS
QueryDatabase( char const *commandString, MYSQL_RES **resultpp );
#endif
