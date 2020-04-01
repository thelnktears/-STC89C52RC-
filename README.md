---
title: 基于STC89C52RC单片机的密码门锁
date: 2020-03-12 15:57:36
tags:
- 目录
- 教程
- 博客
- 门锁
- STC89C52
---

# 门锁介绍
这个门锁是我2018年暑假做的一个小作品，是我入门单片机后的第二个作品，第一个是一个船，使用串口通讯，完成了对船的遥控，而在假期，利用放假时间做了这个门锁，做出来已经好长时间了，就做个简单的展示，为刚学51单片机、而且想做一个自己作品的同学提供一个参考。门锁整体由外部（安装后的室外）矩阵键盘和1602显示器，一个电源开关、内部（室内）主控芯片，两个开关门状态控制开关，一个电源开关，一个蜂鸣器，一个L298N电机驱动及一个电动推杆组成。在外部通过4*4矩阵键盘输入密码和操作控制，LCD1602显示器显示状态提示或已输入密码位数；内部由两个按键分别控制开关门。开门或关门后有灯光和蜂鸣器提示状态。

<!-- more -->

# 门锁软硬件介绍
## STC8989C52单片机
STC89C52RC是STC公司生产的一种低功耗、高性能CMOS8位微控制器，具有8K字节系统可编程Flash存储器。STC89C52使用经典的MCS-51内核，但是做了很多的改进使得芯片具有传统51单片机不具备的功能。在单芯片上，拥有灵巧的8 位CPU 和在系统可编程Flash，使得STC89C52为众多嵌入式控制应用系统提供高灵活、超有效的解决方案。
 8k字节Flash，512字节RAM， 32 位I/O 口线，看门狗定时器，内置4KB EEPROM，MAX810复位电路，3个16 位定时器/计数器，4个外部中断，一个7向量4级中断结构（兼容传统51的5向量2级中断结构），全双工串行口。另外 STC89C52 可降至0Hz 静态逻辑操作，支持2种软件可选择节电模式。空闲模式下，CPU 停止工作，允许RAM、定时器/计数器、串口、中断继续工作。掉电保护方式下，RAM内容被保存，振荡器被冻结，单片机一切工作停止，直到下一个中断或硬件复位为止。最高运作频率35MHz，6T/12T可选。

  ![在这里插入图片描述](https://img-blog.csdnimg.cn/20200117223525545.png)

## LCD1602显示屏
工业字符型液晶，能够同时显示16x02即32个字符，16列2行）。1602液晶也叫1602字符型液晶，它是一种专门用来显示字母、数字、符号等的点阵型液晶模块。它由若干个5X7或者5X11等点阵字符位组成，每个点阵字符位都可以显示一个字符，每位之间有一个点距的间隔，每行之间也有间隔，起到了字符间距和行间距的作用，正因为如此所以它不能很好地显示图形（用自定义CGRAM，显示效果也不好）。1602LCD是指显示的内容为16X2,即可以显示两行，每行16个字符液晶模块（显示字符和数字）。市面上字符液晶大多数是基于HD44780液晶芯片的，控制原理是完全相同的，因此基于HD44780写的控制程序可以很方便地应用于市面上大部分的字符型液晶。
![lcd1602](https://img-blog.csdnimg.cn/20200117223951195.png)
LCD1602操作参考：
链接：[https://blog.csdn.net/weixin_42133183/article/details/80911996]( https://blog.csdn.net/weixin_42133183/article/details/80911996)
链接：[https://blog.csdn.net/qq_34706280/article/details/53165561](https://blog.csdn.net/qq_34706280/article/details/53165561)
链接 :[https://blog.csdn.net/weixin_42625444/article/details/84331059](https://blog.csdn.net/weixin_42625444/article/details/84331059)
上面两个大佬的介绍很详尽了，而我也基本参考各种例程，将其简单驱动起来，显示我想要的字符。
故，我用P2 ^3作为LCD寄存器选择引脚，P2 ^4作为读写选择引脚，P2 ^5作为读写使能引脚，P0作为数据引脚。根据时序写出写数据和写命令函数。

```
/*********************液晶写数据函数******************/
void writedate(uchar date) //写命令
{
	P0=date;			//写入命令
	rs=1;				//选择数据寄存器
	lcden=0;			//写入数据
	delay_xs(10);		//稍作延时保证数据写入
	lcden=1;			//停止写入
	delay_xs(10);
	lcden=0;
}
/*********************液晶写命令函数******************/
void writecom(uchar com) //写命令
{
	P0=com;				//写入命令
	rs=0;               //选择指令寄存器
	lcden=0;            //写入数据
	delay_xs(10);       //稍作延时保证数据写入
	lcden=1;            //停止写入
	delay_xs(10);
	lcden=0;
}
```
## 矩阵键盘
为了减少I/O口的占用，将按键排列成矩阵形式。在矩阵式键盘中，每条水平线和垂直线在交叉处不直接连通，而是通过一个按键加以连接。这样，一个端口（如P3口）就可以构成4*4=16个按键。当按键没有按下时，所有的输入端都是高电平，代表无键按下。行线输出是低电平，一旦有键按下，则输入线就会被拉低，这样，通过读入输入线的状态就可得知是否有键按下了。确定矩阵式键盘上何键被按下介绍一种“行扫描法”。行扫描法又称为逐行（或列）扫描查询法，是一种最常用的按键识别方法。
1、判断键盘中有无键按下 将全部行线置低电平，然后检测列线的状态。只要有一列的电平为低，则表示键盘中有键被按下，而且闭合的键位于低电平线与4根行线相交叉的4个按键之中。若所有列线均为高电平，则键盘中无键按下。
2、判断闭合键所在的位置 在确认有键按下后，即可进入确定具体闭合键的过程。其方法是：依次将行线置为低电平，即在置某根行线为低电平时，其它线为高电平。在确定某根行线位置为低电平后，再逐行检测各列线的电平状态。若某列为低，则该列线与置为低电平的行线交叉处的按键就是闭合的按键。
门锁为了实现部分功能，有些按键没有使用，有些按键在室内，通过并联按键实现同样功能，具体实现如下代码：
```
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
/*
```
链接：[https://blog.csdn.net/dlf1769/article/details/90599686](https://blog.csdn.net/dlf1769/article/details/90599686)
## L298N电机驱动和电动推杆
逻辑部分实现后为实现门锁的功能，我采用了电动推杆作为门销，对门进行简单改造，利用电动推杆伸出来的杆体将门销住。电动推杆里面是一个电机，所以我采用了一个L298N来驱动电机。当密码正确或者有人在内部开门，就让电机开始正转就行，关门时相反。代码简单，也没写单独的函数，在这就不展示了。
# 门锁实物展示
![在这里插入图片描述](https://img-blog.csdnimg.cn/20200117234627335.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3dlaXhpbl80MjU4NTMxNg==,size_16,color_FFFFFF,t_70)


![在这里插入图片描述](https://img-blog.csdnimg.cn/20200117234806946.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3dlaXhpbl80MjU4NTMxNg==,size_16,color_FFFFFF,t_70)


![在这里插入图片描述](https://img-blog.csdnimg.cn/20200117234858493.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3dlaXhpbl80MjU4NTMxNg==,size_16,color_FFFFFF,t_70)

# 门锁代码展示
```
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
	P0=date;			//写入命令
	rs=1;				//选择数据寄存器
	lcden=0;			//写入数据
	delay_xs(10);		//稍作延时保证数据写入
	lcden=1;			//停止写入
	delay_xs(10);
	lcden=0;
}
/*********************液晶写命令函数******************/
void writecom(uchar com) //写命令
{
	P0=com;				//写入命令
	rs=0;               //选择指令寄存器
	lcden=0;            //写入数据
	delay_xs(10);       //稍作延时保证数据写入
	lcden=1;            //停止写入
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
```
附上整个工程代码，上面代码无法复现可试试工程。
链接:[https://download.csdn.net/download/weixin_42585316/12110359](https://download.csdn.net/download/weixin_42585316/12110359)
# 参考文档
[参考文档](https://blog.csdn.net/matsukawahikari/article/details/83756903)https://blog.csdn.net/matsukawahikari/article/details/83756903
[参考文档](https://blog.csdn.net/dlf1769/article/details/90599686)https://blog.csdn.net/dlf1769/article/details/90599686
[参考文档](https://blog.csdn.net/weixin_42133183/article/details/80911996) https://blog.csdn.net/weixin_42133183/article/details/80911996
[参考文档](https://blog.csdn.net/qq_34706280/article/details/53165561)https://blog.csdn.net/qq_34706280/article/details/53165561
[参考文档](https://blog.csdn.net/weixin_42625444/article/details/84331059)https://blog.csdn.net/weixin_42625444/article/details/84331059
# 总结
制作成中各种困难，从中发现了好多学习过程中没注意到的地方，比如说在学习过程中没太注意矩阵按键行扫描时的数据应该怎么去算，在做的过程中再去回顾，算了好几次，中间多次出错，多次检查更正，总算合适，完成了简单地密码输入及各种逻辑。再用LCD1602时因为之前没怎么用过，花了好长时间，做的过程中慢慢摸索，有点门道后参考例程，完成我想要的结果，之后再做其他小作品的时候慢慢深化。
密码门锁的功能是完成了，但相对简单，功能单一，没有新奇之处，但通过这次制作我对所学到的STC89C52单片机的知识有了很好的深化，之后还会对其完善，加上其他各种功能，比如射频卡、指纹等。
