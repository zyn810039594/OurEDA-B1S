/**
 * @file surface.ino
 * @brief
 * @author RedlightASl (dddbbbdd@foxmail.com)
 * @version 1.0
 * @date 2021-08-21
 *
 * @copyright Copyright (c) 2021  RedlightASl
 *
 * @par 修改日志:
 * <table>
 * <tr><th>Date       <th>Version <th>Author  <th>Description
 * <tr><td>2021-08-21 <td>1.0     <td>wangh     <td>Content
 * </table>
 */
#include <FlexiTimer2.h>

#define u8 unsigned char
#define NOP do { __asm__ __volatile__ ("nop"); } while (0)

u8 UART_REV_RING_FLAG = 0; //串口已接收标志位








u8 DeepFlag = 0;
u8 DeepNum = 0;
u8 DirectFlag = 0;
u8 DirectNum = 0;
u8 ControlFlag = 0;
u8 ControlNum = 0;
u8 ArmFlag = 0;
u8 ArmNum = 0;
u8 SideFlag = 0;
u8 SideNum = 0;
u8 LeafFlag = 0;
u8 LeafNum = 8;

int ALR[16] = { 0 };
int ALT[16] = { 0 };
u8 MoveState = 0;
u8 UpState = 0;
u8 Arm1State[5] = { 0 };
u8 Arm2State[5] = { 0 };
u8 LightState = 0;
u8 THState = 0;
u8 TimerState = 0;

void PrintBlank(void)
{
	int i = 50000;
	for (;i > 0;--i)
	{
		NOP;
	}
}

void Surface_ReadSerialata(void);







/**
 * @brief 定时器中断服务程序
 */
void TimerInterrupt()
{
	if (LeafFlag == 1)
	{
		++TimerState;
		switch (TimerState)
		{
		case 25:
			LeafNum = 8;
			LeafFlag = 0;
			TimerState = 0;
			break;
		}
	}

	digitalWrite(2, UART_REV_RING_FLAG);

	Serial.print('$');
	Serial.print(ControlNum);
	Serial.print(':');
	Serial.print(SideNum);
	Serial.print(':');
	if ((ALR[1]) < 100)
	{
		Serial.print("0");
		if ((ALR[1]) < 10)
		{
			Serial.print("0");
		}
	}
	Serial.print(ALR[1]);
	Serial.print(':');
	if ((ALR[0]) < 100)
	{
		Serial.print("0");
		if ((ALR[0]) < 10)
		{
			Serial.print("0");
		}
	}
	Serial.print(ALR[0]);
	Serial.print(":");
	if ((ALR[5]) < 100)
	{
		Serial.print("0");
		if ((ALR[5]) < 10)
		{
			Serial.print("0");
		}
	}
	Serial.print(ALR[5]);
	Serial.print(":");
	if ((ALR[4]) < 100)
	{
		Serial.print("0");
		if ((ALR[4]) < 10)
		{
			Serial.print("0");
		}
	}
	Serial.print(ALR[4]);
	Serial.print(":");
	Serial.print(ArmNum);
	Serial.print(':');
	if ((ALR[2]) < 100)
	{
		Serial.print("0");
		if ((ALR[2]) < 10)
		{
			Serial.print("0");
		}
	}
	Serial.print(ALR[2]);
	Serial.print(":");

	Serial.print("00");
	Serial.print(DeepNum);
	Serial.print(':');
	Serial.print("00");
	Serial.print(DirectNum);
	Serial.print(':');
	if ((ALR[6]) < 100)
	{
		Serial.print("0");
		if ((ALR[6]) < 10)
		{
			Serial.print("0");
		}
	}
	Serial.print(ALR[6]);
	Serial.print(":");
	if ((ALR[7]) < 100)
	{
		Serial.print("0");
		if ((ALR[7]) < 10)
		{
			Serial.print("0");
		}
	}
	Serial.print(ALR[7]);
	Serial.print(":");
	if ((ALR[8]) < 100)
	{
		Serial.print("0");
		if ((ALR[8]) < 10)
		{
			Serial.print("0");
		}
	}
	Serial.print(ALR[8]);
	Serial.print(":");
	if ((ALR[9]) < 100)
	{
		Serial.print("0");
		if ((ALR[9]) < 10)
		{
			Serial.print("0");
		}
	}
	Serial.print(ALR[9]);
	Serial.print(":");
	if ((ALR[10]) < 100)
	{
		Serial.print("0");
		if ((ALR[10]) < 10)
		{
			Serial.print("0");
		}
	}
	Serial.print(ALR[10]);
	Serial.print(":");
	if ((ALR[11]) < 100)
	{
		Serial.print("0");
		if ((ALR[11]) < 10)
		{
			Serial.print("0");
		}
	}
	Serial.print(ALR[11]);
	Serial.print(":");
	if ((ALR[12]) < 100)
	{
		Serial.print("0");
		if ((ALR[12]) < 10)
		{
			Serial.print("0");
		}
	}
	Serial.print(ALR[12]);
	Serial.print(":");
	if ((ALR[13]) < 100)
	{
		Serial.print("0");
		if ((ALR[13]) < 10)
		{
			Serial.print("0");
		}
	}
	Serial.print(ALR[13]);
	Serial.print(":");
	if ((ALR[14]) < 100)
	{
		Serial.print("0");
		if ((ALR[14]) < 10)
		{
			Serial.print("0");
		}

	}
	Serial.print(ALR[14]);
	Serial.print(":");
	if ((ALR[15]) < 100)
	{
		Serial.print("0");
		if ((ALR[15]) < 10)
		{
			Serial.print("0");
		}

	}
	Serial.print(ALR[15]);
	Serial.print(":");
	Serial.print(LeafNum);
	Serial.print('%');
	Serial.println();

}

