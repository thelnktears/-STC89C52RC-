/*������������������������������������*/
/*����������������86445131������������*/

#include<reg52.h>

#define uchar unsigned char
#define uint unsigned int

sbit l1=P2^0;
sbit bump=P1^3; //����������
sbit IN1=P1^0;//��������
sbit IN2=P1^1;
sbit rs=P2^3;	  //Һ��������λ
sbit rw=P2^4;	  //��дѡ���
sbit lcden=P2^5;  //ʹ�ܿ��ƶ�

/**********��ʾ����**************************/
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
static uchar table[8]; //������������ȡ�洢�ռ�
static uchar mima[8]={8,6,4,4,5,1,3,1};
uchar num=0,etimes=0,s=0;
bit cpflog=0,mimaqueren=0;

void compare();	//����ȽϺ���
void keydeal13();	//����
void keydeal14();	//����
void keyscan();		//����ɨ�躯��
void init();	//��ʼ������

/********************��ʱһ���뺯��**************/ 
void delay_xs(uint x)
{   
	uint i,j;
	for(i=x;i>0;i--)
	for(j=110;j>0;j--)
	;
}
/*********************Һ��д���ݺ���******************/
void writedate(uchar date) //д����
{
	P0=date;
	rs=1;
	lcden=0;
	delay_xs(10);
	lcden=1;
	delay_xs(10);
	lcden=0;
}
/*********************Һ��д�������ݺ���******************/
void writecom(uchar com) //д����
{
	P0=com;
	rs=0;
	lcden=0;
	delay_xs(10);
	lcden=1;
	delay_xs(10);
	lcden=0;
}

/************��������Ӧ������***************/
/*************����������*********************/
/************1-2-3-4-5-6-7-8-9-0************/
/**************���ְ���*******************/
void keydeal1()                 //����1
{
	table[num]=1;
	num++;
}
void keydeal2()                //����2
{
	table[num]=2;
	num++;
}
void keydeal3()               //����3
{
	table[num]=3;
	num++;
}
void keydeal4()               //����4
{
	table[num]=4;
	num++;
}
void keydeal5()               //����5
{
	table[num]=5;
	num++;
}
void keydeal6() //����6
{
	table[num]=6;
	num++;
}
void keydeal7() //����7
{
	table[num]=7;
	num++;
}
void keydeal8()  //����8
{
	table[num]=8;
	num++;
}
void keydeal9()   //����9
{
	table[num]=9;
	num++;
}
void keydeal10()   //����10
{
	table[num]=0;   
	num++;
}

