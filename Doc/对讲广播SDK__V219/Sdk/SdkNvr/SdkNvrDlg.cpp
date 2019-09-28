// SdkNvrDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SdkNvr.h"
#include "SdkNvrDlg.h"
#include <mmsystem.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define INI_FILE        "SdkClient.ini"
#define AMSG(x)          ((x) ? "有" : "无")


typedef struct _WAVE_HEADER  
{  
    UINT szRiff;  
    UINT FileSize;  
    UINT szWave;  
  
    UINT szFmt;  
    UINT FmtSize;  
    PCMWAVEFORMAT Fmt;  
    UINT szData;  
    UINT DataSize;  
}WAVE_HEADER, *PWAVE_HEADER; 


CSdkNvrDlg * g_pMainWnd = NULL;
static const int g_nWaveHeaderSize = 38;
static const int g_nWaveFormatSize = 18;

FILE * g_pMp3File[MAX_NVR_CHAN];
FILE * g_pPcmFile[MAX_NVR_CHAN];
int g_nPcmTotalLen[MAX_NVR_CHAN];

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSdkNvrDlg dialog

CSdkNvrDlg::CSdkNvrDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSdkNvrDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSdkNvrDlg)
	m_bSdkPlaySound = TRUE;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	g_pMainWnd = this;

	memset(m_TermList, 0, sizeof(m_TermList));
	memset(m_TermState, 0, sizeof(m_TermState));
	m_nTermCnt = 0;
	memset(&m_NvrChanList, 0, sizeof(TSdkNvrChanList));
	memset(g_pMp3File, 0, sizeof(g_pMp3File));
	memset(g_pPcmFile, 0, sizeof(g_pPcmFile));
	memset(g_nPcmTotalLen, 0, sizeof(g_nPcmTotalLen));

	memset(&m_TTimeBeg, 0, sizeof(TSdkTime));
	memset(&m_TTimeEnd, 0, sizeof(TSdkTime));
	mBeginTime = 0;
	mTimeLen = 0;
	mEndTime = 0;
	mFileSize = 0;
	m_bIsPause = FALSE;
}

void CSdkNvrDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSdkNvrDlg)
	DDX_Control(pDX, IDC_BTN_PLAY_FILE, m_btnPlayFile);
	DDX_Control(pDX, IDC_BTN_PAUSE, m_btnPausePb);
	DDX_Control(pDX, IDC_BTN_STOP, m_btnStopPb);
	DDX_Control(pDX, IDC_SLIDER_PROGRESS, m_sldProgress);
	DDX_Control(pDX, IDC_STATIC_END_TIME, m_stcTimeEnd);
	DDX_Control(pDX, IDC_STATIC_CUR_TIME, m_stcTimePlay);
	DDX_Control(pDX, IDC_STATIC_BEG_TIME, m_stcTimeBegin);
	DDX_Control(pDX, IDC_LST_REC_FILE, m_lstRecFile);
	DDX_Control(pDX, IDC_DATETIMEPICKER1, m_SrchFileDate);
	DDX_Control(pDX, IDC_LIST_CHAN, m_ChanList);
	DDX_Control(pDX, IDC_CBX_TERM_LIST, m_cbxTermList);
	DDX_Control(pDX, IDC_LIST_INFO, m_InfoList);
	DDX_Check(pDX, IDC_CHK_SDK_PLAY_SOUND, m_bSdkPlaySound);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSdkNvrDlg, CDialog)
	//{{AFX_MSG_MAP(CSdkNvrDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_OPEN_CHAN_MP3, OnBtnOpenChanMp3)
	ON_BN_CLICKED(IDC_BTN_CLOSE_CHAN_MP3, OnBtnCloseChanMp3)
	ON_CBN_SELCHANGE(IDC_CBX_TERM_LIST, OnSelchangeCbxTermList)
	ON_BN_CLICKED(IDC_BTN_GET_ALL_CHAN, OnBtnGetAllChan)
	ON_BN_CLICKED(IDC_BTN_OPEN_CHAN_PCM, OnBtnOpenChanPcm)
	ON_BN_CLICKED(IDC_BTN_CLOSE_CHAN_PCM, OnBtnCloseChanPcm)
	ON_BN_CLICKED(IDC_BTN_SEARCH, OnBtnSearch)
	ON_BN_CLICKED(IDC_BTN_PLAY_FILE, OnBtnPlayFile)
	ON_BN_CLICKED(IDC_BTN_PAUSE, OnBtnPause)
	ON_BN_CLICKED(IDC_BTN_STOP, OnBtnStop)
	ON_WM_HSCROLL()
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_PROGRESS, OnReleasedcaptureSliderProgress)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSdkNvrDlg message handlers

