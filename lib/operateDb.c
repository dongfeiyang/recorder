#include <stdio.h>
#include <stdlib.h>
#include <mysql.h>
#include "operateDb.h"
#include "status.h"

/*
 * 内部数据
 */
static char const *localhost = NULL; //主机名
static char const *userName = NULL;  //mysql用户名
static char const *password = NULL;  //它的密码
static char const *databaseName = NULL; //所要操作的数据库

/*
 * 外部接口
 *
 *
 * 设置访问数据库参数
 * _localhost -- 访问数据库的主机名
 * _userName -- 访问数据库的用户名
 * _password -- 对应的密码
 * _databaseName -- 要操作的数据库名字
 * 返回结果状态值，如果没有异常返回OK；否则返回ERROR
 */
STATUS
InitDbParms( char const *_localhost, char const *_userName, char const *_password, char const *_databaseName )
{
        if(_localhost == NULL) //检查参数是否有效
        {
                fprintf(stdout, "传递给函数InitDbParms的参数_localhost无效。\n");
                return ERROR;
        }
        if(_userName == NULL)
 {
                fprintf(stdout, "传递给函数InitDbParms的参数_userName无效。\n");
                return ERROR;
        }
        if(_databaseName == NULL)
        {
                fprintf(stdout, "传递给函数InitDbParms的参数_databaseName无效。\n");
                return ERROR;
        }

        localhost = _localhost;
        userName = _userName;
        password = _password;
        databaseName = _databaseName;
        return OK;
}

/*
 * 操作数据库
 * commandString -- 存放SQL操作命令
 * 如果操作成功返回OK；否则返回ERROR
 */
STATUS
UpdateDatabase( char const *commandString )
{
        MYSQL the_conn;

        if(commandString == NULL) //检查参数是否有效
       {
                fprintf(stdout, "传递给函数UpdateDatabase的参数commandString无效。\n");
                return ERROR;
        }

        if( mysql_init( &the_conn ) == NULL ) //获取本次连接的句柄
        {
                fprintf(stdout, "Error at mysql_init().\n");
                exit( EXIT_FAILURE );
        }

        if( mysql_real_connect( &the_conn, localhost, userName, password, databaseName, MYSQL_PORT, NULL, 0 ) == NULL ) //连接数据库
        {
                fprintf(stdout, "Error at mysql_real_connect().\n");
                exit( EXIT_FAILURE );
        }

        if( mysql_query( &the_conn, commandString ) != 0 ) //操作数据库
        {
                fprintf(stdout, "Error at mysql_query().\n");
                exit(EXIT_FAILURE);
        }
        mysql_close( &the_conn ); //断开连接
        return OK;
}

/*
 * 查询数据库
 * commandString -- 查询sql命令
 * resultpp -- 指向存放查询结果的双指针,注意，如果该参数为NULL，则程序不返回查询的结果。
 * 返回结果状态值，如果操作成功返回OK；否则返回ERROR
 */
STATUS
QueryDatabase( char const *commandString, MYSQL_RES **resultpp )
{
        MYSQL the_conn;
        MYSQL_RES *resultp = NULL;

        if(commandString == NULL) //检查参数是否有效
        {
                fprintf(stdout, "传递给函数QueryDatabase的参数commandString无效。\n");
                return ERROR;
        }

        if(mysql_init( &the_conn ) == NULL) //获取本次连接的句柄
        {
                fprintf(stdout, "Error at mysql_init().\n");
                exit( EXIT_FAILURE );
        }

        if(mysql_real_connect( &the_conn, localhost, userName, password, databaseName, MYSQL_PORT, NULL, 0 ) == NULL) //连接数据库
        {
                fprintf(stdout, "Error at mysql_real_connect().\n");
                exit( EXIT_FAILURE );
        }

        if(mysql_query( &the_conn, commandString ) != 0) //操作数据库
        {
                fprintf(stdout, "Error at mysql_query().\n");
                exit( EXIT_FAILURE );
        }

        resultp = mysql_store_result(&the_conn); //获取查询结果
        mysql_close(&the_conn); //断开连接

        if( mysql_num_rows(resultp) != 0 ) //如果查询结果个数不为0
        {
                if(resultpp == NULL) //判断是否需要返回查询结果
                {
                        mysql_free_result(resultp);
                }
                else
                {
                        *resultpp = resultp; //返回查询结果
                }
                return OK;
        }
        else
        {
                if(resultpp == NULL)
                {
                        mysql_free_result(resultp);
                }
                else
                {
                        *resultpp = resultp = NULL;
                }
                return ERROR;
        }
}
