# OurEDA-B1S

一个水下机器人（ROV/AUV）的下位仓控制系统

请使用Arduino、CubeMX、CubeIDE访问相关代码，使用AD打开硬件原理图和PCB Layout设计

### 目录说明

* Board 硬件电路图与PCB layout

  * CtrlBoard

    控制仓（主控仓）电路原理图与PCB layout

  * PowBoard

    电源仓大功率电源电路原理图与PCB layout

  * PowCtrlBoard

    电源仓（PWM控制仓）电路原理图与PCB layout

  * 部分处理姿态控制算法（使用PID）
  * 收集汇总传感器数据（间隔发送本仓传感器（比电源仓多加一个水深传感器）和电源仓传感器数据）
  * 将传感器数据传输到水面端或自主控制仓
  * 接收上位机指令
  * 将指令传给电源仓的H750进行执行

* DeepF1

  水深传感器数据解析程序

  使用STM32F103C6T6

  * 专用于处理水深传感器的数据

  * 使用Arduino开发

* CtrlH7

  控制仓STM32H750程序

  * 部分处理姿态控制算法（使用PID）
  * 收集汇总传感器数据（间隔发送本仓传感器（比电源仓多加一个水深传感器）和电源仓传感器数据）
  * 将传感器数据传输到水面端或自主控制仓
  * 接收上位机指令
  * 将指令传给电源仓的H750进行执行

* PowH7

  电源仓控制板STM32H750程序

  * 进行温湿度气压、九轴传感器数据解析
  * 将传感器数据传输到控制仓的STM32H750
  * 接收上位机发送的PWM指令控制所有PWM输出

* MT7621

  机器人视频推流、串口网口转换、网络通讯中心设备

  使用成品MT7621路由器模块

  固件、软件配置、内核版本等

* Debug

  在调试过程中遇到重大Bug的解决方案

* Doc

  整体设计文档

* General

  上位机程序

* ControlLibs

  代码中复用的数据传输库文件，可以较方便地移植到其他类似架构的ROV上

* GY-39

  温湿度传感器资料

* P30

  声呐水深传感器资料

  * 使用submodule引用了一个上位机程序

* WT931

  九轴传感器资料

* surface

  上位机内部Arduino程序

  * 使用Arduino获取操纵旋钮的信号

* DataFormat

  Debug时用到的一些上传/下传数据

### 文件说明

* UpFormat.xlsx

  上传数据格式

* DownFormat.xlsx

  下传指令格式

