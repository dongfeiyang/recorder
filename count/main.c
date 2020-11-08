#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../lib/status.h"
#include "../lib/URIDecode.h"
#include "../lib/operateDb.h"

int
main(void)
{
	char *request = NULL; //存放请求的内容
	uChar *userName = NULL;
	uChar *year = NULL; //年
	uChar *month = NULL; //月
	uChar *day = NULL; //日

	MYSQL_RES * resultp = NULL; //存放数据库返回的结果
	MYSQL_ROW record; //存放查询的某一条记录
	char queryStr [QUERY_MAX_SIZE]; //存放SQL指令

	int StrToInteger(char const* string);

	request = getenv("QUERY_STRING"); //获取前端请求信息
	if(GetTheKeyValue(request, "userName", (uChar **)&userName) == ERROR) //获取用户名
	{
		fprintf(stdout, "Content-type:text/html\n\n");
		fprintf(stdout, "failure\n");
		exit(EXIT_FAILURE);
	}
	if(GetTheKeyValue(request, "year", (uChar **)&year) == ERROR) //获取年
	{
		fprintf(stdout, "Content-type:text/html\n\n");
		fprintf(stdout, "failure\n");
		exit(EXIT_FAILURE);
	}
	if(GetTheKeyValue(request, "month", (uChar **)&month) == ERROR) //获取月
	{
		fprintf(stdout, "Content-type:text/html\n\n");
		fprintf(stdout, "failure\n");
		exit(EXIT_FAILURE);
	}
	if(GetTheKeyValue(request, "day", (uChar **)&day) == ERROR) //获取日
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
		if(!strcmp(record [0], "out")) //判断用户是否已经登出帐号
		{
			fprintf(stdout, "Content-type:text/html\n\n");
			fprintf(stdout, "exit\n");
		}
		else //否则，继续查询
		{
			free(resultp); //释放掉resultp当前指向的内存，为下次查询做准备
			resultp = NULL; 

			if(year != NULL && month == NULL && day == NULL)
			{
				/* 如果是年 */
				sprintf(queryStr, "SELECT COUNT(*) FROM notes WHERE userName='%s' AND time LIKE '%s%%';", userName, year);
				free(year);
				year = NULL;
			}
			if(year != NULL && month != NULL && day == NULL)
			{
				/* 如果是年月 */
				sprintf(queryStr, "SELECT COUNT(*) FROM notes WHERE userName='%s' AND time LIKE '%s-%s-%%';", userName, year, month);
				free(year);
				year = NULL;
				free(month);
				month = NULL;
			}
			if(year != NULL && month != NULL && day != NULL)
			{
				/* 如果是年月日 */
				sprintf(queryStr, "SELECT COUNT(*) FROM notes WHERE userName='%s' AND time LIKE '%s-%s-%s%%';", userName, year, month, day);
				free(year);
				year = NULL;
				free(month);
				month = NULL;
				free(day);
				day = NULL;
			}

			if(QueryDatabase(queryStr, &resultp) == ERROR)
			{
				fprintf(stdout, "Content-type:text/html\n\n");
				fprintf(stdout, "failure\n");
			}
			else
			{
				int recordNumber; //存放查询的记录数

				record = mysql_fetch_row(resultp);
				recordNumber = StrToInteger(record [0]);
				fprintf(stdout, "Content-type:text/html\n\n");
				fprintf(stdout, "succeed$%d\n", recordNumber); //返回给前端统计的总数目
			}
		}
	}

	free(resultp); //释放掉不必要的内存
	resultp = NULL;
	free(userName);
	userName = NULL;
	return 1;
}

/*
 * 把字符串转换为数字
 *
 * string -- 指定的字符串
 * 返回转换后的数字。
 */
int
StrToInteger(char const* string)
{
	char const *p = NULL;
	char ch;
	int value;

	if(string == NULL)
	{
		return 0;
	}

	value = 0;
	p = string;
	while((ch = *p) != '\0')
	{
		value *= 10;
		value += (ch - '0');
		p ++;
	}
	return value;
}
