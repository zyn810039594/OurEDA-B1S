#ifndef _CALCULATE_H_
#define _CALCULATE_H_

#include "stm32f4xx.h"

typedef union{
		struct rx{
	uint8_t datahead;
	uint8_t XPos;
	uint8_t YPos;
	uint8_t ZPos;
	uint8_t RZPos;
	uint8_t L2Pos;
	uint8_t R2Pos;
	uint8_t X;
	uint8_t Y;
	uint8_t A;
	uint8_t B;
	uint8_t btn_left;
	uint8_t btn_right;
	uint8_t btn_up;
	uint8_t btn_down;
	uint8_t L1;
	uint8_t R1;
	uint8_t select;
	uint8_t start;
	uint8_t home;
	uint8_t leftpos;
	uint8_t rightpos;
 }rx_data;
  uint8_t byte[sizeof(struct rx)];

	}calculate_typedef;

extern calculate_typedef calculatestruct;


void GetJoybuff(uint8_t data);








#endif /*_CALCULATE_H_*/


