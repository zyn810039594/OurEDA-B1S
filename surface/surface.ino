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
#include "surface.h"

#ifndef DO_NOT_USING_IT_FLAG
#include <FlexiTimer2.h>
#endif

 /* 字符串输出变量 */
u8 WATER_WARNING = 0; //漏水警报
u8 WATER_WARNING_TIMER_COUNT = 0; //漏水警报计时

int StrightNum = 0; //前进后退 A0
int RotateNum = 0; //旋转侧推 A0
int VerticalNum = 0; //垂直 A5
int LightNum = 0; //灯光 A2
int CameraPanNum = 0; //云台 A4
int ConveyerNum = 0; //传送带 A3
int MachineArm_HorizentalNum = 0; //水平机械臂 A11
int MachineArm_LargeNum = 0; //大臂 A12
int MachineArm_MiddleNum = 0; //中臂 A13
int MachineArm_SmallNum = 0; //小臂 A14
int MachineArm_CatchingNum = 0; //夹取 A15

int ReservedNum = 0; //保留位PWM控制数据
int UserANum = 0; //自定义旋钮A A6
int UserBNum = 0; //自定义旋钮B A7

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
u8 MachineArm_B_Switch_FLAG = 0; //机械臂B开关
u8 SideNum = 0;
u8 Mode_FLAG = 0; //侧推旋转模式控制

static void PrintBlank(void)
{
	for (int i = 50000;i > 0;--i)
	{
		NOP;
	}
}

#ifndef DO_NOT_USING_IT_FLAG
/**
 * @brief 定时器中断服务程序
 */
void TimerInterrupt()
{
	if (WATER_WARNING == 1)
	{
		WATER_WARNING_TIMER_COUNT++;
		if (WATER_WARNING_TIMER_COUNT >= 25) //累计25次定时器中断仍存在警报，则进入漏水处理状态
		{
			WaterHandler();
		}
	}
	Surface_IT_SendSerialMesg();
}
#endif

void setup()
{
	Serial.begin(9600);

	pinMode(WATER_DETECTED_WARNING, OUTPUT); //指示漏水警报

	/* 初始化开关控制引脚 */
	pinMode(KEEP_DEPTH_BUTTON, INPUT); //定深开关
	pinMode(ROOT_BUTTON, INPUT); //控制权限开关
	pinMode(MACHINE_ARM_A_BUTTON, INPUT); //机械臂A开关 下潜保持
	pinMode(MACHINE_ARM_ALL_BUTTON, INPUT); //机械臂总开关
	pinMode(MACHINE_ARM_B_BUTTON, INPUT); //TODO:机械臂B开关暂不启用
	pinMode(SIDE_PUSH_MODE_SWITCH_BUTTON, INPUT); //侧推模式开关
	pinMode(KEEP_ORBIT_BUTTON, INPUT); //定向开关

#ifndef DO_NOT_USING_IT_FLAG
	FlexiTimer2::set(200, TimerInterrupt);
	FlexiTimer2::start();
#endif
}

void loop()
{
	Surface_Task_Contrl();
	Surface_Task_ReadSerialData();
	Surface_Task_SendSerialMesg();
}

/**
 * @brief 读取漏水数据并通知定时器中断服务函数进行处理
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
}

/**
 * @brief 实时读取控制按钮和控制杆数据
 */
