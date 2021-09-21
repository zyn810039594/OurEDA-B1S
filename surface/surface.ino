/**
   @file surface.ino
   @brief
   @author RedlightASl (dddbbbdd@foxmail.com)
   @version 1.0
   @date 2021-08-21

   @copyright Copyright (c) 2021  RedlightASl

   @par 修改日志:
   <table>
   <tr><th>Date       <th>Version <th>Author  <th>Description
   <tr><td>2021-08-21 <td>1.0     <td>wangh     <td>Content
   </table>
*/
#include "surface.h"

#ifndef DO_NOT_USING_IT_FLAG
#include <FlexiTimer2.h>
#endif

/* 字符串输出变量 */
u8 WATER_WARNING = 0; //漏水警报
signed char WATER_WARNING_TIMER_COUNT = 0; //漏水警报计时

int StrightNum = 1500; //前进后退 A0
int RotateNum = 1500; //旋转侧推 A0
int VerticalNum = 1500; //垂直 A5
long LightNum = 0; //灯光 A2
int CameraPanNum = 1500; //云台 A4
int ConveyerNum = 1500; //传送带 A3
int MachineArm_HorizentalNum = 1500; //水平机械臂 A11
int MachineArm_LargeNum = 1500; //大臂 A12
int MachineArm_MiddleNum = 1500; //中臂 A13
int MachineArm_SmallNum = 1500; //小臂 A14
int MachineArm_CatchingNum = 1500; //夹取 A15
int ReservedNum = 1500; //保留位PWM控制数据
int UserANum = 1500; //自定义旋钮A A6
int UserBNum = 1500; //自定义旋钮B A7

/* 按钮控制变量 */
u8 KeepDeepNum = 0;
u8 KeepDepth_FLAG = 0; //定深模式控制
u8 KeepOrbitNum = 0;
u8 KeepOrbit_FLAG = 0; //定向模式控制
u8 ControlButtonNum = 0;
u8 Control_FLAG = 0; //控制权限
u8 ArmNum = 0;
u8 MachineArm_ALL_Switch_FLAG = 0; //机械臂总开关
u8 KeepDownNum = 0;
u8 MachineArm_A_Switch_FLAG = 0; //机械臂A开关 下潜保持
u8 TransSwitchNum = 0;
u8 MachineArm_B_Switch_FLAG = 0; //机械臂B开关 传送带 启用
u8 SideNum = 0;
u8 Mode_FLAG = 0; //侧推旋转模式控制

/* 一键抓取标志位与变量 */
u8 GrabStart_FLAG = 0; //开始一键抓取
u8 GrabState = 0; //一键抓取状态机标志

int AutoGrab_ClipNum = 1500; //一键抓取夹手
int AutoGrab_FlipNum = 1500; //一键抓取旋转臂

u16 FInitTime=0; //准备时间变量
u16 FClipTime = 0; //维持夹手张开时间变量
u16 FFlipTime = 0; //维持翻转时间变量

u8 LateBuf = 0; //回传延时标志位

static void PrintBlank()
{
	for (int r = 0;r < 60;r++)
	{
		for (int k = 0;k < 1000;k++)
		{
			for (int j = 0;j < 1000;j++)
			{
				for (int i = 0; i < 32; i++)
				{
					NOP;
				}
			}
		}
	}
}

