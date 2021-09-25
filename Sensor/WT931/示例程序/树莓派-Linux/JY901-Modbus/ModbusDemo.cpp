#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
#include<assert.h>
#include<termios.h>
#include<string.h>
#include<sys/time.h>
#include<time.h>
#include<sys/types.h>
#include<errno.h>

static int ret;
static int fd;

#define BAUD 9600

int uart_open(int fd,const char *pathname)
{
    fd = open(pathname, O_RDWR|O_NOCTTY); 
    if (-1 == fd)
    { 
        perror("Can't Open Serial Port"); 
		return(-1); 
	   } 
    else
		printf("open %s success!\n",pathname);
    /*测试是否为终端设备*/ 
    if(isatty(STDIN_FILENO)==0) 
		printf("standard input is not a terminal device\n"); 
    else 
		printf("isatty success!\n"); 
    return fd; 
}

int uart_set(int fd,int nSpeed, int nBits, char nEvent, int nStop)//串口名、波特率、数据位、奇偶校验位、停止位
{
     struct termios newtio,oldtio; 
     /*保存测试现有串口参数设置，在这里如果串口号等出错，会有相关的出错信息*/ 
     if  ( tcgetattr( fd,&oldtio)  !=  0) {  
      perror("SetupSerial 1");
	   printf("tcgetattr( fd,&oldtio) -> %d\n",tcgetattr( fd,&oldtio)); 
      return -1; 
     } 
     bzero( &newtio, sizeof( newtio ) ); 
     /*步骤一，设置字符大小*/ 
     newtio.c_cflag  |=  CLOCAL | CREAD;  
     newtio.c_cflag &= ~CSIZE;  
/*设置数据位*/ 
     switch( nBits ) 
     { 

      case 7: 

      newtio.c_cflag |= CS7; 

      break; 

      case 8: 

      newtio.c_cflag |= CS8; 

      break; 

     } 

/*设置奇偶校验位*/ 

     switch( nEvent ) 

     { 

     case 'o':

     case 'O': //奇数 

      newtio.c_cflag |= PARENB; 

      newtio.c_cflag |= PARODD; 

      newtio.c_iflag |= (INPCK | ISTRIP); 

      break; 

     case 'e':

     case 'E': //偶数 

      newtio.c_iflag |= (INPCK | ISTRIP); 

      newtio.c_cflag |= PARENB; 

      newtio.c_cflag &= ~PARODD; 

      break;

     case 'n':

     case 'N':  //无奇偶校验位 

      newtio.c_cflag &= ~PARENB; 

      break;

     default:

      break;

     } 

/*设置波特率*/ 

  switch( nSpeed ) 

     { 

     case 2400: 

      cfsetispeed(&newtio, B2400); 

      cfsetospeed(&newtio, B2400); 

      break; 

     case 4800: 

      cfsetispeed(&newtio, B4800); 

      cfsetospeed(&newtio, B4800); 

      break; 

     case 9600: 

      cfsetispeed(&newtio, B9600); 

      cfsetospeed(&newtio, B9600); 

      break; 

     case 115200: 

      cfsetispeed(&newtio, B115200); 

      cfsetospeed(&newtio, B115200); 

      break; 

     case 460800: 

      cfsetispeed(&newtio, B460800); 

      cfsetospeed(&newtio, B460800); 

      break; 

     default: 

      cfsetispeed(&newtio, B9600); 

      cfsetospeed(&newtio, B9600); 

     break; 

     } 

/*设置停止位*/ 

     if( nStop == 1 ) 

      newtio.c_cflag &=  ~CSTOPB; 

     else if ( nStop == 2 ) 

      newtio.c_cflag |=  CSTOPB; 

/*设置等待时间和最小接收字符*/ 

     newtio.c_cc[VTIME]  = 0; 

     newtio.c_cc[VMIN] = 0; 

/*处理未接收字符*/ 

     tcflush(fd,TCIFLUSH); 

/*激活新配置*/ 
if((tcsetattr(fd,TCSANOW,&newtio))!=0) 

     { 

      perror("com set error"); 

      return -1; 

     } 

  printf("set done!\n"); 

  return 0; 

}



int uart_close(int fd)
{
    assert(fd);
    close(fd);

    /*可以在这里做些清理工作*/

    return 0;
}


int send_data(int  fd, char *send_buffer,int length)
{
	length=write(fd,send_buffer,length*sizeof(unsigned char));
	return length;
}


int recv_data(int fd, char* recv_buffer,int length)//读取字节数
{
	length=read(fd,recv_buffer,length);
  
	return length;
}

