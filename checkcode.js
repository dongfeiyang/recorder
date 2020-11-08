var answer; //存放运算的正确答案
var bgcolor; //存放验证码背景色

function //生成四则运算验证码
GetCheckcode()
{
	var operators = ['+','-','*','/'];
	var colors = ['#FF69B4','#9370DB','#F8F8FF','#6495ED','#008B8B','#40E0D0','#00FF00'];
	var value1,value2;
	var index;
	var string;
	var flag;

	do
	{
		flag = false;
		value1 = parseInt(Math.random() * 10) % 10;
		value2 = parseInt(Math.random() * 10) % 10;
		index = parseInt(Math.random() * 10) % 4;
		bgcolor = colors [parseInt(Math.random() * 10) % 7];

		if(index == 3)
		{
			if(value2 == 0) //如果除数为0，则重新生成算式。
			{
				flag = true;
			}
			if(value1 % value2 != 0) //如果不能整除，也重新生成算式。
			{
				flag = true;
			}
		}
	}while
		(flag == true);
	
	switch(index) //获取正确的运算结果
	{
		case 0:
			answer = value1 + value2;
			break;
		case 1:
			answer = value1 - value2;
			break;
		case 2:
			answer = value1 * value2;
			break;
		case 3:
			answer = value1 / value2;
			break;
	}

	string = value1.toString() + " " + operators [index] + " " + value2.toString() + " =" + " ?";
	return string;
}