#ifndef DO_NOT_USING_IT_FLAG
/**
   @brief 定时器中断服务程序
*/
void TimerInterrupt()
{
	// if (LateBuf > 4)
	// {
	// 	Serial.flush();
	// 	delay(1);
	// 	Surface_IT_SendSerialMesg();
	// 	Serial.flush();
	// 	LateBuf = 0;
	// }

	//一键抓取状态机
	// if ((GrabStart_FLAG) && (LateBuf % 4 == 0))
	if(GrabStart_FLAG)
	{
		switch (GrabState)
		{
		case 0: //初始状态
			AutoGrab_ClipNum = INIT_CLIP_ANGLE; //初始化抓取舵机
			AutoGrab_FlipNum = INIT_FLIP_ANGLE; //初始化翻转舵机
			FInitTime++;
			if(FInitTime == 2)
			{
				FInitTime=0;
				GrabState = 1; //状态转移
			}
			break;
		case 1: //夹手合紧，不翻转
			AutoGrab_ClipNum = CLIP_ANGLE;
			AutoGrab_FlipNum = INIT_FLIP_ANGLE;
			FClipTime++;
			if (FClipTime == DoClipTime)
			{
				FClipTime = 0;
				GrabState = 2; //状态转移
			}
			break;
		case 2: //夹手合紧，翻转
			AutoGrab_ClipNum = CLIP_ANGLE;
			AutoGrab_FlipNum = FLIP_ANGLE;
			FFlipTime++;
			if (FFlipTime == DoFlipTime)
			{
				FFlipTime = 0;
				GrabState = 3; //状态转移
			}
			break;
		case 3: //夹手松开，之后恢复原状
			AutoGrab_ClipNum = INIT_CLIP_ANGLE;
			AutoGrab_FlipNum = FLIP_ANGLE;
			FClipTime++;
			if (FClipTime == DoDisClipTime)
			{
				FClipTime = 0;
				GrabState = 0; //回到原始状态
				GrabStart_FLAG = 0;
			}
			break;
		}
	}
	LateBuf++;
}
#endif

void setup()
{
	Serial.begin(UPLOAD_SERIAL_BAUDRATE);

	pinMode(WATER_DETECTED_WARNING, OUTPUT); //指示漏水警报

	/* 初始化开关控制引脚 */
	pinMode(KEEP_DEPTH_BUTTON, INPUT); //定深开关
	pinMode(ROOT_BUTTON, INPUT); //控制权限开关
	pinMode(MACHINE_ARM_A_BUTTON, INPUT); //机械臂A开关 下潜保持
	pinMode(MACHINE_ARM_ALL_BUTTON, INPUT); //机械臂总开关
	pinMode(MACHINE_ARM_B_BUTTON, INPUT); //机械臂B开关 传送带自动启用
	pinMode(SIDE_PUSH_MODE_SWITCH_BUTTON, INPUT); //侧推模式开关
	pinMode(KEEP_ORBIT_BUTTON, INPUT); //定向开关

#ifndef DO_NOT_USING_IT_FLAG
	FlexiTimer2::set(Latency, TimerInterrupt);
	FlexiTimer2::start();
#endif
}

void loop()
{
	Surface_Task_Control();
	Surface_Task_ReadSerialData();
	Surface_Task_SendSerialMesg();
}