char* SendData()//发送给模块
{
  static char byteSend[8];
  byteSend[0] = 0x50;
  byteSend[1] = 0x03;
  byteSend[2] = 0x00;//寄存器高位
  byteSend[3] = 0x34;//寄存器低位 //角度寄存器
  byteSend[4] = 0x00;//寄存器个数高位
  byteSend[5] = 0x0C;//寄存器个数低位 //加速度、角速度、磁场、角度共十二个
  byteSend[6] = 0x00;//CRCH
  byteSend[7] = 0x00;//CRCL
  return byteSend;


}


float a[3],w[3],Angle[3];
int h[3];

void ParseData(char chr)//处理数据
{
  //printf("chr:%x  ",chr);
	static char chrBuf[100];
	static unsigned char chrCnt=0;
  
	signed short sData[12];
	int i;

	time_t now;
	chrBuf[chrCnt++]=chr;
  if (chrCnt<29) //满29字节进行下一步
  {
    return;
  }

	if ((chrBuf[0]!=0x50) || (chrBuf[1]!=0x03) || chrBuf[2]!=0x18) 
  {
    printf("Error:chrBuf[0]=%x chrBuf[1]=%x chrBuf[2]=%x \r\n",chrBuf[0],chrBuf[1],chrBuf[2]);
    memcpy(&chrBuf[0],&chrBuf[1],10);
    chrCnt--;
    return;
  }
	
   for(i=0;i<12;i++)
   {
     sData[i]=(short)((chrBuf[3+i*2]<<8)|chrBuf[4+i*2]);
     //printf("data[%d]=%x  ",i,sData[i] );
   }

  for(i=0;i<3;i++)
  {
    
    a[i] = (float)(sData[i])/32768.0*16.0;//加速度

    w[i] = (float)(sData[3+i])/32768.0*2000.0;//角速度
    
    h[i] = (int)(sData[6+i])*1.0;//磁场
    
    Angle[i] = (float)(sData[9+i])/32768.0*180.0;//角度
    
  }
  time(&now);
  
  chrCnt=0;
  printf("\r\nT:%s ",asctime(localtime(&now)));
  printf("a:%6.3f %6.3f %6.3f ",a[0],a[1],a[2]); 
  printf("w:%7.3f %7.3f %7.3f ",w[0],w[1],w[2]);
  printf("A:%7.3f %7.3f %7.3f ",Angle[0],Angle[1],Angle[2]);
  printf("h:%d %d %d ",h[1],h[1],h[2]);   
	printf("\n");
  return;	
}

int RW_flag;

int main(void)
{
  char r_buf[1024];
  bzero(r_buf,1024);

  char *w_buf=SendData();

  fd = uart_open(fd,"/dev/ttyUSB0"); /*串口号/dev/ttySn,USB口号/dev/ttyUSBn*/
  if(fd == -1)
  {
    fprintf(stderr,"uart_open error\n");
    exit(EXIT_FAILURE);
  }

  if(uart_set(fd,BAUD,8,'N',1) == -1)//初始化串口（串口名、波特率、数据位、奇偶校验位、停止位）
  {
    fprintf(stderr,"uart set failed!\n");
    exit(EXIT_FAILURE);
  }

	FILE *fp;
	fp = fopen("Record.txt","w");


  RW_flag=0;

  while(1)
  {
    //printf("RW_flag=%d\n", );
    if(RW_flag==0)
    {
      
      //printf("111RW_flag=%d  %x\n", RW_flag,*(w_buf));
      ret = send_data(fd,w_buf,8);//写入到串口
      if(ret == -1)
      {
        fprintf(stderr,"uart write failed!\n");
        exit(EXIT_FAILURE);
      }
      usleep(50000);
      RW_flag=1;
    }

    if(RW_flag==1)
    {
      
      bzero(r_buf,1024);
      ret = recv_data(fd,r_buf,29);//从串口读出
      if(ret == -1)
      {
        fprintf(stderr,"uart read failed!\n");
        exit(EXIT_FAILURE);
      }
      for(int i=0;i<ret;i++) 
      {
        fprintf(fp,"%2X ",r_buf[i]);
        ParseData(r_buf[i]);
      }
      usleep(50000);
      RW_flag=0;
    } 

    usleep(1000);

  }

  ret = uart_close(fd);
  if(ret == -1)
  {
    fprintf(stderr,"uart_close error\n");
    exit(EXIT_FAILURE);
  }

  exit(EXIT_SUCCESS);
}
