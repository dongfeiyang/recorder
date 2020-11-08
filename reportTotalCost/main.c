#include <stdio.h>
#include <math.h>
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

	double StrToDouble(char const* string);

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
				sprintf(queryStr, "SELECT pay FROM notes WHERE userName='%s' AND time LIKE '%s%%';", userName, year);
				free(year);
				year = NULL;
			}
			if(year != NULL && month != NULL && day == NULL)
			{
				/* 如果是年月 */
				sprintf(queryStr, "SELECT pay FROM notes WHERE userName='%s' AND time LIKE '%s-%s-%%';", userName, year, month);
				
				free(year);
				year = NULL;
				free(month);
				month = NULL;
			}
			if(year != NULL && month != NULL && day != NULL)
			{
				/* 如果是年月日 */
				sprintf(queryStr, "SELECT pay FROM notes WHERE userName='%s' AND time LIKE '%s-%s-%s%%';", userName, year, month, day);

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
				int i;
				double recordCost;
				double totalCost = 0;

				recordNumber = mysql_num_rows(resultp); //获取查询的数据总数
				for(i = 1; i <= recordNumber; i ++)
				{
					record = mysql_fetch_row(resultp);
					recordCost = StrToDouble(record [0]);
					totalCost += recordCost;
				}
				
				fprintf(stdout, "Content-type:text/html\n\n");
				fprintf(stdout, "succeed$%0.3lf\n", totalCost);
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
 * 所用到的状态值
 *
 * TRUE 表示是负数
 * FALSE 表示是正数
 */
#define TRUE 2
#define FALSE -2

/*
 * 把字符串转换为浮点数
 *
 * string -- 指定的字符串
 * 返回转换后的浮点数。
 */
double
StrToDouble(char const* string)
{
	char const *dotP = NULL; //待指向小数点
	char const *p = NULL; //待指向字符串开头
	char ch;
	double value = 0; //存放转换后的浮点数
	int minusFlag = FALSE; //负号标记，默认是FALSE

	if(string == NULL) //检查参数是否有效
	{
		return 0;
	}
	
	dotP = strchr(string, '.'); //获取小数点的位置
	p = string; //获取字符串开端的位置

	if(*p == '-') //判断是否为负数
	{
		minusFlag = TRUE;
		p ++;
	}

	if(dotP == NULL) //如果是整数
	{
		while((ch = *p) != '\0')
		{
			value *= 10;
			value += (ch - '0');
			p ++;
		}
	}
	else //如果是小数
	{
		double part1 = 0; //整数部分
		double part2 = 0; //小数部分
		int part2Len;

		while((ch = *p) != '.') //获取整数部分
		{
			part1 *= 10;
			part1 += (ch - '0');
			p ++;
		}
		p ++;

		part2Len = strlen(p); //获取小数部分
		while((ch = *p) != '\0')
		{
			part2 *= 10;
			part2 += (ch - '0');
			p ++;
		}
		part2 /= (int)pow(10, part2Len);
		value = part1 + part2;
	}

	return (minusFlag == FALSE)? value : value * -1;
}
