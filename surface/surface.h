/**
 * @file surface.h
 * @brief 
 * @author RedlightASl (dddbbbdd@foxmail.com)
 * @version 1.0
 * @date 2021-08-28
 * 
 * @copyright Copyright (c) 2021  RedlightASl
 * 
 * @par 修改日志:
 * <table>
 * <tr><th>Date       <th>Version <th>Author  <th>Description
 * <tr><td>2021-08-28 <td>1.0     <td>wangh     <td>Content
 * </table>
 */
#ifndef __SURFACE_H
#define __SURFACE_H

#define DO_NOT_USING_IT_FLAG
//是否使用定时器中断定时发送数据
#define u8 unsigned char
#define NOP do { __asm__ __volatile__ ("nop"); } while (0)

const static u8 WATER_DETECTED_WARNING = 2; //漏水检测指示灯
const static u8 KEEP_DEPTH_BUTTON = 3; //定深开关按钮
const static u8 KEEP_ORBIT_BUTTON = 9; //定向开关按钮
const static u8 ROOT_BUTTON = 4; //控制按钮
const static u8 MACHINE_ARM_A_BUTTON = 5; //机械臂A开关按钮 下潜保持
const static u8 MACHINE_ARM_B_BUTTON = 7; //机械臂B开关按钮
const static u8 MACHINE_ARM_ALL_BUTTON = 6; //机械臂总开关按钮
const static u8 SIDE_PUSH_MODE_SWITCH_BUTTON = 8; //侧推模式控制开关按钮

const static u8 FORWARD_PIN = 0; //前进后退旋钮
const static u8 ROTATE_PIN = 1; //左转右转旋钮
const static u8 VERTICAL_PIN = 5; //垂直运动旋钮
const static u8 LIGHT_PIN = 2; //灯光旋钮
const static u8 CAMERA_PAN_PIN = 4; //云台旋钮
const static u8 Conveyer_PIN = 3; //传送带旋钮
const static u8 MACHINE_ARM_HORIZENTAL_PIN = 11; //水平机械臂旋钮
const static u8 MACHINE_ARM_LARGE_PIN = 12; //大臂旋钮
const static u8 MACHINE_ARM_MIDDLE_PIN = 13; //中臂旋钮
const static u8 MACHINE_ARM_SMALL_PIN = 14; //小臂旋钮
const static u8 MACHINE_ARM_CATCHING_PIN = 15; //夹取旋钮

const static u8 USER_PIN_A = 6; //自定义旋钮A
const static u8 USER_PIN_B = 7; //自定义旋钮B

const static int KeepVerticalDownSpeed = 1300; //下潜保持速度

#endif
