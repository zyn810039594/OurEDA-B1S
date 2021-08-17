/*
编写者：Kevin
网址：http://RobotControl.taobao.com
作者E-mail：1609370741@qq.com
编译环境：MDK-Lite  Version: 5.17
初版时间: 2016-1-31
测试： 本程序已在【君悦智控】的STM32Core平台上完成测试
功能：
用STM32Core平台串口2读取JY901的数据，然后通过串口1打印到串口助手,串口助手波特率要选为9600。
JY-901的波特率要修改为9600.
注意：示例程序输出的是ASCLL码，用16进制（HEX）显示是不能看到准确数据的。
硬件接线：
USB-TTL工具                 STM32Core              									 JY901
VCC          -----           VCC        --------------------------   VCC
TX           -----           RX1（管脚10）     									
RX           -----           TX1（管脚9）									
GND          -----           GND        --------------------------   GND
              TX2（管脚2）RX2（管脚3）需要转换成485总线电平
																														A    ----  A
																														B    ----  B
------------------------------------
 */
#include <string.h>
#include <stdio.h>
#include "Main.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "UART1.h"
#include "UART2.h"
#include "delay.h"
#include "JY901.h"
#include "DIO.h"

struct STime		stcTime;
struct SAcc 		stcAcc;
struct SGyro 		stcGyro;
struct SAngle 	stcAngle;
struct SMag 		stcMag;
struct SDStatus stcDStatus;
struct SPress 	stcPress;
struct SLonLat 	stcLonLat;
struct SGPSV 		stcGPSV;
struct SQ       stcQ;

unsigned char cmd[8] = {0X50,0X03,0X00,0X34,0X00,0X0C,0X09,0X80};//读取0X34之后的12个寄存器

void CharToShort(unsigned char cTemp[],short sTemp[],short sShortNum)
{
	int i;
	for (i = 0;i<3;i++) 
		sTemp[i] = (cTemp[2*i+sShortNum]<<8)|(cTemp[2*i+sShortNum+1]&0xff);
}

//CopeSerialData为串口2中断调用函数，串口每收到一个数据，调用一次这个函数。
void CopeSerial2Data(unsigned char ucData)
{
	static unsigned char ucRxBuffer[250];
	static unsigned char ucRxCnt = 0;	
	

	ucRxBuffer[ucRxCnt++]=ucData;	//将收到的数据存入缓冲区中
	if (ucRxBuffer[0]!=0x50) //数据头不对，则重新开始寻找0x55数据头
	{
		ucRxCnt=0;
		return;
	}
	if (ucRxCnt<29) {return;}//数据不满29个，则返回
	else
	{
		ucRxCnt=0;//清空缓存区，将接收缓冲区的字符拷贝到数据结构体里面，从而实现数据的解析。
		CharToShort(ucRxBuffer,stcAcc.a,3);
		CharToShort(ucRxBuffer,stcGyro.w,9);
		CharToShort(ucRxBuffer,stcMag.h,15);
		CharToShort(ucRxBuffer,stcAngle.Angle,21);		
	}
}

void CopeSerial1Data(unsigned char ucData)
{	
	UART2_Put_Char(ucData);//转发串口1收到的数据给串口2（JY模块）
}


int main(void)
{  
	char str[100];
		
	SysTick_init(72,10);//设置时钟频率
	Initial_UART1(9600);//接PC的串口
	Initial_UART2(9600);//接WT901C485模块的串口	
	
	LED_ON();
	delay_ms(1000);//等等WT901C485模初始化完成
	while(1)
	{			
		UART2_Put_String(cmd,8);//发送读取0X34之后的12个寄存器，也就是加速度 角速度 角度和磁场
			delay_ms(500);
		//输出加速度
		printf("Acc:%.3f %.3f %.3f\r\n",(float)stcAcc.a[0]/32768*16,(float)stcAcc.a[1]/32768*16,(float)stcAcc.a[2]/32768*16);
			delay_ms(10);
		//输出角速度
		printf("Gyro:%.3f %.3f %.3f\r\n",(float)stcGyro.w[0]/32768*2000,(float)stcGyro.w[1]/32768*2000,(float)stcGyro.w[2]/32768*2000);
			delay_ms(10);
		//输出角度
		printf("Angle:%.3f %.3f %.3f\r\n",(float)stcAngle.Angle[0]/32768*180,(float)stcAngle.Angle[1]/32768*180,(float)stcAngle.Angle[2]/32768*180);
			delay_ms(10);
		//输出磁场
		printf("Mag:%d %d %d\r\n\r\n",stcMag.h[0],stcMag.h[1],stcMag.h[2]);	
		    delay_ms(10);//等待传输完成
	}//主循环
}



