#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../lib/status.h"
#include "../lib/operateDb.h"
#include "../lib/URIDecode.h"
int
main(void)
{
	char *request = NULL; //存放前端请求的信息，用户名，记录ID，物品名，花费，备注，可能还有时间
	uChar *userName = NULL;
	uChar *recordId = NULL;
	uChar *itemName = NULL;
	uChar *pay = NULL;
	uChar *time = NULL;
	uChar *comment = NULL;

	char queryStr [QUERY_MAX_SIZE]; //存放SQL指令
	MYSQL_RES *resultp = NULL; //存放查询结果
	MYSQL_ROW record; //存放某一条记录

	request = getenv("QUERY_STRING"); //获取前端信息
	if(GetTheKeyValue(request, "userName", &userName) == ERROR)
	{
		fprintf(stdout, "Content-type:text/html\n\n");
		fprintf(stdout, "failure\n");
		exit(EXIT_FAILURE);
	}
	if(GetTheKeyValue(request, "recordId", &recordId) == ERROR)
	{
		fprintf(stdout, "Content-type:text/html\n\n");
		fprintf(stdout, "failure\n");
		exit(EXIT_FAILURE);
	}
	if(GetTheKeyValue(request, "itemName", &itemName) == ERROR)
	{
		fprintf(stdout, "Content-type:text/html\n\n");
		fprintf(stdout, "failure\n");
		exit(EXIT_FAILURE);
	}
	if(GetTheKeyValue(request, "pay", &pay) == ERROR)
	{
		fprintf(stdout, "Content-type:text/html\n\n");
		fprintf(stdout, "failure\n");
		exit(EXIT_FAILURE);
	}
	if(GetTheKeyValue(request, "time", &time) == ERROR)
	{
		fprintf(stdout, "Content-type:text/html\n\n");
		fprintf(stdout, "failure\n");
		exit(EXIT_FAILURE);
	}
	if(GetTheKeyValue(request, "comment", &comment) == ERROR)
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
			if((int)strlen(comment) == 0) //要不要考虑备注部分
			{
				sprintf(queryStr, "UPDATE notes SET itemName='%s', pay='%s', time='%s' WHERE id='%s';", itemName, pay, time, recordId);
			}
			else
			{
				sprintf(queryStr, "UPDATE notes SET itemName='%s', pay='%s', note='%s', time='%s' WHERE id='%s';", itemName, pay, comment, time, recordId);
			}
			

			if(UpdateDatabase(queryStr) == ERROR) //如果修改失败
			{
				fprintf(stdout, "Content-type:text/html\n\n");
				fprintf(stdout, "failure\n");
			}
			else //否则
			{
				fprintf(stdout, "Content-type:text/html\n\n");
				fprintf(stdout, "succeed\n");
			}
		}

		free(resultp);
		resultp = NULL;
	}

	free(userName); //释放掉不必要的内存
	userName = NULL;
	free(recordId);
	recordId = NULL;
	free(itemName);
	itemName = NULL;
	free(pay);
	pay = NULL;
	free(comment);
	comment = NULL;
	return 1;
}
