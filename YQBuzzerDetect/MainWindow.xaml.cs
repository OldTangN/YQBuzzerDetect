using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using YQBuzzerDetect.Helper;
using YQBuzzerDetect.ViewModel;

namespace YQBuzzerDetect
{
    /// <summary>
    /// MainWindow.xaml 的交互逻辑
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
        }

        private void Window_Closed(object sender, EventArgs e)
        {
            BuzzerDetector.TSDK_DeInit();
            Environment.Exit(0);//可关闭后台线程
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            MainViewModel viewModel = new MainViewModel();
            viewModel.OnShowMsg += RaiseError;
            this.DataContext = viewModel;
            try
            {
                viewModel.InitCmd.Execute(null);
            }
            catch (Exception ex)
            {
                RaiseError(ex.Message);
            }
        }

        private void RaiseError(string errMsg)
        {
            this.Dispatcher.Invoke(new Action(()=>
            {
                TextRange textRange = new TextRange(rtxtMsg.Document.ContentStart, rtxtMsg.Document.ContentEnd);
                if (textRange.Text.Length > 2000)
                {
                    rtxtMsg.Document.Blocks.Clear();
                }
                rtxtMsg.AppendText(DateTime.Now.ToString("HH:mm:ss.fff") + errMsg + Environment.NewLine);
            }));
        }
    }
}
