#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../lib/status.h"
#include "../lib/operateDb.h"
#include "../lib/URIDecode.h"
int
main(void)
{
	char *request = NULL; //存放前端请求的信息
	uChar *userName = NULL;
	char queryStr [QUERY_MAX_SIZE]; //存放SQL指令
	char JSONData [150] = ""; //存放返回的JSON数据
	MYSQL_RES *resultp = NULL; //存放查询结果
	MYSQL_ROW record; //存放某一条记录

	request = getenv("QUERY_STRING"); //获取前端信息
	if(GetTheKeyValue(request, "userName", &userName) == ERROR)
	{
		fprintf(stdout, "Content-type:text/html\n\n");
		fprintf(stdout, "failure\n");
		exit(EXIT_FAILURE);
	}

	InitDbParms("localhost", "root", NULL, "recorder"); //操作数据库
	sprintf(queryStr, "SELECT status FROM login WHERE userName='%s';", userName);
	if(QueryDatabase(queryStr, &resultp) == ERROR)
	{
		fprintf(stdout, "Content-type:text/html\n\n");
		fprintf(stdout, "failure\n");
	}
	else
	{
		record = mysql_fetch_row(resultp);
		if(!strcmp(record [0],"out")) //如果用户已经退出登陆
		{
			fprintf(stdout, "Content-type:text/html\n\n");
			fprintf(stdout, "exit\n");
		}
		else
		{
			free(resultp);
			resultp = NULL;

			sprintf(queryStr, "SELECT findingKey, qqEmail FROM login WHERE userName='%s';", userName);
			if(QueryDatabase(queryStr, &resultp) == ERROR) //查询指定的找回密钥和邮箱
			{
				fprintf(stdout, "Content-type:text/html\n\n");
				fprintf(stdout, "failure\n");
			}
			else
			{
				record = mysql_fetch_row(resultp);
				sprintf(JSONData, "{'personalInfo':[{'findingKey':'%s', 'qqEmail':'%s'}]}", record [0], record [1]);
				fprintf(stdout, "Content-type:application/json\n\n");
				fprintf(stdout, "%s\n", JSONData);
			}
		}

		free(resultp);
		resultp = NULL;
	}

	free(userName); //释放掉不必要的内存
	userName = NULL;
	return 1;
}
