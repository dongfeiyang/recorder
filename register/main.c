#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include "../lib/status.h"
#include "../lib/URIDecode.h"
#include "../lib/operateDb.h"
char tray [] = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z',
		'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
		'0','1','2','3','4','5','6','7','8','9'}; //生成找回密钥的基本元素
#define TRAY_SIZE (int)(strlen(tray)/sizeof(char))

int
main()
{
	char *queryString = NULL;
	uChar *userName = NULL;
	uChar *password = NULL;
	uChar *email = NULL;
	uChar *userAnswer = NULL;
	uChar *answer = NULL;
	char *key = NULL;
	char queryStr [QUERY_MAX_SIZE];

	STATUS ComeOutKey(char **keyp);

	queryString = getenv("QUERY_STRING"); //获取前端请求的信息
	if(GetTheKeyValue(queryString, "userName", (uChar **)&userName) == ERROR)
	{
		fprintf(stdout, "Content-type:text/html\n\n");
		fprintf(stdout, "failure\n");
		exit(EXIT_FAILURE);
	}
	if(GetTheKeyValue(queryString, "password", (uChar **)&password) == ERROR)
	{
		fprintf(stdout, "Content-type:text/html\n\n");
		fprintf(stdout, "failure\n");
		exit(EXIT_FAILURE);
	}
	if(GetTheKeyValue(queryString, "email", (uChar **)&email) == ERROR)
	{
		fprintf(stdout, "Content-type:text/html\n\n");
		fprintf(stdout, "failure\n");
		exit(EXIT_FAILURE);
	}
	if(GetTheKeyValue(queryString, "answer", (uChar **)&answer) == ERROR)
	{
		fprintf(stdout, "Content-type:text/html\n\n");
		fprintf(stdout, "failure\n");
		exit(EXIT_FAILURE);
	}
	if(GetTheKeyValue(queryString, "userAnswer", (uChar **)&userAnswer) == ERROR)
	{
		fprintf(stdout, "Content-type:text/html\n\n");
		fprintf(stdout, "failure\n");
		exit(EXIT_FAILURE);
	}

	if(strcmp(answer, userAnswer) != 0) //判断用户的验证码是否正确
	{
		fprintf(stdout, "Content-type:text/html\n\n");
		fprintf(stdout, "checkcodeError\n");
		exit(EXIT_FAILURE);
	}

	InitDbParms("localhost", "root", NULL, "recorder"); //进行数据库操作
	while(ComeOutKey(&key) == ERROR) //生成唯一的七位密钥
		;

	sprintf(queryStr, "SELECT * FROM login WHERE userName='%s';", userName);
	if(QueryDatabase(queryStr, NULL) == OK)
	{
		fprintf(stdout, "Content-type:text/html\n\n");
		fprintf(stdout, "existed\n");
	}
	else
	{
		sprintf(queryStr, "INSERT INTO login(userName, passWord, findingKey, qqEmail) VALUES('%s', '%s', '%s', '%s');", userName, password, key, email);
		if(UpdateDatabase(queryStr) == OK)
		{
			fprintf(stdout, "Content-type:text/html\n\n");
			fprintf(stdout, "succeed$%s$\n", key);
		}
		else
		{
			fprintf(stdout, "Content-type:text/html\n\n");
			fprintf(stdout, "failure\n");
		}
	}

	free(userName); //释放掉不必要的内存
	free(password);
	free(key);
	free(email);
	free(userAnswer);
	free(answer);
	userName = password = key = email = NULL;
	userAnswer = answer = NULL;
	return 1;
}

/*
 * 生成唯一的七位密钥。
 *
 * keyp -- 返回密钥。
 * 返回值，如果生成成功返回OK；否则返回ERROR。
 */
STATUS
ComeOutKey(char **keyp)
{
	char *key = NULL;
	char queryString [QUERY_MAX_SIZE];
	int i;

	if(keyp == NULL) //检查参数是否有效
	{
		fprintf(stdout, "传递给函数ComeOutKey的参数keyp无效。\n");
		return ERROR;
	}

	key = (char *)malloc(10 * sizeof(char));
	if(key == NULL)
	{
		fprintf(stdout, "内存不足。\n");
		exit(EXIT_FAILURE);
	}

	srand((unsigned int)time(NULL)); //生成不同的随机序列
	for(i = 0; i < 7; i++)
	{
		key [i] = tray [rand() % TRAY_SIZE];
	}key [i] = '\0';

	sprintf(queryString, "SELECT * FROM login WHERE findingKey='%s';", key);
	if(QueryDatabase(queryString, NULL) == OK) //如果数据库已经存在这个密钥
	{
		return ERROR;
	}
	else //否则，返回新的密钥
	{
		*keyp = key;
		return OK;
	}
}

/*
 * 把字符串转换为整数
 *
 * str -- 数字字符串
 * 返回值，返回转换后的结果。
 */
int
StrToNum(char const *str)
{
	char const *p;
	char ch;
	int value = 0;

	for(p = str; (ch = *p) != '\0'; p ++)
	{
		value *= 10;
		value += (ch - '0');
	}
	return value;
}
