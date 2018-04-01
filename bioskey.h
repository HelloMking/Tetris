//作	者：寒冬飞雪  /*使用请保留此行*/
//使用说明：
//函数声明：int bioskey (int) 
// 参  数 ：	0  若有键按下则返回相应数值，若无键按下则继续
//		1  若有键按下则返回相应数值，若无键按下则等待
// 返回值 ： 若有键按下，则返回键位的高位按键的一百倍，或
//			 键位的ASCII值，参数错误则返回-1。若无键按下，
//			 则返回0。
//使用此函数必需包含 conio.h头文件
//use this function must include the header file of conio.h
int bioskey(int cmd)
{
	int Ckey=0,gkey=0;


	if (cmd==1) 
	{
		Ckey=1;
	}
	else if (cmd==0)
	{
		Ckey=kbhit();
	}
	else 
	{
		return (-1);
	}
	if (Ckey==1)
	{
		gkey=getch();
		Ckey=kbhit();	
		if (Ckey==1 && (gkey>=128 || gkey<=0))
		{
			gkey=getch();
			return (gkey*100);			
		}
		else
		{
			return gkey;
		}
	}
	else
	{
		return 0;
	}

}
