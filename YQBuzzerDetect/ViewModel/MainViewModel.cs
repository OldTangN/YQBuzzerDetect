using MyLogLib;
using Newtonsoft.Json;
using RabbitMQ;
using RabbitMQ.YQMsg;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Input;
using YQBuzzerDetect.Helper;

namespace YQBuzzerDetect.ViewModel
{
    public class MainViewModel : ObservableObject
    {
        public MainViewModel()
        {

        }

        private ClientMQ MQClient;
        public HeartStatus CurrStatus { get => _CurrStatus; set => Set(ref _CurrStatus, value); }
        private HeartStatus _CurrStatus = HeartStatus.Initializing;

        #region 心跳
        private void HeartBeat()
        {
            Task.Run(() =>
            {
                while (true)
                {
                    try
                    {
                        HeartBeatMsg msg = new HeartBeatMsg()
                        {
                            DEVICE_TYPE = SysCfg.DEVICE_TYPE,
                            NO = SysCfg.NO,
                            STATUS = ((int)CurrStatus).ToString()
                        };
                        string strMsg = JsonConvert.SerializeObject(msg);
                        MQClient.SentMessage(strMsg);
                    }
                    catch (Exception ex)
                    {
                        MyLog.WriteLog(ex);
                        ShowMsg("心跳上报失败!" + ex.Message);
                    }
                    Thread.Sleep(SysCfg.HEARTBEAT_TIMESPAN);
                }
            });
        }
        #endregion

        #region 初始化
        private ICommand _InitCmd;
        public ICommand InitCmd => _InitCmd ?? (_InitCmd = new RelayCommand(Init));

        private bool _InitMQFlag = false;
        private bool _InitBuzzerFLag = false;

        /// <summary>
        /// 初始化MQ标志，true=成功
        /// </summary>
        public bool InitMQFlag
        {
            get => _InitMQFlag; set
            {
                _InitMQFlag = value;
                if (InitBuzzerFLag && InitMQFlag && CurrStatus == HeartStatus.Initializing)
                {
                    CurrStatus = HeartStatus.Init_Complete;
                }
            }
        }

        /// <summary>
        /// 初始化蜂鸣器标志 true=成功
        /// </summary>
        public bool InitBuzzerFLag
        {
            get => _InitBuzzerFLag;
            set
            {
                _InitBuzzerFLag = value;
                if (InitBuzzerFLag && InitMQFlag && CurrStatus == HeartStatus.Initializing)
                {
                    CurrStatus = HeartStatus.Init_Complete;
                }
            }
        }

        private async void Init()
        {
            CurrStatus = HeartStatus.Initializing;
            InitMQFlag = await InitMQ();
            if (InitMQFlag)
            {
                HeartBeat();
            }
            InitBuzzer();
        }

        /// <summary>
        /// 初始化MQ，因为无法连接时耗时较长，所以开启异步调用
        /// </summary>
        /// <returns></returns>
        private async Task<bool> InitMQ()
        {
            return await Task.Run<bool>(() =>
             {
                 ShowMsg("连接MQ服务器中。。。");
                 try
                 {
                     MQClient = new ClientMQ();
                     MQClient.singleArrivalEvent += MQClient_singleArrivalEvent;
                     MQClient.ReceiveMessage();
                     ShowMsg("连接成功！");
                     return true;
                 }
                 catch (Exception ex)
                 {
                     MyLog.WriteLog(ex);
                     ShowMsg("连接失败。。。" + ex.Message);
                     return false;
                 }
             });

        }

        /// <summary>
        /// 当前执行检测的消息
        /// </summary>
        private ExecuteMsg CurrExecMsg;

        /// <summary>
        /// MQ接收消息回调
        /// </summary>
        /// <param name="data"></param>
        private void MQClient_singleArrivalEvent(string data)
        {
            ShowMsg(data);
            MsgBase msg = null;
            try
            {
                msg = JsonConvert.DeserializeObject<MsgBase>(data);
            }
            catch (Exception ex)
            {
                string errMsg = "协议格式错误！";
                MyLog.WriteLog(errMsg, ex);
                ShowMsg(errMsg);
                return;
            }
            if (msg.DEVICE_TYPE != SysCfg.DEVICE_TYPE || msg.NO != SysCfg.NO)//验证是否本专机消息
            {
                ShowMsg(string.Format("消息中设备信息与专机配置不匹配！消息：{0}+{1}; 专机设置：{2}+{3}。", msg.DEVICE_TYPE, msg.NO, SysCfg.DEVICE_TYPE, SysCfg.NO));
                return;
            }

            if (msg.MESSAGE_TYPE == "task")//制令
            {
                if (msg is TaskMsg tskMsg)
                {

                }
            }
            else if (msg.MESSAGE_TYPE == "execute")//启动
            {
                if (!(CurrStatus == HeartStatus.Init_Complete || CurrStatus == HeartStatus.Finished))
                {
                    ShowMsg("正在检测，不会执行此次命令！");
                    return;
                }
                if (msg is ExecuteMsg execMsg)
                {
                    CurrStatus = HeartStatus.Working;
                    CurrExecMsg = execMsg;
                    lstDBValues.Clear();
                }
            }
            else
            {

            }
        }

