#include "BasicCtrl.h"

static u8 XorCaculate(u8 *CacString, u8 CacStringSize);
static u8 IdTest(u8 *String, u8 Format);

__attribute__((section(".RAM_D1")))          u8 DownDataReceive[Up_UART_RXLen] =
{ 0 };
__attribute__((section(".RAM_D1")))          u8 DownDataSend[Down_UART_TXLEN] =
{ 0 };

__attribute__((section(".RAM_D1")))          u8 UpDataReceive[Down_UART_RXLen] =
{ 0 };
__attribute__((section(".RAM_D1")))          u8 UpDataSend[Up_UART_TXLen] =
{ 0 };

//捕获上位向下位发送的数据
DownDataDef CaptureDownData(void)
{
	DownDataDef CaptureData;
	CaptureData.StraightNum = ((DownDataReceive[2] << 8) | DownDataReceive[1]);
	CaptureData.RotateNum = ((DownDataReceive[4] << 8) | DownDataReceive[3]);
	CaptureData.VerticalNum = ((DownDataReceive[6] << 8) | DownDataReceive[5]);
	CaptureData.LightPWM = ((DownDataReceive[8] << 8) | DownDataReceive[7]);
	CaptureData.THPWM = ((DownDataReceive[10] << 8) | DownDataReceive[9]);
	CaptureData.TranspPWM = ((DownDataReceive[12] << 8) | DownDataReceive[11]);
	CaptureData.ArmPWM[0] = ((DownDataReceive[14] << 8) | DownDataReceive[13]);
	CaptureData.ArmPWM[1] = ((DownDataReceive[16] << 8) | DownDataReceive[15]);
	CaptureData.ArmPWM[2] = ((DownDataReceive[18] << 8) | DownDataReceive[17]);
	CaptureData.ArmPWM[3] = ((DownDataReceive[20] << 8) | DownDataReceive[19]);
	CaptureData.ArmPWM[4] = ((DownDataReceive[22] << 8) | DownDataReceive[21]);
	CaptureData.ArmPWM[5] = ((DownDataReceive[24] << 8) | DownDataReceive[23]);
	CaptureData.ResPWM = ((DownDataReceive[26] << 8) | DownDataReceive[25]);
	CaptureData.Mode = (DownDataReceive[27] & 7);
	CaptureData.Relay = (DownDataReceive[27] & 8);
	CaptureData.IdTest = IdTest(DownDataReceive, 0);
	__HAL_UART_ENABLE_IT(&Up_UART, UART_IT_IDLE);
	HAL_UART_Receive_DMA(&Up_UART, DownDataReceive, Up_UART_RXLen);
	return CaptureData;
}
//向下位发送数据
void SendDownData(DownDataDef SendData)
{
	DownDataSend[0] = 0x25;
	DownDataSend[1] = SendData.StraightNum;
	DownDataSend[2] = SendData.StraightNum >> 8;
	DownDataSend[3] = SendData.RotateNum;
	DownDataSend[4] = SendData.RotateNum >> 8;
	DownDataSend[5] = SendData.VerticalNum;
	DownDataSend[6] = SendData.VerticalNum >> 8;
	DownDataSend[7] = SendData.LightPWM;
	DownDataSend[8] = SendData.LightPWM >> 8;
	DownDataSend[9] = SendData.THPWM;
	DownDataSend[10] = SendData.THPWM >> 8;
	DownDataSend[11] = SendData.TranspPWM;
	DownDataSend[12] = SendData.TranspPWM >> 8;
	DownDataSend[13] = SendData.ArmPWM[0];
	DownDataSend[14] = SendData.ArmPWM[0] >> 8;
	DownDataSend[15] = SendData.ArmPWM[1];
	DownDataSend[16] = SendData.ArmPWM[1] >> 8;
	DownDataSend[17] = SendData.ArmPWM[2];
	DownDataSend[18] = SendData.ArmPWM[2] >> 8;
	DownDataSend[19] = SendData.ArmPWM[3];
	DownDataSend[20] = SendData.ArmPWM[3] >> 8;
	DownDataSend[21] = SendData.ArmPWM[4];
	DownDataSend[22] = SendData.ArmPWM[4] >> 8;
	DownDataSend[23] = SendData.ArmPWM[5];
	DownDataSend[24] = SendData.ArmPWM[5] >> 8;
	DownDataSend[25] = SendData.ResPWM;
	DownDataSend[26] = SendData.ResPWM >> 8;
	DownDataSend[27] = (SendData.Mode | SendData.Relay);
	DownDataSend[28] = XorCaculate(DownDataSend, 28);
	DownDataSend[29] = 0x21;
	HAL_UART_Transmit_DMA(&Down_UART, DownDataSend, Down_UART_TXLEN);
}
//捕获下位向上位发送的数据
UpDataDef CaptureUpData(void)
{
	UpDataDef CaptureData;
	CaptureData.CabinNum = (UpDataReceive[1] & 1);
	CaptureData.WaterDetect = (UpDataReceive[1] & 6);
	CaptureData.CabinTemperature = ((UpDataReceive[3] << 8) | UpDataReceive[2]);
	CaptureData.CabinBaro = ((UpDataReceive[7] << 24) | (UpDataReceive[6] << 16)
			| (UpDataReceive[5] << 8) | UpDataReceive[4]);
	CaptureData.CabinHum = ((UpDataReceive[9] << 8) | UpDataReceive[8]);
	CaptureData.AccNum[0] = ((UpDataReceive[11] << 8) | UpDataReceive[10]);
	CaptureData.AccNum[1] = ((UpDataReceive[13] << 8) | UpDataReceive[12]);
	CaptureData.AccNum[2] = ((UpDataReceive[15] << 8) | UpDataReceive[14]);
	CaptureData.RotNum[0] = ((UpDataReceive[17] << 8) | UpDataReceive[16]);
	CaptureData.RotNum[1] = ((UpDataReceive[19] << 8) | UpDataReceive[18]);
	CaptureData.RotNum[2] = ((UpDataReceive[21] << 8) | UpDataReceive[20]);
	CaptureData.EulNum[0] = ((UpDataReceive[23] << 8) | UpDataReceive[22]);
	CaptureData.EulNum[1] = ((UpDataReceive[25] << 8) | UpDataReceive[24]);
	CaptureData.EulNum[2] = ((UpDataReceive[27] << 8) | UpDataReceive[26]);
	CaptureData.MagNum[0] = ((UpDataReceive[29] << 8) | UpDataReceive[28]);
	CaptureData.MagNum[1] = ((UpDataReceive[31] << 8) | UpDataReceive[30]);
	CaptureData.MagNum[2] = ((UpDataReceive[33] << 8) | UpDataReceive[32]);
	CaptureData.WaterTemperature =
			((UpDataReceive[35] << 8) | UpDataReceive[34]);
	CaptureData.WaterDepth = ((UpDataReceive[37] << 8) | UpDataReceive[36]);
	CaptureData.IdTest = IdTest(UpDataReceive, 1);
	__HAL_UART_ENABLE_IT(&Down_UART, UART_IT_IDLE);
	HAL_UART_Receive_DMA(&Down_UART, DownDataReceive, Down_UART_RXLen);
	return CaptureData;
}
//向上位机发送数据
void SendUpData(UpDataDef SendData)
{
	UpDataSend[0] = 0x25;
	UpDataSend[1] = (SendData.CabinNum | SendData.WaterDetect);
	UpDataSend[2] = SendData.CabinTemperature;
	UpDataSend[3] = SendData.CabinTemperature >> 8;
	UpDataSend[4] = SendData.CabinBaro;
	UpDataSend[5] = SendData.CabinBaro >> 8;
	UpDataSend[6] = SendData.CabinBaro >> 16;
	UpDataSend[7] = SendData.CabinBaro >> 24;
	UpDataSend[8] = SendData.CabinHum;
	UpDataSend[9] = SendData.CabinHum >> 8;
	UpDataSend[10] = SendData.AccNum[0];
	UpDataSend[11] = SendData.AccNum[0] >> 8;
	UpDataSend[12] = SendData.AccNum[1];
	UpDataSend[13] = SendData.AccNum[1] >> 8;
	UpDataSend[14] = SendData.AccNum[2];
	UpDataSend[15] = SendData.AccNum[2] >> 8;
	UpDataSend[16] = SendData.RotNum[0];
	UpDataSend[17] = SendData.RotNum[0] >> 8;
	UpDataSend[18] = SendData.RotNum[1];
	UpDataSend[19] = SendData.RotNum[1] >> 8;
	UpDataSend[20] = SendData.RotNum[2];
	UpDataSend[21] = SendData.RotNum[2] >> 8;
	UpDataSend[22] = SendData.EulNum[0];
	UpDataSend[23] = SendData.EulNum[0] >> 8;
	UpDataSend[24] = SendData.EulNum[1];
	UpDataSend[25] = SendData.EulNum[1] >> 8;
	UpDataSend[26] = SendData.EulNum[2];
	UpDataSend[27] = SendData.EulNum[2] >> 8;
	UpDataSend[28] = SendData.MagNum[0];
	UpDataSend[29] = SendData.MagNum[0] >> 8;
	UpDataSend[30] = SendData.MagNum[1];
	UpDataSend[31] = SendData.MagNum[1] >> 8;
	UpDataSend[32] = SendData.MagNum[2];
	UpDataSend[33] = SendData.MagNum[2] >> 8;
	UpDataSend[34] = SendData.WaterTemperature;
	UpDataSend[35] = SendData.WaterTemperature >> 8;
	UpDataSend[36] = SendData.WaterDepth;
	UpDataSend[37] = SendData.WaterDepth >> 8;
	UpDataSend[38] = XorCaculate(UpDataSend, 38);
	UpDataSend[39] = 0xff;
	UpDataSend[40] = 0xff;
	HAL_UART_Transmit_DMA(&Up_UART, UpDataSend, Up_UART_TXLen);
}
//基本的运动控制函数
MoveThruster MoveControl(u16 StraightNum, u16 RotateNum, u16 VerticalNum,
		u8 ModeNum)
{
	MoveThruster ThrusterTemp;
	if (ModeNum == 0)
	{
		u8 AFlag = (RotateNum > StraightNum);
		u8 BFlag = ((RotateNum + StraightNum) > 3000);
		u8 CFlag = (RotateNum > 1500);
		u8 DFlag = (StraightNum > 1500);
		u8 SFlag = AFlag * 8 + BFlag * 4 + CFlag * 2 + DFlag;
		switch (SFlag)
		{
		case 0:
		case 15:
			ThrusterTemp.HorizontalThruster[0] = (vu32) (RotateNum);
			ThrusterTemp.HorizontalThruster[1] = (vu32) (RotateNum);
			ThrusterTemp.HorizontalThruster[2] = (vu32) ((1500 - RotateNum
					+ StraightNum));
			ThrusterTemp.HorizontalThruster[3] = (vu32) ((1500 - RotateNum
					+ StraightNum));
			break;
		case 7:
		case 8:
			ThrusterTemp.HorizontalThruster[0] = (vu32) (StraightNum);
			ThrusterTemp.HorizontalThruster[1] = (vu32) (StraightNum);
			ThrusterTemp.HorizontalThruster[2] = (vu32) ((1500 - RotateNum
					+ StraightNum));
			ThrusterTemp.HorizontalThruster[3] = (vu32) ((1500 - RotateNum
					+ StraightNum));
			break;
		case 5:
		case 10:
			ThrusterTemp.HorizontalThruster[0] = (vu32) ((RotateNum
					+ StraightNum - 1500));
			ThrusterTemp.HorizontalThruster[1] = (vu32) ((RotateNum
					+ StraightNum - 1500));
			ThrusterTemp.HorizontalThruster[2] = (vu32) (StraightNum);
			ThrusterTemp.HorizontalThruster[3] = (vu32) (StraightNum);
			break;
		case 1:
		case 14:
			ThrusterTemp.HorizontalThruster[0] = (vu32) ((RotateNum
					+ StraightNum - 1500));
			ThrusterTemp.HorizontalThruster[1] = (vu32) ((RotateNum
					+ StraightNum - 1500));
			ThrusterTemp.HorizontalThruster[2] = (vu32) ((3000 - RotateNum));
			ThrusterTemp.HorizontalThruster[3] = (vu32) ((3000 - RotateNum));
			break;
		}
		ThrusterTemp.VerticalThruster[0] = (vu32) (VerticalNum);
		ThrusterTemp.VerticalThruster[1] = (vu32) (VerticalNum);
	}
	else if (ModeNum == 1)
	{
		ThrusterTemp.HorizontalThruster[0] = (vu32) (RotateNum);
		ThrusterTemp.HorizontalThruster[1] = (vu32) ((3000 - RotateNum));
		ThrusterTemp.HorizontalThruster[2] = (vu32) (RotateNum);
		ThrusterTemp.HorizontalThruster[3] = (vu32) (3000 - RotateNum);
		ThrusterTemp.VerticalThruster[0] = (vu32) (VerticalNum);
		ThrusterTemp.VerticalThruster[1] = (vu32) (VerticalNum);
	}
	return ThrusterTemp;
}

