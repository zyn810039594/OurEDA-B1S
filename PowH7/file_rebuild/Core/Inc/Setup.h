#ifndef SETTING_H
#define SETTING_H

//选择运行仓位
//#define CtrlSide
#define PowerSide

//数据校验开关
//#define DataIdentify

//串口位置定义
//接九轴板
#define WT931_UART huart2
//接温湿度气压
#define GY39_UART huart1
//接深度解算板
#define Deep_UART huart1
//接上位机
#define Up_UART huart3
//接下位机
#define Down_UART huart3

//PID数据位
//定深PID参数
//定深比例系数
#define PID_D_Kp 200
//定深积分系数
#define PID_D_Ki 2
//定深微分系数
#define PID_D_Kd 300

//定向PID参数
//定向比例系数
#define PID_O_Kp 100
//定向积分系数
#define PID_O_Ki 1
//定向微分系数
#define PID_O_Kd 150

//串口长度定义
#define Deep_UART_RXLen 4
#define GY39_UART_RXLen 18
#define GY39_UART_TXLen 3
#define WT931_UART_RXLen 44
#define Up_UART_TXLen 47
#define Up_UART_RXLen 30
#define Down_UART_RXLen 47
#define Down_UART_TXLEN 30

#endif