        private void InitBuzzer()
        {
            On_SDK_CallBack = new ON_CTS_SDK_CALL_BACK(InitCallback);
            enSdkErrCode rlt = BuzzerDetector.TSDK_Init(On_SDK_CallBack, true, 0x12345678);
            if (rlt != enSdkErrCode.CERR_SUCCESS)
            {
                ShowMsg("SDK初始化失败！" + rlt.ToString());
            }
            else
            {
                ShowMsg("SDK初始化完成！" + rlt.ToString());
            }
            ShowMsg("等待拾音器连接...");
        }

        /// <summary>
        /// SDK初始化回调委托
        /// </summary>
        private ON_CTS_SDK_CALL_BACK On_SDK_CallBack;
        /// <summary>
        /// 当前蜂鸣器设备信息
        /// </summary>
        private TSdkEventTermConnect currTerm;

        /// <summary>
        /// 检测过程中累计分贝值列表
        /// </summary>
        private List<int> lstDBValues = new List<int>();

        private int InitCallback(enSdkCbType eCbType, IntPtr pParam, int dwSize, int usr_data)
        {
            switch (eCbType)
            {
                case enSdkCbType.CB_Post_TermState:
                    var state = Marshal.PtrToStructure<TSdkPostTermState>(pParam);
                    ShowMsg(state.eTermState.ToString());
                    break;
                case enSdkCbType.CB_Post_TermDbValL:
                    var valL = Marshal.PtrToStructure<TSdkPostTermDbValL>(pParam);
                    DBValL = valL.nDbVal;
                    break;
                case enSdkCbType.CB_Post_TermDbValR:
                    lock (lockObj)
                    {
                        var valR = Marshal.PtrToStructure<TSdkPostTermDbValL>(pParam);
                        DBValR = valR.nDbVal;
                        if (CurrStatus != HeartStatus.Working)
                        {
                            break;
                        }
                        lstDBValues.Add(DBValR);
                        if (lstDBValues.Count > SysCfg.READ_TIMES)//取20次分贝值 TODO:读取频率未测试
                        {
                            if (lstDBValues.Average() > SysCfg.DB_THRESHOLD)
                            {
                                CurrExecMsg.DATA[0].result = "0";
                            }
                            else
                            {
                                CurrExecMsg.DATA[0].result = "1";
                            }
                            //上传结果到MQ
                            DataMsg msg = new DataMsg()
                            {
                                DEVICE_TYPE = SysCfg.DEVICE_TYPE,
                                NO = SysCfg.NO,
                                DATA = CurrExecMsg.DATA
                            };
                            string strMsg = JsonConvert.SerializeObject(msg);
                            try
                            {
                                MQClient.SentMessage(strMsg);
                            }
                            catch (Exception ex)
                            {
                                MyLog.WriteLog("上传结果失败！", ex);
                                ShowMsg("上传结果失败！");
                            }
                            CurrStatus = HeartStatus.Finished;
                        }
                    }
                    break;
                case enSdkCbType.CB_Event_TermConnect:
                    #region 指针指向的byte[]
                    //int size = Marshal.SizeOf(typeof(TSdkEventTermConnect));
                    //byte[] byt = new byte[size];
                    //if (pParam != null)
                    //{
                    //    Marshal.Copy(pParam, byt, 0, size);
                    //    string rlt = "";
                    //    foreach (byte b in byt)
                    //    {
                    //        rlt += (b.ToString("X2") + " ");
                    //    }
                    //    MessageBox.Show(rlt);
                    //}
                    #endregion
                    currTerm = Marshal.PtrToStructure<TSdkEventTermConnect>(pParam);
                    ShowMsg("已连接!");
                    GetDBValue();//开始监听分贝值
                    break;
                case enSdkCbType.CB_Event_TermCnnLost:
                    var lost = Marshal.PtrToStructure<TSdkEventTermCnnLost>(pParam);
                    ShowMsg("断开连接！");//未测试出该情况
                    break;
                default:
                    break;
            }
            return 0;
        }
        #endregion

        #region 启动获取分贝数
        private ICommand _GetDBValueCmd;
        public ICommand GetDBValueCmd => _GetDBValueCmd ?? (_GetDBValueCmd = new RelayCommand(GetDBValue));

        private void GetDBValue()
        {
            enSdkErrCode rlt = BuzzerDetector.TSDK_Req_DbAnalyze(currTerm.dwTermID, true);//4437081607091760
            if (rlt != enSdkErrCode.CERR_SUCCESS)
            {
                InitBuzzerFLag = false;
                ShowMsg("监听分贝值失败！" + rlt.ToString());
            }
            else
            {
                InitBuzzerFLag = true;
                ShowMsg("监听分贝值开始！" + rlt.ToString());
            }
        }
        #endregion

        public Action<string> OnShowMsg { get; set; }

        private void ShowMsg(string msg)
        {
            try
            {
                OnShowMsg?.Invoke(msg);
            }
            catch (Exception ex)
            {
                MyLog.WriteLog("OnShowMsg委托调用异常！", ex);
            }
        }

        private int _DBValL;
        /// <summary>
        /// 左声道--远点
        /// </summary>
        public int DBValL { get => _DBValL; set => Set(ref _DBValL, value); }

        private int _DBValR;
        /// <summary>
        /// 右声道--近点
        /// </summary>
        public int DBValR { get => _DBValR; set => Set(ref _DBValR, value); }

        /// <summary>
        /// 锁对象
        /// </summary>
        private readonly object lockObj = new { };
    }
}
