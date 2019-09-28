using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace YQBuzzerDetect.Helper
{
    public enum enSdkCbType
    {
        CB_Asw_OpenTermAudVid = 101, // 应答打开终端音频视频
        CB_Asw_CloseTermAudVid = 102, // 应答关闭终端音频视频
        CB_Asw_OpenTermTalk = 103, // 应答打开终端对讲
        CB_Asw_CloseTermTalk = 104, // 应答关闭终端对讲
        CB_Asw_GetAudLevel = 105, // 应答获取终端音量级别
        CB_Asw_SetAudLevel = 106, // 应答设置终端音量级别
        CB_Asw_DismissTerm = 107, // 应答解除分机呼叫
        CB_Asw_GetDbAlmCfg = 108, // 应答获取终端喧哗报警配置
        CB_Asw_SetDbAlmCfg = 109, // 应答设置终端喧哗报警配置
        CB_Asw_NvrSrchRecFile = 110, // 应答NVR查找录音文件结果
        CB_Asw_NvrPlayRecFile = 111, // 应答NVR回放录音文件结果

        CB_Post_TermSos = 201, // 推送终端呼叫
        /// <summary>
        /// 推送终端状态
        /// </summary>
        CB_Post_TermState = 202, // 
        CB_Post_485PipeData = 203, // 推送485管道数据(接收)
        CB_Post_Mp3PlayFinish = 204, // 推送MP3文件播放结束通知
        CB_Post_AlmNotify = 205, // 推送报警通知
        CB_Post_NvrChanNotify = 206, // 推送NVR通道通知
        /// <summary>
        /// 推送网络拾音器第一声道音量值
        /// </summary>
        CB_Post_TermDbValL = 207, 
        /// <summary>
        /// 推送网络拾音器第二声道音量值
        /// </summary>
        CB_Post_TermDbValR = 208, // 
        CB_Post_NvrSrchRecFile = 209, // 推送NVR查找录音文件列表信息
        CB_Post_NvrPlayProg = 210, // 推送NVR录音文件回放进度

        /// <summary>
        /// 终端请求注册通知，返回-1代表注册失败，返回0代表注册成功
        /// </summary>
        CB_Event_TermRegister = 301,

        /// <summary>
        /// 终端请求连接通知，返回-1代表不允许连接，返回0代表允许连接
        /// </summary>
        CB_Event_TermConnect = 302,

        /// <summary>
        /// 终端断开连接通知
        /// </summary>
        CB_Event_TermCnnLost = 303,

        CB_Data_TermAudio = 401, // 终端的音频，已经压缩过的，需要调用SDK播放接口才能播放
        CB_Data_TermVideo = 402, // 终端的H264标准视频帧，可以调用SDK播放接口播放
        CB_Data_PcMicAudio = 403, // 电脑的音频，为PCM帧，8K采样率，16位/样
        CB_Data_BypassAudio = 404, // 旁路音频，为PCM帧，8K采样率，16位/样
        /// <summary>
        /// 网络拾音器第一声道MP3音频
        /// </summary>
        CB_Data_TermMp3L = 405,
        /// <summary>
        /// 网络拾音器第二声道MP3音频
        /// </summary>
        CB_Data_TermMp3R = 406,
        CB_Data_TermPcmL = 407, // 网络拾音器第一声道PCM音频(16K采样率16位)
        CB_Data_TermPcmR = 408, // 网络拾音器第二声道PCM音频(16K采样率16位)
        CB_Data_NvrMp3 = 409, // 录音主机MP3音频
        CB_Data_NvrPcm = 410, // 录音主机PCM音频(16K采样率16位)
        CB_Data_NvrPlayMp3 = 411, // 录音主机回放MP3音频

    }

    public enum enSdkErrCode
    {
        CERR_SUCCESS = 0,
        CERR_UNKNOWN = -1,
        CERR_NOT_INIT = -801,
        CERR_INVALID_PARAMETER = -802,
        CERR_CONNECT_TIMEOUT = -803,
        CERR_SOCKET_RECV_ERROR = -804,
        CERR_ALREADY_CONNECTED = -805,
        CERR_NOT_CONNECTED = -806,
        CERR_DEVICE_BUSY = -807,
        CERR_HOST_REGISTER_ERROR = -908,
        CERR_INVALID_DEV_STATE = -909,
        CERR_LISTEN_PORT_ERROR = -910,
        CERR_OPEN_FILE_ERROR = -911,
        CERR_INVALID_FILE_PATH = -912,
        CERR_INVALID_FILE_SIZE = -913,
        CERR_INVALID_FILE_DATA = -914,
        CERR_TERM_IS_OCCUPIED = -915,
        CERR_GROUP_NOT_SETUP = -916,
        CERR_TERM_LIST_FULL = -917,
        CERR_DATA_NOT_SYNC = -918,
        CERR_INVALID_BIOS_VERSION = -919,
        CERR_INVALID_DEV_TYPE = -920,
        CERR_CHAN_NOT_REGISTERED = -921,

        CERR_HST_NUM_OCCUPIED = -701,
        CERR_TRM_NUM_OCCUPIED = -702,
        CERR_HST_NUM_NOT_EXISTENT = -703,
        CERR_TRM_NUM_NOT_EXISTENT = -704,
        CERR_HST_IS_IN_CALLING = -705,
        CERR_TRM_NUM_NOT_FIT_HOST = -706,
        CERR_HST_CNT_REACH_MAX = -707,
        CERR_GROUP_NUM_NOT_EXISTENT = -708,
        CERR_DEV_NUM_OCCUPIED = -709,
        CERR_DEV_NOT_REGISTERED = -710,
    }
    public delegate int ON_CTS_SDK_CALL_BACK(enSdkCbType eCbType, IntPtr pParam, int dwSize, int usr_data);
    public enum enSdkDevType
    {
        /// <summary>
        /// 未知设备类型
        /// </summary>
        TSDK_DEV_TYPE_UNDEF = 0,  // 未知设备类型
        /// <summary>
        /// 对讲终端 或 对讲广播终端
        /// </summary>
        TSDK_DEV_TYPE_TALK = 1,  // 对讲终端 或 对讲广播终端
        /// <summary>
        /// 广播终端
        /// </summary>
        TSDK_DEV_TYPE_BROAD = 2,  // 广播终端
        /// <summary>
        /// 网络拾音器
        /// </summary>
        TSDK_DEV_TYPE_MP3 = 3,  // 网络拾音器
        /// <summary>
        /// 录音主机
        /// </summary>
        TSDK_DEV_TYPE_NVR = 4,  // 录音主机
    }
    public enum enSdkDevState
    {
        /// <summary>
        /// 离线
        /// </summary>
        TSDK_DEV_STATE_OFFLINE = 0,  // 离线
        /// <summary>
        /// 在线
        /// </summary>
        TSDK_DEV_STATE_ONLINE = 1,  // 在线
        /// <summary>
        /// 正在播放mp3
        /// </summary>
        TSDK_DEV_STATE_PLAYING = 2,  // 正在播放mp3
        /// <summary>
        /// 正在对讲
        /// </summary>
        TSDK_DEV_STATE_TALKING = 3,  // 正在对讲
        /// <summary>
        /// 正在对讲和播放mp3
        /// </summary>
        TSDK_DEV_STATE_TALK_PLAY = 4   // 正在对讲和播放mp3
    }

    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    public struct TSdkPostTermState
    {
        public uint dwTermID;
        public enSdkDevState eTermState;
        public byte AlmInMask;   // 报警输入通道有效指示码，每位对应一路报警: 0(通道无效), 1(通道有效)。暂时只支持2路报警输入，第1位为通道1，第2位为通道2。
        public byte AlmInState;  // 报警输入通道状态指示码，每位对应一路报警: 0(无报警), 1(有报警)。
        public byte AlmOutMask;  // 报警输出通道有效指示码，每位对应一路报警: 0(通道无效), 1(通道有效)。暂时只支持2路报警输出，第1位为通道1，第2位为通道2。
        public byte AlmOutState; // 报警输出通道状态指示码，每位对应一路报警: 0(无报警), 1(有报警)。
    }

    public struct TSdkEventTermCnnLost
    {
        public uint dwTermID;
    }

    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    public struct TSdkPostTermDbValL
    {
        public int dwTermID;
        public int nDbVal;
    }

    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    public struct TSdkEventTermConnect
    {
        public enSdkDevType eTermType;

        public uint dwTermID;    // 终端设备ID号，高16位为“配置工具”注册设备的“区域号”(1～10)，低16位为“配置工具”注册设备的“设备号”(1～600)

        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 6)]
        public byte[] TermMac;   // 终端MAC地址 6

        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 16)]
        public char[] TermIp;   // 终端IP地址 16

        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 32)]
        public char[] TermName; // 终端名称 32

        public bool bHasVideo;    // TRUE:有视频和音频 , FALSE:无视频只有音频

        public bool bTalkbackEnable; // 只针对网络拾音器，TRUE:开启对讲模式，FALSE:关闭对讲模式
    }



    public static class BuzzerDetector
    {
        // 分析网络拾音器当前 DB 值
        [DllImport("CtsSdk.dll", EntryPoint = "TSDK_Req_DbAnalyze")]
        public static extern enSdkErrCode TSDK_Req_DbAnalyze(uint dwTermID, bool bEnable); // 分析第一声道 DB 值

        [DllImport("CtsSdk.dll", EntryPoint = "TSDK_Init", CallingConvention = CallingConvention.StdCall)]
        public static extern enSdkErrCode TSDK_Init(ON_CTS_SDK_CALL_BACK CbFunc, bool bServerMode, int usr_data); // SDK 初始化。

        [DllImport("CtsSdk.dll", EntryPoint = "TSDK_DeInit")]
        public static extern enSdkErrCode TSDK_DeInit(); // SDK 注销。

    }
}