/**
   @brief 实时读取控制按钮和控制杆数据
*/
void Surface_Task_Control()
{
	/* 读取基本控制数据 */
	StrightNum = map(analogRead(FORWARD_PIN), 0, 1023, 2300, 700);
	RotateNum = map(analogRead(ROTATE_PIN), 0, 1023, 700, 2300);
	VerticalNum = map(analogRead(VERTICAL_PIN), 0, 1023, 2300, 700);
	LightNum = map(analogRead(LIGHT_PIN), 0, 1023, 0, 65535);
	CameraPanNum = map(analogRead(CAMERA_PAN_PIN), 0, 1023, 2250, 1000);
	ConveyerNum = map(analogRead(Conveyer_PIN), 0, 1023, 2500, 500);

	/* 读取机械臂控制数据 */
	// MachineArm_HorizentalNum = map(analogRead(MACHINE_ARM_HORIZENTAL_PIN), 0, 800, 2500, 500); //小臂
	// MachineArm_LargeNum = map(analogRead(MACHINE_ARM_LARGE_PIN), 300, 1023, 2500, 500); //大臂
	// MachineArm_MiddleNum = map(analogRead(MACHINE_ARM_MIDDLE_PIN), 0, 1023, 2500, 800); //中臂
	// MachineArm_SmallNum = map(analogRead(MACHINE_ARM_SMALL_PIN), 0, 1023, 500, 2500); //水平
	// MachineArm_CatchingNum = map(analogRead(MACHINE_ARM_CATCHING_PIN), 0, 1023, 1700, 1000); //夹手

	/* 读取其他旋钮数据 */
	UserANum = map(analogRead(USER_PIN_A), 0, 1023, 500, 2500); //自定义旋钮A 旋转臂
	UserBNum = map(analogRead(USER_PIN_B), 0, 1023, 500, 2500); //自定义旋钮B 新夹手

	/* 读取按钮数据 */
	if (ControlButtonNum != digitalRead(ROOT_BUTTON)) //主控权限开关
	{
		ControlButtonNum = digitalRead(ROOT_BUTTON);
		// Control_FLAG = 1;
	}
	if (KeepDeepNum != digitalRead(KEEP_DEPTH_BUTTON)) //定深开关
	{
		KeepDeepNum = digitalRead(KEEP_DEPTH_BUTTON);
		// KeepDepth_FLAG = 1;
	}
	if (KeepOrbitNum != digitalRead(KEEP_ORBIT_BUTTON)) //定向开关 一键抓取
	{
		KeepOrbitNum = digitalRead(KEEP_ORBIT_BUTTON);
		// KeepOrbit_FLAG = 1;
	}
	if (ArmNum != digitalRead(MACHINE_ARM_ALL_BUTTON)) //机械臂控制开关 直接控制继电器
	{
		ArmNum = digitalRead(MACHINE_ARM_ALL_BUTTON);
		// MachineArm_ALL_Switch_FLAG = 1;
	}
	if (SideNum != digitalRead(SIDE_PUSH_MODE_SWITCH_BUTTON)) //侧推模式开关
	{
		SideNum = digitalRead(SIDE_PUSH_MODE_SWITCH_BUTTON);
		// Mode_FLAG = 1;
	}
	if (KeepDownNum != digitalRead(MACHINE_ARM_A_BUTTON)) //下潜保持开关
	{
		KeepDownNum = digitalRead(MACHINE_ARM_A_BUTTON);
		// MachineArm_A_Switch_FLAG = 1;
	}
	if (TransSwitchNum != digitalRead(MACHINE_ARM_B_BUTTON)) //传送带启用开关
	{
		TransSwitchNum = digitalRead(MACHINE_ARM_B_BUTTON);
		// MachineArm_B_Switch_FLAG = 1;
	}

	//一键抓取
	if (digitalRead(KEEP_ORBIT_BUTTON))
	{
		GrabStart_FLAG = 1;
	}
}

/**
   @brief 读取漏水数据并通知定时器中断服务函数进行处理
*/
void Surface_Task_ReadSerialData()
{
	static u8 isRev;
	if (Serial.available())
	{
		isRev = Serial.read();
		if (isRev == '1')
		{
			WATER_WARNING = 1;
		}
		else if (isRev == '0')
		{
			WATER_WARNING = 0;
		}
	}
	WaterHandler();
}

/**
   @brief 当定时器中断指示主程序发送后，在主程序内进行发送，避免影响中断
*/
void Surface_Task_SendSerialMesg()
{
	if (ControlButtonNum)
	{
		Serial.print('%'); //发送起始位0x25
		Serial.print(':');
		Serial.print(StrightNum); //前进、后退
		Serial.print(':');
		Serial.print(RotateNum); //左转、右转
		Serial.print(':');
		if (KeepDownNum) //下潜保持
		{
			Serial.print(KeepVerticalDownSpeed);
		}
		else
		{
			Serial.print(VerticalNum); //上浮、下潜
		}
		Serial.print(':');
		if (LightNum < 32768) //灯光控制
		{
			Serial.print(0);
		}
		else if (LightNum >= 32768)
		{
			Serial.print(65535);
		}
		Serial.print(':');
		if (GrabStart_FLAG) //一键抓取
		{
			Serial.print(AutoGrab_FlipNum); //一键抓取旋转臂控制
		}
		else //普通模式
		{
			Serial.print(UserANum); //小臂 旋转臂
		}
		Serial.print(':');
		if (TransSwitchNum) //传送带控制
		{
			if ((ConveyerNum >= 1300) && (ConveyerNum <= 1700)) //传送带保护
			{
				Serial.print(1500);
			}
			else
			{
				Serial.print(ConveyerNum);
			}
		}
		else
		{
			Serial.print(1500);
		}
		Serial.print(':');
		Serial.print(MachineArm_MiddleNum); //中臂
		Serial.print(':');
		Serial.print(MachineArm_HorizentalNum); //水平
		Serial.print(':');
		if (GrabStart_FLAG) //一键抓取
		{
			Serial.print(AutoGrab_ClipNum); //一键抓取夹手控制
		}
		else //普通模式
		{
			Serial.print(UserBNum); //夹手 新夹手
		}
		Serial.print(':');
		Serial.print(CameraPanNum); //云台
		Serial.print(':');
		Serial.print(MachineArm_LargeNum); //大臂
		Serial.print(':');
		Serial.print(1500); //预留机械臂
		Serial.print(':');
		Serial.print(ReservedNum); //预留PWM
		Serial.print(':');
		Serial.print((ArmNum << 3) | (KeepDeepNum << 2) | (0 << 1) | SideNum); //模式开关控制位
		Serial.print(':');
		Serial.print(0); //数据校验位
		Serial.print(':');
		Serial.print('!'); //结束位0x21
		Serial.println();
	}
}

