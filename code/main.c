#include<reg52.h>
#include<intrins.h>

//引脚定义
sbit LSA = P1^0;//三八译码器片选引脚
sbit LSB = P1^1;
sbit LSC = P1^2;
sbit IO = P1^4;//DS1302数据引脚
sbit CE = P1^5;//总开关
sbit SCLK = P1^3;//时序引脚
#define LED P2//数码管接口
//关键词定义
typedef unsigned int uint;
typedef unsigned char uchar; 

//函数声明
void writeinit();//初始化（开关写保护）
void writetime(uchar waddr,uchar wdat);//写初始时间
void timestorage();	//将时间存储至数组Time
uchar readtime(uchar raddr);//读取时间
void light();//数码管驱动
void delay(uchar i);//延迟


uchar code WriteAddr[8] = {0x80,0x82,0x84,0x86,0x88,0x8a,0x8c,0x8e};//写入初始化地址
uchar code InitTime[8] = {0x00,0x25,0x19,0x00,0x00,0x00,0x00,0x00};//写入初始化时间
//设定时间为：          秒   分   时   日   月   周      年 
uchar ReadAddr[8] = {0x81,0x83,0x85,0x87,0x89,0x8b,0x8d,0x8f};//写入读取时间地址
uchar Time[8];//读取的时间
uint BCDcode[16];//数码管对应的时间代码
//uint code LEDplay[16] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};//从0到9对应的数码管代码
uint code LEDplay[16] = {0xfc,0x60,0xda,0xf2,0x66,0xb6,0xbe,0xe0,0xfe,0xf6};

/********************************************************************************
*函数名：delay
*功能：延迟
*输入：延迟的时间长度
*输出：
********************************************************************************/
void delay(uchar i){
	while(i){
		i--;
	}
}

/********************************************************************************
*函数名：writeinit
*功能：初始化（开关写保护并调用writetime函数逐位写入时间数据）
*输入：
*输出：
********************************************************************************/
void writeinit(){
	uchar i;
	writetime(WriteAddr[8],InitTime[8]);//开写保护地址0x8e数据0x00 
	for(i=0;i<=6;i++){
		writetime(WriteAddr[i],InitTime[i]);//写入初始时间
	}
	writetime(WriteAddr[8],InitTime[8]);//关写保护地址0x8e数据0x00 
}

/********************************************************************************
*函数名：
*功能：写入一位(BCD)初始时间数据
*输入：addr,dat
*输出：
********************************************************************************/ 
void writetime(uchar addr,uchar dat){
	uchar a;
	CE = 0;//关闭总开关
	_nop_();
	SCLK = 0;//时序置低电平
	_nop_();
	CE = 1;//打开总开关
	_nop_();

	for(a=0;a<=7;a++){//写地址 
		IO = addr & 0x01;//传入一位地址
		addr >>= 1;//地址右移准备传递下一位
		SCLK = 1;//时序上升沿触发
		_nop_();
		SCLK = 0;//时序置低电平
		_nop_();
	}		
	for(a=0;a<=7;a++){//写数据 
		IO = dat & 0x01;//传入一位数据
		dat >>= 1;//数据右移准备传递下一位
		SCLK = 1;//时序上升沿触发
		_nop_();
		SCLK = 0;//时序置低电平
		_nop_();
	}
	CE = 0;//关闭总开关
	_nop_();
}

