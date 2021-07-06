# OurEDA-B1S

一个水下机器人（ROV/AUV）的下位仓控制系统

请使用Arduino、CubeMX、CubeIDE访问相关代码，使用AD打开硬件原理图和PCB Layout设计

### 目录说明

* CtrlBoard

  控制仓电路原理图与PCB layout

* CtrlH7

  控制仓STM32H750程序

  * 部分处理姿态控制算法（使用PID）
  * 收集汇总传感器数据
  * 将传感器数据传输到水面端或自主控制仓

* DeepF1

  水深传感器数据解析程序

  使用STM32F103C6T6

  * 专用于处理水深传感器的数据
  * 使用Arduino开发

* PowBoard

  电源仓大功率电源电路原理图与PCB layout

* PowCtrlBoard

  电源仓控制板电路原理图与PCB layout

* PowH7

  电源仓控制板STM32H750程序

  * 进行温湿度、气压、九轴传感器数据解析
  * 将传感器数据传输到控制仓的STM32H750
  * 接收上位机发送的PWM指令控制所有PWM输出