/*  beep.c  */

#include "beep.h"

/***********************************************************************/
//此程序适用于七彩光子太极M3—STM32F103开发板及部分兼容的STM32系统，
//只供学习使用，未经作者许可，不得用于其它任何用途！
/***********************************************************************/ 

//初始化BEEP所在管脚为输出口.并使能这个口的时钟		    

void Beep_Init(void)
{
 GPIO_InitTypeDef  GPIO_InitStructure;
	
 RCC_APB2PeriphClockCmd(BEEP_GPIO_RCC_CLK, ENABLE);	 //使能PB,PE端口时钟
  
  BEEP_OFF();
  
 GPIO_InitStructure.GPIO_Pin = BEEP_PIN;				       //LED0端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(BEEP_GPIO, &GPIO_InitStructure);					   //根据设定参数初始化LED0
 //GPIO_SetBits(BEEP_GPIO,BEEP_PIN);						         //LED0输出高
}
 