/********************************************************************************
*函数名：timestorage
*功能：逐位读取时间数据，将个位和十位一起存储的数据改为逐一存储并改为数码管对应代码
*输入：
*输出：
********************************************************************************/
void timestorage(){
	uchar i;
	for(i=0;i<=7;i++){ //调用读取时间函数7次并将时间数据存入Time数组
		Time[i] = readtime(ReadAddr[i]);	
	} 	//将个位和十位一起存储的数据改为逐一存储并改为数码管对应代码
	/*适配硬件bug临时注释
	BCDcode[0] = LEDplay[Time[0]&0x0f];
	Time[0] = Time[0] >> 4; 
	BCDcode[1] = LEDplay[Time[0]];			    	//秒
	BCDcode[2] = 0x02;								//-
	BCDcode[3] = LEDplay[Time[1]&0x0f];	
	Time[1] = Time[1] >> 4; 
	BCDcode[4] = LEDplay[Time[1]];			    	//分
	BCDcode[5] = 0x02;								//-
	BCDcode[6] = LEDplay[Time[2]&0x0f];	
	Time[2] = Time[2] >> 4; 
	BCDcode[7] = LEDplay[Time[2]];			    	//时
	*/
	BCDcode[4] = LEDplay[Time[0]&0x0f];
	Time[0] = Time[0] >> 4; 
	BCDcode[5] = LEDplay[Time[0]];			    	//秒
	BCDcode[6] = 0x02;								//-
	BCDcode[7] = LEDplay[Time[1]&0x0f];	
	Time[1] = Time[1] >> 4; 
	BCDcode[2] = LEDplay[Time[1]];			    	//分
	BCDcode[3] = 0x02;								//-
	BCDcode[0] = LEDplay[Time[2]&0x0f];	
	Time[2] = Time[2] >> 4; 
	BCDcode[1] = LEDplay[Time[2]];			    	//时
			 
} 

/********************************************************************************
*函数名：readtime
*功能：读取一位(BCD)时间数据
*输入：addr
*输出：dat
********************************************************************************/
//将时间存储至数组Time函数
uchar readtime(uchar addr){
	uchar a,d;
	uchar dat,newdat;
	dat = 0x00;//初始数据归0
	CE = 0;//关闭总开关
	_nop_();
	SCLK = 0;//时序置低电平
	_nop_();
	CE = 1;//打开总开关
	_nop_();
	
	for(a=0;a<=7;a++){//写地址 
		IO = addr & 0x01;//传入一位地址
		addr >>= 1;//地址右移准备传递下一位
		SCLK = 1;//时序上升沿触发
		_nop_();
		SCLK = 0;//时序置低电平
		_nop_();
	}
	for(d=0;d<=7;d++){//写时间
		newdat = IO;//读取一位
		dat = (dat>>1) | (newdat<<7);//将读取到的一位数据与之前的数据合并
		SCLK = 1;//时序上升沿触发
		_nop_();
		SCLK = 0;//时序置低电平
		_nop_();	
	}
	CE = 0;//时序置低电平
	_nop_();
	SCLK = 1;//时序上升沿触发
	_nop_();
	//延迟复位
	IO = 0;
	_nop_();
	IO = 1;
	_nop_();
	return dat;//返回读取到的时间
}

/********************************************************************************
*函数名：light
*功能：驱动数码管显示数据
*输入：
*输出：
********************************************************************************/
void light(){
	uchar i;
	for(i=0;i<=7;i++){
		switch (i) {
			case 0://LED1(最右边的灯) 
				LSA=0;LSB=0;LSC=0;
				break; 	
			case 1://LED2
				LSA=1;LSB=0;LSC=0;
				break;	
			case 2://LED3
				LSA=0;LSB=1;LSC=0;
				break;
			case 3://LED4
				LSA=1;LSB=1;LSC=0;
				break;
			case 4://LED5
				LSA=0;LSB=0;LSC=1;
				break;
			case 5://LED6
				LSA=1;LSB=0;LSC=1;
				break;	  
			case 6://LED7
				LSA=0;LSB=1;LSC=1;
				break; 
			case 7://LED8(最左边的灯)
				LSA=1;LSB=1;LSC=1;
				break; 
		}
		LED = BCDcode[i];
		delay(100);
		LED = 0x00;
	}
}

/********************************************************************************
*函数名：main
*功能：
*输入：
*输出：
********************************************************************************/
void main(){	
	writeinit();
	while(1){
		timestorage();
		light();
	}
}



