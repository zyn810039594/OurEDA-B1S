#include "BasicCtrl.h"

//接收指针定义
static u8 *PDString = 0;
//上传指针定义
static u8 *PUString = 0;

static u8 *PDBegin = 0; 	//起始位，应为0x25
static u16 *PDStraight = 0; //前进后退
static u16 *PDRotate = 0; 	//旋转侧推
static u16 *PDVertical = 0; //垂直
static u16 *PDLight = 0; 	//灯光
static u16 *PDTH = 0; 		//云台
static u16 *PDTransp = 0; 	//传送
static u16 *PDArm[6] =
{ 0 }; 						//机械臂1-6
static u16 *PDRes = 0; 		//预留PWM
static u8 *PDMode = 0; 		//模式开关
static u8 *PDId = 0; 		//验证位
static u8 *PDEnd = 0; 		//结束位，应为0x21

//起始位，应为0x25
static u8 *PUCabin = 0; 	//仓位
static u16 *PUTemp = 0; 	//温度
static u16 *PUBaro[2] =
{ 0 }; 						//气压
static u16 *PUHum = 0; 		//湿度
static u16 *PUAcc[3] =
{ 0 }; 						//加速度x、y、z
static u16 *PURot[3] =
{ 0 }; 						//角速度x、y、z
static u16 *PUEul[3] =
{ 0 }; 						//角度Roll、Pitch、Yaw
static u16 *PUMag[3] =
{ 0 }; 						//磁场x、y、z
static u16 *PUWaterT = 0; 	//水温
static u16 *PUWaterD = 0; 	//水深
static u8 *PUId = 0; 		//确认位
//结束位，应为0xFF

//传感器IO读写位指针
static u8 *DeepSensorIO = 0; 	//深度传感器
static u8 *GY39SensorIO = 0; 	//温湿度和大气压传感器
static u8 *WT931SensorIO = 0; 	//九轴传感器

//下传数据读取格式化
void DownDetectPoint(u8 *RXString)
{
	PDBegin = RXString; //起始位对准
	/* 其他各位对齐 */
	PDStraight = (u16*) (RXString + 1);
	PDRotate = (u16*) (RXString + 3);
	PDVertical = (u16*) (RXString + 5);
	PDLight = (u16*) (RXString + 7);
	PDTH = (u16*) (RXString + 9);
	PDTransp = (u16*) (RXString + 11);
	PDArm[0] = (u16*) (RXString + 13);
	PDArm[1] = (u16*) (RXString + 15);
	PDArm[2] = (u16*) (RXString + 17);
	PDArm[3] = (u16*) (RXString + 19);
	PDArm[4] = (u16*) (RXString + 21);
	PDArm[5] = (u16*) (RXString + 23);
	PDRes = (u16*) (RXString + 25);
	PDMode = RXString + 27;
	PDId = RXString + 28;
	PDEnd = RXString + 29; //结束位对准
}

/* 下传数据解析
 * 从下传数据中获取控制数据
 * 并保存到控制指针
 * */
void DownDetectReceive(u16 *StraightNum, u16 *RotateNum, u16 *VerticalNum,
		u32 *LightNum, u32 *THNum, u32 *TranspNum, u32 *ArmNum[6], u32 *ResPWM,
		u8 *ModeNum, u8 *RelaySW)
{
#ifdef DataIdentify
	if (IdTest(PDString, 0)) //如果开启数据校验，则自动对校验位进行检测
	{
#endif
		if ((*PDBegin == 0x25) && (*PDEnd == 0x21)) //识别起始和结束位
		{
			/* 保存各控制位数据 */
			*StraightNum = *PDStraight;
			*RotateNum = *PDRotate;
			*VerticalNum = *PDVertical;
			*LightNum = *PDLight;
			*THNum = *PDTH;
			*TranspNum = *PDTransp;
			*ArmNum[0] = *PDArm[0];
			*ArmNum[1] = *PDArm[1];
			*ArmNum[2] = *PDArm[2];
			*ArmNum[3] = *PDArm[3];
			*ArmNum[4] = *PDArm[4];
			*ArmNum[5] = *PDArm[5];
			*ResPWM = *PDRes;
			*ModeNum = (*PDMode & 7);
			*RelaySW = (*PDMode & 8);
		}
#ifdef DataIdentify
	}
#endif

}

/* 运动模式判断
 * 主控制端使用&6模式，推进器控制端使用&4模式
 * */
u8 DownMoveDetect(void)
{
#ifdef CtrlSide
	return *PDMode & 6;
#endif
#ifdef PowerSide
	return *PDMode & 4;
#endif
}

/* 机器人正常运动处理函数
 * 使用经验公式解算推进器参数
 * 最好不要修改这部分代码
 * */
