这里的文件用于处理莫名其妙的串口bug

> 根据目前的debug，该错误由于从VS-keil插件迁移到CubeIDE造成

> **很可能是因为CubeIDE的gcc编译器将变量错误优化到了h750的DTCM中**

> 而h750的**DMA无法支持对DTCM进行读写**

Bug已解决

原因：CubeIDE的gcc编译器将变量错误优化到了h750的DCache中，DMA无法访问，但检测到DTCM中有被DCache映射到的数据后直接默认为完成了相关操作，于是将数据转发到串口，导致串口只输出0，并且无法通过指针按地址传递数据

解决方法：

1. 关闭DCache、ICache

2. 将独立的指针改为结构体，直接通过结构体指针传递数据

3. 使用`__attribute__`修改变量保存位置到RAM

   ```c
   __attribute__((section(".RAM_D1")))
   ```

4. 设置新的.ld文件更改变量保存位置

