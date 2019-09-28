using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace YQBuzzerDetect.Helper
{
    public class SysCfg
    {
        /// <summary>
        /// 设备类型
        /// </summary>
        public static string DEVICE_TYPE => ConfigurationUtil.GetConfiguration(Convert.ToString, () => "E012");

        /// <summary>
        /// 设备类型
        /// </summary>
        public static string NO => ConfigurationUtil.GetConfiguration(Convert.ToString, () => "E01201");


        /// <summary>
        /// 分贝值读取次数
        /// <para>默认20次</para>
        /// </summary>
        public static int READ_TIMES => ConfigurationUtil.GetConfiguration(int.Parse, () => 20);

        /// <summary>
        /// 最小分贝数阈值
        /// <para>默认30</para>
        /// </summary>
        public static int DB_THRESHOLD => ConfigurationUtil.GetConfiguration(int.Parse, () => 30);

        /// <summary>
        /// 最小分贝数阈值
        /// <para>默认1000ms</para>
        /// </summary>
        public static int HEARTBEAT_TIMESPAN => ConfigurationUtil.GetConfiguration(int.Parse, () => 1000);

        
        public static bool SetConfiguration(string key, object val)
        {
            return ConfigurationUtil.SetConfiguration(key, val.ToString());
        }
    }
}
