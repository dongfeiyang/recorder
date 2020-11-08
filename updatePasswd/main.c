#include <stdio.h>
#include <stdlib.h>
#include "../lib/status.h"
#include "../lib/operateDb.h"
#include "../lib/URIDecode.h"
int
main(void)
{
	char *request = NULL; //存放请求内容
	uChar *userName = NULL;
	uChar *newPasswd = NULL;
	char queryStr [QUERY_MAX_SIZE]; //存放SQL指令
	MYSQL_RES *resultp = NULL; //存放查询的结果

	request = getenv("QUERY_STRING"); //获取前端请求的信息
	if(GetTheKeyValue(request, "userName", (uChar **)&userName) == ERROR)
	{
		fprintf(stdout, "Content-type:text/html\n\n");
		fprintf(stdout, "failure\n");
	}
	if(GetTheKeyValue(request, "newPasswd", (uChar **)&newPasswd) == ERROR)
	{
		fprintf(stdout, "Content-type:text/html\n\n");
		fprintf(stdout, "failure\n");
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
		MYSQL_ROW record;

		record = mysql_fetch_row(resultp);
		if(!strcmp(record [0], "out"))
		{
			fprintf(stdout, "Content-type:text/html\n\n");
			fprintf(stdout, "exit\n");
		}
		else
		{
			sprintf(queryStr, "UPDATE login SET passWord='%s' WHERE userName='%s';", newPasswd, userName);
			if(UpdateDatabase(queryStr) == ERROR)
			{
				fprintf(stdout, "Content-type:text/html\n\n");
				fprintf(stdout, "failure\n");
			
			}
			else
			{
				fprintf(stdout, "Content-type:text/html\n\n");
				fprintf(stdout, "succeed\n");
			}
		}

		free(resultp); //释放掉不必要的内存
		resultp = NULL;
	}

	free(userName); //释放不必要的内存
	userName = NULL;
	free(newPasswd);
	newPasswd = NULL;
	return 1;
}
