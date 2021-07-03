# 控制板与电源板硬件设计

**horziontal**：第一版叠层5V/3.3V电源板，已废弃

Fatal：存在功率电感容易过流损坏的问题

**horziontal_v2**：第二版叠层5V/3.3V电源板，采用大功率绕线电感并优化了布线

Debug：修正了电感画错（没有旋转90度导致大短路）的问题

**vertical**：直插5V/3.3V电源板，测试阶段未见Bug

Fatal：老师觉得难看

**Processor**：包含PRO和PRO2两版主控板设计，PRO单纯作为留档，PRO2修正了PRO中存在的一些问题。

Debug：IIC接口添加4.7K上拉电阻

Debug：添加JTAG接口

Debug：添加可选的AMS1117辅助供电

**ProConnect**：网口、摄像头转接板

Debug：JP3所接LAN口无法使用（有供电但无信号传输），暂时无法确认是否为PCB问题

### 几个板子都是新手走线，高血压警告