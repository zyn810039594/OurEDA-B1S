#ifndef _SETTING_H_
#define _SETTING_H_

//选择运行仓位
//#define CtrlSide
#define PowerSide

//数据校验开关
//#define DataIdentify

//PID数据位
//定深PID参数
//定深比例系数
#define PID_D_Kp 200
//定深积分系数
#define PID_D_Ki 2
//定深微分系数
#define PID_D_Kd 0

//定向PID参数
//定向比例系数
#define PID_O_Kp 100
//定向积分系数
#define PID_O_Ki 1
//定向微分系数
#define PID_O_Kd 0

#endif