//PID算法运行
u16 SpecialMovePID(u8 ModeType, u16 SetValue, u16 ActualValue)
{
	//当前误差
	static float Ek;
	//前一次误差
	static float Ek1;
	//累计积分位置
	static float LocSum;
	//数据清空标志位
	static u8 PIDData = 0;

	if (ModeType == 4)
	{
		PIDData = 0;
		u16 PIDLoc;
		Ek = (float) (SetValue - ActualValue);
		LocSum += Ek;
		PIDLoc =
				(u16) (1500
						+ range(
								(int16_t)(PID_D_Kp * Ek + (PID_D_Ki * LocSum) + PID_D_Kd * (Ek1 - Ek)),
								-1000, 1000));
		return PIDLoc;
	}
	else if (ModeType == 2)
	{
		PIDData = 0;
		u16 PIDLoc;
		Ek = (float) (SetValue - ActualValue);
		LocSum += Ek;
		PIDLoc =
				(u16) (1500
						+ range(
								(int16_t)(PID_O_Kp * Ek + (PID_O_Ki * LocSum) + PID_O_Kd * (Ek1 - Ek)),
								-1000, 1000));
		return PIDLoc;
	}
	else
	{
		if (!PIDData)
		{
			PIDData = 1;
			Ek = 0;
			LocSum = 0;
		}
		return 0;
	}

}

//异或运算位检查，其中Format上传格式为1下传格式为0，正确为1错误为0
static u8 IdTest(u8 *String, u8 Format)
{
	if (Format)
	{
		if (*(String + 38) == XorCaculate(String, 38))
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		if (*(String + 28) == XorCaculate(String, 28))
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	return 1;
}

//异或运算位计算
static u8 XorCaculate(u8 *CacString, u8 CacStringSize)
{
	u8 CacResult = CacString[0];
	for (u8 i = 0; i < CacStringSize; ++i)
	{
		CacResult ^= CacString[i];
	}
	return CacResult;
}