void Surface_Task_Contrl(void)
{
	/* 控制数据映射 */
	StrightNum = map(analogRead(FORWARD_PIN), 0, 1023, 500, 2500);
	RotateNum = map(analogRead(ROTATE_PIN), 0, 1023, 500, 2500);
	VerticalNum = map(analogRead(VERTICAL_PIN), 0, 1023, 500, 2500);
	LightNum = map(analogRead(LIGHT_PIN), 0, 1023, 500, 2500);
	CameraPanNum = map(analogRead(CAMERA_PAN_PIN), 0, 1023, 500, 2500);
	ConveyerNum = map(analogRead(Conveyer_PIN), 0, 1023, 500, 2500);
	MachineArm_HorizentalNum = map(analogRead(MACHINE_ARM_HORIZENTAL_PIN), 0, 1023, 500, 2500);
	MachineArm_LargeNum = map(analogRead(MACHINE_ARM_LARGE_PIN), 0, 1023, 500, 2500);
	MachineArm_MiddleNum = map(analogRead(MACHINE_ARM_MIDDLE_PIN), 0, 1023, 500, 2500);
	MachineArm_SmallNum = map(analogRead(MACHINE_ARM_SMALL_PIN), 0, 1023, 500, 2500);
	MachineArm_CatchingNum = map(analogRead(MACHINE_ARM_CATCHING_PIN), 0, 1023, 500, 2500);


	/* 按钮判断 */
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
	if (KeepOrbitNum != digitalRead(KEEP_ORBIT_BUTTON)) //定向开关
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
	if (KeepDownNum != digitalRead(MACHINE_ARM_A_BUTTON))//下潜保持开关
	{
		KeepDownNum = digitalRead(MACHINE_ARM_A_BUTTON);
		// MachineArm_A_Switch_FLAG = 1;
	}
}

#ifdef DO_NOT_USING_IT_FLAG
/**
 * @brief 当定时器中断指示主程序发送后，在主程序内进行发送，避免影响中断
 */
void Surface_Task_SendSerialMesg(void)
{
	if (ControlButtonNum)
	{
		Serial.print('$'); //发送起始位0x25
		Serial.print(':');
		Serial.print(StrightNum);
		Serial.print(':');
		Serial.print(RotateNum);
		Serial.print(':');
		if (KeepDownNum)
		{
			Serial.print(KeepVerticalDownSpeed);
		}
		else
		{
			Serial.print(VerticalNum);
		}
		Serial.print(':');
		Serial.print(LightNum);
		Serial.print(':');
		Serial.print(CameraPanNum);
		Serial.print(':');
		Serial.print(ConveyerNum);
		Serial.print(':');

		//BUG:这里的数据排列可能存在错误
		Serial.print(MachineArm_HorizentalNum);
		Serial.print(':');
		Serial.print(MachineArm_LargeNum);
		Serial.print(':');
		Serial.print(MachineArm_MiddleNum);
		Serial.print(':');
		Serial.print(MachineArm_SmallNum);
		Serial.print(':');
		Serial.print(MachineArm_CatchingNum);
		Serial.print(':');

		Serial.print(ReservedNum);
		Serial.print(':');
		Serial.print((ArmNum << 3) | (KeepDeepNum << 2) | (KeepOrbitNum << 1) | SideNum); //模式开关控制位
		Serial.print(':');
		Serial.print(0); //数据校验位
		Serial.print(':');
		Serial.print('%');
		Serial.println();
	}
}
#else
/**
 * @brief 在定时器中断内上传串口数据到上位机
 */
void Surface_IT_SendSerialMesg(void)
{
	Serial.print('$'); //发送起始位0x25
	Serial.print(StrightNum);
	Serial.print(':');
	Serial.print(RotateNum);
	Serial.print(':');
	Serial.print(VerticalNum);
	Serial.print(':');
	Serial.print(LightNum);
	Serial.print(':');
	Serial.print(CameraPanNum);
	Serial.print(':');
	Serial.print(ConveyerNum);
	Serial.print(':');

	//BUG:这里的数据排列可能存在错误
	Serial.print(MachineArm_HorizentalNum);
	Serial.print(':');
	Serial.print(MachineArm_LargeNum);
	Serial.print(':');
	Serial.print(MachineArm_MiddleNum);
	Serial.print(':');
	Serial.print(MachineArm_SmallNum);
	Serial.print(':');
	Serial.print(MachineArm_CatchingNum);
	Serial.print(':');

	Serial.print(ReservedNum);
	Serial.print(':');
	Serial.print(Mode_FLAG);
	Serial.print(':');
	Serial.print("0000");
	Serial.print(':');
	Serial.print('%');
	Serial.println();
}
#endif

/**
 * @brief 漏水处理函数
 */
void WaterHandler()
{
	WATER_WARNING_TIMER_COUNT = 25;
	digitalWrite(WATER_DETECTED_WARNING, WATER_WARNING);
}

