#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../lib/status.h"
#include "../lib/URIDecode.h"
#include "../lib/operateDb.h"
int
main(void)
{
	char queryString [QUERY_MAX_SIZE]; //存放访问数据库命令
	char *content = NULL;  //存放前端请求的信息
	uChar *userName = NULL;
	uChar *password = NULL;
	uChar *userAnswer = NULL;
	uChar *answer = NULL;

	MYSQL_RES *resultp = NULL; //存放查询的结果
	MYSQL_ROW record; //存放某一条查询记录

	content = getenv("QUERY_STRING"); //从请求信息中获取键值
	if(GetTheKeyValue(content, "userName", &userName) == ERROR)
	{
		fprintf(stdout, "Content-type:text/html\n\n");
		fprintf(stdout, "failure\n");
		exit(EXIT_FAILURE);
	}
	if(GetTheKeyValue(content, "password", &password) == ERROR)
	{
		fprintf(stdout, "Content-type:text/html\n\n");
		fprintf(stdout, "failure\n");
		exit(EXIT_FAILURE);
	}
	if(GetTheKeyValue(content, "userAnswer", &userAnswer) == ERROR)
	{
		fprintf(stdout, "Content-type:text/html\n\n");
		fprintf(stdout, "failure\n");
		exit(EXIT_FAILURE);
	}
	if(GetTheKeyValue(content, "answer", &answer) == ERROR)
	{
		fprintf(stdout, "Content-type:text/html\n\n");
		fprintf(stdout, "failure\n");
		exit(EXIT_FAILURE);
	}

	InitDbParms("localhost", "root", NULL, "recorder"); //操作数据库
	if(strcmp(answer, userAnswer) != 0) //如果验证码和正确答案不相等
	{
		fprintf(stdout, "Content-type:text/html\n\n");
		fprintf(stdout, "checkCodeError\n");
		exit(EXIT_FAILURE);
	}

	sprintf(queryString, "SELECT * FROM login WHERE userName='%s';", userName);
	if(QueryDatabase(queryString, NULL) == OK) //判断用户名是否存在
	{
		sprintf(queryString, "SELECT status FROM login WHERE userName='%s' AND passWord='%s';", userName, password);
		if(QueryDatabase(queryString, &resultp) == OK) //判断用户名和密码是否都正确
		{
			record = mysql_fetch_row(resultp);
			if(!strcmp(record [0], "in")) //如果用户已经登陆
			{
				fprintf(stdout, "Content-type:text/html\n\n");
				fprintf(stdout, "login\n");
			}
			else //否则
			{
				sprintf(queryString, "UPDATE login SET status='in' WHERE userName='%s';", userName);
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
			}
			free(resultp);
			resultp = NULL;
		}
		else
		{
			fprintf(stdout, "Content-type:text/html\n\n");
			fprintf(stdout, "failure\n");
		}
	}
	else
	{
		fprintf(stdout, "Content-type:text/html\n\n");
		fprintf(stdout, "none\n");
	}

	free(userName); //释放掉不必要的内存
	free(password);
	free(answer);
	free(userAnswer);
	userName= NULL;
	password = NULL;
	answer = NULL;
	userAnswer = NULL;
	return 1;
}

