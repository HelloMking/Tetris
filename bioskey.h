//��	�ߣ�������ѩ  /*ʹ���뱣������*/
//ʹ��˵����
//����������int bioskey (int) 
// ��  �� ��	0  ���м������򷵻���Ӧ��ֵ�����޼����������
//		1  ���м������򷵻���Ӧ��ֵ�����޼�������ȴ�
// ����ֵ �� ���м����£��򷵻ؼ�λ�ĸ�λ������һ�ٱ�����
//			 ��λ��ASCIIֵ�����������򷵻�-1�����޼����£�
//			 �򷵻�0��
//ʹ�ô˺���������� conio.hͷ�ļ�
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
