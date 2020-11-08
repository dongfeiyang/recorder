#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../lib/status.h"
#include "../lib/operateDb.h"
#include "../lib/URIDecode.h"
int
main(void)
{
	char *request = NULL; //存放前端请求的信息，用户名，记录ID
	uChar *userName = NULL;
	uChar *recordId = NULL;

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
			sprintf(queryStr, "DELETE FROM notes WHERE id='%s';", recordId);
			if(UpdateDatabase(queryStr) == ERROR) //如果删除失败
			{
				fprintf(stdout, "Content-type:text/html\n\n");
				fprintf(stdout, "failure\n");
			}
			else //否则，重新导入数据，保证ID是连贯递增
			{
				free(resultp);
				resultp = NULL;

				sprintf(queryStr, "SELECT * FROM notes;"); //把数据重新导入数据库
				if(QueryDatabase(queryStr, &resultp) == ERROR)
				{
					fprintf(stdout, "Content-type:text/html\n\n");
					fprintf(stdout, "failure\n");
				}
				else
				{
					int recordNumber = mysql_num_rows(resultp);
					int i;

					UpdateDatabase("TRUNCATE TABLE notes;");
					for(i = 1; i <= recordNumber; i ++)
					{
						record = mysql_fetch_row(resultp);
						sprintf(queryStr, "INSERT INTO notes(userName,itemName,pay,note,time) VALUES('%s','%s','%s','%s','%s');", record [1], record [2], record [3], record [4], record [5]);
						UpdateDatabase(queryStr);
					}
				}

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
	return 1;
}
