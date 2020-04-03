/*——————密码门锁————————*/
/*——————密码86445131——————*/

#include<reg52.h>

#define uchar unsigned char
#define uint unsigned int

sbit l1=P2^0;
sbit bump=P1^3; //蜂鸣器控制
sbit IN1=P1^0;//驱动引脚
sbit IN2=P1^1;
sbit rs=P2^3;	  //液晶屏控制位
sbit rw=P2^4;	  //读写选择端
sbit lcden=P2^5;  //使能控制端

/**********显示内容**************************/
uchar code xianshi0[]="input  password:";
uchar code xianshi1[]="please come in!";
uchar code xianshi2[]="close the door!";
uchar code xianshi3[]="*";
uchar code xianshi4[]=" you are thief!";
uchar code xianshi5[]="  first error!";
uchar code xianshi6[]="  second error!";
uchar code xianshi7[]="  third error!!";
uchar code xianshi8[]="  Good Bye!!";
uchar code xianshi9[]=" Wait a moment! ";
static uchar table[8]; //给按键输入留取存储空间
static uchar mima[8]={8,6,4,4,5,1,3,1};
uchar num=0,etimes=0,s=0;
bit cpflog=0,mimaqueren=0;

void compare();	//密码比较函数
void keydeal13();	//关门
void keydeal14();	//开门
void keyscan();		//键盘扫描函数
void init();	//初始化函数

/********************延时一毫秒函数**************/ 
void delay_xs(uint x)
{   
	uint i,j;
	for(i=x;i>0;i--)
	for(j=110;j>0;j--)
	;
}
/*********************液晶写数据函数******************/
void writedate(uchar date) //写命令
{
	P0=date;
	rs=1;
	lcden=0;
	delay_xs(10);
	lcden=1;
	delay_xs(10);
	lcden=0;
}
/*********************液晶写命令数据函数******************/
void writecom(uchar com) //写命令
{
	P0=com;
	rs=0;
	lcden=0;
	delay_xs(10);
	lcden=1;
	delay_xs(10);
	lcden=0;
}

/************各按键对应处理函数***************/
/*************各按键功能*********************/
/************1-2-3-4-5-6-7-8-9-0************/
/**************数字按键*******************/
void keydeal1()                 //按键1
{
	table[num]=1;
	num++;
}
void keydeal2()                //按键2
{
	table[num]=2;
	num++;
}
void keydeal3()               //按键3
{
	table[num]=3;
	num++;
}
void keydeal4()               //按键4
{
	table[num]=4;
	num++;
}
void keydeal5()               //按键5
{
	table[num]=5;
	num++;
}
void keydeal6() //按键6
{
	table[num]=6;
	num++;
}
void keydeal7() //按键7
{
	table[num]=7;
	num++;
}
void keydeal8()  //按键8
{
	table[num]=8;
	num++;
}
void keydeal9()   //按键9
{
	table[num]=9;
	num++;
}
void keydeal10()   //按键10
{
	table[num]=0;   
	num++;
}

