#include <stdio.h>
#include <stdlib.h>
#include "../lib/status.h"
#include "../lib/URIDecode.h"
#include "../lib/operateDb.h"
int
main(void)
{
	char *request = NULL; //存放前端请求的内容
	uChar *userName = NULL; //存放用户名
	char queryString [QUERY_MAX_SIZE]; //存放SQL指令
	
	request = getenv("QUERY_STRING"); //获取键值
	if(GetTheKeyValue(request, "userName", (uChar **)&userName) == ERROR)
	{
		fprintf(stdout, "Content-type:text/html\n\n");
		fprintf(stdout, "failure\n");
		exit(EXIT_FAILURE);
	}

	InitDbParms("localhost", "root", NULL, "recorder"); //操作数据库
	sprintf(queryString, "UPDATE login SET status = 'out' WHERE userNAME = '%s';", userName);
	if(UpdateDatabase(queryString) == OK)
	{
		fprintf(stdout, "Content-type:text/html\n\n");
		fprintf(stdout, "succeed\n");
	}
	else
	{
		fprintf(stdout, "Content-type:text/html\n\n");
		fprintf(stdout, "failure\n");
	}
	return 1;
}