void setup()
{
	Serial.begin(9600);

	pinMode(2, OUTPUT); //用于通知上位机接已经读取到串口数据

	pinMode(3, INPUT);
	pinMode(4, INPUT);
	pinMode(9, INPUT);
	pinMode(8, INPUT);
	pinMode(5, INPUT);
	pinMode(7, INPUT);
	pinMode(6, INPUT);

	FlexiTimer2::set(200, TimerInterrupt);
	FlexiTimer2::start();
}

void loop()
{
	Surface_ReadSerialata(); //读取串口数据

	if (ControlNum == 1)
	{
		ALT[0] = map(analogRead(0), 0, 1023, 180, 20);
		ALT[1] = map(analogRead(1), 0, 1023, 20, 180);
		ALT[2] = map(analogRead(2), 0, 1023, 200, 0);
		ALT[3] = map(analogRead(3), 0, 1023, 0, 200);
		ALT[4] = map(analogRead(4), 0, 1023, 185, 75);
		ALT[5] = map(analogRead(5), 0, 1023, 190, 10);
		ALT[6] = map(analogRead(6), 0, 1023, 0, 200);
		ALT[7] = map(analogRead(7), 0, 1023, 200, 0);
		ALT[8] = map(analogRead(8), 0, 1023, 0, 200);
		ALT[9] = map(analogRead(9), 0, 1023, 15, 185);
		ALT[10] = map(analogRead(10), 0, 1023, 30, 120);
		ALT[11] = map(analogRead(11), 0, 1023, 0, 200);
		ALT[12] = map(analogRead(12), 0, 1023, 0, 200);
		ALT[13] = map(analogRead(13), 0, 1023, 0, 200);
		ALT[14] = map(analogRead(14), 0, 1023, 15, 185);
		ALT[15] = map(analogRead(15), 0, 1023, 50, 120);
		//    if ((ALR[0] != ALT[0]) || (ALR[1] != ALT[1]))
		//    {
		MoveState = 1;
		ALR[0] = ALT[0];
		ALR[1] = ALT[1];
		//    }
		//    if (ALR[5] != ALT[5])
		//    {
		UpState = 1;
		ALR[5] = ALT[5];
		//    }
		//    if ((ALR[6] != ALT[6]))
		//    {
		Arm1State[0] = 1;
		ALR[6] = ALT[6];
		//    }
		//    if ((ALR[7] != ALT[7]))
		//    {
		Arm1State[1] = 1;
		ALR[7] = ALT[7];
		//    }
		//    if ((ALR[8] != ALT[8]))
		//    {
		Arm1State[2] = 1;
		ALR[8] = ALT[8];
		//    }
		//    if ((ALR[9] != ALT[9]))
		//    {
		Arm1State[3] = 1;
		ALR[9] = ALT[9];
		//    }
		//    if ((ALR[10] != ALT[10]))
		//    {
		Arm1State[4] = 1;
		ALR[10] = ALT[10];
		//    }
		//    if ((ALR[11] != ALT[11]))
		//    {
		Arm2State[0] = 1;
		ALR[11] = ALT[11];
		//    }
		//    if ((ALR[12] != ALT[12]))
		//    {
		Arm2State[1] = 1;
		ALR[12] = ALT[12];
		//    }
		//    if ((ALR[13] != ALT[13]))
		//    {
		Arm2State[2] = 1;
		ALR[13] = ALT[13];
		//    }
		//    if ((ALR[14] != ALT[14]))
		//    {
		Arm2State[3] = 1;
		ALR[14] = ALT[14];
		//    }
		//    if ((ALR[15] != ALT[15]))
		//    {
		Arm2State[4] = 1;
		ALR[15] = ALT[15];
		//    }
		//    if (ALR[2] != ALT[2])
		//    {
		LightState = 1;
		ALR[2] = ALT[2];
		//    }
		//    if (ALR[4] != ALT[4])
		//    {
		THState = 1;
		ALR[4] = ALT[4];
		//    }

		ALR[3] = ALT[3];
	}
	if (DeepNum != digitalRead(3))
	{
		DeepNum = digitalRead(3);
		DeepFlag = 1;
	}
	if (DirectNum != digitalRead(9))
	{
		DirectNum = digitalRead(9);
		DirectFlag = 1;
	}
	if (ControlNum != digitalRead(4))
	{
		ControlNum = digitalRead(4);
		ControlFlag = 1;
	}
	if (ArmNum != digitalRead(6))
	{
		ArmNum = digitalRead(6);
		ArmFlag = 1;
	}
	if (digitalRead(7) == 1)
	{
		if (LeafFlag != 1)
		{
			LeafFlag = 0;
			LeafNum = 1;
		}
	}
	if (SideNum != digitalRead(8))
	{
		SideNum = digitalRead(8);
		SideFlag = 1;
	}
}

/**
 * @brief 处理串口数据并通知定时器中断服务函数进行处理
 */
void Surface_ReadSerialata(void)
{
	u8 isRev;
	if (Serial.available())
	{
		isRev = Serial.read();
		if (isRev == '1')
		{
			UART_REV_RING_FLAG = 1;
		}
		else if (isRev == '0')
		{
			UART_REV_RING_FLAG = 0;
		}
	}
}








