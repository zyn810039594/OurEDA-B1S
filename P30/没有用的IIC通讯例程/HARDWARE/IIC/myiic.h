#ifndef __MYIIC_H
#define __MYIIC_H
#include "sys.h"
/////////////////////////////////////////////////////////////////////////////////////////		 
//B30 深度传感器测试例程
//开发板：BlueTest STM32
//交流论坛: www.Bluerobots.cn （BlueRobots 水下机器人社区）
//修改日期: 2019/4/30
//例程版本：V1.2
//联系邮箱：info@bluerobots.cn
//特别声明：本程序来源于网络，经BlueRobots 社区网友修改后用于交流分享，使用者自行承担一切后果！
/////////////////////////////////////////////////////////////////////////////////////////	
   	   		   
//IO方向设置, 
#define SDA_IN()  {GPIOC->CRH&=0XFFFF0FFF;GPIOC->CRH|=8<<12;} // PC11 = SDA 作为输入时 
#define SDA_OUT() {GPIOC->CRH&=0XFFFF0FFF;GPIOC->CRH|=3<<12;} // PC11 = SDA 作为输出时

//IO操作函数	 
#define IIC_SCL    PCout(10) //SCL = PC10
#define IIC_SDA    PCout(11) //SDA = PC11 作为输出时
#define READ_SDA   PCin(11)  //SDA = PC11 作为输入时
 
//IIC所有操作函数
void IIC_Init(void);                //初始化IIC的IO口				 
void IIC_Start(void);								//发送IIC开始信号
void IIC_Stop(void);	  						//发送IIC停止信号
void IIC_Send_Byte(u8 txd);					//IIC发送一个字节
u8 IIC_Read_Byte(unsigned char ack);//IIC读取一个字节
u8 IIC_Wait_Ack(void); 							//IIC等待ACK信号
void IIC_Ack(void);									//IIC发送ACK信号
void IIC_NAck(void);								//IIC不发送ACK信号

void IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 IIC_Read_One_Byte(u8 daddr,u8 addr);	  

#endif

// BlueRobots Lab














