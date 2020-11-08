#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#define TRUE 1 //允许通过生成随机数的循环
#define FALSE -1 //不允许通过

int
main(void)
{
	char JSONDataStr [70] = ""; //存放验证码和验证码等式的正确结果
	char checkCode [10] = ""; //存放验证码
	int value1; //存放操作数1
	int value2; //存放操作数2
	int result; //存放结果
	char opTray [] = {'+', '-', '*', '/'}; //存放四则运算操作符
	char opHanzi [][10] = {"+", "－", "×", "÷"}; //存放它们的汉字形式
	char op; //存放操作符
	int flag = TRUE; //存放标记,默认是TRUE
	int index;

	srand((unsigned int)time(NULL)); //置随机种子
	do
	{
		value1 = rand() % 10;
		value2 = rand() % 10;
		index = rand() % 4;
		op = opTray [index];
		if(op == '/')
		{
			if(value2 == 0) //如果除数是零
			{
				flag = FALSE;
			}
			else
			{
				if(value1 % value2 != 0) //如果不能整除
				{
					flag = FALSE;
				}
				else
				{
					flag = TRUE;
				}
			}
		}
	}while
		(flag == FALSE);

	switch(op) //计算正确结果
	{
		case '+':
			result = value1 + value2;
			break;
		case '-':
			result = value1 - value2;
			break;
		case '*':
			result = value1 * value2;
			break;
		case '/':
			result = value1 / value2;
			break;

		default:
			fprintf(stdout, "Content-type:text/html\n\n");
			fprintf(stdout, "failure\n");
			exit(EXIT_FAILURE);
	}
	
	sprintf(JSONDataStr, "{'checkCodeStr':[{'checkCode':'%d %s %d = ?', 'result':'%d'}]}", value1, opHanzi [index], value2, result); //生成JSON的TXT形式
	fprintf(stdout, "Content-type:application/json\n\n");
	fprintf(stdout, "%s\n", JSONDataStr);
	return 1;
}