void MoveControl(u16 StraightNum, u16 RotateNum, u16 VerticalNum, u8 ModeNum,
		u32 *MoveThruster[6])
{
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
			*MoveThruster[0] = (u32) (RotateNum);
			*MoveThruster[1] = (u32) (RotateNum);
			*MoveThruster[2] = (u32) ((1500 - RotateNum + StraightNum));
			*MoveThruster[3] = (u32) ((1500 - RotateNum + StraightNum));
			break;
		case 7:
		case 8:
			*MoveThruster[0] = (u32) (StraightNum);
			*MoveThruster[1] = (u32) (StraightNum);
			*MoveThruster[2] = (u32) ((1500 - RotateNum + StraightNum));
			*MoveThruster[3] = (u32) ((1500 - RotateNum + StraightNum));
			break;
		case 5:
		case 10:
			*MoveThruster[0] = (u32) ((RotateNum + StraightNum - 1500));
			*MoveThruster[1] = (u32) ((RotateNum + StraightNum - 1500));
			*MoveThruster[2] = (u32) (StraightNum);
			*MoveThruster[3] = (u32) (StraightNum);
			break;
		case 1:
		case 14:
			*MoveThruster[0] = (u32) ((RotateNum + StraightNum - 1500));
			*MoveThruster[1] = (u32) ((RotateNum + StraightNum - 1500));
			*MoveThruster[2] = (u32) ((3000 - RotateNum));
			*MoveThruster[3] = (u32) ((3000 - RotateNum));
			break;
		}
		*MoveThruster[4] = (u32) (VerticalNum);
		*MoveThruster[5] = (u32) (VerticalNum);
	}
	else if (ModeNum == 1)
	{
		*MoveThruster[0] = (u32) (*PDRotate);
		*MoveThruster[1] = (u32) ((3000 - *PDRotate));
		*MoveThruster[2] = (u32) (*PDRotate);
		*MoveThruster[3] = (u32) (3000 - *PDRotate);
		*MoveThruster[4] = (u32) (VerticalNum);
		*MoveThruster[5] = (u32) (VerticalNum);
	}
}

/* 特殊运动模式处理
 * 使用PID算法实现机器人自主定向定深
 * */
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
						+ RANGE(
								(int16_t) (PID_D_Kp * Ek + (PID_D_Ki * LocSum) + PID_D_Kd * (Ek1 - Ek)),
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
						+ RANGE(
								(int16_t) (PID_O_Kp * Ek + (PID_O_Ki * LocSum) + PID_O_Kd * (Ek1 - Ek)),
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

/* 上传指针数据格式化
 * 将读取到的传感器数据合并
 * 然后格式化成可以上传的十六进制数据
 * */
void SendBackPoint(u8 *TXString, u8 *DeepSIO, u8 *WT931SIO, u8 *GY39SIO)
{
	DeepSensorIO = DeepSIO; //深度传感器数据
	WT931SensorIO = WT931SIO; //九轴传感器数据
	GY39SensorIO = GY39SIO; //温湿度气压传感器数据
	TXString[0] = 0x25; //起始位
	TXString[39] = 0xFF; //结束位
	TXString[40] = 0xFF; //结束位
	/* 封装数据 */
	PUString = TXString;
	PUCabin = TXString + 1;
	PUTemp = (u16*) (TXString + 2);
	PUBaro[0] = (u16*) (TXString + 4);
	PUBaro[1] = (u16*) (TXString + 6);
	PUHum = (u16*) (TXString + 8);
	PUAcc[0] = (u16*) (TXString + 10);
	PUAcc[1] = (u16*) (TXString + 12);
	PUAcc[2] = (u16*) (TXString + 14);
	PURot[0] = (u16*) (TXString + 16);
	PURot[1] = (u16*) (TXString + 18);
	PURot[2] = (u16*) (TXString + 20);
	PUEul[0] = (u16*) (TXString + 22);
	PUEul[1] = (u16*) (TXString + 24);
	PUEul[2] = (u16*) (TXString + 26);
	PUMag[0] = (u16*) (TXString + 28);
	PUMag[1] = (u16*) (TXString + 30);
	PUMag[2] = (u16*) (TXString + 32);
	PUWaterT = (u16*) (TXString + 34);
	PUWaterD = (u16*) (TXString + 36);
	PUId = TXString + 38;
	*PUId = XorCaculate(TXString, 38); //获取并封装奇偶校验位
}

/* 上传指针数据解析
 * 将封装好的传感器数据再解析成能够回传到上位机和其他舱室的数据
 * */
void SendBack(u8 WaterDetect, u16 TempNum, u16 BaroNum[2], u16 HumNum,
		u16 AccNum[3], u16 RotNum[3], u16 EulNum[3], u16 MagNum[3],
		u16 WaterTNum, u16 WaterDNum)
{
	*PUCabin = CabinNum;

	if (WaterDetect) //漏水检测
	{
		*PUCabin += WaterNum;
	}
	if (!(*GY39SensorIO)) //温湿度大气压传感器
	{
		*PUTemp = TempNum;
		*PUBaro[0] = BaroNum[0];
		*PUBaro[1] = BaroNum[1];
		*PUHum = HumNum;
	}
	if (!(*WT931SensorIO)) //九轴传感器
	{
		*PUAcc[0] = AccNum[0];
		*PUAcc[1] = AccNum[1];
		*PUAcc[2] = AccNum[2];
		*PURot[0] = RotNum[0];
		*PURot[1] = RotNum[1];
		*PURot[2] = RotNum[2];
		*PUEul[0] = EulNum[0];
		*PUEul[1] = EulNum[1];
		*PUEul[2] = EulNum[2];
		*PUMag[0] = MagNum[0];
		*PUMag[1] = MagNum[1];
		*PUMag[2] = MagNum[2];
	}
#ifdef CtrlSide
	//主控制仓有水深传感器，需要先检测是否正常连接
	if (!(*DeepSensorIO))
	{
		*PUWaterT = WaterTNum;
		*PUWaterD = WaterDNum;
	}
#endif

#ifdef PowerSide
	//推进器控制仓没有水深传感器
	*PUWaterT = NULL;
	*PUWaterD = NULL;
#endif
	*PUId = XorCaculate(PUString, 38);
}

//异或运算位检查，其中Format上传格式为1下传格式为0，正确为1错误为0
u8 IdTest(u8 *String, u8 Format)
{
#ifdef DataIdentify
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
#endif
	return 1;
}

//异或运算位计算
u8 XorCaculate(u8 *CacString, u8 CacStringSize)
{
	u8 CacResult = CacString[0];
	for (u8 i = 0; i < CacStringSize; ++i)
	{
		CacResult ^= CacString[i];
	}
	return CacResult;
}