BOOL CSdkNvrDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here

	int nResult = TSDK_Init(OnTzlSdkCallback, TRUE, 0x12345678);
	CString strText;
	if(nResult == CERR_SUCCESS)
		strText.Format("SDK 初始化成功！");
	else
		strText.Format("SDK 初始化失败！错误代码:%d", nResult);
	m_InfoList.AddString(strText);

	LoadTermList();
	UpdateTermListUI();

	DWORD dwStyle = m_ChanList.GetExtendedStyle();
	dwStyle |= (LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);//|LVS_EX_CHECKBOXES);
	m_ChanList.SetExtendedStyle(dwStyle);
	m_ChanList.InsertColumn(0, "通道号", LVCFMT_LEFT, 55);
	m_ChanList.InsertColumn(1, "通道类型", LVCFMT_LEFT, 90);
	m_ChanList.InsertColumn(2, "通道名称", LVCFMT_LEFT, 140);
	m_ChanList.InsertColumn(3, "连接状态", LVCFMT_LEFT, 70);
	m_ChanList.InsertColumn(4, "录像状态", LVCFMT_LEFT, 70);
	m_ChanList.InsertColumn(5, "MP3 录像文件路径", LVCFMT_LEFT, 240);
	m_ChanList.InsertColumn(6, "PCM 录像文件路径", LVCFMT_LEFT, 240);

	dwStyle = m_lstRecFile.GetExtendedStyle();
	dwStyle |= (LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	m_lstRecFile.SetExtendedStyle(dwStyle);
	m_lstRecFile.InsertColumn(0, "录音文件名称", LVCFMT_LEFT, 170);
	m_lstRecFile.InsertColumn(1, "大小", LVCFMT_LEFT, 90);

	m_sldProgress.SetRange(0, 1000);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSdkNvrDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSdkNvrDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CSdkNvrDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CSdkNvrDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here

	TSDK_DeInit();

	for(int i=0; i<MAX_NVR_CHAN; i++)
	{
		if(g_pMp3File[i])
		{
			fclose(g_pMp3File[i]);
			g_pMp3File[i] = NULL;
		}
	}
}

void CSdkNvrDlg::OnOK() 
{

}

void CSdkNvrDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

void CSdkNvrDlg::UpdateTermListUI()
{
	int nCurSel = m_cbxTermList.GetCurSel();
	m_cbxTermList.ResetContent();

	CString strText;
	for(int i=0; i<m_nTermCnt; i++)
	{
		strText.Format("ID:[%X]  名称:[%s]  IP:[%s]  状态:[%s]", m_TermList[i].dwTermID, m_TermList[i].TermName,
			m_TermList[i].TermIp, m_TermState[i].eState==TSDK_DEV_STATE_OFFLINE ? "不在线":"在线");

		m_cbxTermList.AddString(strText);
	}

	if(m_nTermCnt > 0)
	{
		if(nCurSel >= 0 && nCurSel < m_nTermCnt)
			m_cbxTermList.SetCurSel(nCurSel);
		else
			m_cbxTermList.SetCurSel(0);
	}
}

CString CSdkNvrDlg::GetExeDir()
{
	CString strExeDir = "";

	char aExeName[MAX_PATH];
	char aDriver[MAX_PATH];
	char aDir[MAX_PATH];

	DWORD dwRet = ::GetModuleFileName (AfxGetApp()->m_hInstance, aExeName, MAX_PATH);
	if(dwRet == 0)
		return strExeDir;

	::_splitpath (aExeName, aDriver, aDir, NULL, NULL);

	strExeDir = aDriver;
	strExeDir += aDir;

	return strExeDir;
}

void CSdkNvrDlg::LoadTermList()
{
	CString strIniFile;
	CString strSection, strKey;

	strIniFile.Format("%s%s", GetExeDir(), INI_FILE);
	strSection = "TermRegister";

	strKey = "TermCnt";
	m_nTermCnt = GetPrivateProfileInt(strSection, strKey, 0, strIniFile);

	for(int i=0; i<m_nTermCnt; i++)
	{
		strKey.Format("t%04d_TermID", i);
		m_TermList[i].dwTermID = GetPrivateProfileInt(strSection, strKey, 0, strIniFile);

		strKey.Format("t%04d_TermMac1", i);
		int nMac1 = GetPrivateProfileInt(strSection, strKey, 0, strIniFile);
		strKey.Format("t%04d_TermMac2", i);
		int nMac2 = GetPrivateProfileInt(strSection, strKey, 0, strIniFile);
		memcpy(m_TermList[i].TermMac, &nMac1, 3);
		memcpy(m_TermList[i].TermMac+3, &nMac2, 3);

		strKey.Format("t%04d_TermIp", i);
		GetPrivateProfileString(strSection, strKey, "", m_TermList[i].TermIp, 16, strIniFile);

		strKey.Format("t%04d_TermName", i);
		GetPrivateProfileString(strSection, strKey, "", m_TermList[i].TermName, 32, strIniFile);
	}
}

void CSdkNvrDlg::StoreTermList()
{
	CString strIniFile;
	CString strSection, strKey, strTmp;

	strIniFile.Format("%s%s", GetExeDir(), INI_FILE);
	strSection = "TermRegister";
	
	for(int i=0; i<m_nTermCnt; i++)
	{
		strKey.Format("t%04d_TermID", i);
		strTmp.Format("0x%X", m_TermList[i].dwTermID);
		WritePrivateProfileString(strSection, strKey, strTmp, strIniFile);

		LPBYTE p = m_TermList[i].TermMac;
		int nMac1 = p[0] | (p[1] << 8) | (p[2] << 16);
		int nMac2 = p[3] | (p[4] << 8) | (p[5] << 16);
		strKey.Format("t%04d_TermMac1", i);
		strTmp.Format("0x%06X", nMac1);
		WritePrivateProfileString(strSection, strKey, strTmp, strIniFile);
		strKey.Format("t%04d_TermMac2", i);
		strTmp.Format("0x%06X", nMac2);
		WritePrivateProfileString(strSection, strKey, strTmp, strIniFile);

		strKey.Format("t%04d_TermIp", i);
		WritePrivateProfileString(strSection, strKey, m_TermList[i].TermIp, strIniFile);

		strKey.Format("t%04d_TermName", i);
		WritePrivateProfileString(strSection, strKey, m_TermList[i].TermName, strIniFile);
	}
	
	strKey = "TermCnt";
	strTmp.Format("%d", m_nTermCnt);
	WritePrivateProfileString(strSection, strKey, strTmp, strIniFile);
}

