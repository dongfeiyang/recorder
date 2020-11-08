#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../lib/status.h"
#include "../lib/URIDecode.h"
#include "../lib/operateDb.h"
#define RECORD_NUM_PER_PAGE 7 //要显示的每页最大记录数目

int
main(void)
{
	char *request = NULL; //存放请求的内容
	uChar *userName = NULL;
	uChar *year = NULL; //年
	uChar *month = NULL; //月
	uChar *day = NULL; //日
	uChar *pageNoStr = NULL; //存放前端请求的页码

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
	if(GetTheKeyValue(request, "pageNo", (uChar **)&pageNoStr) == ERROR) //获取请求的页码
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
				sprintf(queryStr, "SELECT id,itemName,pay,time,note FROM notes WHERE userName='%s' AND time LIKE '%s%%' ORDER BY time DESC;", userName, year);
				free(year);
				year = NULL;
			}
			if(year != NULL && month != NULL && day == NULL)
			{
				/* 如果是年月 */
				sprintf(queryStr, "SELECT id,itemName,pay,time,note FROM notes WHERE userName='%s' AND time LIKE '%s-%s-%%' ORDER BY time DESC;", userName, year, month);
				
				free(year);
				year = NULL;
				free(month);
				month = NULL;
			}
			if(year != NULL && month != NULL && day != NULL)
			{
				/* 如果是年月日 */
				sprintf(queryStr, "SELECT id,itemName,pay,time,note FROM notes WHERE userName='%s' AND time LIKE '%s-%s-%s%%' ORDER BY time DESC;", userName, year, month, day);

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
				char *dataText = NULL; //存放查询的json形式的数据
				char *p = NULL;

				int recordNumber; //存放查询的记录数
				int storageSize; //数据所占内存大小
				int pageNo; //数字形式的请求页码
				int startIndex; //开始读取记录的位置
				int endIndex; //最后读取记录的位置
				int i;

				recordNumber = mysql_num_rows(resultp); //获取查询的数据总数
				pageNo = StrToInteger(pageNoStr); //获取数字形式的pageNo
				startIndex = (pageNo - 1) * RECORD_NUM_PER_PAGE + 1; //计算开始读取记录的位置
				endIndex = (startIndex + RECORD_NUM_PER_PAGE - 1 < recordNumber)? startIndex + RECORD_NUM_PER_PAGE - 1 : recordNumber; //计算最后读取记录的位置 
				storageSize = (endIndex - startIndex + 1) * 211 + 25; //计算所需内存大小
				dataText = (char *)malloc(storageSize * sizeof(char)); //申请存放文本形式的JSON数据
				if(dataText == NULL)
				{
					fprintf(stdout, "Content-type:text/html\n\n");
					fprintf(stdout, "failure\n");
					exit(EXIT_FAILURE);
				}
				memset((uChar *)dataText, 0, storageSize * sizeof(char)); //内存清零
				
				strcpy(dataText, "succeed${'records':["); //把查询结果以JSON文本形式包装
				for(i = 1; i <= endIndex; i ++)
				{
					record = mysql_fetch_row(resultp);

					if(i < startIndex)
					{
						continue;
					}

					strcat(dataText, "{'id':'");
					strcat(dataText, record [0]);
					strcat(dataText, "','itemName':'");
					strcat(dataText, record [1]);
					strcat(dataText, "','pay':'");
					strcat(dataText, record [2]);
					strcat(dataText, "','time':'");
					strcat(dataText, record [3]);
					strcat(dataText, "','comment':'");
					strcat(dataText, record [4]);
					strcat(dataText, "'},");
				}
				p = strrchr(dataText, ','); //去掉多余的','
				*p = '\0';

				strcat(dataText, "]}"); 


				fprintf(stdout, "Content-type:application/json\n\n"); //返回给前端JSON文本数据
				fprintf(stdout, "%s\n", dataText);

				free(dataText); //释放掉不必要的内存
				dataText = NULL;
			}
		}
	}

	free(resultp); //释放掉不必要的内存
	resultp = NULL;
	free(userName);
	userName = NULL;
	free(pageNoStr);
	pageNoStr = NULL;
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
