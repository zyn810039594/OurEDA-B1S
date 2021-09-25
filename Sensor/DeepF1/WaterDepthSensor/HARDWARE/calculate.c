#include "calculate.h"
/*
	公司：哈诺科技
	作者：杨康
	时间：2019.6
	淘宝：https://shop341500796.taobao.com/?spm=a230r.7195193.1997079397.2.628a4fb6DAI9Pc
*/
//使用时只需从calculatestruct结构体中取出相应的值即可。
calculate_typedef calculatestruct;

uint8_t rxcnt =0;
uint8_t i=0;
void GetJoybuff(uint8_t data)
{
	uint8_t rxbuff[50];
  rxbuff[rxcnt++] = data;
	if(rxbuff[0]!=0xAA){rxcnt = 0;return;}
	if(rxcnt<sizeof(calculatestruct.rx_data)){return;}
	for(i=0;i<sizeof(calculatestruct.rx_data);i++)
	{
	calculatestruct.byte[i] = rxbuff[i];
	}
	rxcnt =0;	
}



