/**
 * @file BasicCtrl.c
 * @brief
 * @author Harold Zhao(zyn810039594) and RedlightASl (dddbbbdd@foxmail.com)
 * @version 1.0
 * @date 2021-08-19
 *
 * @copyright Copyright (c) 2021  RedlightASl
 *
 * @par 修改日志:
 * <table>
 * <tr><th>Date       <th>Version <th>Author  <th>Description
 * <tr><td>2021-08-19 <td>1.0     <td>wangh     <td>Content
 * </table>
 */
#include "BasicCtrl.h"

static u8 XorCaculate(u8 *CacString, u8 CacStringSize);
static u8 IdTest(u8 *String, u8 Format, u8 SendUpLength, u8 SendDownLength);

__attribute__((section(".RAM_D1")))  u8 DownDataReceive[Up_UART_RXLen] =
{ 0 };
__attribute__((section(".RAM_D1")))  u8 DownDataSend[Down_UART_TXLEN] =
{ 0 };
__attribute__((section(".RAM_D1")))  u8 UpDataReceive[Down_UART_RXLen] =
{ 0 };
__attribute__((section(".RAM_D1")))  u8 UpDataSend[Up_UART_TXLen] =
{ 0 };

/**
 * @brief 捕获上位机向下位机发送的指令
 * @return DownDataDef 上位机向下位机发送的指令
 */
DownDataDef CaptureDownData(void)
{
	DownDataDef CaptureData;

	CaptureData.StraightNum = ((DownDataReceive[1] << 8) | DownDataReceive[2]);
	CaptureData.RotateNum = ((DownDataReceive[3] << 8) | DownDataReceive[4]);
	CaptureData.VerticalNum = ((DownDataReceive[5] << 8) | DownDataReceive[6]);
	CaptureData.LightPWM = ((DownDataReceive[7] << 8) | DownDataReceive[8]);
	CaptureData.THPWM = ((DownDataReceive[9] << 8) | DownDataReceive[10]);
	CaptureData.TranspPWM = ((DownDataReceive[11] << 8) | DownDataReceive[12]);
	CaptureData.ArmPWM[0] = ((DownDataReceive[13] << 8) | DownDataReceive[14]);
	CaptureData.ArmPWM[1] = ((DownDataReceive[15] << 8) | DownDataReceive[16]);
	CaptureData.ArmPWM[2] = ((DownDataReceive[17] << 8) | DownDataReceive[18]);
	CaptureData.ArmPWM[3] = ((DownDataReceive[19] << 8) | DownDataReceive[20]);
	CaptureData.ArmPWM[4] = ((DownDataReceive[21] << 8) | DownDataReceive[22]);
	CaptureData.ArmPWM[5] = ((DownDataReceive[23] << 8) | DownDataReceive[24]);
	CaptureData.ResPWM = ((DownDataReceive[25] << 8) | DownDataReceive[26]);
#ifdef CtrlSide
	CaptureData.Mode = (DownDataReceive[27]); //方便数据下传与中途获取
	CaptureData.Relay = (DownDataReceive[27]);
#else
#ifdef PowerSide
	CaptureData.Mode = ((DownDataReceive[27] & 0b0001) ? 1 : 0); //控制侧推模式 1侧推模式开启；0侧推模式关闭
	CaptureData.Relay = ((DownDataReceive[27] & 0b1000) ? 1U : 0U); //控制继电器开关 8继电器开；0继电器关
#else
	CaptureData.Mode = (DownDataReceive[27] & 0b0001);
	CaptureData.Relay = (DownDataReceive[27] & 0b1000);
#endif
#endif
	CaptureData.IdTest = IdTest(DownDataReceive, 0, 44, 28);

	__HAL_UART_ENABLE_IT(&Up_UART, UART_IT_IDLE);
	HAL_UART_Receive_DMA(&Up_UART, DownDataReceive, Up_UART_RXLen);

	return CaptureData;
}

/**
 * @brief 向下位机发送数据
 * @param  SendData         待发送的数据结构体
 * @note 已完成Debug
 */
