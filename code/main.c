#include<reg52.h>
#include<intrins.h>

//���Ŷ���
sbit LSA = P1^0;//����������Ƭѡ����
sbit LSB = P1^1;
sbit LSC = P1^2;
sbit IO = P1^4;//DS1302��������
sbit CE = P1^5;//�ܿ���
sbit SCLK = P1^3;//ʱ������
#define LED P2//����ܽӿ�
//�ؼ��ʶ���
typedef unsigned int uint;
typedef unsigned char uchar; 

//��������
void writeinit();//��ʼ��������д������
void writetime(uchar waddr,uchar wdat);//д��ʼʱ��
void timestorage();	//��ʱ��洢������Time
uchar readtime(uchar raddr);//��ȡʱ��
void light();//���������
void delay(uchar i);//�ӳ�


uchar code WriteAddr[8] = {0x80,0x82,0x84,0x86,0x88,0x8a,0x8c,0x8e};//д���ʼ����ַ
uchar code InitTime[8] = {0x00,0x25,0x19,0x00,0x00,0x00,0x00,0x00};//д���ʼ��ʱ��
//�趨ʱ��Ϊ��          ��   ��   ʱ   ��   ��   ��      �� 
uchar ReadAddr[8] = {0x81,0x83,0x85,0x87,0x89,0x8b,0x8d,0x8f};//д���ȡʱ���ַ
uchar Time[8];//��ȡ��ʱ��
uint BCDcode[16];//����ܶ�Ӧ��ʱ�����
//uint code LEDplay[16] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};//��0��9��Ӧ������ܴ���
uint code LEDplay[16] = {0xfc,0x60,0xda,0xf2,0x66,0xb6,0xbe,0xe0,0xfe,0xf6};

/********************************************************************************
*��������delay
*���ܣ��ӳ�
*���룺�ӳٵ�ʱ�䳤��
*�����
********************************************************************************/
void delay(uchar i){
	while(i){
		i--;
	}
}

/********************************************************************************
*��������writeinit
*���ܣ���ʼ��������д����������writetime������λд��ʱ�����ݣ�
*���룺
*�����
********************************************************************************/
void writeinit(){
	uchar i;
	writetime(WriteAddr[8],InitTime[8]);//��д������ַ0x8e����0x00 
	for(i=0;i<=6;i++){
		writetime(WriteAddr[i],InitTime[i]);//д���ʼʱ��
	}
	writetime(WriteAddr[8],InitTime[8]);//��д������ַ0x8e����0x00 
}

/********************************************************************************
*��������
*���ܣ�д��һλ(BCD)��ʼʱ������
*���룺addr,dat
*�����
********************************************************************************/ 
void writetime(uchar addr,uchar dat){
	uchar a;
	CE = 0;//�ر��ܿ���
	_nop_();
	SCLK = 0;//ʱ���õ͵�ƽ
	_nop_();
	CE = 1;//���ܿ���
	_nop_();

	for(a=0;a<=7;a++){//д��ַ 
		IO = addr & 0x01;//����һλ��ַ
		addr >>= 1;//��ַ����׼��������һλ
		SCLK = 1;//ʱ�������ش���
		_nop_();
		SCLK = 0;//ʱ���õ͵�ƽ
		_nop_();
	}		
	for(a=0;a<=7;a++){//д���� 
		IO = dat & 0x01;//����һλ����
		dat >>= 1;//��������׼��������һλ
		SCLK = 1;//ʱ�������ش���
		_nop_();
		SCLK = 0;//ʱ���õ͵�ƽ
		_nop_();
	}
	CE = 0;//�ر��ܿ���
	_nop_();
}