/******************功能键*****************/
/***********确定，取消，开门，关门*******/
//确认键
void keydeal11()  //确认键
{
	uchar j,i;
	if(num==8)         //判断是否输入八个数字
	{
		num=0;         //将输入数字个数清零
		compare();         //进行密码比较
		for(j=0;j<8;j++) //用FFFFFFFF将输入的数据清空
		{
			table[j]=0;
		}
	}
	if(cpflog==1) //如果密码正确，标志位为1
	{
		l1=0; //点亮开锁灯
		cpflog=0; //使比较密码标志位归零
		etimes=0; //使记错次数归零
		IN1=1;
		IN2=0;		 //开始开门
		delay_xs(1000);
		delay_xs(1000);
		delay_xs(1000);
		delay_xs(1000);
		IN1=0;
		IN2=0;	    //完成开门		  
		writecom(0x01); //液晶屏清空显示
		writecom(0x80); //让液晶显示“please come in！”
		for(j=0;j<15;j++)
		{
			writedate(xianshi1[j]);
			delay_xs(3);
		}
		for(j=0;j<2;j++)  //蜂鸣器响两声提示开锁成功
		{
			bump=0; 
			delay_xs(200);
			bump=1; 
			delay_xs(200);
			bump=0; 
			delay_xs(200);
			bump=1; 
			delay_xs(200);
			bump=0;
		}
		writecom(0x01); 
		writecom(0x80);//清屏
		for(i=0;i<15;i++)				  //显示“close the door”
		{	
			writedate(xianshi2[i]); 
		} 
		mimaqueren=1;			  //关门标志置一，等待关门
		while(s==0)				 //等待关门
		{
			keyscan();			//键盘扫描，等待按下关门键
			l1=~l1;
			delay_xs(100);	
		}
		s=0;
		mimaqueren=0;	//关门标志置零，开始关门
		keydeal13();	//关门
	}
	else //否则密码错误，执行密码错误指令
	{
		num=0; //将输入数据个数清零
		etimes++; //记录错误次数加1 
		for(j=0;j<8;j++)  //清空修改密码输入数据
		{ 
			table[j]=0x0f; 
		}
		if(etimes==1)  //如果输错一次
		{
			writecom(0x01);   //清屏
			writecom(0x80);
			for(j=0;j<14;j++)   //显示first error
			{ 
				writedate(xianshi5[j]); 
			}
			delay_xs(2000);   //延时两秒
			writecom(0xc);
			writecom(0x80);
			for(j=0;j<16;j++)   //第一行显示input passsword
			{
				writedate(xianshi0[j]); 
				delay_xs(5);
			}   
			writecom(0x80+0x40+4); //第二行显示--------
			for(j=0;j<8;j++) 
			{
				writedate('-'); 
			}
		}
		if(etimes==2)   //如果输错两次
		{
			writecom(0x01); 
			writecom(0x80);
			for(j=0;j<15;j++)   //显示second error
			{
				writedate(xianshi6[j]);
			}
			delay_xs(2000);   //延时两秒
			writecom(0xc);
			writecom(0x80);
			for(j=0;j<16;j++) //第一行显示input passsword
			{
				writedate(xianshi0[j]); 
				delay_xs(5);
			}
			writecom(0x80+0x40+4);
			for(j=0;j<8;j++) //第二行显示--------
			{
				writedate('-');
			}
		}
		if(etimes==3) //如果输错三次
		{
			writecom(0x01);
			writecom(0x80);
			for(j=0;j<15;j++) //显示  third error!!
			{
				writedate(xianshi7[j]);
			}
			writecom(0x01);
			writecom(0x80);
			for(j=0;j<15;j++) //显示you are thief！
			{
				writedate(xianshi4[j]);
			}			
			for(j=0;j<10;j++) //报警十秒钟
			{
				bump=0; 
				delay_xs(500);
				bump=1; 
				delay_xs(500);
			}
			writecom(0x01);
			writecom(0x80);
			for(j=0;j<16;j++) //显示Wait a moment
			{
				writedate(xianshi9[j]);
			}
			delay_xs(10000);													   
			init(); //初始化
		}
	}
}
void keydeal12()	   //重新输入
{
	init();
}
void keydeal13()	   //关门
{
	uchar i;
	if(mimaqueren==1)
	{
		s=1;
	}
	if(mimaqueren==0)
	{
		IN1=0;
		IN2=1;		   //通电，开始关门
		delay_xs(1000);
		delay_xs(1000);
		delay_xs(1000);
		delay_xs(1000);
		IN1=0;
		IN2=0;				
		writecom(0x01); 
		writecom(0x80);//清屏
		for(i=0;i<12;i++)				  //显示“Good bye”
		{	
			writedate(xianshi8[i]); 
		}
		for(i=0;i<4;i++)  //蜂鸣器响两声提示开锁成功
		{
			bump=0; 
			delay_xs(200);
			bump=1; 
			delay_xs(200);
			bump=0; 
			delay_xs(200);
			bump=1; 
			delay_xs(200);
			bump=0;
		}
		l1=0;		   //ledl亮，显示可以下次正常输入
		init();		   //初始化，为下次开门做准备
	}
}
void keydeal14()		   //开门
{
	uchar i,j;
	IN1=1;
	IN2=0;		
	delay_xs(1000);
	delay_xs(1000);
	delay_xs(1000);
	delay_xs(1000);
	IN1=0;
	IN2=0;
	delay_xs(100);
	IN1=0;
	IN2=0;		   
	writecom(0x01); 
	writecom(0x80);//清屏
	for(i=0;i<15;i++)				  //显示“please come in”
	{	
		writedate(xianshi1[i]); 
	}
	delay_xs(1000);		  //延时，保证门确实开开
	for(j=0;j<4;j++)  //蜂鸣器响两声提示开锁成功
	{
		bump=0; 
		delay_xs(200);
		bump=1; 
		delay_xs(200);
		bump=0; 
		delay_xs(200);
		bump=1; 
		delay_xs(200);
		bump=0;
	}	
	l1=1;
	writecom(0x01); 
	writecom(0x80);//清屏
	for(i=0;i<15;i++)				  //显示“close the door”
	{	
		writedate(xianshi2[i]); 
	}
	while(P3!=0xe7);	//等待关门
	init();		  //关门后初始化，为下次输入做准备
}
void compare() //比较密码正确与否函数
{
	uchar j,i=0;
	for(j=0;j<8;j++)
	{
		if(table[j]==mima[j])
		{			
			i=i+1 ;
		}
	}
	if(i==8)
	{
		cpflog=1;	 //密码正确标志置1
	}	 
}
/******************键盘扫描函数**************************/
void keyscan()
{
	uchar temp;
	/**************第一行扫描********************/
	P3=0xfe;
	temp=P3&0xf0;
	if(temp!=0xf0)
	{
		delay_xs(10);
		if(temp!=0xf0) 
			temp=P3;
		switch(temp)
		{
			case 0xee: keydeal1(); break;
			case 0xde: keydeal2(); break;
			case 0xbe: keydeal3(); break;
			case 0x7e: keydeal4(); break;
		}
		while((P3&0xf0)!=0xf0); //松手检测
	}
/******************第二行扫描**********************/
	P3=0xfd;
	temp=P3&0xf0;
	if(temp!=0xf0)
	{
		delay_xs(10);
		if(temp!=0xf0) temp=P3;
		switch(temp)
		{
			case 0xed: keydeal5(); break;
			case 0xdd: keydeal6(); break;
			case 0xbd: keydeal7(); break;
			case 0x7d: keydeal8(); break;		
		}
		while((P3&0xf0)!=0xf0);   //松手检测
	}
/*****************第三行扫描*********************/
	P3=0xfb; 
	temp=P3&0xf0;
	if(temp!=0xf0)
	{
	delay_xs(10);
	if(temp!=0xf0) 
	temp=P3;
	switch(temp)
	{
		case 0xeb: keydeal9(); break;
		case 0xdb: keydeal10(); break;
		case 0xbb: keydeal11(); break;
		case 0x7b: keydeal12(); break;	
	}
		while((P3&0xf0)!=0xf0);	
	}
/********************第四行扫描********************/
	P3=0xf7;
	temp=P3&0xf0;
	if(temp!=0xf0)
		{
		delay_xs(10);
		if(temp!=0xf0) 
		temp=P3;
		switch(temp)
		{
			case 0xe7: keydeal13();	break;
			case 0xd7: keydeal14();	break;
			case 0xb7: /*keydeal15();*/ break;
			case 0x77: /*keydeal16();*/ break;
		}
		while((P3&0xf0)!=0xf0);	
	}
}
/****************显示函数*************/
void display()
{
	uint c;
	writecom(0x80+0x40+4);
	for(c=0;c<num;c++)
	{ 
		writedate('*'); 
		delay_xs(5); 
	}
}
/*******************初始化函数***********/
void init()
{
	uint a,b;
	bump=0;
	num=0;  //输入数据个数清零
	etimes=0;
	IN1=0;	//关门
	IN2=0;
	l1=0;   //关闭开锁灯
	delay_xs(5); 
	rw=0;
	writecom(0x38);   //显示模式设置：16×2显示，5×7点阵，8位数据接口
	delay_xs(20);
	writecom(0x0f);   //显示模式设置
	delay_xs(20);
	writecom(0x06);   //显示模式设置：光标右移，字符不移
	delay_xs(20);
	writecom(0x01);   //清屏幕指令，将以前的显示内容清除
	delay_xs(20);
	for(a=0;a<16;a++) //显示input password：
	{
		writedate(xianshi0[a]); 
		delay_xs(5);
	}
	writecom(0x80+0x40+4); //第二行显示--------
	for(b=0;b<8;b++)
	{ 
		writedate('-'); 
	} 
}
/*****************主函数******************/
void main()
{
	init();   //初始化
	while(1) //不停地对键盘和显示进行扫描
	{
		keyscan(); //键盘扫描和密码输入处理
		display(); //LCD显示
	}
}