static LPCSTR GetStateText(enSdkDevState eState)
{
	switch(eState)
	{
	case TSDK_DEV_STATE_OFFLINE:
		return "不在线";
	case TSDK_DEV_STATE_ONLINE:
		return "在线";
	case TSDK_DEV_STATE_PLAYING:
		return "播放音乐";
	case TSDK_DEV_STATE_TALKING:
		return "通话中";
	case TSDK_DEV_STATE_TALK_PLAY:
		return "通话中并播放音乐";
	}

	return "未知状态";
}

static LPCSTR GetTermType(enSdkDevType eTermType)
{
	switch(eTermType)
	{
	case TSDK_DEV_TYPE_TALK:
		return "[对讲广播终端]";
	case TSDK_DEV_TYPE_BROAD:
		return "[广播终端]";
	case TSDK_DEV_TYPE_MP3:
		return "[网络拾音器]";
	case TSDK_DEV_TYPE_NVR:
		return "[录音主机]";
	}

	return "[未知类型终端]";
}

int CALLBACK CSdkNvrDlg::OnTzlSdkCallback(enSdkCbType eCbType, LPVOID pParam, DWORD dwSize, int usr_data)
{
	switch(eCbType)
	{
		case CB_Event_TermRegister:
		{
			ASSERT(dwSize == sizeof(TSdkEventTermRegister));
			TSdkEventTermRegister * pEventTermRegister = (TSdkEventTermRegister*)pParam;
			LPBYTE p = pEventTermRegister->TermMac;
			CString strText;
			strText.Format("===> %s 请求注册：终端ID:%X , MAC地址:%02X-%02X-%02X-%02X-%02X-%02X , IP地址:%s , 名称:%s",
				GetTermType(pEventTermRegister->eTermType), pEventTermRegister->dwTermID, p[0],p[1],p[2],p[3],p[4],p[5], pEventTermRegister->TermIp, pEventTermRegister->TermName);
			g_pMainWnd->m_InfoList.AddString(strText);

			for(int i=0; i<g_pMainWnd->m_nTermCnt; i++)
			{
				if(g_pMainWnd->m_TermList[i].dwTermID == pEventTermRegister->dwTermID)
					break;
			}

			if(i < g_pMainWnd->m_nTermCnt)
			{
				// 更新
				g_pMainWnd->m_TermList[i] = *pEventTermRegister;

				g_pMainWnd->StoreTermList();
				g_pMainWnd->UpdateTermListUI();
			}
			else
			{
				// 注册
				if(g_pMainWnd->m_nTermCnt >= MAX_TERM_COUNT)
					return -1; // 返回 0 代表注册成功，否则代表注册失败 !!!

				g_pMainWnd->m_TermList[i] = *pEventTermRegister;
				g_pMainWnd->m_TermState[i].eState = TSDK_DEV_STATE_OFFLINE;
				g_pMainWnd->m_nTermCnt++;

				g_pMainWnd->StoreTermList();
				g_pMainWnd->UpdateTermListUI();
			}
			
			return CERR_SUCCESS; // 返回 0 代表注册成功，否则代表注册失败 !!!
		}

		case CB_Event_TermConnect:
		{
			ASSERT(dwSize == sizeof(TSdkEventTermConnect));
			TSdkEventTermConnect * pEventTermConnect = (TSdkEventTermConnect*)pParam;
			LPBYTE p = pEventTermConnect->TermMac;
			CString strText;
			strText.Format(">>> %s 请求连接：终端ID:%X , %s视频 , MAC地址:%02X-%02X-%02X-%02X-%02X-%02X , IP地址:%s , 名称:%s",
				GetTermType(pEventTermConnect->eTermType), pEventTermConnect->dwTermID, (pEventTermConnect->bHasVideo ? "有" : "无"),
				p[0],p[1],p[2],p[3],p[4],p[5], pEventTermConnect->TermIp, pEventTermConnect->TermName);
			g_pMainWnd->m_InfoList.AddString(strText);

			for(int i=0; i<g_pMainWnd->m_nTermCnt; i++)
			{
				if( (g_pMainWnd->m_TermList[i].dwTermID == pEventTermConnect->dwTermID) &&
					memcmp(g_pMainWnd->m_TermList[i].TermMac, pEventTermConnect->TermMac, 6) == 0)
				{
					break;
				}
			}

			if(i < g_pMainWnd->m_nTermCnt)
			{
				// 已注册过
				BOOL bChangeIp = strcmp(g_pMainWnd->m_TermList[i].TermIp, pEventTermConnect->TermIp);
				if(bChangeIp)
					strcpy(g_pMainWnd->m_TermList[i].TermIp, pEventTermConnect->TermIp);

				BOOL bChangeName = strcmp(g_pMainWnd->m_TermList[i].TermName, pEventTermConnect->TermName);
				if(bChangeName)
					strcpy(g_pMainWnd->m_TermList[i].TermName, pEventTermConnect->TermName);

				g_pMainWnd->m_TermList[i].eTermType = pEventTermConnect->eTermType;
				g_pMainWnd->m_TermState[i].eState = TSDK_DEV_STATE_ONLINE;

				if(bChangeIp || bChangeName)
					g_pMainWnd->StoreTermList();

				g_pMainWnd->UpdateTermListUI();

				return CERR_SUCCESS; // 返回 0 代表允许连接，否则代表不允许 !!!
			}
			else
			{
				// 未注册
				return -1; // 返回 0 代表允许连接，否则代表不允许 !!!
			}
		}

		case CB_Event_TermCnnLost:
		{
			ASSERT(dwSize == sizeof(TSdkEventTermCnnLost));
			TSdkEventTermCnnLost * pEventTermCnnLost = (TSdkEventTermCnnLost*)pParam;
			CString strText;
			strText.Format("xxx 终端断开连接：终端ID:%X", pEventTermCnnLost->dwTermID);
			g_pMainWnd->m_InfoList.AddString(strText);

			for(int i=0; i<g_pMainWnd->m_nTermCnt; i++)
			{
				if(g_pMainWnd->m_TermList[i].dwTermID == pEventTermCnnLost->dwTermID)
				{
					g_pMainWnd->m_TermState[i].eState = TSDK_DEV_STATE_OFFLINE;
					break;
				}
			}

			g_pMainWnd->UpdateTermListUI();

			break;
		}
		
		case CB_Post_TermState:
		{
			ASSERT(dwSize == sizeof(TSdkPostTermState));
			TSdkPostTermState * pPostTermState = (TSdkPostTermState*)pParam;
			
			for(int i=0; i<g_pMainWnd->m_nTermCnt; i++)
			{
				if(g_pMainWnd->m_TermList[i].dwTermID == pPostTermState->dwTermID)
				{
					g_pMainWnd->m_TermState[i].eState = pPostTermState->eTermState;
					g_pMainWnd->m_TermState[i].AlmInState = pPostTermState->AlmInState;
					g_pMainWnd->m_TermState[i].AlmOutState = pPostTermState->AlmOutState;
					break;
				}
			}

			CString strText;
			strText.Format("终端ID:%X 终端状态:%s , IO报警 == 输入1:%s , 输入2:%s , 输出1:%s , 输出2:%s", pPostTermState->dwTermID, GetStateText(pPostTermState->eTermState),
				AMSG(pPostTermState->AlmInState & 0x1), AMSG((pPostTermState->AlmInState >> 1) & 0x1),
				AMSG(pPostTermState->AlmOutState & 0x1), AMSG((pPostTermState->AlmOutState >> 1) & 0x1));
			g_pMainWnd->m_InfoList.AddString(strText);

			break;
		}

		case CB_Post_NvrChanNotify:
		{
			ASSERT(dwSize == sizeof(TSdkPostNvrChanNotify));
			TSdkPostNvrChanNotify * pPostNvrChanNotify = (TSdkPostNvrChanNotify*)pParam;

			if(g_pMainWnd->m_ChanList.GetItemCount() == 0)
				break;

			int ch = pPostNvrChanNotify->nChanNum - 1;
			TSDK_NvrGetChan(pPostNvrChanNotify->dwTermID, pPostNvrChanNotify->nChanNum, &g_pMainWnd->m_NvrChanList.ChanList[ch]);

			CString strText;
			if(g_pMainWnd->m_NvrChanList.ChanList[ch].bIsLocal)
			{
				g_pMainWnd->m_ChanList.SetItemText(ch, 1, "[本地]");
			}
			else
			{
				if(g_pMainWnd->m_NvrChanList.ChanList[ch].bValid)
				{
					g_pMainWnd->m_ChanList.SetItemText(ch, 1, "[网络]");
				}
				else
				{
					g_pMainWnd->m_ChanList.SetItemText(ch, 1, "网络--未注册");
					g_pMainWnd->m_ChanList.SetItemText(ch, 2, "");
					g_pMainWnd->m_ChanList.SetItemText(ch, 3, "");
					g_pMainWnd->m_ChanList.SetItemText(ch, 4, "");
					break;
				}
			}

			g_pMainWnd->m_ChanList.SetItemText(ch, 2, g_pMainWnd->m_NvrChanList.ChanList[ch].ChanName);
			g_pMainWnd->m_ChanList.SetItemText(ch, 3, g_pMainWnd->m_NvrChanList.ChanList[ch].bIsOnline ? "在线" : "离线");
			g_pMainWnd->m_ChanList.SetItemText(ch, 4, g_pMainWnd->m_NvrChanList.ChanList[ch].bIsRecording ? "正在录像" : "停止录像");

			break;
		}

		case CB_Data_NvrMp3:
		{
			ASSERT(dwSize == sizeof(TSdkDataNvrMp3));
			TSdkDataNvrMp3 * pDataNvrMp3 = (TSdkDataNvrMp3*)pParam;
//			TRACE("== MP3音频 ID:%X ：大小 %d 字节\n", pDataNvrMp3->dwTermID,pDataNvrMp3->nDataSize);
			if(g_pMp3File[pDataNvrMp3->nChanNum - 1])
				fwrite(pDataNvrMp3->pMp3Data, 1, pDataNvrMp3->nDataSize, g_pMp3File[pDataNvrMp3->nChanNum - 1]);
			break;
		}

		case CB_Data_NvrPcm:
		{
			ASSERT(dwSize == sizeof(TSdkDataNvrPcm));
			TSdkDataNvrPcm * pDataNvrPcm = (TSdkDataNvrPcm*)pParam;
//			TRACE("== Pcm音频 ID:%X ：大小 %d 字节\n", pDataNvrPcm->dwTermID,pDataNvrPcm->nDataSize);
			int chn = pDataNvrPcm->nChanNum - 1;
			if(g_pPcmFile[chn])
				fwrite(pDataNvrPcm->pPcmData, 1, pDataNvrPcm->nDataSize, g_pPcmFile[chn]);
			g_nPcmTotalLen[chn] += pDataNvrPcm->nDataSize;
/*
			{
				short * pSrc = (short*)pDataNvrPcm->pPcmData;
				for(int i=0; i<pDataNvrPcm->nDataSize; i+=2)
				{
					TRACE("%d ", *pSrc++);
				}
				TRACE("\n");
			}
*/
			break;
		}

		case CB_Data_NvrPlayMp3:
		{
			ASSERT(dwSize == sizeof(TSdkDataNvrPlayMp3));
			TSdkDataNvrPlayMp3 * pDataNvrPlayMp3 = (TSdkDataNvrPlayMp3*)pParam;
//			TRACE("== 回放Mp3音频 ID:%X ：大小 %d 字节\n", pDataNvrPlayMp3->dwTermID,pDataNvrPlayMp3->nDataSize);

			break;
		}

		case CB_Post_NvrSrchRecFile:
		{
			ASSERT(dwSize == sizeof(TSdkPostNvrSrchRecFile));
			TSdkPostNvrSrchRecFile * pPostNvrSrchRecFile = (TSdkPostNvrSrchRecFile*)pParam;

			CString strText;
			for(int i=0; i<pPostNvrSrchRecFile->nFileCnt; i++)
			{
				int nCnt = g_pMainWnd->m_lstRecFile.GetItemCount();
				g_pMainWnd->m_lstRecFile.InsertItem(nCnt, pPostNvrSrchRecFile->FileList[i].Name);
				g_pMainWnd->m_lstRecFile.SetItemData(nCnt, pPostNvrSrchRecFile->FileList[i].nSize);
				strText.Format("%d K", pPostNvrSrchRecFile->FileList[i].nSize / 1024);
				g_pMainWnd->m_lstRecFile.SetItemText(nCnt, 1, strText);
			}

			break;
		}
		
		case CB_Post_NvrPlayProg:
		{
			ASSERT(dwSize == sizeof(TSdkPostNvrPlayProg));
			TSdkPostNvrPlayProg * pPostNvrPlayProg = (TSdkPostNvrPlayProg*)pParam;

			if(::GetAsyncKeyState(VK_LBUTTON))
				break; // 鼠标左键拖动进度条

			time_t PlayTime = (time_t)(g_pMainWnd->mBeginTime + ((__int64)pPostNvrPlayProg->nPlayingFilePos) * g_pMainWnd->mTimeLen / g_pMainWnd->mFileSize);

			struct tm * ptm;
			ptm = localtime(&PlayTime);
			
			CString strText;
			strText.Format("%02d:%02d:%02d", ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
			g_pMainWnd->m_stcTimePlay.SetWindowText(strText);

			g_pMainWnd->m_sldProgress.SetPos((int)(((__int64)pPostNvrPlayProg->nPlayingFilePos) * 1000 / g_pMainWnd->mFileSize));

			break;
		}

		case CB_Asw_NvrSrchRecFile:
		{
			ASSERT(dwSize == sizeof(TSdkAswNvrSrchRecFile));
			TSdkAswNvrSrchRecFile * pAswNvrSrchRecFile = (TSdkAswNvrSrchRecFile*)pParam;
			if(pAswNvrSrchRecFile->nResult != CERR_SUCCESS)
			{
				CString strMsg;
				strMsg.Format("查找录音文件失败！错误代码：%d", pAswNvrSrchRecFile->nResult);
				AfxMessageBox(strMsg);
			}

			break;
		}

		case CB_Asw_NvrPlayRecFile:
		{
			ASSERT(dwSize == sizeof(TSdkAswNvrPlayRecFile));
			TSdkAswNvrPlayRecFile * pAswNvrPlayRecFile = (TSdkAswNvrPlayRecFile*)pParam;
			if(pAswNvrPlayRecFile->nResult != CERR_SUCCESS)
			{
				CString strMsg;
				strMsg.Format("回放录音文件失败！错误代码：%d", pAswNvrPlayRecFile->nResult);
				AfxMessageBox(strMsg);
			}

			g_pMainWnd->mTimeLen = pAswNvrPlayRecFile->nTotalRecTime;
			g_pMainWnd->mEndTime = g_pMainWnd->mBeginTime + g_pMainWnd->mTimeLen;

			struct tm * ptm;
			ptm = localtime(&g_pMainWnd->mEndTime);
			g_pMainWnd->m_TTimeEnd.Year = ptm->tm_year+1900;
			g_pMainWnd->m_TTimeEnd.Month = ptm->tm_mon+1;
			g_pMainWnd->m_TTimeEnd.Day = ptm->tm_mday;
			g_pMainWnd->m_TTimeEnd.Hour = ptm->tm_hour;
			g_pMainWnd->m_TTimeEnd.Minute = ptm->tm_min;
			g_pMainWnd->m_TTimeEnd.Second = ptm->tm_sec;

			CString strText;
			strText.Format("%02d:%02d:%02d", g_pMainWnd->m_TTimeBeg.Hour, g_pMainWnd->m_TTimeBeg.Minute, g_pMainWnd->m_TTimeBeg.Second);
			g_pMainWnd->m_stcTimeBegin.SetWindowText(strText);
			g_pMainWnd->m_stcTimePlay.SetWindowText(strText);

			strText.Format("%02d:%02d:%02d", g_pMainWnd->m_TTimeEnd.Hour, g_pMainWnd->m_TTimeEnd.Minute, g_pMainWnd->m_TTimeEnd.Second);
			g_pMainWnd->m_stcTimeEnd.SetWindowText(strText);

			g_pMainWnd->m_btnPlayFile.EnableWindow(FALSE);
			g_pMainWnd->m_btnStopPb.EnableWindow(TRUE);
			g_pMainWnd->m_btnPausePb.EnableWindow(TRUE);
			g_pMainWnd->m_sldProgress.EnableWindow(TRUE);
			g_pMainWnd->m_btnPausePb.SetWindowText("暂停");

			g_pMainWnd->m_bIsPause = FALSE;

			break;
		}

		default:
		{
			ASSERT(FALSE);
			break;
		}
	}

	return CERR_SUCCESS;
}

void CSdkNvrDlg::OnBtnOpenChanMp3() 
{
	CString strText, strPath;
	int nSel = m_cbxTermList.GetCurSel();
	if(nSel < 0)
	{
		MessageBox("请选择录音主机！", "提示", MB_ICONWARNING);
		return;
	}

	if(m_TermState[nSel].eState == TSDK_DEV_STATE_OFFLINE)
	{
		MessageBox("设备不在线！", "提示", MB_ICONWARNING);
		return;
	}

	if(m_TermList[nSel].eTermType != TSDK_DEV_TYPE_NVR)
	{
		MessageBox("设备不是录音主机！", "提示", MB_ICONWARNING);
		return;
	}

	int nChan = m_ChanList.GetSelectionMark();
	if(nChan < 0)
		return;

	if(!m_NvrChanList.ChanList[nChan].bValid)
	{
		MessageBox("所选通道未注册设备！", "提示", MB_ICONWARNING);
		return;
	}

	if(g_pMp3File[nChan])
		return;

	int nResult = TSDK_NvrOpenChanMp3(m_TermList[nSel].dwTermID, nChan + 1);
	if(nResult != CERR_SUCCESS)
	{
		strText.Format("请求录音主机通道MP3流失败！错误代码:%d", nResult);
		MessageBox(strText, "错误", MB_ICONERROR);
		return;
	}

	strPath.Format("%s%02d.mp3", GetExeDir(), nChan+1);
	g_pMp3File[nChan] = fopen(strPath, "wb");
	m_ChanList.SetItemText(nChan, 5, strPath);
}

void CSdkNvrDlg::OnBtnCloseChanMp3() 
{
	CString strText, strPath;
	int nSel = m_cbxTermList.GetCurSel();
	if(nSel < 0)
	{
		MessageBox("请选择录音主机！", "提示", MB_ICONWARNING);
		return;
	}

	if(m_TermState[nSel].eState == TSDK_DEV_STATE_OFFLINE)
	{
		MessageBox("设备不在线！", "提示", MB_ICONWARNING);
		return;
	}

	if(m_TermList[nSel].eTermType != TSDK_DEV_TYPE_NVR)
	{
		MessageBox("设备不是录音主机！", "提示", MB_ICONWARNING);
		return;
	}

	int nChan = m_ChanList.GetSelectionMark();
	if(nChan < 0)
		return;

	if(!m_NvrChanList.ChanList[nChan].bValid)
	{
		MessageBox("所选通道未注册设备！", "提示", MB_ICONWARNING);
		return;
	}

	if(!g_pMp3File[nChan])
		return;

	int nResult = TSDK_NvrCloseChanMp3(m_TermList[nSel].dwTermID, nChan + 1);
	if(nResult != CERR_SUCCESS)
	{
		strText.Format("操作失败！错误代码:%d", nResult);
		MessageBox(strText, "错误", MB_ICONERROR);
	}	

	if(g_pMp3File[nChan])
	{
		fclose(g_pMp3File[nChan]);
		g_pMp3File[nChan] = NULL;
	}
	
	m_ChanList.SetItemText(nChan, 5, "");
}

void CSdkNvrDlg::OnSelchangeCbxTermList() 
{
	// TODO: Add your control notification handler code here
	
}

void CSdkNvrDlg::OnBtnGetAllChan() 
{
	m_ChanList.DeleteAllItems();

	int nSel = m_cbxTermList.GetCurSel();
	if(nSel < 0)
	{
		MessageBox("请选择录音主机！", "提示", MB_ICONWARNING);
		return;
	}

	if(m_TermState[nSel].eState == TSDK_DEV_STATE_OFFLINE)
	{
		MessageBox("设备不在线！", "提示", MB_ICONWARNING);
		return;
	}

	if(m_TermList[nSel].eTermType != TSDK_DEV_TYPE_NVR)
	{
		MessageBox("设备不是录音主机！", "提示", MB_ICONWARNING);
		return;
	}

	memset(&m_NvrChanList, 0, sizeof(TSdkNvrChanList));
	int nResult = TSDK_NvrGetChanList(m_TermList[nSel].dwTermID, &m_NvrChanList);
	if(nResult != CERR_SUCCESS)
	{
		CString strText;
		strText.Format("获取通道信息失败！错误代码:%d", nResult);
		MessageBox(strText, "错误", MB_ICONERROR);
		return;
	}

	CString strText;
	for(int i=0; i<MAX_NVR_CHAN; i++)
	{
		strText.Format("%d", m_NvrChanList.ChanList[i].nChanNum);
		m_ChanList.InsertItem(i, strText);
		if(m_NvrChanList.ChanList[i].bIsLocal)
		{
			m_ChanList.SetItemText(i, 1, "[本地]");
		}
		else
		{
			if(m_NvrChanList.ChanList[i].bValid)
			{
				m_ChanList.SetItemText(i, 1, "[网络]");
			}
			else
			{
				m_ChanList.SetItemText(i, 1, "网络--未注册");
				continue;
			}
		}

		m_ChanList.SetItemText(i, 2, m_NvrChanList.ChanList[i].ChanName);
		m_ChanList.SetItemText(i, 3, m_NvrChanList.ChanList[i].bIsOnline ? "在线" : "离线");
		m_ChanList.SetItemText(i, 4, m_NvrChanList.ChanList[i].bIsRecording ? "正在录像" : "停止录像");
	}
}

void CSdkNvrDlg::OnBtnOpenChanPcm() 
{
	CString strText, strPath;
	int nSel = m_cbxTermList.GetCurSel();
	if(nSel < 0)
	{
		MessageBox("请选择录音主机！", "提示", MB_ICONWARNING);
		return;
	}

	if(m_TermState[nSel].eState == TSDK_DEV_STATE_OFFLINE)
	{
		MessageBox("设备不在线！", "提示", MB_ICONWARNING);
		return;
	}

	if(m_TermList[nSel].eTermType != TSDK_DEV_TYPE_NVR)
	{
		MessageBox("设备不是录音主机！", "提示", MB_ICONWARNING);
		return;
	}

	int nChan = m_ChanList.GetSelectionMark();
	if(nChan < 0)
		return;

	if(!m_NvrChanList.ChanList[nChan].bValid)
	{
		MessageBox("所选通道未注册设备！", "提示", MB_ICONWARNING);
		return;
	}

	if(g_pPcmFile[nChan])
		return;

	int nResult = TSDK_NvrOpenChanPcm(m_TermList[nSel].dwTermID, nChan + 1);
	if(nResult != CERR_SUCCESS)
	{
		strText.Format("请求录音主机通道Pcm流失败！错误代码:%d", nResult);
		MessageBox(strText, "错误", MB_ICONERROR);
		return;
	}

	strPath.Format("%s%02d.wav", GetExeDir(), nChan+1);
	g_pPcmFile[nChan] = fopen(strPath, "wb");
	m_ChanList.SetItemText(nChan, 6, strPath);

	WAVEFORMATEX    wfx;
	memset(&wfx, 0, sizeof(WAVEFORMATEX));
	wfx.wFormatTag		= WAVE_FORMAT_PCM;
	wfx.nChannels		= 1;
	wfx.nSamplesPerSec	= 16000;
	wfx.wBitsPerSample	= 16;
	wfx.nAvgBytesPerSec	= 2 * 16000;
	wfx.nBlockAlign		= 2;
	wfx.cbSize			= 0;

	g_nPcmTotalLen[nChan] = 0;

	int nFileLen1=(g_nWaveHeaderSize + 4 + 4 + g_nPcmTotalLen[nChan] - 8);

	fwrite("RIFF", 1, 4, g_pPcmFile[nChan]);
	fwrite(&nFileLen1, 1, sizeof(nFileLen1), g_pPcmFile[nChan]);
	fwrite("WAVE", 1, 4, g_pPcmFile[nChan]);
	fwrite("fmt ", 1, 4, g_pPcmFile[nChan]);
	fwrite(&g_nWaveFormatSize, 1, sizeof(g_nWaveFormatSize), g_pPcmFile[nChan]);
	fwrite(&wfx, 1, g_nWaveFormatSize, g_pPcmFile[nChan]);
	fwrite("data", 1, 4, g_pPcmFile[nChan]);
	fwrite(&g_nPcmTotalLen[nChan], 1, sizeof(int), g_pPcmFile[nChan]);

}

void CSdkNvrDlg::OnBtnCloseChanPcm() 
{
	CString strText, strPath;
	int nSel = m_cbxTermList.GetCurSel();
	if(nSel < 0)
	{
		MessageBox("请选择录音主机！", "提示", MB_ICONWARNING);
		return;
	}

	if(m_TermState[nSel].eState == TSDK_DEV_STATE_OFFLINE)
	{
		MessageBox("设备不在线！", "提示", MB_ICONWARNING);
		return;
	}

	if(m_TermList[nSel].eTermType != TSDK_DEV_TYPE_NVR)
	{
		MessageBox("设备不是录音主机！", "提示", MB_ICONWARNING);
		return;
	}

	int nChan = m_ChanList.GetSelectionMark();
	if(nChan < 0)
		return;

	if(!m_NvrChanList.ChanList[nChan].bValid)
	{
		MessageBox("所选通道未注册设备！", "提示", MB_ICONWARNING);
		return;
	}

	if(!g_pPcmFile[nChan])
		return;

	int nResult = TSDK_NvrCloseChanPcm(m_TermList[nSel].dwTermID, nChan + 1);
	if(nResult != CERR_SUCCESS)
	{
		strText.Format("操作失败！错误代码:%d", nResult);
		MessageBox(strText, "错误", MB_ICONERROR);
	}		

	Sleep(500);

	int nFileLen1=(g_nWaveHeaderSize + 4 + 4 + g_nPcmTotalLen[nChan] - 8);
	
	if(g_pPcmFile[nChan])
	{
		fseek(g_pPcmFile[nChan], 4, SEEK_SET);
		fwrite(&nFileLen1, 1, sizeof(nFileLen1), g_pPcmFile[nChan]);
		fseek(g_pPcmFile[nChan], g_nWaveHeaderSize + 4, SEEK_SET);
		fwrite(&g_nPcmTotalLen[nChan], 1, sizeof(int), g_pPcmFile[nChan]);
		fclose(g_pPcmFile[nChan]);
		g_pPcmFile[nChan] = NULL;
	}

	m_ChanList.SetItemText(nChan, 6, "");
}

void CSdkNvrDlg::OnBtnSearch() 
{
	UpdateData(TRUE);

	CString strText, strPath;
	int nSel = m_cbxTermList.GetCurSel();
	if(nSel < 0)
	{
		MessageBox("请选择录音主机！", "提示", MB_ICONWARNING);
		return;
	}

	if(m_TermState[nSel].eState == TSDK_DEV_STATE_OFFLINE)
	{
		MessageBox("设备不在线！", "提示", MB_ICONWARNING);
		return;
	}

	if(m_TermList[nSel].eTermType != TSDK_DEV_TYPE_NVR)
	{
		MessageBox("设备不是录音主机！", "提示", MB_ICONWARNING);
		return;
	}

	int nChan = m_ChanList.GetSelectionMark();
	if(nChan < 0)
	{
		MessageBox("请选择左边通道列表中的某一通道！", "提示", MB_ICONWARNING);
		return;
	}

	if(!m_NvrChanList.ChanList[nChan].bValid)
	{
		MessageBox("所选通道未注册设备！", "提示", MB_ICONWARNING);
		return;
	}

	m_lstRecFile.DeleteAllItems();

	CTime date;
	m_SrchFileDate.GetTime(date);

	TSdkTime SdkDate;
	memset(&SdkDate, 0, sizeof(TSdkTime));
	SdkDate.Year = date.GetYear();
	SdkDate.Month = date.GetMonth();
	SdkDate.Day = date.GetDay();

	int nResult = TSDK_Req_NvrSrchRecFile(m_TermList[nSel].dwTermID, nChan + 1, &SdkDate);
	if(nResult != CERR_SUCCESS)
	{
		strText.Format("操作失败！错误代码:%d", nResult);
		MessageBox(strText, "错误", MB_ICONERROR);
	}
}

void CSdkNvrDlg::OnBtnPlayFile() 
{
	UpdateData(TRUE);

	CString strText, strPath;
	int nSel = m_cbxTermList.GetCurSel();
	if(nSel < 0)
	{
		MessageBox("请选择录音主机！", "提示", MB_ICONWARNING);
		return;
	}

	if(m_TermState[nSel].eState == TSDK_DEV_STATE_OFFLINE)
	{
		MessageBox("设备不在线！", "提示", MB_ICONWARNING);
		return;
	}

	if(m_TermList[nSel].eTermType != TSDK_DEV_TYPE_NVR)
	{
		MessageBox("设备不是录音主机！", "提示", MB_ICONWARNING);
		return;
	}

	int nChan = m_ChanList.GetSelectionMark();
	if(nChan < 0)
	{
		MessageBox("请选择左边通道列表中的某一通道！", "提示", MB_ICONWARNING);
		return;
	}

	if(!m_NvrChanList.ChanList[nChan].bValid)
	{
		MessageBox("所选通道未注册设备！", "提示", MB_ICONWARNING);
		return;
	}

	int nFile = m_lstRecFile.GetSelectionMark();
	if(nFile < 0)
	{
		MessageBox("请选择录音文件！", "提示", MB_ICONWARNING);
		return;
	}

	mFileSize = m_lstRecFile.GetItemData(nFile);
	m_TTimeBeg.Year = 2019;
	m_TTimeBeg.Month = 1;
	m_TTimeBeg.Day = 1;

	CString strFileName = m_lstRecFile.GetItemText(nFile, 0);
	LPCSTR p = (LPCSTR)strFileName;
	m_TTimeBeg.Hour = (p[0]-'0')*10 + (p[1]-'0')*1;
	m_TTimeBeg.Minute = (p[3]-'0')*10 + (p[4]-'0')*1;
	m_TTimeBeg.Second = (p[6]-'0')*10 + (p[7]-'0')*1;
	
	CTime BegTime(m_TTimeBeg.Year, m_TTimeBeg.Month, m_TTimeBeg.Day, m_TTimeBeg.Hour, m_TTimeBeg.Minute, m_TTimeBeg.Second, 0);
	mBeginTime = BegTime.GetTime();

	CTime date;
	m_SrchFileDate.GetTime(date);
	TSdkTime SdkDate;
	memset(&SdkDate, 0, sizeof(TSdkTime));
	SdkDate.Year = date.GetYear();
	SdkDate.Month = date.GetMonth();
	SdkDate.Day = date.GetDay();

	int nResult = TSDK_Req_NvrPlayRecFile(m_TermList[nSel].dwTermID, nChan + 1, &SdkDate, strFileName, m_bSdkPlaySound);
	if(nResult != CERR_SUCCESS)
	{
		strText.Format("操作失败！错误代码:%d", nResult);
		MessageBox(strText, "错误", MB_ICONERROR);
	}
}

void CSdkNvrDlg::OnBtnPause() 
{
	m_bIsPause = !m_bIsPause;

	g_pMainWnd->m_btnPausePb.SetWindowText(m_bIsPause ? "继续" : "暂停");

	TSDK_NvrPausePlay(m_bIsPause);
}

void CSdkNvrDlg::OnBtnStop() 
{
	TSDK_NvrStopPlay();

	m_btnPlayFile.EnableWindow(TRUE);
	m_btnStopPb.EnableWindow(FALSE);
	m_btnPausePb.EnableWindow(FALSE);
	m_sldProgress.EnableWindow(FALSE);
	m_btnPausePb.SetWindowText("暂停");

	m_stcTimeBegin.SetWindowText("00:00:00");
	m_stcTimePlay.SetWindowText("00:00:00");
	m_stcTimeEnd.SetWindowText("00:00:00");

	m_sldProgress.SetPos(0);
}

void CSdkNvrDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default

	time_t CurTime = (time_t)(mBeginTime + ((__int64)m_sldProgress.GetPos()) * mTimeLen / 1000);

	struct tm * ptm;
	ptm = localtime(&CurTime);
	
	CString strText;
	strText.Format("%02d:%02d:%02d", ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
	m_stcTimePlay.SetWindowText(strText);
	
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CSdkNvrDlg::OnReleasedcaptureSliderProgress(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here

	time_t CurTime = (time_t)(mBeginTime + ((__int64)m_sldProgress.GetPos()) * mTimeLen / 1000);

	struct tm * ptm;
	ptm = localtime(&CurTime);
	
	CString strText;
	strText.Format("%02d:%02d:%02d", ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
	m_stcTimePlay.SetWindowText(strText);

	m_bIsPause = FALSE;
	m_btnPausePb.SetWindowText("暂停");

	TSDK_NvrSetPlayFilePos((DWORD)(((__int64)(m_sldProgress.GetPos())) * mFileSize / 1000));
		
	*pResult = 0;
}