void SendDownData(DownDataDef SendData)
{
	DownDataSend[0] = 0x25;
	DownDataSend[1] = SendData.StraightNum >> 8;
	DownDataSend[2] = SendData.StraightNum;
	DownDataSend[3] = SendData.RotateNum >> 8;
	DownDataSend[4] = SendData.RotateNum;
	DownDataSend[5] = SendData.VerticalNum >> 8;
	DownDataSend[6] = SendData.VerticalNum;
	DownDataSend[7] = SendData.LightPWM >> 8;
	DownDataSend[8] = SendData.LightPWM;
	DownDataSend[9] = SendData.THPWM >> 8;
	DownDataSend[10] = SendData.THPWM;
	DownDataSend[11] = SendData.TranspPWM >> 8;
	DownDataSend[12] = SendData.TranspPWM;
	DownDataSend[13] = SendData.ArmPWM[0] >> 8;
	DownDataSend[14] = SendData.ArmPWM[0];
	DownDataSend[15] = SendData.ArmPWM[1] >> 8;
	DownDataSend[16] = SendData.ArmPWM[1];
	DownDataSend[17] = SendData.ArmPWM[2] >> 8;
	DownDataSend[18] = SendData.ArmPWM[2];
	DownDataSend[19] = SendData.ArmPWM[3] >> 8;
	DownDataSend[20] = SendData.ArmPWM[3];
	DownDataSend[21] = SendData.ArmPWM[4] >> 8;
	DownDataSend[22] = SendData.ArmPWM[4];
	DownDataSend[23] = SendData.ArmPWM[5] >> 8;
	DownDataSend[24] = SendData.ArmPWM[5];
	DownDataSend[25] = SendData.ResPWM >> 8;
	DownDataSend[26] = SendData.ResPWM;
	DownDataSend[27] = SendData.Relay; //直接下传数据
	DownDataSend[28] = XorCaculate(DownDataSend, 28);
	DownDataSend[29] = 0x21;

	HAL_UART_Transmit_DMA(&Down_UART, DownDataSend, Down_UART_TXLEN);
}

/**
 * @brief 捕获下位向上位发送的数据
 * @return UpDataDef 下位机上传的数据结构体
 */
UpDataDef CaptureUpData(void)
{
	UpDataDef CaptureData;

	CaptureData.CabinNum = (UpDataReceive[1] & 0b0001); //0b0000控制仓，0b0001==1电源仓
	CaptureData.WaterDetect = (UpDataReceive[1] & 0b0010); //0b0000正常，0b0010==2漏水
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
	CaptureData.DepthToBottom = ((UpDataReceive[37] << 24)
			| (UpDataReceive[36] << 16) | (UpDataReceive[35] << 8)
			| UpDataReceive[34]);
	CaptureData.Confidence = ((UpDataReceive[39] << 8) | UpDataReceive[38]);
	CaptureData.WaterTemperature =
			((UpDataReceive[40] << 8) | UpDataReceive[41]);
	CaptureData.WaterDepth = ((UpDataReceive[42] << 8) | UpDataReceive[43]);
	CaptureData.IdTest = IdTest(UpDataReceive, 1, 44, 28);

	__HAL_UART_ENABLE_IT(&Down_UART, UART_IT_IDLE);
	HAL_UART_Receive_DMA(&Down_UART, DownDataReceive, Down_UART_RXLen);

	return CaptureData;
}

/**
 * @brief 向上位机发送数据
 * @param  SendData         待发送的数据结构体
 */
void SendUpData(UpDataDef SendData)
{
	UpDataSend[0] = 0x25;
	UpDataSend[1] = ((SendData.WaterDetect) | (SendData.CabinNum));
	UpDataSend[2] = SendData.CabinTemperature >> 8;
	UpDataSend[3] = SendData.CabinTemperature;
	UpDataSend[4] = SendData.CabinBaro >> 16;
	UpDataSend[5] = SendData.CabinBaro >> 24;
	UpDataSend[6] = SendData.CabinBaro;
	UpDataSend[7] = SendData.CabinBaro >> 8;
	UpDataSend[8] = SendData.CabinHum;
	UpDataSend[9] = SendData.CabinHum >> 8;
	UpDataSend[10] = SendData.AccNum[0] >> 8;
	UpDataSend[11] = SendData.AccNum[0];
	UpDataSend[12] = SendData.AccNum[1] >> 8;
	UpDataSend[13] = SendData.AccNum[1];
	UpDataSend[14] = SendData.AccNum[2] >> 8;
	UpDataSend[15] = SendData.AccNum[2];
	UpDataSend[16] = SendData.RotNum[0] >> 8;
	UpDataSend[17] = SendData.RotNum[0];
	UpDataSend[18] = SendData.RotNum[1] >> 8;
	UpDataSend[19] = SendData.RotNum[1];
	UpDataSend[20] = SendData.RotNum[2] >> 8;
	UpDataSend[21] = SendData.RotNum[2];
	UpDataSend[22] = SendData.EulNum[0] >> 8;
	UpDataSend[23] = SendData.EulNum[0];
	UpDataSend[24] = SendData.EulNum[1] >> 8;
	UpDataSend[25] = SendData.EulNum[1];
	UpDataSend[26] = SendData.EulNum[2] >> 8;
	UpDataSend[27] = SendData.EulNum[2];
	UpDataSend[28] = SendData.MagNum[0] >> 8;
	UpDataSend[29] = SendData.MagNum[0];
	UpDataSend[30] = SendData.MagNum[1] >> 8;
	UpDataSend[31] = SendData.MagNum[1];
	UpDataSend[32] = SendData.MagNum[2] >> 8;
	UpDataSend[33] = SendData.MagNum[2];
	UpDataSend[34] = SendData.DepthToBottom >> 24;
	UpDataSend[35] = SendData.DepthToBottom >> 16;
	UpDataSend[36] = SendData.DepthToBottom >> 8;
	UpDataSend[37] = SendData.DepthToBottom;
	UpDataSend[38] = SendData.Confidence >> 8;
	UpDataSend[39] = SendData.Confidence;
	UpDataSend[40] = SendData.WaterTemperature >> 8;
	UpDataSend[41] = SendData.WaterTemperature;
	UpDataSend[42] = SendData.WaterDepth >> 8;
	UpDataSend[43] = SendData.WaterDepth;
	UpDataSend[44] = XorCaculate(UpDataSend, 38);
	UpDataSend[45] = 0xff;
	UpDataSend[46] = 0xff;

	HAL_UART_Transmit_DMA(&Up_UART, UpDataSend, Up_UART_TXLen);
}