/******************���ܼ�*****************/
/***********ȷ����ȡ�������ţ�����*******/
//ȷ�ϼ�
void keydeal11()  //ȷ�ϼ�
{
	uchar j,i;
	if(num==8)         //�ж��Ƿ�����˸�����
	{
		num=0;         //���������ָ�������
		compare();         //��������Ƚ�
		for(j=0;j<8;j++) //��FFFFFFFF��������������
		{
			table[j]=0;
		}
	}
	if(cpflog==1) //���������ȷ����־λΪ1
	{
		l1=0; //����������
		cpflog=0; //ʹ�Ƚ������־λ����
		etimes=0; //ʹ�Ǵ��������
		IN1=1;
		IN2=0;		 //��ʼ����
		delay_xs(1000);
		delay_xs(1000);
		delay_xs(1000);
		delay_xs(1000);
		IN1=0;
		IN2=0;	    //��ɿ���		  
		writecom(0x01); //Һ���������ʾ
		writecom(0x80); //��Һ����ʾ��please come in����
		for(j=0;j<15;j++)
		{
			writedate(xianshi1[j]);
			delay_xs(3);
		}
		for(j=0;j<2;j++)  //��������������ʾ�����ɹ�
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
		writecom(0x80);//����
		for(i=0;i<15;i++)				  //��ʾ��close the door��
		{	
			writedate(xianshi2[i]); 
		} 
		mimaqueren=1;			  //���ű�־��һ���ȴ�����
		while(s==0)				 //�ȴ�����
		{
			keyscan();			//����ɨ�裬�ȴ����¹��ż�
			l1=~l1;
			delay_xs(100);	
		}
		s=0;
		mimaqueren=0;	//���ű�־���㣬��ʼ����
		keydeal13();	//����
	}
	else //�����������ִ���������ָ��
	{
		num=0; //���������ݸ�������
		etimes++; //��¼���������1 
		for(j=0;j<8;j++)  //����޸�������������
		{ 
			table[j]=0x0f; 
		}
		if(etimes==1)  //������һ��
		{
			writecom(0x01);   //����
			writecom(0x80);
			for(j=0;j<14;j++)   //��ʾfirst error
			{ 
				writedate(xianshi5[j]); 
			}
			delay_xs(2000);   //��ʱ����
			writecom(0xc);
			writecom(0x80);
			for(j=0;j<16;j++)   //��һ����ʾinput passsword
			{
				writedate(xianshi0[j]); 
				delay_xs(5);
			}   
			writecom(0x80+0x40+4); //�ڶ�����ʾ--------
			for(j=0;j<8;j++) 
			{
				writedate('-'); 
			}
		}
		if(etimes==2)   //����������
		{
			writecom(0x01); 
			writecom(0x80);
			for(j=0;j<15;j++)   //��ʾsecond error
			{
				writedate(xianshi6[j]);
			}
			delay_xs(2000);   //��ʱ����
			writecom(0xc);
			writecom(0x80);
			for(j=0;j<16;j++) //��һ����ʾinput passsword
			{
				writedate(xianshi0[j]); 
				delay_xs(5);
			}
			writecom(0x80+0x40+4);
			for(j=0;j<8;j++) //�ڶ�����ʾ--------
			{
				writedate('-');
			}
		}
		if(etimes==3) //����������
		{
			writecom(0x01);
			writecom(0x80);
			for(j=0;j<15;j++) //��ʾ  third error!!
			{
				writedate(xianshi7[j]);
			}
			writecom(0x01);
			writecom(0x80);
			for(j=0;j<15;j++) //��ʾyou are thief��
			{
				writedate(xianshi4[j]);
			}			
			for(j=0;j<10;j++) //����ʮ����
			{
				bump=0; 
				delay_xs(500);
				bump=1; 
				delay_xs(500);
			}
			writecom(0x01);
			writecom(0x80);
			for(j=0;j<16;j++) //��ʾWait a moment
			{
				writedate(xianshi9[j]);
			}
			delay_xs(10000);													   
			init(); //��ʼ��
		}
	}
}
void keydeal12()	   //��������
{
	init();
}
void keydeal13()	   //����
{
	uchar i;
	if(mimaqueren==1)
	{
		s=1;
	}
	if(mimaqueren==0)
	{
		IN1=0;
		IN2=1;		   //ͨ�磬��ʼ����
		delay_xs(1000);
		delay_xs(1000);
		delay_xs(1000);
		delay_xs(1000);
		IN1=0;
		IN2=0;				
		writecom(0x01); 
		writecom(0x80);//����
		for(i=0;i<12;i++)				  //��ʾ��Good bye��
		{	
			writedate(xianshi8[i]); 
		}
		for(i=0;i<4;i++)  //��������������ʾ�����ɹ�
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
		l1=0;		   //ledl������ʾ�����´���������
		init();		   //��ʼ����Ϊ�´ο�����׼��
	}
}
void keydeal14()		   //����
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
	writecom(0x80);//����
	for(i=0;i<15;i++)				  //��ʾ��please come in��
	{	
		writedate(xianshi1[i]); 
	}
	delay_xs(1000);		  //��ʱ����֤��ȷʵ����
	for(j=0;j<4;j++)  //��������������ʾ�����ɹ�
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
	writecom(0x80);//����
	for(i=0;i<15;i++)				  //��ʾ��close the door��
	{	
		writedate(xianshi2[i]); 
	}
	while(P3!=0xe7);	//�ȴ�����
	init();		  //���ź��ʼ����Ϊ�´�������׼��
}
void compare() //�Ƚ�������ȷ�����
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
		cpflog=1;	 //������ȷ��־��1
	}	 
}
/******************����ɨ�躯��**************************/
void keyscan()
{
	uchar temp;
	/**************��һ��ɨ��********************/
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
		while((P3&0xf0)!=0xf0); //���ּ��
	}
/******************�ڶ���ɨ��**********************/
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
		while((P3&0xf0)!=0xf0);   //���ּ��
	}
/*****************������ɨ��*********************/
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
/********************������ɨ��********************/
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
/****************��ʾ����*************/
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
/*******************��ʼ������***********/
void init()
{
	uint a,b;
	bump=0;
	num=0;  //�������ݸ�������
	etimes=0;
	IN1=0;	//����
	IN2=0;
	l1=0;   //�رտ�����
	delay_xs(5); 
	rw=0;
	writecom(0x38);   //��ʾģʽ���ã�16��2��ʾ��5��7����8λ���ݽӿ�
	delay_xs(20);
	writecom(0x0f);   //��ʾģʽ����
	delay_xs(20);
	writecom(0x06);   //��ʾģʽ���ã�������ƣ��ַ�����
	delay_xs(20);
	writecom(0x01);   //����Ļָ�����ǰ����ʾ�������
	delay_xs(20);
	for(a=0;a<16;a++) //��ʾinput password��
	{
		writedate(xianshi0[a]); 
		delay_xs(5);
	}
	writecom(0x80+0x40+4); //�ڶ�����ʾ--------
	for(b=0;b<8;b++)
	{ 
		writedate('-'); 
	} 
}
/*****************������******************/
void main()
{
	init();   //��ʼ��
	while(1) //��ͣ�ضԼ��̺���ʾ����ɨ��
	{
		keyscan(); //����ɨ����������봦��
		display(); //LCD��ʾ
	}
}