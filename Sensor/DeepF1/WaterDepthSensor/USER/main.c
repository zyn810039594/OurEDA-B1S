#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "usart2.h"
#include "ms5837.h"



void updata(void);
int main(void)
{  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2，2位相响应，2位抢断
	delay_init(168);
	LED_Init();
	uart_init(115200);	
	usart2_init(115200);
	LED0 = 0;
	//updata();
	while(1)
	{		
//		updata();
//		OutputData();
//		OutputMS5837();
		ProcessMS5837Data();
//		pritdata();
		delay_ms(100);
		LED0=~LED0;
	}
}


/* 测试获取数据 */
/*
void OutputData(void)
{
	if(ms5837.ms5837rx_data.T !=0 )
	{
		printf("%c ", ms5837.ms5837rx_data.T);
	}
	else if(ms5837.ms5837rx_data.Fe != 0)
	{
		printf("%c ", ms5837.ms5837rx_data.Fe);
	}
	else if(ms5837.ms5837rx_data.temperature != 0 )
	{
		printf("%.2f ", ms5837.ms5837rx_data.temperature);
	}
	else if(ms5837.ms5837rx_data.D != 0)
	{
		printf("%c ",ms5837.ms5837rx_data.D);
	}
	else if(ms5837.ms5837rx_data.Se != 0)
	{
		printf("%c ",ms5837.ms5837rx_data.Se);
	}
	else if(ms5837.ms5837rx_data.pressure != 0)
	{
		printf("%.2f ", ms5837.ms5837rx_data.pressure);
	}
	else if(ms5837.ms5837rx_data.r != 0)
	{
		printf("%c ",ms5837.ms5837rx_data.r);
	}
	else if(ms5837.ms5837rx_data.n != 0)
	{
		printf("%c ",ms5837.ms5837rx_data.n);
	}
}
*/