/**
 * @brief 基本的运动控制函数
 * @param  StraightNum      前进控制参数
 * @param  RotateNum        旋转控制参数
 * @param  VerticalNum      垂直控制参数
 * @param  ModeNum          模式 0转向模式 1侧推模式
 * @return MoveThruster 推进器控制数据
 * @note 基于经验公式，不应进行改动
 */
MoveThruster MoveControl(u16 StraightNum, u16 RotateNum, u16 VerticalNum,
		u8 ModeNum)
{
	MoveThruster ThrusterTemp;
	if (ModeNum == 0) //转向模式
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
	else if (ModeNum == 1) //侧推模式
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

/**
 * @brief PID算法控制函数
 * @param  ModeType         模式控制 2定向模式 4定深模式
 * @param  SetValue         目标值
 * @param  ActualValue      当前值
 * @return u16 位置PID数据
 */
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

	if (ModeType == 4) //定深模式
	{
		PIDData = 0;
		u16 PIDLoc;
		Ek = (float) (SetValue - ActualValue);
		LocSum += Ek;
		PIDLoc =
				(u16) (1500
						+ BASICCTRL_RANGE(
								(int16_t)(PID_D_Kp * Ek + (PID_D_Ki * LocSum) + PID_D_Kd * (Ek1 - Ek)),
								-1000, 1000));
		return PIDLoc;
	}
	else if (ModeType == 2) //定向模式
	{
		PIDData = 0;
		u16 PIDLoc;
		Ek = (float) (SetValue - ActualValue);
		LocSum += Ek;
		PIDLoc =
				(u16) (1500
						+ BASICCTRL_RANGE(
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

/**
 * @brief 异或运算位检查(奇偶校验)
 * @param  String           待校验的数据
 * @param  Format           上传格式为1；下传格式为0
 * @param  SendUpLength     上传数据长度
 * @param  SendDownLength   下传指令长度
 * @return u8 正确为1；错误为0，如果不开启奇偶校验默认为1
 */
static u8 IdTest(u8 *String, u8 Format, u8 SendUpLength, u8 SendDownLength)
{
#ifdef DataIdentify
	if (Format) //上传数据格式
	{
		if (*(String + SendUpLength) == XorCaculate(String, SendUpLength))
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else //下传指令格式
	{
		if (*(String + SendDownLength) == XorCaculate(String, SendDownLength))
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	return 1;
#else
	return 1; //不开启奇偶校验时默认成功
#endif
}

/**
 * @brief 异或运算
 * @param  CacString        待校验数据
 * @param  CacStringSize    待校验数据长度
 * @return u8 异或运算结果，如果不开启数据校验则默认返回0
 */
static u8 XorCaculate(u8 *CacString, u8 CacStringSize)
{
#ifdef DataIdentify
	u8 CacResult = CacString[0];
	for (u8 i = 0; i < CacStringSize; ++i)
	{
		CacResult ^= CacString[i];
	}
	return CacResult;
#else
	return 0;
#endif
}
