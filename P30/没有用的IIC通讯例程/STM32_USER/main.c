/////////////////////////////////////////////////////////////////////////////////////////		 
//B30 深度传感器测试例程
//开发板：BlueTest STM32
//交流论坛: www.Bluerobots.cn （BlueRobots 水下机器人社区）
//修改日期: 2019/4/30
//例程版本：V1.2
//联系邮箱：info@bluerobots.cn
//特别声明：本程序来源于网络，经BlueRobots 社区网友修改后用于交流分享，使用者自行承担一切后果！
/////////////////////////////////////////////////////////////////////////////////////////	

#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "myiic.h"
#include "MS5837.h"
int main(void)
{
	SystemInit();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);                          // 设置中断优先级分组2
	delay_init();	    	                                                     //延时函数初始化	  
	uart_init(115200);	                                                     //串口初始化
	delay_ms(100);
	IIC_Init();	                                                             //初始化BlueTest STM32开发板 IIC口(模拟IIC)
	delay_ms(100);

	while (1)
	{
		MS5837_30BA_ReSet();	                                                   //复位MS5837
		delay_ms(20);
		MS5837_30BA_PROM();                                                     //初始化MS5837
		delay_ms(20);
		if (!MS5837_30BA_Crc4())                                               //CRC校验
		{
			printf("  初始化失败\r\n");
			printf("  请检查接线是否正确\r\n");

		}
		else
		{
			printf("  初始化成功\r\n");
			printf("  检测到MS5837_30BA\r\n\r\n");
			break;
		}
	}
	while (1)
	{
		MS5837_30BA_GetData();                                      //获取数据 
		delay_ms(200);
		printf("  Welcome to BlueRobots Community! \r\n");          //串口输出原始数据
		printf("  Temperature : %.2f C \r\n", Temperature);          //串口输出原始数据
		printf("  Pressure : %u mBar \r\n\r\n\r\n", Pressure);       //串口输出原始数据

	}


}
// BlueRobots Lab	