/********************************************************************************
*��������timestorage
*���ܣ���λ��ȡʱ�����ݣ�����λ��ʮλһ��洢�����ݸ�Ϊ��һ�洢����Ϊ����ܶ�Ӧ����
*���룺
*�����
********************************************************************************/
void timestorage(){
	uchar i;
	for(i=0;i<=7;i++){ //���ö�ȡʱ�亯��7�β���ʱ�����ݴ���Time����
		Time[i] = readtime(ReadAddr[i]);	
	} 	//����λ��ʮλһ��洢�����ݸ�Ϊ��һ�洢����Ϊ����ܶ�Ӧ����
	/*����Ӳ��bug��ʱע��
	BCDcode[0] = LEDplay[Time[0]&0x0f];
	Time[0] = Time[0] >> 4; 
	BCDcode[1] = LEDplay[Time[0]];			    	//��
	BCDcode[2] = 0x02;								//-
	BCDcode[3] = LEDplay[Time[1]&0x0f];	
	Time[1] = Time[1] >> 4; 
	BCDcode[4] = LEDplay[Time[1]];			    	//��
	BCDcode[5] = 0x02;								//-
	BCDcode[6] = LEDplay[Time[2]&0x0f];	
	Time[2] = Time[2] >> 4; 
	BCDcode[7] = LEDplay[Time[2]];			    	//ʱ
	*/
	BCDcode[4] = LEDplay[Time[0]&0x0f];
	Time[0] = Time[0] >> 4; 
	BCDcode[5] = LEDplay[Time[0]];			    	//��
	BCDcode[6] = 0x02;								//-
	BCDcode[7] = LEDplay[Time[1]&0x0f];	
	Time[1] = Time[1] >> 4; 
	BCDcode[2] = LEDplay[Time[1]];			    	//��
	BCDcode[3] = 0x02;								//-
	BCDcode[0] = LEDplay[Time[2]&0x0f];	
	Time[2] = Time[2] >> 4; 
	BCDcode[1] = LEDplay[Time[2]];			    	//ʱ
			 
} 

/********************************************************************************
*��������readtime
*���ܣ���ȡһλ(BCD)ʱ������
*���룺addr
*�����dat
********************************************************************************/
//��ʱ��洢������Time����
uchar readtime(uchar addr){
	uchar a,d;
	uchar dat,newdat;
	dat = 0x00;//��ʼ���ݹ�0
	CE = 0;//�ر��ܿ���
	_nop_();
	SCLK = 0;//ʱ���õ͵�ƽ
	_nop_();
	CE = 1;//���ܿ���
	_nop_();
	
	for(a=0;a<=7;a++){//д��ַ 
		IO = addr & 0x01;//����һλ��ַ
		addr >>= 1;//��ַ����׼��������һλ
		SCLK = 1;//ʱ�������ش���
		_nop_();
		SCLK = 0;//ʱ���õ͵�ƽ
		_nop_();
	}
	for(d=0;d<=7;d++){//дʱ��
		newdat = IO;//��ȡһλ
		dat = (dat>>1) | (newdat<<7);//����ȡ����һλ������֮ǰ�����ݺϲ�
		SCLK = 1;//ʱ�������ش���
		_nop_();
		SCLK = 0;//ʱ���õ͵�ƽ
		_nop_();	
	}
	CE = 0;//ʱ���õ͵�ƽ
	_nop_();
	SCLK = 1;//ʱ�������ش���
	_nop_();
	//�ӳٸ�λ
	IO = 0;
	_nop_();
	IO = 1;
	_nop_();
	return dat;//���ض�ȡ����ʱ��
}

/********************************************************************************
*��������light
*���ܣ������������ʾ����
*���룺
*�����
********************************************************************************/
void light(){
	uchar i;
	for(i=0;i<=7;i++){
		switch (i) {
			case 0://LED1(���ұߵĵ�) 
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
			case 7://LED8(����ߵĵ�)
				LSA=1;LSB=1;LSC=1;
				break; 
		}
		LED = BCDcode[i];
		delay(100);
		LED = 0x00;
	}
}

/********************************************************************************
*��������main
*���ܣ�
*���룺
*�����
********************************************************************************/
void main(){	
	writeinit();
	while(1){
		timestorage();
		light();
	}
}



