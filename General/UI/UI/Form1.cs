using CefSharp;
using CefSharp.WinForms;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.IO;
using System.IO.Ports;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace UI
{
    public partial class Form1 : Form
    {

        string command = "sadasd1";
        string autocom = "1";
        string[] comm = new string[20];//控制仓
        string[] comm2 = new string[20];//电源仓
        Thread myThread;
        public delegate void MyDelegateUI(); //定义委托类型
        MyDelegateUI myDelegateUI; //声明委托对象
        static Socket ClientSocket;
        static Socket ServerSocket;
        SerialPort sp = new SerialPort();
        SerialPort sp2 = new SerialPort();//媒体收传感数据
        ChromiumWebBrowser chromeBrowser;
        ChromiumWebBrowser chromeBrowser2;
        ChromiumWebBrowser chromeBrowser3;
        ChromiumWebBrowser chromeBrowser4;
        ChromiumWebBrowser chromeBrowser5;
        ChromiumWebBrowser chromeBrowser6;
        ChromiumWebBrowser chromeb;
        Color[] color1 = { Color.FromArgb(67, 67, 67), Color.FromArgb(0, 64, 0),Color.FromArgb(255, 77, 59) };
        //string jsck = "信息监视窗口";
        Color ck = Color.FromArgb(67, 67, 67);//串口状态
        Color wk= Color.FromArgb(67, 67, 67);//网口状态
        string[] temcomm=new string[17];

        Color kzc = Color.FromArgb(0, 192, 0);//控制舱是否漏水
        Color dyc = Color.FromArgb(0, 192, 0);//电源舱是否漏水
        Color kzqx = Color.FromArgb(67, 67, 67);//控制权限
        Color sd1 = Color.FromArgb(67, 67, 67);//
        int ks1 = -1, ks2 = -1;
        int qjht = 0;//前进  后退

        int zy1 = 0;//左右
        int sx1 = 0;//上下
        int scd1 = 0;
        double roll = 0;
        double pitch = 0;
        double yaw = 0;
        public Form1()
        {
            InitializeComponent();
            CefSettings settings = new CefSettings();
            Cef.Initialize(settings);
           
        }


        private void Form1_Load(object sender, EventArgs e)
        {

            torectangle();

            temcomm[6] = "1500";
            comm[0] = "0";
            comm[1] = "0";
            comm[2] = "0";
            comm[3] = "0";
            comm[4] = "0";
            comm[5] = "0";
            comm[6] = "0";
            comm[7] = "0";
            comm[8] = "0";
            comm[9] = "1";
            comm[10] = "0";
            comm[11] = "0";
            comm[12] = "0";
            comm[13] = "0";
            comm[14] = "0";
            comm[15] = "0";
            comm[16] = "0";
            comm[17] = "0";
            comm[18] = "0";

            comm2[0] = "0";
            comm2[1] = "0";
            comm2[2] = "0";
            label4.BringToFront();
            myThread = new Thread(doWork);
            myDelegateUI = new MyDelegateUI(initAll);//绑定委托
            myThread.Start();
      
        }

        void torectangle()
        {
            GraphicsPath gp1 = new GraphicsPath();
            GraphicsPath gp3 = new GraphicsPath();
            GraphicsPath gp4 = new GraphicsPath();
            GraphicsPath gp5 = new GraphicsPath();
            GraphicsPath gp6 = new GraphicsPath();
            gp1.AddEllipse(pictureBox1.ClientRectangle);
            gp3.AddEllipse(pictureBox3.ClientRectangle);
            gp4.AddEllipse(pictureBox4.ClientRectangle);
            gp5.AddEllipse(pictureBox5.ClientRectangle);
            gp6.AddEllipse(pictureBox6.ClientRectangle);
            Region region1 = new Region(gp1);
            Region region3 = new Region(gp3);
            Region region4 = new Region(gp3);
            Region region5 = new Region(gp3);
            Region region6 = new Region(gp3);
            pictureBox1.Region = region1;
            pictureBox3.Region = region3;
            pictureBox4.Region = region4;
            pictureBox5.Region = region5;
            pictureBox6.Region = region6;

            gp1.Dispose();
            region1.Dispose();
            gp3.Dispose();
            region3.Dispose();

            gp4.Dispose();
            region4.Dispose();
            gp5.Dispose();
            region5.Dispose();
            gp6.Dispose();
            region6.Dispose();



        }
        private void initAll() //信息处理函数定义
        {
        
            listBox2.Items.Insert(0, autocom);
            label1.BackColor = ck;
            label2.BackColor = wk;
            label4.Text = "温度:" + Math.Round(Convert.ToDouble(comm[0]), 1) + "℃";//仓内温度
            label5.Text = "湿度:" + comm[2] + "%";//湿度
            label24.Text = "气压:"+ Math.Round(Convert.ToDouble(comm[1]),1)+"KPa";
            label33.Text = "气压:" + Math.Round(Convert.ToDouble(comm2[1]), 1) + "KPa";
            label11.Text = "温度:" + Math.Round(Convert.ToDouble(comm2[0]), 1) + "℃";
            label10.Text = "湿度:" + comm2[2].ToString() + "%";
            label13.Text = "水温:" + Math.Round(Convert.ToDouble(comm[11]), 1) + "℃";//水温
            label20.Text = "深度:" + Math.Round(Convert.ToDouble(comm[12]), 2) + "M";//水深

            //label17.Text = "横纵角" + Math.Round(360.0-(Convert.ToDouble(comm[3]) / 32768.0),1) + "°";///Roll
            //label18.Text = "俯仰角" + Math.Round(Convert.ToDouble(comm[4]) / 32768.0,1) + "°";//Pitch
            if( Convert.ToInt32((Convert.ToDouble(comm[3]) / 32768.0))<60)
                label17.Text = "横滚角-" + Convert.ToInt32(Convert.ToDouble(comm[3]) / 32768.0) + "°";///Roll

            if (Convert.ToInt32((Convert.ToDouble(comm[3]) / 32768.0)) > 300)
                label17.Text = "横滚角" +(360-Convert.ToInt32(Convert.ToDouble(comm[3]) / 32768.0) )+ "°";///Roll

            label18.Text = "俯仰角" + Convert.ToInt32(Convert.ToDouble(comm[4]) / 32768.0) + "°";//Pitch
            horiCt1.Hori_Disp(Convert.ToDouble(comm[4])/32768.0, -Convert.ToDouble(comm[3]) / 32768.0); 
            //label19.Text = "航向:" + Math.Round(Convert.ToDouble(comm[5]),1);
            label19.Text = "航向:" +Convert.ToInt32(comm[5])+"°";
            //  label20.Text = "深度:" + comm[12]+"M"; 
            label21.Text = "高度:" + Math.Round(Convert.ToDouble(comm[9]), 2) + "M";  
            label6.BackColor = kzc;
            label8.BackColor = dyc;
            label31.BackColor = kzqx;
            //label32.BackColor = sd1;

            label35.Text = qjht.ToString();
            label36.Text = zy1.ToString();
            label37.Text = sx1.ToString();
            if(qjht>=0)
                ucProcessLine1.Value = qjht;
            else
                ucProcessLine1.Value = -qjht;
            if (zy1 >= 0)
                ucProcessLine2.Value = zy1;
            else
                ucProcessLine2.Value = -zy1;
            if (sx1 >= 0)
                ucProcessLine3.Value = sx1;
            else
                ucProcessLine3.Value = -sx1;



            //侧推1 定向2 定深4 机械臂8
            //25 23        22  27
            if (scd1 > 0) { ucConveyor1.ConveyorDirection = HZH_Controls.Controls.ConveyorDirection.Forward;ucArrow1.BackColor = color1[1]; }
            if (scd1 < 0) { ucConveyor1.ConveyorDirection = HZH_Controls.Controls.ConveyorDirection.Backward; ucArrow2.BackColor = color1[1]; }
            if (scd1 == 0) { ucConveyor1.ConveyorDirection = HZH_Controls.Controls.ConveyorDirection.None; ucArrow1.BackColor = color1[2]; ucArrow2.BackColor = color1[2]; }
             //侧推模式
            if (temcomm[14] == "1"){ 
                label25.BackColor = color1[1];label22.BackColor = color1[0];
                label23.BackColor = color1[0];label27.BackColor = color1[0];
            
            }
            else if (temcomm[14] == "9") { 
                label25.BackColor = color1[1]; 
                label27.BackColor = color1[1]; 
                label22.BackColor = color1[0];
                label23.BackColor = color1[0];
            }
            else if (temcomm[14] == "2") { label23.BackColor = color1[1]; label22.BackColor = color1[0];
                label25.BackColor = color1[0]; label27.BackColor = color1[0];
            }//定向
            else if (temcomm[14] == "4") { label22.BackColor = color1[1]; label23.BackColor = color1[0];
                label25.BackColor = color1[0]; label27.BackColor = color1[0];
            }//定深
            else if (temcomm[14] == "8") { label27.BackColor = color1[1];
                label22.BackColor = color1[0]; label23.BackColor = color1[0];
                label25.BackColor = color1[0];
            }//机械臂
            else if (temcomm[14] == "3") {
                label23.BackColor = color1[1]; 
                label25.BackColor = color1[1]; 
                label22.BackColor = color1[0];
                label27.BackColor = color1[0];
            }
            else if (temcomm[14] == "5") { 
                label22.BackColor = color1[1]; label25.BackColor = color1[1];
                label23.BackColor = color1[0]; label27.BackColor = color1[0];

            }
            else if (temcomm[14] == "6") { 
                label22.BackColor = color1[1]; label23.BackColor = color1[1];
                label25.BackColor = color1[0]; label27.BackColor = color1[0];
            }
            else if (temcomm[14] == "10") { 
                label23.BackColor = color1[1]; label27.BackColor = color1[1];
                label22.BackColor = color1[0]; label25.BackColor = color1[0];
            }
            else if (temcomm[14] == "12") { 
                
                label22.BackColor = color1[1]; label27.BackColor = color1[1];
                label23.BackColor = color1[0]; label25.BackColor = color1[0];

            }
            else if (temcomm[14] == "7") { label22.BackColor = color1[1]; label23.BackColor = color1[1]; label25.BackColor = color1[1]; label27.BackColor = color1[0]; }
            else if (temcomm[14] == "11") { label27.BackColor = color1[1]; label23.BackColor = color1[1]; label25.BackColor = color1[1]; label22.BackColor = color1[0]; }
            else if (temcomm[14] == "14") { label27.BackColor = color1[1]; label23.BackColor = color1[1]; label22.BackColor = color1[1]; label25.BackColor = color1[0]; }
            else if (temcomm[14] == "15") {
                label27.BackColor = color1[1]; label23.BackColor = color1[1]; 
                label22.BackColor = color1[1]; label25.BackColor = color1[1];
            }
            else { label22.BackColor = color1[0]; label23.BackColor = color1[0]; label25.BackColor = color1[0]; label27.BackColor = color1[0]; }


            if (Convert.ToInt32(temcomm[3]) == 1300)
                label28.BackColor = color1[1];
            else
                label28.BackColor = color1[0];
          
                label26.Text = "云台:" + (int)((Convert.ToInt32(temcomm[10]) - 500)/10);
           

            if(Convert.ToInt32(temcomm[6]) > 1520)
            {
                //label29.BackColor = color1[1];
                label30.BackColor = color1[1];
                ucConveyor1.ConveyorDirection = HZH_Controls.Controls.ConveyorDirection.Forward;
                ucArrow1.ArrowColor = color1[1];
                ucArrow2.ArrowColor = color1[2];
            }
            if(Convert.ToInt32(temcomm[6])<1480)
            {
               // label29.BackColor = color1[1];
                label30.BackColor = color1[1];
                ucConveyor1.ConveyorDirection = HZH_Controls.Controls.ConveyorDirection.Backward;
                ucArrow2.ArrowColor = color1[1];
                ucArrow1.ArrowColor = color1[2];
            }
            if ((Convert.ToInt32(temcomm[6])>1480)&& (Convert.ToInt32(temcomm[6])<1520)) { 
               // label29.BackColor = color1[0];
                label30.BackColor = color1[0];
                ucConveyor1.ConveyorDirection = HZH_Controls.Controls.ConveyorDirection.None;
                ucArrow2.ArrowColor = color1[2];
                ucArrow1.ArrowColor = color1[2];
            }

            if (Convert.ToInt32(temcomm[7]) > 100)//照明
                label29.BackColor = color1[1];
            else
                label29.BackColor = color1[0];



        }

   

        private void doWork()
        {
            while (true)
            {
                try
                {
                    Thread.Sleep(200);
                    this.Invoke(myDelegateUI); 
                    Application.DoEvents();
                }
                catch (Exception ex) { Console.WriteLine(ex); break; }

            }


        } 
        
        byte[] buffer1 = new byte[30];
        private void panel9_Click(object sender, EventArgs e)
        {
           

            String IP = "192.168.0.1";
            int port = 1234;
            
            IPAddress ip = IPAddress.Parse(IP);  //将IP地址字符串转换成IPAddress实例   
            ClientSocket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);//使用指定的地址簇协议、套接字类型和通信协议
            IPEndPoint endPoint = new IPEndPoint(ip, port); // 用指定的ip和端口号初始化IPEndPoint实例
            listBox1.Items.Add("网络连接中...");
            MessageBox.Show("连接网络");
            try
            {
               
                ClientSocket.Connect(endPoint);  //与远程主机建立连接
                Console.WriteLine("连接成功");
                listBox1.Items.Add("连接成功");
                wk = color1[1];
                Thread th = new Thread(ReceiveMsg);
                th.IsBackground = true;
                th.Start();
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex);
                MessageBox.Show("网口连接失败");
                listBox1.Items.Add("网络连接失败");
            }

        }
        void ReceiveMsg()
        {
            byte[] receive = new byte[8*1024];
            string buffer = "";

            while (true)
            {
                try
                {
                    int length = ClientSocket.Receive(receive);
                    if (length == 0)
                        continue;
                    buffer = BitConverter.ToString(receive).Replace("-", "");
                    if (buffer.IndexOf("25", 1) == -1)
                        continue;
                    command = buffer.Substring(buffer.IndexOf("25", 1), 94);
                    if (command[90] != 'F' || command[91] != 'F' || command[92] != 'F' || command[93] != 'F')
                        continue;


                    if (sp2.IsOpen) {
                        sp2.Write(command);
                      
                    }



                    if (command[3] == '2')//控制舱漏水
                    {
                        ks1 = 1;
                        kzc = Color.FromArgb(255, 0, 0);
                        if(Convert.ToInt32(command.Substring(4, 4), 16) / 100.0<=60.0)
                            comm[0] = (Convert.ToInt32(command.Substring(4, 4), 16) / 100.0).ToString();
                        if (Convert.ToInt32(command.Substring(8, 8), 16) / 100000 < 1000)
                            comm[1] = (Convert.ToInt32(command.Substring(8, 8), 16) / 100000.0).ToString();
                        comm[2] = (Convert.ToInt32(command.Substring(16, 4), 16) / 100.0).ToString();//湿度


                    }
                    if (command[3] == '3')
                    {
                        ks2 = 1;
                        dyc=Color.FromArgb(255, 0, 0);
                        // Console.WriteLine("电源仓漏水");
                        double dycwd = Convert.ToInt32(command.Substring(4, 4), 16) / 100.0;
                        int dycyq = Convert.ToInt32(command.Substring(8, 8), 16) / 100000;
                        if (dycwd <= 60&& dycwd>1.1)
                            comm2[0] = (Convert.ToInt32(command.Substring(4, 4), 16) / 100.0).ToString();
                        if(dycyq<1000&&dycyq>10)
                            comm2[1] = (Convert.ToInt32(command.Substring(8, 8), 16) / 100000.0).ToString();
                        comm2[2] = (Convert.ToInt32(command.Substring(16, 4), 16) / 100.0).ToString();
                    }

                    if (command[3] == '0')//控制仓
                    {
                        kzc = Color.FromArgb(0, 192, 0);
                        ks1 = 0;
                        if (Convert.ToInt32(command.Substring(4, 4), 16) / 100.0 <= 60.0)
                            comm[0] = (Convert.ToInt32(command.Substring(4, 4), 16) / 100.0).ToString();
                        if (Convert.ToInt32(command.Substring(8, 8), 16) / 100000 < 1000)
                            comm[1] = (Convert.ToInt32(command.Substring(8, 8), 16) / 100000.0).ToString();
                        comm[2] = (Convert.ToInt32(command.Substring(16, 4), 16) / 100.0).ToString();//湿度

                        if ((Convert.ToInt32(command.Substring(44, 4), 16) * 180 / 32768.0) < 45.0|| (Convert.ToInt32(command.Substring(44, 4), 16) * 180 / 32768.0) > 300.0)
                        {
                            comm[3] = ((Convert.ToInt32(command.Substring(44, 4), 16)) * 180).ToString();//roll
                          

                        }
                       
                        /*       if (Convert.ToInt32(command.Substring(48, 4), 16)*180.0 / 32768.0<50)
                                   comm[4] = (Convert.ToInt32(command.Substring(48, 4), 16) * 180.0).ToString();//Pitch
                               else
                                   comm[4] = ((Convert.ToInt32(command.Substring(48, 4), 16) - 65535) * 180).ToString();*/
                        if (Convert.ToInt32(command.Substring(48, 4), 16) * 180.0 / 32768.0 < 50)
                            comm[4] = (Convert.ToInt32(command.Substring(48, 4), 16) * 180.0).ToString();//Pitch
                        if(Convert.ToInt32(command.Substring(48, 4), 16) * 180.0 / 32768.0>310)
                            comm[4] = ((Convert.ToInt32(command.Substring(48, 4), 16) - 65535) * 180).ToString();



                        // string tem111 = command.Substring(54, 2) + command.Substring(52, 2);
                        // int hx1 = Convert.ToInt32(tem111, 16) * 180 / 32768;
                        int hx1 = Convert.ToInt32(command.Substring(52, 4), 16) * 180 / 32768;

                        int hx2 = hx1 - 180;
                        if (hx2 > 0)
                        {
                            hx2 = System.Math.Abs(hx2 - 180);
                        }
                        else
                        {
                            hx2 = System.Math.Abs(hx2) + 180;
                        }

                        if (hx2 < 190)
                            hx2 = hx2 + 170;
                        else
                        {
                            hx2 = hx2 + 170 - 360;
                        }

                        // comm[5] = hx2.ToString();
                        comm[5] = hx1.ToString();

                        double wattem = Convert.ToInt32(command.Substring(80, 4), 16) / 100.00;//水温
                        if (wattem > 1.01&&wattem<99.9)
                            comm[11] = (Convert.ToInt32(command.Substring(80, 4), 16) / 100.00).ToString();//水温
                        if(Convert.ToInt32(command.Substring(84, 4), 16) / 100.00>0.01)
                        comm[12] = (Convert.ToInt32(command.Substring(84, 4), 16) / 100.00).ToString();//水深
                        double sndp = (Convert.ToInt32(command.Substring(68, 8), 16) / 1000.0);
                        if (sndp <= 30&& sndp>0)
                            comm[9] = (Convert.ToInt32(command.Substring(68, 8), 16) / 1000.0).ToString();//声呐depth


                        comm[10] = (Convert.ToInt32(command.Substring(76, 4), 16)).ToString();//声呐确信度
                    }
                    if (command[3] == '1')//电源仓
                    {
                        ks2 = 0;
                        dyc = Color.FromArgb(0, 192, 0);
                        double dycwd = Convert.ToInt32(command.Substring(4, 4), 16) / 100.0;
                        int dycyq = Convert.ToInt32(command.Substring(8, 8), 16) / 100000;
                        double sd222 = Convert.ToInt32(command.Substring(16, 4), 16) / 100.0;
                        if (dycwd <= 60.0&&dycwd>1.1)
                            comm2[0] = (Convert.ToInt32(command.Substring(4, 4), 16) / 100.0).ToString();
                        
                        if (dycyq< 1000&&dycyq>10)
                            comm2[1] = (Convert.ToInt32(command.Substring(8, 8), 16) / 100000.0).ToString();
                        if(sd222>1.1)
                            comm2[2] = (Convert.ToInt32(command.Substring(16, 4), 16) / 100.0).ToString();


                    }


                    comm[6] = (Convert.ToInt32(command.Substring(56, 4), 16)).ToString();//Hx
                    comm[7] = (Convert.ToInt32(command.Substring(60, 4), 16)).ToString();//Hy
                    comm[8] = (Convert.ToInt32(command.Substring(64, 4), 16)).ToString();//Hz
                   
                    
                    comm[13] = (Convert.ToInt32(command.Substring(20, 4), 16) / 32768.0 * 16).ToString();//Ax
                    comm[14] = (Convert.ToInt32(command.Substring(24, 4), 16) / 32768.0 * 16).ToString();//Ay
                    comm[15] = (Convert.ToInt32(command.Substring(28, 4), 16) / 32768.0 * 16).ToString();//Az
                    comm[16] = (Convert.ToInt32(command.Substring(32, 4), 16) / 32768.0 * 2000).ToString();//Wx
                    comm[17] = (Convert.ToInt32(command.Substring(36, 4), 16) / 32768.0 * 2000).ToString();//Wy
                    comm[18] = (Convert.ToInt32(command.Substring(40, 4), 16) / 32768.0 * 2000).ToString();//Wz
                    Console.WriteLine(command);
                }
                catch (Exception ex)
                {
                    Console.WriteLine(ex);
                    ClientSocket.Close();

                    break;
                }

            }

        }

        
       
      
        private void pictureBox3_Click(object sender, EventArgs e)
        { 
           
            try
            { sp2.PortName = "COM8";
            sp2.BaudRate = 9600;
                sp2.Open();
            }
            catch(Exception ex) { MessageBox.Show(ex.ToString()); Console.WriteLine(ex); }
        }


        private void panel8_Click(object sender, EventArgs e)
        {
            sp.PortName = "COM3";
            sp.BaudRate = 9600;
            listBox1.Items.Add("连接串口");
            processThreadStart();
        }
        Thread listenThread;
        private void processThreadStart()//开端口解析数据并发出去
        {
            listenThread = new Thread(processHandle);
            listenThread.IsBackground = true;
            listenThread.Start();
        }
       
        string downstr = "";

        private void processHandle()
        {
           
            try
            {
                if(!sp.IsOpen)
                    this.sp.Open();
                sfkq = 1;

            }catch(Exception ex) { MessageBox.Show(ex.ToString()); }
            
            label32.BackColor = color1[1];
            ck = color1[1]; 
            while (true)
            {
                try
                {
                    string str = this.sp.ReadLine();

                    if (str == null)
                    { 
                        kzqx = Color.FromArgb(255, 255, 255);
                       // sd1= Color.FromArgb(255, 255, 255);
                    }

                    if (str != null)
                    {
                        downstr = str;
                        kzqx = Color.FromArgb(0, 64, 0);//控制权限
                       // sd1 = Color.FromArgb(0, 64, 0);
                        string[] comm = str.Split(':');
                        string s = "";
                        temcomm = comm;
                        byte[] buft = new byte[30];
                        for (int i = 1; i < comm.Length - 1; i++)
                        {
                            if (i < 14)
                            {
                                int a = Convert.ToInt32(comm[i]);

                                s += a.ToString("X4");
                            }
                            else
                            {
                                int a = Convert.ToInt32(comm[i]);
                                s += a.ToString("X2");
                            }


                        } 
                        s = "25" + s + "21";
                       
                        
                        for (int j1 = 0, j2 = 0; j2 <= s.Length - 2; j1++)
                        {
                            buft[j1] = Convert.ToByte(s.Substring(j2, 2), 16);
                            j2 = j2 + 2;
                        }  
                        qjht = (Convert.ToInt32(temcomm[1]) - 1500) / 10;//前进后退
                        zy1= (Convert.ToInt32(temcomm[2]) - 1500) / 10;//左右
                        sx1=(Convert.ToInt32(temcomm[3]) - 1500) / 10;
                        scd1= (Convert.ToInt32(temcomm[6]) - 1500) / 10;//传送带
                        buffer1 = buft;
                        ClientSocket.Send(buft);
                        
                    }
                    if (ks1 == 1|| ks2 == 1) { sp.Write("1"); }
                    if (ks1 == 0&&ks2==0) { sp.Write("0"); }
                  


                    


                }
                catch (Exception e) { Console.WriteLine(e); }

            }
        }

        int fla = 0;//主摄像头
        int fla2 = 0;
        int fla3 = 0;
        int fla4 = 0;//多摄像头
        int fla5 = 0;
        int fla6 = 0;
        private void button1_Click(object sender, EventArgs e)//主摄像头
        {
           
            if (fla2 == 1) { chromeBrowser2.Stop(); fla2 = 0; } 
            if (fla3 == 1) { chromeBrowser3.Stop(); fla3 = 0; }
            if (fla4 == 1) { chromeBrowser4.Stop(); fla4 = 0; }
            if (fla5 == 1) { chromeBrowser5.Stop(); fla5 = 0; }
            if (fla6 == 1) { chromeBrowser6.Stop(); fla6 = 0; }

            if (fla == 0)
            {
                string page = "http://192.168.0.123:5000/mono_cam_viewer_no_det";
             //   string page = "http://www.baidu.com";
                chromeBrowser = new ChromiumWebBrowser(page);
                panel1.Controls.Add(chromeBrowser);
                fla = 1;
                chromeBrowser.Dock = DockStyle.Fill;
                chromeBrowser.BringToFront();
               // Cef.EnableHighDPISupport();
              
               // chromeBrowser.Site=
                //chromeBrowser.
              // chromeBrowser.SetZoomLevel(1.25);
            }
            webBrowser1.Visible = false;
        }

        private void button3_Click(object sender, EventArgs e)
        {
            if (fla == 1) { chromeBrowser.Stop(); fla = 0; }
            if (fla2 == 1) { chromeBrowser2.Stop(); fla2 = 0; }
            if (fla4 == 1) { chromeBrowser4.Stop(); fla4 = 0; }
            if (fla5 == 1) { chromeBrowser5.Stop(); fla5 = 0; }
            if (fla6 == 1) { chromeBrowser6.Stop(); fla6 = 0; }

            if (fla3 == 0)
            {
                string page = "http://192.168.0.123:5000/mono_cam_viewer";
                chromeBrowser3 = new ChromiumWebBrowser(page);
                panel1.Controls.Add(chromeBrowser3);
                fla3 = 1;
                chromeBrowser3.Dock = DockStyle.Fill;
                chromeBrowser3.BringToFront();
            }
            webBrowser1.Visible = false;
        }

        private void button2_Click(object sender, EventArgs e)//双目
        {
            if (fla == 1) { chromeBrowser.Stop(); fla = 0; }
            if (fla3 == 1) { chromeBrowser3.Stop(); fla3 = 0; }
            if (fla4 == 1) { chromeBrowser4.Stop(); fla4 = 0; }
            if (fla5 == 1) { chromeBrowser5.Stop(); fla5 = 0; }
            if (fla6 == 1) { chromeBrowser6.Stop(); fla6 = 0; }

            if (fla2 == 0)
            {
                string page = "http://192.168.0.123:5000/stero_cam_viewer_no_det";
                chromeBrowser2 = new ChromiumWebBrowser(page);
                panel1.Controls.Add(chromeBrowser2);
                fla2 = 1;
                chromeBrowser2.Dock = DockStyle.Fill;
                chromeBrowser2.BringToFront();
            }
            webBrowser1.Visible = false;
           
        }

        private void button5_Click(object sender, EventArgs e)//图像增强
        {
            if (fla == 1) { chromeBrowser.Stop(); fla = 0; }
            if (fla2 == 1) { chromeBrowser2.Stop(); fla2 = 0; }
            if (fla3 == 1) { chromeBrowser3.Stop(); fla3 = 0; }
            if (fla4 == 1) { chromeBrowser4.Stop(); fla4 = 0; }
            if (fla6 == 1) { chromeBrowser6.Stop(); fla6 = 0; }

            if (fla5 == 0)
            {
                string page = "http://192.168.0.123:5000/enhance";
                chromeBrowser5 = new ChromiumWebBrowser(page);
                panel1.Controls.Add(chromeBrowser5);
                fla5 = 1;
                chromeBrowser5.Dock = DockStyle.Fill;
                chromeBrowser5.BringToFront();
            }
            webBrowser1.Visible = false;
        }

        private void button6_Click(object sender, EventArgs e)//
        {
            if (fla == 1) { chromeBrowser.Stop(); fla = 0; }
            if (fla2 == 1) { chromeBrowser2.Stop(); fla2 = 0; }
            if (fla3 == 1) { chromeBrowser3.Stop(); fla3 = 0; }
            if (fla4 == 1) { chromeBrowser4.Stop(); fla4 = 0; }
            if (fla5 == 1) { chromeBrowser5.Stop(); fla5 = 0; }

            if (fla6 == 0)
            {    string page = "http://192.168.0.8";
            chromeBrowser6 = new ChromiumWebBrowser(page);
              panel1.Controls.Add(chromeBrowser6);
              fla6 = 1;
              chromeBrowser6.Dock = DockStyle.Fill;
             chromeBrowser6.SendToBack();
             webBrowser1.Visible = true;
            webBrowser1.Navigate("http://192.168.0.8");
            webBrowser1.ScriptErrorsSuppressed = true;
            webBrowser1.BringToFront();
         

            }

            /*if (fla == 1) { chromeBrowser.Stop(); fla = 0; }
            if (fla2 == 1) { chromeBrowser2.Stop(); fla2 = 0; }
            if (fla3 == 1) { chromeBrowser3.Stop(); fla3 = 0; }
            if (fla4 == 1) { chromeBrowser4.Stop(); fla4 = 0; }
            if (fla5 == 1) { chromeBrowser5.Stop(); fla5 = 0; }

            if (fla6 == 0)
            {
                string page = "http://192.168.0.8";
                chromeBrowser6 = new ChromiumWebBrowser(page);
                panel1.Controls.Add(chromeBrowser6);
                fla6 = 1;
                chromeBrowser6.Dock = DockStyle.Fill;
                chromeBrowser6.BringToFront();
            }
           */

            webBrowser1.Visible = true;

        }
  
        private void panel19_Paint(object sender, PaintEventArgs e)
        {
            ControlPaint.DrawBorder(e.Graphics,
                               this.panel19.ClientRectangle,
                               Color.Silver,//7f9db9
                               1,
                               ButtonBorderStyle.Solid,
                               Color.Silver,
                               1,
                               ButtonBorderStyle.Solid,
                               Color.Silver,
                               1,
                               ButtonBorderStyle.Solid,
                               Color.Silver,
                               1,
                               ButtonBorderStyle.Solid);
        }

        private void panel2_Paint(object sender, PaintEventArgs e)
        {
            ControlPaint.DrawBorder(e.Graphics,
                               this.panel2.ClientRectangle,
                               Color.Silver,//7f9db9
                               1,
                               ButtonBorderStyle.Solid,
                               Color.Silver,
                               1,
                               ButtonBorderStyle.Solid,
                               Color.Silver,
                               1,
                               ButtonBorderStyle.Solid,
                               Color.Silver,
                               1,
                               ButtonBorderStyle.Solid);
        }

        private void panel3_Paint(object sender, PaintEventArgs e)
        {
            ControlPaint.DrawBorder(e.Graphics,
                               this.panel3.ClientRectangle,
                               Color.Silver,//7f9db9
                               1,
                               ButtonBorderStyle.Solid,
                               Color.Silver,
                               1,
                               ButtonBorderStyle.Solid,
                               Color.Silver,
                               1,
                               ButtonBorderStyle.Solid,
                               Color.Silver,
                               1,
                               ButtonBorderStyle.Solid);
        }

        private void panel4_Paint(object sender, PaintEventArgs e)
        {
            ControlPaint.DrawBorder(e.Graphics,
                               this.panel4.ClientRectangle,
                               Color.Silver,//7f9db9
                               1,
                               ButtonBorderStyle.Solid,
                               Color.Silver,
                               1,
                               ButtonBorderStyle.Solid,
                               Color.Silver,
                               1,
                               ButtonBorderStyle.Solid,
                               Color.Silver,
                               1,
                               ButtonBorderStyle.Solid);
        }

        private void panel5_Paint(object sender, PaintEventArgs e)
        {
            ControlPaint.DrawBorder(e.Graphics,
                               this.panel5.ClientRectangle,
                               Color.Silver,//7f9db9
                               1,
                               ButtonBorderStyle.Solid,
                               Color.Silver,
                               1,
                               ButtonBorderStyle.Solid,
                               Color.Silver,
                               1,
                               ButtonBorderStyle.Solid,
                               Color.Silver,
                               1,
                               ButtonBorderStyle.Solid);
        }

        private void panel6_Paint(object sender, PaintEventArgs e)
        {
            ControlPaint.DrawBorder(e.Graphics,
                               this.panel6.ClientRectangle,
                               Color.Silver,//7f9db9
                               1,
                               ButtonBorderStyle.Solid,
                               Color.Silver,
                               1,
                               ButtonBorderStyle.Solid,
                               Color.Silver,
                               1,
                               ButtonBorderStyle.Solid,
                               Color.Silver,
                               1,
                               ButtonBorderStyle.Solid);
        }

        private void panel7_Paint(object sender, PaintEventArgs e)
        {
            ControlPaint.DrawBorder(e.Graphics,
                               this.panel7.ClientRectangle,
                               Color.Silver,//7f9db9
                               1,
                               ButtonBorderStyle.Solid,
                               Color.Silver,
                               1,
                               ButtonBorderStyle.Solid,
                               Color.Silver,
                               1,
                               ButtonBorderStyle.Solid,
                               Color.Silver,
                               1,
                               ButtonBorderStyle.Solid);
        }

        private void panel16_Paint(object sender, PaintEventArgs e)
        {
            ControlPaint.DrawBorder(e.Graphics,
                               this.panel16.ClientRectangle,
                               Color.Silver,//7f9db9
                               1,
                               ButtonBorderStyle.Solid,
                               Color.Silver,
                               1,
                               ButtonBorderStyle.Solid,
                               Color.Silver,
                               1,
                               ButtonBorderStyle.Solid,
                               Color.Silver,
                               1,
                               ButtonBorderStyle.Solid);
        }

        private void panel17_Paint(object sender, PaintEventArgs e)
        {
            ControlPaint.DrawBorder(e.Graphics,
                                 this.panel17.ClientRectangle,
                                 Color.Silver,//7f9db9
                                 1,
                                 ButtonBorderStyle.Solid,
                                 Color.Silver,
                                 1,
                                 ButtonBorderStyle.Solid,
                                 Color.Silver,
                                 1,
                                 ButtonBorderStyle.Solid,
                                 Color.Silver,
                                 1,
                                 ButtonBorderStyle.Solid);

        }

        private void panel12_Paint(object sender, PaintEventArgs e)
        {
            ControlPaint.DrawBorder(e.Graphics,
                              this.panel12.ClientRectangle,
                              Color.Silver,//7f9db9
                              1,
                              ButtonBorderStyle.Solid,
                              Color.Silver,
                              1,
                              ButtonBorderStyle.Solid,
                              Color.Silver,
                              1,
                              ButtonBorderStyle.Solid,
                              Color.Silver,
                              1,
                              ButtonBorderStyle.Solid);
        }

        private void panel10_Paint(object sender, PaintEventArgs e)
        {
            ControlPaint.DrawBorder(e.Graphics,
                              this.panel10.ClientRectangle,
                              Color.Silver,//7f9db9
                              1,
                              ButtonBorderStyle.Solid,
                              Color.Silver,
                              1,
                              ButtonBorderStyle.Solid,
                              Color.Silver,
                              1,
                              ButtonBorderStyle.Solid,
                              Color.Silver,
                              1,
                              ButtonBorderStyle.Solid);
        }

        private void panel11_Paint(object sender, PaintEventArgs e)
        {
            ControlPaint.DrawBorder(e.Graphics,
                              this.panel11.ClientRectangle,
                              Color.Silver,//7f9db9
                              1,
                              ButtonBorderStyle.Solid,
                              Color.Silver,
                              1,
                              ButtonBorderStyle.Solid,
                              Color.Silver,
                              1,
                              ButtonBorderStyle.Solid,
                              Color.Silver,
                              1,
                              ButtonBorderStyle.Solid);
        }

    
      
       
        private void panel13_Paint(object sender, PaintEventArgs e)
        {
            ControlPaint.DrawBorder(e.Graphics,
                              this.panel13.ClientRectangle,
                              Color.Silver,//7f9db9
                              1,
                              ButtonBorderStyle.Solid,
                              Color.Silver,
                              1,
                              ButtonBorderStyle.Solid,
                              Color.Silver,
                              1,
                              ButtonBorderStyle.Solid,
                              Color.Silver,
                              1,
                              ButtonBorderStyle.Solid);
        }

        private void panel14_Paint(object sender, PaintEventArgs e)
        {
            ControlPaint.DrawBorder(e.Graphics,
                              this.panel14.ClientRectangle,
                              Color.Silver,//7f9db9
                              1,
                              ButtonBorderStyle.Solid,
                              Color.Silver,
                              1,
                              ButtonBorderStyle.Solid,
                              Color.Silver,
                              1,
                              ButtonBorderStyle.Solid,
                              Color.Silver,
                              1,
                              ButtonBorderStyle.Solid);
        }

        private void panel15_Paint(object sender, PaintEventArgs e)
        {
            ControlPaint.DrawBorder(e.Graphics,
                              this.panel15.ClientRectangle,
                              Color.Silver,//7f9db9
                              1,
                              ButtonBorderStyle.Solid,
                              Color.Silver,
                              1,
                              ButtonBorderStyle.Solid,
                              Color.Silver,
                              1,
                              ButtonBorderStyle.Solid,
                              Color.Silver,
                              1,
                              ButtonBorderStyle.Solid);
        }
        int zzms = 0;
        int sfkq = -1;
        private void button7_Click(object sender, EventArgs e)
        {
            if (zzms == 0)//开启自主抓取
            { 
                String IP = "192.168.0.123";
                int port = 6000;
                IPAddress ip = IPAddress.Parse(IP);  //将IP地址字符串转换成IPAddress实例   
                ServerSocket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);//使用指定的地址簇协议、套接字类型和通信协议
                IPEndPoint endPoint = new IPEndPoint(ip, port); // 用指定的ip和端口号初始化IPEndPoint实例
                button7.BackColor = color1[1];
                label32.BackColor = color1[0];
                label38.BackColor = color1[1];
                zzms = 1;
                try
                {

                     ServerSocket.Connect(endPoint);  //与远程主机建立连接
                     Console.WriteLine("连接成功");
                     listBox1.Items.Add("连接成功");
                    
                   
                    Thread th = new Thread(ReceiveMsg2);
                    th.IsBackground = true;
                    th.Start();
                    string asd1 = "2505DC05DC05DC05DC05DC05DC0000000000000000000000000000000021";
                    byte[] buft = new byte[30];
                    for (int j1 = 0, j2 = 0; j2 <= asd1.Length - 2; j1++)
                    {
                        buft[j1] = Convert.ToByte(asd1.Substring(j2, 2), 16);
                        j2 = j2 + 2;
                    }
                       for(int zxcaq=0;zxcaq<10;zxcaq++)
                           ServerSocket.Send(buft);


                    /*for (int j1 = 0; ; j1++)
                    {
                        buft = Convert.ToByte(command.Substring(84, 4), 16);
                    
                    }*/
                 

                }
                catch (Exception ex)
                 {
                      MessageBox.Show("连接失败");  
                      listBox1.Items.Add("网络连接失败");
                  }
                if (sfkq == 1)
                {
                 //   sp.Close();
                  //  listenThread.Abort();
                    sfkq = 0;
                 
                }
            }
            else//关闭自主抓取
            {
               /* try 
                {
                  
      
                    listenThread.Start();
                    
                }
                catch(Exception ex) { MessageBox.Show(ex.ToString());}*/
                listBox1.Items.Add("关闭自主抓取");
                label32.BackColor = color1[1];
                label38.BackColor = color1[0];
                button7.BackColor = color1[0];
                zzms = 0;
                try 
                {
                    ServerSocket.Close(); 
                }
                catch(Exception ex) { MessageBox.Show(ex.ToString()); }
                
            }
        
        }
        void ReceiveMsg2()
        {
            byte[] receive2 = new byte[8 * 1024];
            string buffer2;
            string tem_ttt;
            while (true)
            {
                try
                {
                  
                    int length = ServerSocket.Receive(receive2);
                    if (length == 0)
                        continue;
                    buffer2 = BitConverter.ToString(receive2).Replace("-", "");
                    if (buffer2.IndexOf("25", 1) == -1)
                        continue;
                    tem_ttt = buffer2.Substring(buffer2.IndexOf("25", 1), 60);
                    if (tem_ttt[58] != '2' || tem_ttt[59] != '1' )
                        continue;
                    autocom = tem_ttt;
                  
                    byte[] buft = new byte[30];
                    for (int j1 = 0, j2 = 0; j2 <= tem_ttt.Length - 2; j1++)
                    {
                        buft[j1] = Convert.ToByte(tem_ttt.Substring(j2, 2), 16);
                        j2 = j2 + 2;
                    }

                    ClientSocket.Send(buft);
               
                  /* byte[] buft2= Encoding.UTF8.GetBytes(command.Substring(84, 4));
                    ServerSocket.Send(buft2);*/
                }
                catch(Exception ex) {
                    Console.WriteLine(ex);
                    ServerSocket.Close();
                    break; }
                }
        }

       private void label22_Paint(object sender, PaintEventArgs e)
        {
            ControlPaint.DrawBorder(e.Graphics,
                             this.label22.ClientRectangle,
                             Color.Silver,//7f9db9
                             1,
                             ButtonBorderStyle.Solid,
                             Color.Silver,
                             1,
                             ButtonBorderStyle.Solid,
                             Color.Silver,
                             1,
                             ButtonBorderStyle.Solid,
                             Color.Silver,
                             1,
                             ButtonBorderStyle.Solid);
        }

        private void label23_Paint(object sender, PaintEventArgs e)
        {
            ControlPaint.DrawBorder(e.Graphics,
                            this.label23.ClientRectangle,
                            Color.Silver,//7f9db9
                            1,
                            ButtonBorderStyle.Solid,
                            Color.Silver,
                            1,
                            ButtonBorderStyle.Solid,
                            Color.Silver,
                            1,
                            ButtonBorderStyle.Solid,
                            Color.Silver,
                            1,
                            ButtonBorderStyle.Solid);
        }

        private void label25_Paint(object sender, PaintEventArgs e)
        {
            ControlPaint.DrawBorder(e.Graphics,
                               this.label25.ClientRectangle,
                               Color.Silver,//7f9db9
                               1,
                               ButtonBorderStyle.Solid,
                               Color.Silver,
                               1,
                               ButtonBorderStyle.Solid,
                               Color.Silver,
                               1,
                               ButtonBorderStyle.Solid,
                               Color.Silver,
                               1,
                               ButtonBorderStyle.Solid);

        }

        private void label26_Paint(object sender, PaintEventArgs e)
        {
            ControlPaint.DrawBorder(e.Graphics,
                                  this.label26.ClientRectangle,
                                  Color.Silver,//7f9db9
                                  1,
                                  ButtonBorderStyle.Solid,
                                  Color.Silver,
                                  1,
                                  ButtonBorderStyle.Solid,
                                  Color.Silver,
                                  1,
                                  ButtonBorderStyle.Solid,
                                  Color.Silver,
                                  1,
                                  ButtonBorderStyle.Solid);

        }

        private void label27_Paint(object sender, PaintEventArgs e)
        {
            ControlPaint.DrawBorder(e.Graphics,
                               this.label27.ClientRectangle,
                               Color.Silver,//7f9db9
                               1,
                               ButtonBorderStyle.Solid,
                               Color.Silver,
                               1,
                               ButtonBorderStyle.Solid,
                               Color.Silver,
                               1,
                               ButtonBorderStyle.Solid,
                               Color.Silver,
                               1,
                               ButtonBorderStyle.Solid);
        }

        private void label28_Paint(object sender, PaintEventArgs e)
        {
            ControlPaint.DrawBorder(e.Graphics,
                               this.label28.ClientRectangle,
                               Color.Silver,//7f9db9
                               1,
                               ButtonBorderStyle.Solid,
                               Color.Silver,
                               1,
                               ButtonBorderStyle.Solid,
                               Color.Silver,
                               1,
                               ButtonBorderStyle.Solid,
                               Color.Silver,
                               1,
                               ButtonBorderStyle.Solid);
        }

        private void label29_Paint(object sender, PaintEventArgs e)
        {
            ControlPaint.DrawBorder(e.Graphics,
                               this.label29.ClientRectangle,
                               Color.Silver,//7f9db9
                               1,
                               ButtonBorderStyle.Solid,
                               Color.Silver,
                               1,
                               ButtonBorderStyle.Solid,
                               Color.Silver,
                               1,
                               ButtonBorderStyle.Solid,
                               Color.Silver,
                               1,
                               ButtonBorderStyle.Solid);
        }

        private void label30_Paint(object sender, PaintEventArgs e)
        {
            ControlPaint.DrawBorder(e.Graphics,
                               this.label30.ClientRectangle,
                               Color.Silver,//7f9db9
                               1,
                               ButtonBorderStyle.Solid,
                               Color.Silver,
                               1,
                               ButtonBorderStyle.Solid,
                               Color.Silver,
                               1,
                               ButtonBorderStyle.Solid,
                               Color.Silver,
                               1,
                               ButtonBorderStyle.Solid);
        }

        private void panel18_Paint(object sender, PaintEventArgs e)
        {
            ControlPaint.DrawBorder(e.Graphics,
                               this.panel18.ClientRectangle,
                               Color.Silver,//7f9db9
                               1,
                               ButtonBorderStyle.Solid,
                               Color.Silver,
                               1,
                               ButtonBorderStyle.Solid,
                               Color.Silver,
                               1,
                               ButtonBorderStyle.Solid,
                               Color.Silver,
                               1,
                               ButtonBorderStyle.Solid);
        }

        private void label32_Paint(object sender, PaintEventArgs e)
        {
            ControlPaint.DrawBorder(e.Graphics,
                               this.label32.ClientRectangle,
                               Color.Silver,//7f9db9
                               1,
                               ButtonBorderStyle.Solid,
                               Color.Silver,
                               1,
                               ButtonBorderStyle.Solid,
                               Color.Silver,
                               1,
                               ButtonBorderStyle.Solid,
                               Color.Silver,
                               1,
                               ButtonBorderStyle.Solid);
        }

        private void label38_Paint(object sender, PaintEventArgs e)
        {
            ControlPaint.DrawBorder(e.Graphics,
                               this.label38.ClientRectangle,
                               Color.Silver,//7f9db9
                               1,
                               ButtonBorderStyle.Solid,
                               Color.Silver,
                               1,
                               ButtonBorderStyle.Solid,
                               Color.Silver,
                               1,
                               ButtonBorderStyle.Solid,
                               Color.Silver,
                               1,
                               ButtonBorderStyle.Solid);
        }

        private void label31_Paint(object sender, PaintEventArgs e)
        {
            ControlPaint.DrawBorder(e.Graphics,
                               this.label31.ClientRectangle,
                               Color.Silver,//7f9db9
                               1,
                               ButtonBorderStyle.Solid,
                               Color.Silver,
                               1,
                               ButtonBorderStyle.Solid,
                               Color.Silver,
                               1,
                               ButtonBorderStyle.Solid,
                               Color.Silver,
                               1,
                               ButtonBorderStyle.Solid);
        }
        string curdateTime = "";
        public void GetScreen()//截图
        {
            if (!System.IO.Directory.Exists(@"D:\picture"))
            {
                System.IO.Directory.CreateDirectory(@"D:\picture");//不存在就创建目录 
            }

            //获取整个屏幕图像,不包括任务栏
            Rectangle ScreenArea = Screen.GetWorkingArea(this);
            Bitmap bmp = new Bitmap(ScreenArea.Width, ScreenArea.Height);
            using (Graphics g = Graphics.FromImage(bmp))
            {
                g.CopyFromScreen(0, 0, 0, 0, new Size(ScreenArea.Width, ScreenArea.Height));
                curdateTime = DateTime.Now.ToString("yyyy_MM_dd_HH_mm_ss");

                bmp.Save("D:\\picture" + "\\" + curdateTime + ".jpg");//直接存储在c盘会出错

            }
            return;
        }
        public void GetCommand()
        {
            string tem_com1 = downstr;
            string result = tem_com1.Trim(); //输入文本
             StreamWriter sw = File.AppendText(@"D:\\picture\\"+ curdateTime+".txt"); //保存到指定路径
            sw.Write(result);
            sw.Flush();
            sw.Close();

        }
       

        private void pictureBox4_Click(object sender, EventArgs e)//保存截图和命令
        {
            GetScreen();
            listBox1.Items.Add("截图成功");
            GetCommand();
         
        }

        

        private void button4_Click(object sender, EventArgs e)//双目检测
        {
            if (fla == 1) { chromeBrowser.Stop();fla = 0; }
            if (fla2 == 1) { chromeBrowser2.Stop(); fla2 = 0; }
            if (fla3 == 1) { chromeBrowser3.Stop(); fla3 = 0; }
            if (fla5 == 1) { chromeBrowser5.Stop(); fla5 = 0; }
            if (fla6 == 1) { chromeBrowser6.Stop(); fla6 = 0; }

            if (fla4 == 0)
            {
                //string page = "http://192.168.0.123:5000";
                string page = "http://192.168.0.123:5000/stero_cam_viewer";
                chromeBrowser4 = new ChromiumWebBrowser(page);
                panel1.Controls.Add(chromeBrowser4);
                fla4 = 1;
                chromeBrowser4.Dock = DockStyle.Fill;
                chromeBrowser4.BringToFront();
            }
            webBrowser1.Visible = false;

        }







    }
}