/**
   @brief 在定时器中断内上传串口数据到上位机
*/
// void Surface_IT_SendSerialMesg()
// {
// 	if (ControlButtonNum)
// 	{
// 		Serial.print('%'); //发送起始位0x25
// 		Serial.print(':');
// 		Serial.print(StrightNum); //前进、后退
// 		Serial.print(':');
// 		Serial.print(RotateNum); //左转、右转
// 		Serial.print(':');
// 		if (KeepDownNum) //下潜保持
// 		{
// 			Serial.print(KeepVerticalDownSpeed);
// 		}
// 		else
// 		{
// 			Serial.print(VerticalNum); //上浮、下潜
// 		}
// 		Serial.print(':');
// 		if (LightNum < 32768) //灯光控制
// 		{
// 			Serial.print(0);
// 		}
// 		else if (LightNum >= 32768)
// 		{
// 			Serial.print(65535);
// 		}
// 		Serial.print(':');
// 		Serial.print(MachineArm_SmallNum); //小臂
// 		Serial.print(':');
// 		if (TransSwitchNum) //传送带控制
// 		{
// 			if ((ConveyerNum >= 1300) && (ConveyerNum <= 1700)) //传送带保护
// 			{
// 				Serial.print(1500);
// 			}
// 			else
// 			{
// 				Serial.print(ConveyerNum);
// 			}
// 		}
// 		else
// 		{
// 			Serial.print(1500);
// 		}
// 		Serial.print(':');
// 		Serial.print(MachineArm_MiddleNum); //中臂
// 		Serial.print(':');
// 		Serial.print(MachineArm_HorizentalNum); //水平
// 		Serial.print(':');
// 		Serial.print(UserBNum); //新夹手
// 		Serial.print(':');
// 		Serial.print(CameraPanNum); //云台
// 		Serial.print(':');
// 		Serial.print(UserANum); //大臂
// 		Serial.print(':');
// 		Serial.print(1500); //预留机械臂
// 		Serial.print(':');
// 		Serial.print(ReservedNum); //预留PWM
// 		Serial.print(':');
// 		Serial.print((ArmNum << 3) | (KeepDeepNum << 2) | (KeepOrbitNum << 1) | SideNum); //模式开关控制位
// 		Serial.print(':');
// 		Serial.print(0); //数据校验位
// 		Serial.print(':');
// 		Serial.print('!'); //结束位0x21
// 		Serial.println();
// 	}
// }

/**
   @brief 漏水处理函数
*/
void WaterHandler()
{
	/* 以25为阈值进行滤波 */
	if (WATER_WARNING == 1)
	{
		WATER_WARNING_TIMER_COUNT++;
		if (WATER_WARNING_TIMER_COUNT >= 100) //累计100次仍存在警报，则进入漏水处理状态
		{
			WATER_WARNING_TIMER_COUNT = 100;
			digitalWrite(WATER_DETECTED_WARNING, WATER_WARNING);
		}
	}
	else if (WATER_WARNING == 0)
	{
		WATER_WARNING_TIMER_COUNT--;
		if (WATER_WARNING_TIMER_COUNT <= 0) //累计100次警报消失，则退出漏水处理状态
		{
			WATER_WARNING_TIMER_COUNT = 0;
			digitalWrite(WATER_DETECTED_WARNING, WATER_WARNING);
		}
	}
}
