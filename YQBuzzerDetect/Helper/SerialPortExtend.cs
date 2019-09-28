using System;
using System.Collections.Generic;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace YQBuzzerDetect.Helper
{
    /// <summary>
    /// 定义判断是否完整信息的判断方法委托
    /// </summary>
    /// <param name="datas">数据数组</param>
    /// <returns></returns>
    public delegate bool JudgeFullMessage(ref byte[] datas, string codeno);
    public delegate void ReceivedMessage(byte[] datas);

    public static class SerialPortExtend
    {

        /// <summary>
        /// 发送命令，并获取接收信息
        /// </summary>
        /// <param name="serialPort"></param>
        /// <param name="SendData">发送数据</param>
        /// <param name="sendTimes">尝试发送次数</param>
        /// <param name="ReceiveData">接收到的数据</param>
        /// <param name="receiveTimes">每次发送后等待接收次数</param>
        /// <param name="rcvSleep">每次接收前等待时间</param>
        /// <param name="hopeRcvLen">期望得到的应答数据长度</param>
        /// <returns>接收数据的长度，串口未打开返回-1</returns>
        public static int SendCmd(this SerialPort serialPort, byte[] SendData, int sendTimes, ref List<byte> ReceiveData, int receiveTimes, int rcvSleep,int hopeRcvLen)
        {
            if (!serialPort.IsOpen)
            {
                return -1;
            }
            ReceiveData = new List<byte>();
            try
            {
                bool readComplete = false;//是否读取完毕
                do
                {
                    serialPort.DiscardInBuffer();//清空接收缓冲区
                    serialPort.DiscardOutBuffer();
                    serialPort.Write(SendData, 0, SendData.Length);
                    while (receiveTimes-- > 0)
                    {
                        Thread.Sleep(rcvSleep);//TODO:延迟过短的话，如果不判断帧格式，会导致断帧
                        if (serialPort.BytesToRead > 0)
                        {
                            byte[] buffer = new byte[serialPort.BytesToRead];
                            serialPort.Read(buffer, 0, buffer.Length);
                            //serialPort.DiscardInBuffer();//清空接收缓冲区
                            ReceiveData.AddRange(buffer);
                        }
                        else
                        {
                            continue;
                        }
                        //判断是否读取一条完整的信息，如果没有判断完整信息的方法，则读到数据之后就结束
                        if (ReceiveData.Count >= hopeRcvLen)
                        {
                            readComplete = true;
                            break;
                        }
                    }
                    if (readComplete)
                    {
                        break;
                    }
                } while (sendTimes-- > 0 && !readComplete);
            }
            catch (Exception ex)
            {
                throw ex;
            }
            return ReceiveData.Count;
        }
    }
}
