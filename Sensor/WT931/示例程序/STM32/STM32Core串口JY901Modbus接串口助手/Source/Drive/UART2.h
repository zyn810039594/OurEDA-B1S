#ifndef __UART2_H
#define __UART2_H

void Initial_UART2(unsigned long baudrate);
void UART2_Put_Char(unsigned char DataToSend);
void UART2_Put_String(unsigned char *Str,unsigned char length);
#endif

//------------------End of File----------------------------

