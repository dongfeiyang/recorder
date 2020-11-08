#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "../lib/status.h"
#include "../lib/operateDb.h"
#include "../lib/URIDecode.h"
int
main(void)
{
	uChar *userName = NULL; //存放用户名
	uChar *key = NULL; //存放密钥
	uChar *email = NULL; //存放邮箱
	char *content = NULL; //存放前端请求的信息
	char queryString [QUERY_MAX_SIZE]; //存放操作数据库的指令
	MYSQL_RES *resultp = NULL; //存放数据库查询的结果	

	STATUS SendEmailBySmtp(char const *userName, char const *password, char const *email);

	content = getenv("QUERY_STRING"); 	//获取用户名和邮箱
	if(GetTheKeyValue(content, "userName", &userName) == ERROR)
	{
		fprintf(stdout, "Error at GetTheKeyValue().");
		exit(EXIT_FAILURE);
	}
	if(GetTheKeyValue(content, "key", &key) == ERROR)
	{
		fprintf(stdout, "Error at GetTheKeyValue().");
		exit(EXIT_FAILURE);
	}
	if(GetTheKeyValue(content, "email", &email) == ERROR)
	{
		fprintf(stdout, "Error at GetTheKeyValue().");
		exit(EXIT_FAILURE);
	}

	InitDbParms("localhost", "root", NULL, "recorder"); //操作数据库
	sprintf(queryString, "SELECT * FROM login WHERE userName='%s';", userName);
	if(QueryDatabase(queryString, NULL) == ERROR) //如果用户名没有注册
	{
		fprintf(stdout, "Content-type:text/html\n\n");
		fprintf(stdout, "none\n");
	}
	else
	{
		MYSQL_ROW record;
		char *password = NULL;

		sprintf(queryString, "SELECT * FROM login WHERE userName='%s' AND findingKey='%s';", userName, key);
		if(QueryDatabase(queryString, NULL) == ERROR) //如果密钥错误
		{
			fprintf(stdout, "Content-type:text/html\n\n");
			fprintf(stdout, "keyError\n");
		}
		else
		{
			sprintf(queryString, "SELECT password FROM login WHERE findingKey='%s' AND qqEmail='%s';", key, email);
			if(QueryDatabase(queryString, &resultp) == ERROR) //如果邮箱错误
			{
				fprintf(stdout, "Content-type:text/html\n\n");
				fprintf(stdout, "emailError\n");
			}
			else
			{
				record = mysql_fetch_row(resultp);
				password = record [0];
		
				if(SendEmailBySmtp(userName, password, email) == OK) //如果密码成功发送到用户的QQ邮箱
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

	}

	free(userName); //释放掉不必要的内存
	free(key);
	free(email);
	userName = key = email = NULL;
	return 1;
}

/*
 * 把重要信息（用户名要找回的密码）发送到对方的邮箱里
 *
 * userName -- 用户名
 * password -- 密码
 * email -- 邮箱
 * 返回值，如果发送成功返回OK；否则返回ERROR。
 */
STATUS 
SendEmailBySmtp(char const *userName, char const *password, char const *email)
{
	int socketFd; //套接字描述符
	struct sockaddr_in destAddr; //存放目标主机的IP和端口号
	char buffer [1024];
	int bufferLen = 0;
	struct hostent *hostStruct = NULL;
	char **str = NULL;
	char *ip = NULL;
	STATUS status = ERROR;

	STATUS ShakeHands(int socketFd, char const *command, int delayTime, char const *respondOk);

	if(userName == NULL) //检查参数
	{
		fprintf(stdout, "传递给函数SendEmailBySmtp的参数userName无效。\n");
		return ERROR;
	}
	if(password == NULL)
	{
		fprintf(stdout, "传递给函数SendEmailBySmtp的参数password无效。\n");
		return ERROR;
	}
	if(email == NULL)
	{
		fprintf(stdout, "传递给函数SendEmailBySmtp的参数email无效。\n");
		return ERROR;
	}

	hostStruct = gethostbyname("smtp.163.com"); //把目标主机名转换为IP
	str = hostStruct->h_addr_list;
	ip = inet_ntoa(**(struct in_addr **)str);

	socketFd = socket(AF_INET, SOCK_STREAM, 0); //申请套接字ID
	destAddr.sin_family = AF_INET;
	destAddr.sin_port = htons(25);
	destAddr.sin_addr.s_addr = inet_addr(ip);
	memset((void *)&destAddr.sin_zero, 0, 8);
	
	if(connect(socketFd, (struct sockaddr *)&destAddr, sizeof(struct sockaddr)) == -1) //向服务器发送连接请求
	{
		fprintf(stdout, "Error at connect().\n");
		return ERROR;
	}
	bufferLen = recv(socketFd, buffer, 1024, 0); //接受服务器返回的信息
	buffer [bufferLen] = '\0';
	
	if(strstr(buffer, "220") == NULL) //判断是否请求成功
	{
		fprintf(stdout, "Error at connect().\n");
		return ERROR;
	}

	while(ShakeHands(socketFd, "HELO C\r\n", 300, "250") == ERROR) //问候smtp.163.com服务器
		;


	while(ShakeHands(socketFd, "AUTH LOGIN\r\n", 1000, "dXNlcm5hbWU6") == ERROR) //登陆该服务器
		;
	while(ShakeHands(socketFd, "cmVjb3JkZXJfMjAxOA==\r\n", 1000, "UGFzc3dvcmQ6") == ERROR)
		;
	while(ShakeHands(socketFd, "cmlnaHQyMDE4\r\n", 1000, "235") == ERROR)
		;


	while(ShakeHands(socketFd, "MAIL FROM:<recorder_2018@163.com>\r\n", 1000, "250") == ERROR) //设置邮件发送者
		;

	sprintf(buffer, "RCPT TO:<%s>\r\n", email);
	while(ShakeHands(socketFd, buffer, 1000, "250") == ERROR) //设置邮件接收者
		;

	while(ShakeHands(socketFd, "DATA\r\n", 1000, "354") == ERROR) //请求填写邮件内容
		;
	
	sprintf(buffer, "From: recorder_2018@163.com\nTo: %s\nSubject:关于我的记账本用户密码找回\n亲爱的用户%s，您好：\n        您的密码是%s，请记好。感谢你的使用。\r\n.\r\n", email, userName, password);
	if(ShakeHands(socketFd, buffer, 2000, "250") == OK) //填写内容，发送
	{
		status = OK;
	}
	else
	{
		status = ERROR;
	}

	while(ShakeHands(socketFd, "QUIT\r\n", 1000, "221") == ERROR) //退出smtp.163.com服务
		;

	close(socketFd); //关闭套接字连接
	return status;
}

/*
 * 和服务器握手。
 *
 * socketFd -- 套接字描述符
 * command -- 发送的指令
 * delayTime -- 发送指令前延时的时间，单位毫秒
 * respondOk -- 服务器响应讯号
 * 返回值，如果握手成功返回OK；否则，返回ERROR。
 */
STATUS
ShakeHands(int socketFd, char const *command, int delayTime, char const *respondOk)
{
	char buffer [1024] = "";
	int bufferLen = 0;

	if(socketFd == -1) //检查参数是否有效
	{
		fprintf(stdout, "传递给函数ShakeHands的参数socketFd无效。\n");
		return ERROR;
	}
	if(command == NULL) 
	{
		fprintf(stdout, "传递给函数ShakeHands的参数command无效。\n");
		return ERROR;
	}
	if(delayTime < 0)
	{
		fprintf(stdout, "传递给函数ShakeHands的参数delayTime无效。\n");
		return ERROR;
	}
	if(respondOk == NULL)
	{
		fprintf(stdout, "传递给函数ShakeHands的参数respondOk无效。\n");
		return ERROR;
	}

	usleep(delayTime); //延迟理想时间然后发送请求指令
	send(socketFd, command, strlen(command), 0);

	usleep(1000); //延迟一会儿接受服务器的反馈
	bufferLen = recv(socketFd, buffer, 1024, 0);
	buffer [bufferLen] = '\0';

	if(strstr(buffer, respondOk) == NULL) //判断是否对操作服务器的指令发送成功
	{
		return ERROR;
	}
	else
	{
		return OK;
	}
}
