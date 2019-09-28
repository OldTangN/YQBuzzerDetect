// SdkMp3Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "SdkMp3.h"
#include "SdkMp3Dlg.h"
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


CSdkMp3Dlg * g_pMainWnd = NULL;
static int g_MyGroupNum = 6;
FILE * g_pMp3File1 = NULL;
FILE * g_pMp3File2 = NULL;
FILE * g_pPcmFile1 = NULL;
FILE * g_pPcmFile2 = NULL;
int g_nPcmTotalLen1 = 0;
int g_nPcmTotalLen2 = 0;
static const int g_nWaveHeaderSize = 38;
static const int g_nWaveFormatSize = 18;

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
// CSdkMp3Dlg dialog

CSdkMp3Dlg::CSdkMp3Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSdkMp3Dlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSdkMp3Dlg)
	m_nCapLevel = -1;
	m_bPreviewL = FALSE;
	m_bPreviewR = FALSE;
	m_bPreviewL2 = FALSE;
	m_bPreviewR2 = FALSE;
	m_nAmpLevel = -1;
	m_bDbAnalyzeEnable = FALSE;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	g_pMainWnd = this;

	memset(m_TermList, 0, sizeof(m_TermList));
	memset(m_TermState, 0, sizeof(m_TermState));
	m_nTermCnt = 0;
	m_nRecordTermIdx = -1;
	m_nRecordTermIdx2 = -1;
	m_bIsBroadingVoice = FALSE;
	m_nTalkTermIdx = -1;
}

void CSdkMp3Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSdkMp3Dlg)
	DDX_Control(pDX, IDC_CBX_TERM_LIST, m_cbxTermList);
	DDX_Control(pDX, IDC_LIST_INFO, m_InfoList);
	DDX_CBIndex(pDX, IDC_CMB_CAP, m_nCapLevel);
	DDX_Check(pDX, IDC_CHK_PREVIEW_L, m_bPreviewL);
	DDX_Check(pDX, IDC_CHK_PREVIEW_R, m_bPreviewR);
	DDX_Check(pDX, IDC_CHK_PREVIEW_L2, m_bPreviewL2);
	DDX_Check(pDX, IDC_CHK_PREVIEW_R2, m_bPreviewR2);
	DDX_CBIndex(pDX, IDC_CMB_AMP, m_nAmpLevel);
	DDX_Check(pDX, IDC_CHK_DB_ANALYZE, m_bDbAnalyzeEnable);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSdkMp3Dlg, CDialog)
	//{{AFX_MSG_MAP(CSdkMp3Dlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_GET_LEVEL, OnGetLevel)
	ON_BN_CLICKED(IDC_SET_LEVEL, OnSetLevel)
	ON_BN_CLICKED(IDC_BTN_OPEN_MP3, OnBtnOpenMp3)
	ON_BN_CLICKED(IDC_BTN_CLOSE_MP3, OnBtnCloseMp3)
	ON_BN_CLICKED(IDC_CHK_PREVIEW_L, OnChkPreviewL)
	ON_BN_CLICKED(IDC_CHK_PREVIEW_R, OnChkPreviewR)
	ON_BN_CLICKED(IDC_BTN_OPEN_PCM, OnBtnOpenPcm)
	ON_BN_CLICKED(IDC_BTN_CLOSE_PCM, OnBtnClosePcm)
	ON_BN_CLICKED(IDC_CHK_PREVIEW_L2, OnChkPreviewL2)
	ON_BN_CLICKED(IDC_CHK_PREVIEW_R2, OnChkPreviewR2)
	ON_BN_CLICKED(IDC_BTN_OPEN_TALK, OnBtnOpenTalk)
	ON_BN_CLICKED(IDC_BTN_CLOSE_TALK, OnBtnCloseTalk)
	ON_BN_CLICKED(IDC_BTN_OPEN_ALM_OUT_1, OnBtnOpenAlmOut1)
	ON_BN_CLICKED(IDC_BTN_OPEN_ALM_OUT_2, OnBtnOpenAlmOut2)
	ON_CBN_SELCHANGE(IDC_CBX_TERM_LIST, OnSelchangeCbxTermList)
	ON_BN_CLICKED(IDC_BTN_CLEAR, OnBtnClear)
	ON_BN_CLICKED(IDC_BTN_OPEN_TALK_R, OnBtnOpenTalk_R)
	ON_BN_CLICKED(IDC_CHK_DB_ANALYZE, OnChkDbAnalyze)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSdkMp3Dlg message handlers

BOOL CSdkMp3Dlg::OnInitDialog()
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
	if (nResult == CERR_SUCCESS)
		strText.Format("SDK 初始化成功！");
	else
		strText.Format("SDK 初始化失败！错误代码:%d", nResult);
	m_InfoList.AddString(strText);

	LoadTermList();
	UpdateTermListUI();

	GetDlgItem(IDC_STATIC_FILE_1)->SetWindowText("录音文件 1: ");
	GetDlgItem(IDC_STATIC_FILE_2)->SetWindowText("录音文件 2: ");
	GetDlgItem(IDC_STATIC_FILE_3)->SetWindowText("录音文件 1: ");
	GetDlgItem(IDC_STATIC_FILE_4)->SetWindowText("录音文件 2: ");

	///////////////////////////////////////////////////////////
	GetDlgItem(IDC_BTN_OPEN_TALK)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_OPEN_TALK_R)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_CLOSE_TALK)->EnableWindow(FALSE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSdkMp3Dlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CSdkMp3Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM)dc.GetSafeHdc(), 0);

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
HCURSOR CSdkMp3Dlg::OnQueryDragIcon()
{
	return (HCURSOR)m_hIcon;
}

void CSdkMp3Dlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: Add your message handler code here

	if (m_nRecordTermIdx != -1)
	{
		TSDK_Req_CloseTermAudVid(m_TermList[m_nRecordTermIdx].dwTermID);
		TSDK_Req_PreviewMp3L(m_TermList[m_nRecordTermIdx].dwTermID, FALSE);
		TSDK_Req_PreviewMp3R(m_TermList[m_nRecordTermIdx].dwTermID, FALSE);

		if (g_pMp3File1)
		{
			fclose(g_pMp3File1);
			g_pMp3File1 = NULL;
		}

		if (g_pMp3File2)
		{
			fclose(g_pMp3File2);
			g_pMp3File2 = NULL;
		}
	}
	if (m_nRecordTermIdx2 != -1)
	{
		TSDK_Req_CloseTermPcm(m_TermList[m_nRecordTermIdx2].dwTermID);
		TSDK_Req_PreviewPcmL(m_TermList[m_nRecordTermIdx2].dwTermID, FALSE);
		TSDK_Req_PreviewPcmR(m_TermList[m_nRecordTermIdx2].dwTermID, FALSE);

		if (g_pPcmFile1)
		{
			fclose(g_pPcmFile1);
			g_pPcmFile1 = NULL;
		}

		if (g_pPcmFile2)
		{
			fclose(g_pPcmFile2);
			g_pPcmFile2 = NULL;
		}
	}

	TSDK_DeInit();
}

void CSdkMp3Dlg::OnOK()
{

}

void CSdkMp3Dlg::OnCancel()
{
	// TODO: Add extra cleanup here

	CDialog::OnCancel();
}

void CSdkMp3Dlg::UpdateTermListUI()
{
	int nCurSel = m_cbxTermList.GetCurSel();
	m_cbxTermList.ResetContent();

	CString strText;
	for (int i = 0; i < m_nTermCnt; i++)
	{
		strText.Format("ID:[%X]  名称:[%s]  IP:[%s]  状态:[%s]", m_TermList[i].dwTermID, m_TermList[i].TermName,
			m_TermList[i].TermIp, m_TermState[i].eState == TSDK_DEV_STATE_OFFLINE ? "不在线" : "在线");

		m_cbxTermList.AddString(strText);
	}

	if (m_nTermCnt > 0)
	{
		if (nCurSel >= 0 && nCurSel < m_nTermCnt)
			m_cbxTermList.SetCurSel(nCurSel);
		else
			m_cbxTermList.SetCurSel(0);
	}
}

CString CSdkMp3Dlg::GetExeDir()
{
	CString strExeDir = "";

	char aExeName[MAX_PATH];
	char aDriver[MAX_PATH];
	char aDir[MAX_PATH];

	DWORD dwRet = ::GetModuleFileName(AfxGetApp()->m_hInstance, aExeName, MAX_PATH);
	if (dwRet == 0)
		return strExeDir;

	::_splitpath(aExeName, aDriver, aDir, NULL, NULL);

	strExeDir = aDriver;
	strExeDir += aDir;

	return strExeDir;
}

void CSdkMp3Dlg::LoadTermList()
{
	CString strIniFile;
	CString strSection, strKey;

	strIniFile.Format("%s%s", GetExeDir(), INI_FILE);
	strSection = "TermRegister";

	strKey = "TermCnt";
	m_nTermCnt = GetPrivateProfileInt(strSection, strKey, 0, strIniFile);

	for (int i = 0; i < m_nTermCnt; i++)
	{
		strKey.Format("t%04d_TermID", i);
		m_TermList[i].dwTermID = GetPrivateProfileInt(strSection, strKey, 0, strIniFile);

		strKey.Format("t%04d_TermMac1", i);
		int nMac1 = GetPrivateProfileInt(strSection, strKey, 0, strIniFile);
		strKey.Format("t%04d_TermMac2", i);
		int nMac2 = GetPrivateProfileInt(strSection, strKey, 0, strIniFile);
		memcpy(m_TermList[i].TermMac, &nMac1, 3);
		memcpy(m_TermList[i].TermMac + 3, &nMac2, 3);

		strKey.Format("t%04d_TermIp", i);
		GetPrivateProfileString(strSection, strKey, "", m_TermList[i].TermIp, 16, strIniFile);

		strKey.Format("t%04d_TermName", i);
		GetPrivateProfileString(strSection, strKey, "", m_TermList[i].TermName, 32, strIniFile);
	}
}

void CSdkMp3Dlg::StoreTermList()
{
	CString strIniFile;
	CString strSection, strKey, strTmp;

	strIniFile.Format("%s%s", GetExeDir(), INI_FILE);
	strSection = "TermRegister";

	for (int i = 0; i < m_nTermCnt; i++)
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
	switch (eState)
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
	switch (eTermType)
	{
	case TSDK_DEV_TYPE_TALK:
		return "[对讲广播终端]";
	case TSDK_DEV_TYPE_BROAD:
		return "[广播终端]";
	case TSDK_DEV_TYPE_MP3:
		return "[网络拾音器]";
	}

	return "[未知类型终端]";
}

int CALLBACK CSdkMp3Dlg::OnTzlSdkCallback(enSdkCbType eCbType, LPVOID pParam, DWORD dwSize, int usr_data)
{
	switch (eCbType)
	{
	case CB_Event_TermRegister:
	{
		ASSERT(dwSize == sizeof(TSdkEventTermRegister));
		TSdkEventTermRegister * pEventTermRegister = (TSdkEventTermRegister*)pParam;
		LPBYTE p = pEventTermRegister->TermMac;
		CString strText;
		strText.Format("===> %s 请求注册：终端ID:%X , MAC地址:%02X-%02X-%02X-%02X-%02X-%02X , IP地址:%s , 名称:%s",
			GetTermType(pEventTermRegister->eTermType), pEventTermRegister->dwTermID, p[0], p[1], p[2], p[3], p[4], p[5], pEventTermRegister->TermIp, pEventTermRegister->TermName);
		g_pMainWnd->m_InfoList.AddString(strText);
		int i;
		for (; i < g_pMainWnd->m_nTermCnt; i++)
		{
			if (g_pMainWnd->m_TermList[i].dwTermID == pEventTermRegister->dwTermID)
				break;
		}

		if (i < g_pMainWnd->m_nTermCnt)
		{
			// 更新
			g_pMainWnd->m_TermList[i] = *pEventTermRegister;

			g_pMainWnd->StoreTermList();
			g_pMainWnd->UpdateTermListUI();
		}
		else
		{
			// 注册
			if (g_pMainWnd->m_nTermCnt >= MAX_TERM_COUNT)
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
			p[0], p[1], p[2], p[3], p[4], p[5], pEventTermConnect->TermIp, pEventTermConnect->TermName);
		g_pMainWnd->m_InfoList.AddString(strText);
		int i=0;
		for (; i < g_pMainWnd->m_nTermCnt; i++)
		{
			if ((g_pMainWnd->m_TermList[i].dwTermID == pEventTermConnect->dwTermID) &&
				memcmp(g_pMainWnd->m_TermList[i].TermMac, pEventTermConnect->TermMac, 6) == 0)
			{
				break;
			}
		}

		if (i < g_pMainWnd->m_nTermCnt)
		{
			// 已注册过
			BOOL bChangeIp = strcmp(g_pMainWnd->m_TermList[i].TermIp, pEventTermConnect->TermIp);
			if (bChangeIp)
				strcpy(g_pMainWnd->m_TermList[i].TermIp, pEventTermConnect->TermIp);

			BOOL bChangeName = strcmp(g_pMainWnd->m_TermList[i].TermName, pEventTermConnect->TermName);
			if (bChangeName)
				strcpy(g_pMainWnd->m_TermList[i].TermName, pEventTermConnect->TermName);

			g_pMainWnd->m_TermList[i].eTermType = pEventTermConnect->eTermType;
			g_pMainWnd->m_TermState[i].bTalkbackEnable = pEventTermConnect->bTalkbackEnable;
			g_pMainWnd->m_TermState[i].eState = TSDK_DEV_STATE_ONLINE;

			if (bChangeIp || bChangeName)
				g_pMainWnd->StoreTermList();

			g_pMainWnd->UpdateTermListUI();

			int nCurSel = g_pMainWnd->m_cbxTermList.GetCurSel();
			if (nCurSel == i)
			{
				int nShow = g_pMainWnd->m_TermState[i].bTalkbackEnable ? SW_HIDE : SW_SHOW;
				g_pMainWnd->GetDlgItem(IDC_STATIC_PCM_TITLE)->ShowWindow(nShow);
				g_pMainWnd->GetDlgItem(IDC_BTN_OPEN_PCM)->ShowWindow(nShow);
				g_pMainWnd->GetDlgItem(IDC_BTN_CLOSE_PCM)->ShowWindow(nShow);
				g_pMainWnd->GetDlgItem(IDC_STATIC_FILE_3)->ShowWindow(nShow);
				g_pMainWnd->GetDlgItem(IDC_STATIC_FILE_4)->ShowWindow(nShow);
				g_pMainWnd->GetDlgItem(IDC_CHK_PREVIEW_L2)->ShowWindow(nShow);
				g_pMainWnd->GetDlgItem(IDC_CHK_PREVIEW_R2)->ShowWindow(nShow);
			}

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

		for (int i = 0; i < g_pMainWnd->m_nTermCnt; i++)
		{
			if (g_pMainWnd->m_TermList[i].dwTermID == pEventTermCnnLost->dwTermID)
			{
				g_pMainWnd->m_TermState[i].eState = TSDK_DEV_STATE_OFFLINE;
				break;
			}
		}

		g_pMainWnd->UpdateTermListUI();

		break;
	}

	case CB_Asw_OpenTermAudVid:
	{
		ASSERT(dwSize == sizeof(TSdkAswOpenTermAudVid));
		TSdkAswOpenTermAudVid * pAswOpenTermAudVid = (TSdkAswOpenTermAudVid*)pParam;
		CString strText;
		if (pAswOpenTermAudVid->nResult == CERR_SUCCESS)
			strText.Format("打开终端音频成功！终端ID:%X ", pAswOpenTermAudVid->dwTermID);
		else
			strText.Format("打开终端音频失败！终端ID:%X ，错误代码:%d", pAswOpenTermAudVid->dwTermID, pAswOpenTermAudVid->nResult);
		g_pMainWnd->m_InfoList.AddString(strText);
		break;
	}

	case CB_Asw_CloseTermAudVid:
	{
		ASSERT(dwSize == sizeof(TSdkAswCloseTermAudVid));
		TSdkAswCloseTermAudVid * pAswCloseTermAudVid = (TSdkAswCloseTermAudVid*)pParam;
		CString strText;
		if (pAswCloseTermAudVid->nResult == CERR_SUCCESS)
			strText.Format("关闭终端音频成功！终端ID:%X ", pAswCloseTermAudVid->dwTermID);
		else
			strText.Format("关闭终端音频失败！终端ID:%X ，错误代码:%d", pAswCloseTermAudVid->dwTermID, pAswCloseTermAudVid->nResult);

		break;
	}

	case CB_Asw_GetAudLevel:
	{
		ASSERT(dwSize == sizeof(TSdkAswGetAudLevel));
		TSdkAswGetAudLevel * pAswGetAudLevel = (TSdkAswGetAudLevel*)pParam;
		g_pMainWnd->m_nCapLevel = pAswGetAudLevel->Level.eCapLevel;
		g_pMainWnd->m_nAmpLevel = pAswGetAudLevel->Level.eAmpLevel;
		((CComboBox*)g_pMainWnd->GetDlgItem(IDC_CMB_CAP))->SetCurSel(g_pMainWnd->m_nCapLevel);
		((CComboBox*)g_pMainWnd->GetDlgItem(IDC_CMB_AMP))->SetCurSel(g_pMainWnd->m_nAmpLevel);
		break;
	}

	case CB_Asw_SetAudLevel:
	{
		ASSERT(dwSize == sizeof(TSdkAswSetAudLevel));
		TSdkAswSetAudLevel * pAswSetAudLevel = (TSdkAswSetAudLevel*)pParam;
		CString strText;
		if (pAswSetAudLevel->nResult == CERR_SUCCESS)
			strText.Format("设置终端音量成功！终端ID:%X ", pAswSetAudLevel->dwTermID);
		else
			strText.Format("设置终端音量失败！终端ID:%X ，错误代码:%d", pAswSetAudLevel->dwTermID, pAswSetAudLevel->nResult);
		g_pMainWnd->m_InfoList.AddString(strText);
		break;
	}

	case CB_Asw_SetDbAlmCfg:
	{
		ASSERT(dwSize == sizeof(TSdkAswSetDbAlmCfg));
		TSdkAswSetDbAlmCfg * pAswSetDbAlmCfg = (TSdkAswSetDbAlmCfg*)pParam;
		CString strText;
		if (pAswSetDbAlmCfg->nResult == CERR_SUCCESS)
			strText.Format("修改终端配置成功！终端ID:%X ", pAswSetDbAlmCfg->dwTermID);
		else
			strText.Format("修改终端配置失败！终端ID:%X ，错误代码:%d", pAswSetDbAlmCfg->dwTermID, pAswSetDbAlmCfg->nResult);
		g_pMainWnd->m_InfoList.AddString(strText);
		break;
	}

	case CB_Asw_OpenTermTalk:
	{
		ASSERT(dwSize == sizeof(TSdkAswOpenTermTalk));
		TSdkAswOpenTermTalk * pAswOpenTermTalk = (TSdkAswOpenTermTalk*)pParam;
		CString strText;
		if (pAswOpenTermTalk->nResult == CERR_SUCCESS)
			strText.Format("打开终端对讲成功！终端ID:%X ", pAswOpenTermTalk->dwTermID);
		else
			strText.Format("打开终端对讲失败！终端ID:%X ，错误代码:%d", pAswOpenTermTalk->dwTermID, pAswOpenTermTalk->nResult);
		g_pMainWnd->m_InfoList.AddString(strText);
		break;
	}

	case CB_Asw_CloseTermTalk:
	{
		ASSERT(dwSize == sizeof(TSdkAswCloseTermTalk));
		TSdkAswCloseTermTalk * pAswCloseTermTalk = (TSdkAswCloseTermTalk*)pParam;
		CString strText;
		if (pAswCloseTermTalk->nResult == CERR_SUCCESS)
			strText.Format("关闭终端对讲成功！终端ID:%X ", pAswCloseTermTalk->dwTermID);
		else
			strText.Format("关闭终端对讲失败！终端ID:%X ，错误代码:%d", pAswCloseTermTalk->dwTermID, pAswCloseTermTalk->nResult);
		g_pMainWnd->m_InfoList.AddString(strText);
		break;
	}

	case CB_Post_TermState:
	{
		ASSERT(dwSize == sizeof(TSdkPostTermState));
		TSdkPostTermState * pPostTermState = (TSdkPostTermState*)pParam;

		for (int i = 0; i < g_pMainWnd->m_nTermCnt; i++)
		{
			if (g_pMainWnd->m_TermList[i].dwTermID == pPostTermState->dwTermID)
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

		int nSel = g_pMainWnd->m_cbxTermList.GetCurSel();
		if (nSel >= 0 && g_pMainWnd->m_TermList[nSel].dwTermID == pPostTermState->dwTermID)
		{
			if (pPostTermState->AlmInState & 0x1)
				g_pMainWnd->GetDlgItem(IDC_STATIC_ALM_IN_1)->SetWindowText("按键1：激活");
			else
				g_pMainWnd->GetDlgItem(IDC_STATIC_ALM_IN_1)->SetWindowText("按键1：");

			if ((pPostTermState->AlmInState >> 1) & 0x1)
				g_pMainWnd->GetDlgItem(IDC_STATIC_ALM_IN_2)->SetWindowText("按键2：激活");
			else
				g_pMainWnd->GetDlgItem(IDC_STATIC_ALM_IN_2)->SetWindowText("按键2：");

			if (pPostTermState->AlmOutState & 0x1)
				g_pMainWnd->GetDlgItem(IDC_BTN_OPEN_ALM_OUT_1)->SetWindowText(" 输出1：激活");
			else
				g_pMainWnd->GetDlgItem(IDC_BTN_OPEN_ALM_OUT_1)->SetWindowText(" 输出1：");

			if ((pPostTermState->AlmOutState >> 1) & 0x1)
				g_pMainWnd->GetDlgItem(IDC_BTN_OPEN_ALM_OUT_2)->SetWindowText(" 输出2：激活");
			else
				g_pMainWnd->GetDlgItem(IDC_BTN_OPEN_ALM_OUT_2)->SetWindowText(" 输出2：");
		}

		break;
	}

	case CB_Post_AlmNotify:
	{
		ASSERT(dwSize == sizeof(TSdkPostAlmNotify));
		TSdkPostAlmNotify * pPostAlmNotify = (TSdkPostAlmNotify*)pParam;
		CString strText;
		if ((pPostAlmNotify->eAlmType & SDK_ALMTYPE_DB_ALARM) == SDK_ALMTYPE_DB_ALARM)
			strText.Format("终端喧哗报警 !!!    终端ID:%X", pPostAlmNotify->dwTermID);
		g_pMainWnd->m_InfoList.AddString(strText);
		break;
	}

	case CB_Data_TermMp3L:
	{
		ASSERT(dwSize == sizeof(TSdkDataTermMp3L));
		TSdkDataTermMp3L * pDataTermMp3L = (TSdkDataTermMp3L*)pParam;
		//			TRACE("== MP3音频 ID:%X ：大小 %d 字节\n", pDataTermMp3L->dwTermID,pDataTermMp3L->nDataSize);
		if (g_pMp3File1 && pDataTermMp3L->dwTermID == g_pMainWnd->m_TermList[g_pMainWnd->m_nRecordTermIdx].dwTermID)
			fwrite(pDataTermMp3L->pMp3Data, 1, pDataTermMp3L->nDataSize, g_pMp3File1);
		break;
	}

	case CB_Data_TermMp3R:
	{
		ASSERT(dwSize == sizeof(TSdkDataTermMp3R));
		TSdkDataTermMp3R * pDataTermMp3R = (TSdkDataTermMp3R*)pParam;
		//			TRACE("== MP3音频 ID:%X ：大小 %d 字节\n", pDataTermMp3R->dwTermID,pDataTermMp3R->nDataSize);
		if (g_pMp3File2 && pDataTermMp3R->dwTermID == g_pMainWnd->m_TermList[g_pMainWnd->m_nRecordTermIdx].dwTermID)
			fwrite(pDataTermMp3R->pMp3Data, 1, pDataTermMp3R->nDataSize, g_pMp3File2);
		break;
	}

	case CB_Data_TermPcmL:
	{
		ASSERT(dwSize == sizeof(TSdkDataTermPcmL));
		TSdkDataTermPcmL * pDataTermPcmL = (TSdkDataTermPcmL*)pParam;
		//			TRACE("== Pcm音频 ID:%X ：大小 %d 字节\n", pDataTermPcmL->dwTermID,pDataTermPcmL->nDataSize);
		if (g_pPcmFile1 && pDataTermPcmL->dwTermID == g_pMainWnd->m_TermList[g_pMainWnd->m_nRecordTermIdx2].dwTermID)
			fwrite(pDataTermPcmL->pPcmData, 1, pDataTermPcmL->nDataSize, g_pPcmFile1);
		g_nPcmTotalLen1 += pDataTermPcmL->nDataSize;
		break;
	}

	case CB_Data_TermPcmR:
	{
		ASSERT(dwSize == sizeof(TSdkDataTermPcmR));
		TSdkDataTermPcmR * pDataTermPcmR = (TSdkDataTermPcmR*)pParam;
		//			TRACE("== Pcm音频 ID:%X ：大小 %d 字节\n", pDataTermPcmR->dwTermID,pDataTermPcmR->nDataSize);
		if (g_pPcmFile2 && pDataTermPcmR->dwTermID == g_pMainWnd->m_TermList[g_pMainWnd->m_nRecordTermIdx2].dwTermID)
			fwrite(pDataTermPcmR->pPcmData, 1, pDataTermPcmR->nDataSize, g_pPcmFile2);
		g_nPcmTotalLen2 += pDataTermPcmR->nDataSize;
		break;
	}

	case CB_Data_TermAudio:
	{
		ASSERT(dwSize == sizeof(TSdkDataTermAudio));
		TSdkDataTermAudio * pDataTermAudio = (TSdkDataTermAudio*)pParam;
		//			TRACE("== 终端音频 ID:%X ：大小 %d 字节\n", pDataTermAudio->dwTermID,pDataTermAudio->nDataSize);
		TSDK_Player_AudVidAdd(1, TSDK_AUDIO, pDataTermAudio->pAudData, pDataTermAudio->nDataSize);
		break;
	}

	case CB_Data_PcMicAudio:
	{
		ASSERT(dwSize == sizeof(TSdkDataPcMicAudio));
		TSdkDataPcMicAudio * pDataPcAudio = (TSdkDataPcMicAudio*)pParam;
		//			TRACE("== 电脑麦克风音频：大小 %d 字节\n", pDataPcAudio->nDataSize);

		if (g_pMainWnd->m_nTalkTermIdx != -1)
		{
			TSDK_Send_TalkAudio(g_pMainWnd->m_TermList[g_pMainWnd->m_nTalkTermIdx].dwTermID,
				pDataPcAudio->pPcmData, pDataPcAudio->nDataSize);
		}

		break;
	}

	case CB_Post_TermDbValL:
	{
		ASSERT(dwSize == sizeof(TSdkPostTermDbValL));
		TSdkPostTermDbValL * pPostTermDbValL = (TSdkPostTermDbValL*)pParam;
		CString strText;
		strText.Format("%d", pPostTermDbValL->nDbVal);
		g_pMainWnd->GetDlgItem(IDC_STATIC_DB_VAL_1)->SetWindowText(strText);
		break;
	}

	case CB_Post_TermDbValR:
	{
		ASSERT(dwSize == sizeof(TSdkPostTermDbValR));
		TSdkPostTermDbValR * pPostTermDbValR = (TSdkPostTermDbValR*)pParam;
		CString strText;
		strText.Format("%d", pPostTermDbValR->nDbVal);
		g_pMainWnd->GetDlgItem(IDC_STATIC_DB_VAL_2)->SetWindowText(strText);
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

void CSdkMp3Dlg::OnGetLevel()
{
	UpdateData(TRUE);

	int nSel = m_cbxTermList.GetCurSel();
	if (nSel < 0)
	{
		MessageBox("请选择终端设备！", "提示", MB_ICONWARNING);
		return;
	}

	if (m_TermState[nSel].eState == TSDK_DEV_STATE_OFFLINE)
	{
		MessageBox("设备不在线！", "提示", MB_ICONWARNING);
		return;
	}

	int nResult = TSDK_Req_GetAudLevel(m_TermList[nSel].dwTermID);
	if (nResult != CERR_SUCCESS)
	{
		CString strText;
		strText.Format("获取音量级别失败！错误代码:%d", nResult);
		MessageBox(strText, "错误", MB_ICONERROR);
	}
}

void CSdkMp3Dlg::OnSetLevel()
{
	UpdateData(TRUE);

	int nSel = m_cbxTermList.GetCurSel();
	if (nSel < 0)
	{
		MessageBox("请选择终端设备！", "提示", MB_ICONWARNING);
		return;
	}

	if (m_TermState[nSel].eState == TSDK_DEV_STATE_OFFLINE)
	{
		MessageBox("设备不在线！", "提示", MB_ICONWARNING);
		return;
	}

	if (m_nCapLevel == -1)
	{
		MessageBox("请选择音量级别！", "提示", MB_ICONWARNING);
		return;
	}

	TSdkAudioLevel Level;
	memset(&Level, 0, sizeof(TSdkAudioLevel));
	Level.eCapLevel = (enSdkAudCapLevel)m_nCapLevel;
	Level.eAmpLevel = (enSdkAudAmpLevel)m_nAmpLevel;
	Level.eCapTalkLevel = SDK_ACAP_TALK_LEVEL_2;

	int nResult = TSDK_Req_SetAudLevel(m_TermList[nSel].dwTermID, &Level);
	if (nResult != CERR_SUCCESS)
	{
		CString strText;
		strText.Format("设置音量级别失败！错误代码:%d", nResult);
		MessageBox(strText, "错误", MB_ICONERROR);
	}
}

void CSdkMp3Dlg::OnBtnOpenMp3()
{
	UpdateData(TRUE);

	CString strText, strTip;

	int nSel = m_cbxTermList.GetCurSel();
	if (nSel < 0)
	{
		MessageBox("请选择终端设备！", "提示", MB_ICONWARNING);
		return;
	}

	if (m_TermState[nSel].eState == TSDK_DEV_STATE_OFFLINE)
	{
		MessageBox("设备不在线！", "提示", MB_ICONWARNING);
		return;
	}

	if (m_nRecordTermIdx != -1)
	{
		MessageBox("操作繁忙！", "提示", MB_ICONWARNING);
		return;
	}

	m_nRecordTermIdx = nSel;

	int nResult = TSDK_Req_OpenTermAudVid(m_TermList[m_nRecordTermIdx].dwTermID);

	if (nResult != CERR_SUCCESS)
	{
		strText.Format("请求网络拾音器MP3流失败！错误代码:%d", nResult);
		MessageBox(strText, "错误", MB_ICONERROR);
	}

	strText.Format("%s%s", GetExeDir(), "1.mp3");
	g_pMp3File1 = fopen(strText, "wb");
	strTip.Format("录音文件 1: %s", strText);
	GetDlgItem(IDC_STATIC_FILE_1)->SetWindowText(strTip);

	strText.Format("%s%s", GetExeDir(), "2.mp3");
	g_pMp3File2 = fopen(strText, "wb");
	strTip.Format("录音文件 2: %s", strText);
	GetDlgItem(IDC_STATIC_FILE_2)->SetWindowText(strTip);

	GetDlgItem(IDC_BTN_OPEN_MP3)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_CLOSE_MP3)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHK_PREVIEW_L)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHK_PREVIEW_R)->EnableWindow(TRUE);
}

void CSdkMp3Dlg::OnBtnCloseMp3()
{
	UpdateData(TRUE);

	CString strText;

	if (m_nRecordTermIdx == -1)
		return;

	int nResult = TSDK_Req_CloseTermAudVid(m_TermList[m_nRecordTermIdx].dwTermID);

	if (nResult != CERR_SUCCESS)
	{
		strText.Format("关闭网络拾音器MP3流失败！错误代码:%d", nResult);
		MessageBox(strText, "错误", MB_ICONERROR);
	}

	TSDK_Req_PreviewMp3L(m_TermList[m_nRecordTermIdx].dwTermID, FALSE);
	TSDK_Req_PreviewMp3R(m_TermList[m_nRecordTermIdx].dwTermID, FALSE);

	if (g_pMp3File1)
	{
		fclose(g_pMp3File1);
		g_pMp3File1 = NULL;
	}

	if (g_pMp3File2)
	{
		fclose(g_pMp3File2);
		g_pMp3File2 = NULL;
	}

	m_bPreviewL = FALSE;
	m_bPreviewR = FALSE;

	GetDlgItem(IDC_STATIC_FILE_1)->SetWindowText("录音文件 1: ");
	GetDlgItem(IDC_STATIC_FILE_2)->SetWindowText("录音文件 2: ");

	GetDlgItem(IDC_BTN_OPEN_MP3)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_CLOSE_MP3)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHK_PREVIEW_L)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHK_PREVIEW_R)->EnableWindow(FALSE);

	m_nRecordTermIdx = -1;

	UpdateData(FALSE);
}

void CSdkMp3Dlg::OnChkPreviewL()
{
	UpdateData(TRUE);

	CString strText;

	int nSel = m_cbxTermList.GetCurSel();
	if (nSel < 0)
	{
		MessageBox("请选择终端设备！", "提示", MB_ICONWARNING);
		return;
	}

	if (m_TermState[nSel].eState == TSDK_DEV_STATE_OFFLINE)
	{
		MessageBox("设备不在线！", "提示", MB_ICONWARNING);
		return;
	}

	TSDK_Req_PreviewMp3L(m_TermList[m_nRecordTermIdx].dwTermID, m_bPreviewL);
}

void CSdkMp3Dlg::OnChkPreviewR()
{
	UpdateData(TRUE);

	CString strText;

	int nSel = m_cbxTermList.GetCurSel();
	if (nSel < 0)
	{
		MessageBox("请选择终端设备！", "提示", MB_ICONWARNING);
		return;
	}

	if (m_TermState[nSel].eState == TSDK_DEV_STATE_OFFLINE)
	{
		MessageBox("设备不在线！", "提示", MB_ICONWARNING);
		return;
	}

	TSDK_Req_PreviewMp3R(m_TermList[m_nRecordTermIdx].dwTermID, m_bPreviewR);
}

void CSdkMp3Dlg::OnBtnOpenPcm()
{
	UpdateData(TRUE);

	CString strText, strTip;

	int nSel = m_cbxTermList.GetCurSel();
	if (nSel < 0)
	{
		MessageBox("请选择终端设备！", "提示", MB_ICONWARNING);
		return;
	}

	if (m_TermState[nSel].eState == TSDK_DEV_STATE_OFFLINE)
	{
		MessageBox("设备不在线！", "提示", MB_ICONWARNING);
		return;
	}

	if (m_nRecordTermIdx2 != -1)
	{
		MessageBox("操作繁忙！", "提示", MB_ICONWARNING);
		return;
	}

	m_nRecordTermIdx2 = nSel;

	int nResult = TSDK_Req_OpenTermPcm(m_TermList[m_nRecordTermIdx2].dwTermID);

	if (nResult != CERR_SUCCESS)
	{
		strText.Format("请求网络拾音器PCM流失败！错误代码:%d", nResult);
		MessageBox(strText, "错误", MB_ICONERROR);
	}

	strText.Format("%s%s", GetExeDir(), "1.wav");
	g_pPcmFile1 = fopen(strText, "wb");
	strTip.Format("录音文件 1: %s", strText);
	GetDlgItem(IDC_STATIC_FILE_3)->SetWindowText(strTip);

	strText.Format("%s%s", GetExeDir(), "2.wav");
	g_pPcmFile2 = fopen(strText, "wb");
	strTip.Format("录音文件 2: %s", strText);
	GetDlgItem(IDC_STATIC_FILE_4)->SetWindowText(strTip);

	WAVEFORMATEX    wfx;
	memset(&wfx, 0, sizeof(WAVEFORMATEX));
	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = 1;
	wfx.nSamplesPerSec = 16000;
	wfx.wBitsPerSample = 16;
	wfx.nAvgBytesPerSec = 2 * 16000;
	wfx.nBlockAlign = 2;
	wfx.cbSize = 0;

	g_nPcmTotalLen1 = 0;
	g_nPcmTotalLen2 = 0;

	int nFileLen1 = (g_nWaveHeaderSize + 4 + 4 + g_nPcmTotalLen1 - 8);
	int nFileLen2 = (g_nWaveHeaderSize + 4 + 4 + g_nPcmTotalLen2 - 8);

	fwrite("RIFF", 1, 4, g_pPcmFile1);
	fwrite(&nFileLen1, 1, sizeof(nFileLen1), g_pPcmFile1);
	fwrite("WAVE", 1, 4, g_pPcmFile1);
	fwrite("fmt ", 1, 4, g_pPcmFile1);
	fwrite(&g_nWaveFormatSize, 1, sizeof(g_nWaveFormatSize), g_pPcmFile1);
	fwrite(&wfx, 1, g_nWaveFormatSize, g_pPcmFile1);
	fwrite("data", 1, 4, g_pPcmFile1);
	fwrite(&g_nPcmTotalLen1, 1, sizeof(g_nPcmTotalLen1), g_pPcmFile1);

	fwrite("RIFF", 1, 4, g_pPcmFile2);
	fwrite(&nFileLen2, 1, sizeof(nFileLen2), g_pPcmFile2);
	fwrite("WAVE", 1, 4, g_pPcmFile2);
	fwrite("fmt ", 1, 4, g_pPcmFile2);
	fwrite(&g_nWaveFormatSize, 1, sizeof(g_nWaveFormatSize), g_pPcmFile2);
	fwrite(&wfx, 1, g_nWaveFormatSize, g_pPcmFile2);
	fwrite("data", 1, 4, g_pPcmFile2);
	fwrite(&g_nPcmTotalLen2, 1, sizeof(g_nPcmTotalLen2), g_pPcmFile2);

	GetDlgItem(IDC_BTN_OPEN_PCM)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_CLOSE_PCM)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHK_PREVIEW_L2)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHK_PREVIEW_R2)->EnableWindow(TRUE);
}

void CSdkMp3Dlg::OnBtnClosePcm()
{
	UpdateData(TRUE);

	CString strText;

	if (m_nRecordTermIdx2 == -1)
		return;

	int nResult = TSDK_Req_CloseTermPcm(m_TermList[m_nRecordTermIdx2].dwTermID);

	if (nResult != CERR_SUCCESS)
	{
		strText.Format("关闭网络拾音器PCM流失败！错误代码:%d", nResult);
		MessageBox(strText, "错误", MB_ICONERROR);
	}

	TSDK_Req_PreviewPcmL(m_TermList[m_nRecordTermIdx2].dwTermID, FALSE);
	TSDK_Req_PreviewPcmR(m_TermList[m_nRecordTermIdx2].dwTermID, FALSE);

	Sleep(500);

	int nFileLen1 = (g_nWaveHeaderSize + 4 + 4 + g_nPcmTotalLen1 - 8);
	int nFileLen2 = (g_nWaveHeaderSize + 4 + 4 + g_nPcmTotalLen2 - 8);

	if (g_pPcmFile1)
	{
		fseek(g_pPcmFile1, 4, SEEK_SET);
		fwrite(&nFileLen1, 1, sizeof(nFileLen1), g_pPcmFile1);
		fseek(g_pPcmFile1, g_nWaveHeaderSize + 4, SEEK_SET);
		fwrite(&g_nPcmTotalLen1, 1, sizeof(g_nPcmTotalLen1), g_pPcmFile1);
		fclose(g_pPcmFile1);
		g_pPcmFile1 = NULL;
	}

	if (g_pPcmFile2)
	{
		fseek(g_pPcmFile2, 4, SEEK_SET);
		fwrite(&nFileLen2, 1, sizeof(nFileLen2), g_pPcmFile2);
		fseek(g_pPcmFile2, g_nWaveHeaderSize + 4, SEEK_SET);
		fwrite(&g_nPcmTotalLen2, 1, sizeof(g_nPcmTotalLen2), g_pPcmFile2);
		fclose(g_pPcmFile2);
		g_pPcmFile2 = NULL;
	}

	m_bPreviewL2 = FALSE;
	m_bPreviewR2 = FALSE;

	GetDlgItem(IDC_STATIC_FILE_3)->SetWindowText("录音文件 1: ");
	GetDlgItem(IDC_STATIC_FILE_4)->SetWindowText("录音文件 2: ");

	GetDlgItem(IDC_BTN_OPEN_PCM)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_CLOSE_PCM)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHK_PREVIEW_L2)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHK_PREVIEW_R2)->EnableWindow(FALSE);

	m_nRecordTermIdx2 = -1;

	UpdateData(FALSE);
}

void CSdkMp3Dlg::OnChkPreviewL2()
{
	UpdateData(TRUE);

	CString strText;

	int nSel = m_cbxTermList.GetCurSel();
	if (nSel < 0)
	{
		MessageBox("请选择终端设备！", "提示", MB_ICONWARNING);
		return;
	}

	if (m_TermState[nSel].eState == TSDK_DEV_STATE_OFFLINE)
	{
		MessageBox("设备不在线！", "提示", MB_ICONWARNING);
		return;
	}

	TSDK_Req_PreviewPcmL(m_TermList[m_nRecordTermIdx2].dwTermID, m_bPreviewL2);
}

void CSdkMp3Dlg::OnChkPreviewR2()
{
	UpdateData(TRUE);

	CString strText;

	int nSel = m_cbxTermList.GetCurSel();
	if (nSel < 0)
	{
		MessageBox("请选择终端设备！", "提示", MB_ICONWARNING);
		return;
	}

	if (m_TermState[nSel].eState == TSDK_DEV_STATE_OFFLINE)
	{
		MessageBox("设备不在线！", "提示", MB_ICONWARNING);
		return;
	}

	TSDK_Req_PreviewPcmR(m_TermList[m_nRecordTermIdx2].dwTermID, m_bPreviewR2);
}

void CSdkMp3Dlg::OnBtnOpenTalk()
{
	UpdateData(TRUE);

	CString strText, strTip;

	int nSel = m_cbxTermList.GetCurSel();
	if (nSel < 0)
	{
		MessageBox("请选择终端设备！", "提示", MB_ICONWARNING);
		return;
	}

	if (m_TermState[nSel].eState == TSDK_DEV_STATE_OFFLINE)
	{
		MessageBox("设备不在线！", "提示", MB_ICONWARNING);
		return;
	}

	if (!m_TermState[nSel].bTalkbackEnable)
	{
		MessageBox("设备不支持对讲，请先用“配置工具”开启对讲模式！", "提示", MB_ICONWARNING);
		return;
	}

	if (m_nTalkTermIdx != -1)
	{
		MessageBox("操作繁忙！", "提示", MB_ICONWARNING);
		return;
	}

	m_nTalkTermIdx = nSel;

	// 激活电平输出
	OnBtnOpenAlmOut1(); // 激活电平输出1
//	OnBtnOpenAlmOut2(); // 激活电平输出2

	//////////////////////////////////////////////////////////////////////////////////////////////////
	// 注意：正常情况打开终端对讲前，应该先检查终端是否处于对讲状态(被别人占用)(m_TermState[x].eState==TSDK_DEV_STATE_TALKING) ,
	// 如果被占用就不应该执行下面操作 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	TSDK_Player_AudVidOpen(1, m_hWnd);
	int nResult = TSDK_Req_OpenTermTalk(m_TermList[nSel].dwTermID);
	if (nResult == CERR_SUCCESS)
	{
		TSDK_Player_PcMicOpen();
	}
	else
	{
		strText.Format("请求设备对讲失败！错误代码:%d", nResult);
		MessageBox(strText, "错误", MB_ICONERROR);

		TSDK_Player_AudVidClose(1);

		m_nTalkTermIdx = -1;
	}

	///////////////////////////////////////////////////////////
	GetDlgItem(IDC_BTN_OPEN_TALK)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_OPEN_TALK_R)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_CLOSE_TALK)->EnableWindow(TRUE);

}

void CSdkMp3Dlg::OnBtnOpenTalk_R()
{
	UpdateData(TRUE);

	CString strText, strTip;

	int nSel = m_cbxTermList.GetCurSel();
	if (nSel < 0)
	{
		MessageBox("请选择终端设备！", "提示", MB_ICONWARNING);
		return;
	}

	if (m_TermState[nSel].eState == TSDK_DEV_STATE_OFFLINE)
	{
		MessageBox("设备不在线！", "提示", MB_ICONWARNING);
		return;
	}

	if (!m_TermState[nSel].bTalkbackEnable)
	{
		MessageBox("设备不支持对讲，请先用“配置工具”开启对讲模式！", "提示", MB_ICONWARNING);
		return;
	}

	if (m_nTalkTermIdx != -1)
	{
		MessageBox("操作繁忙！", "提示", MB_ICONWARNING);
		return;
	}

	m_nTalkTermIdx = nSel;

	// 激活电平输出
	OnBtnOpenAlmOut1(); // 激活电平输出1
//	OnBtnOpenAlmOut2(); // 激活电平输出2

	//////////////////////////////////////////////////////////////////////////////////////////////////
	// 注意：正常情况打开终端对讲前，应该先检查终端是否处于对讲状态(被别人占用)(m_TermState[x].eState==TSDK_DEV_STATE_TALKING) ,
	// 如果被占用就不应该执行下面操作 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	TSDK_Player_AudVidOpen(1, m_hWnd);
	int nResult = TSDK_Req_OpenTermTalk_R(m_TermList[nSel].dwTermID);
	if (nResult == CERR_SUCCESS)
	{
		TSDK_Player_PcMicOpen();
	}
	else
	{
		strText.Format("请求设备对讲失败！错误代码:%d", nResult);
		MessageBox(strText, "错误", MB_ICONERROR);

		TSDK_Player_AudVidClose(1);

		m_nTalkTermIdx = -1;
	}

	///////////////////////////////////////////////////////////
	GetDlgItem(IDC_BTN_OPEN_TALK)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_OPEN_TALK_R)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_CLOSE_TALK)->EnableWindow(TRUE);

}

void CSdkMp3Dlg::OnBtnCloseTalk()
{
	UpdateData(TRUE);

	CString strText;

	if (m_nTalkTermIdx == -1)
		return;

	TSDK_Player_PcMicClose();

	int nResult = TSDK_Req_CloseTermTalk(m_TermList[m_nTalkTermIdx].dwTermID);
	if (nResult != CERR_SUCCESS && nResult != CERR_INVALID_DEV_STATE)
	{
		strText.Format("请求关闭设备对讲失败！错误代码:%d", nResult);
		MessageBox(strText, "错误", MB_ICONERROR);
	}

	TSDK_Player_AudVidClose(1);

	m_nTalkTermIdx = -1;

	// 恢复电平输出
	OnBtnOpenAlmOut1(); // 激活电平输出1
//	OnBtnOpenAlmOut2(); // 激活电平输出2

	///////////////////////////////////////////////////////////
	GetDlgItem(IDC_BTN_OPEN_TALK)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_OPEN_TALK_R)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_CLOSE_TALK)->EnableWindow(FALSE);
}

void CSdkMp3Dlg::OnBtnOpenAlmOut1()
{
	UpdateData(TRUE);

	int nSel = m_cbxTermList.GetCurSel();
	if (nSel < 0)
	{
		MessageBox("请选择终端设备！", "提示", MB_ICONWARNING);
		return;
	}

	if (m_TermState[nSel].eState == TSDK_DEV_STATE_OFFLINE)
	{
		MessageBox("设备不在线！", "提示", MB_ICONWARNING);
		return;
	}

	int nResult = TSDK_Req_SetAlmOut(m_TermList[nSel].dwTermID, 0x1, (m_TermState[nSel].AlmOutState & 0x1) ? 0x0 : 0x1);
	if (nResult != CERR_SUCCESS)
	{
		CString strText;
		strText.Format("请求报警输出1失败！错误代码:%d", nResult);
		MessageBox(strText, "错误", MB_ICONERROR);
	}
}

void CSdkMp3Dlg::OnBtnOpenAlmOut2()
{
	UpdateData(TRUE);

	int nSel = m_cbxTermList.GetCurSel();
	if (nSel < 0)
	{
		MessageBox("请选择终端设备！", "提示", MB_ICONWARNING);
		return;
	}

	if (m_TermState[nSel].eState == TSDK_DEV_STATE_OFFLINE)
	{
		MessageBox("设备不在线！", "提示", MB_ICONWARNING);
		return;
	}

	int nResult = TSDK_Req_SetAlmOut(m_TermList[nSel].dwTermID, 0x2, (m_TermState[nSel].AlmOutState & 0x2) ? 0x0 : 0x2);
	if (nResult != CERR_SUCCESS)
	{
		CString strText;
		strText.Format("请求报警输出2失败！错误代码:%d", nResult);
		MessageBox(strText, "错误", MB_ICONERROR);
	}
}

void CSdkMp3Dlg::OnSelchangeCbxTermList()
{
	UpdateData(TRUE);

	int nSel = m_cbxTermList.GetCurSel();
	if (nSel >= 0)
	{
		if (m_TermState[nSel].AlmInState & 0x1)
			GetDlgItem(IDC_STATIC_ALM_IN_1)->SetWindowText("按键1：激活");
		else
			GetDlgItem(IDC_STATIC_ALM_IN_1)->SetWindowText("按键1：");

		if ((m_TermState[nSel].AlmInState >> 1) & 0x1)
			GetDlgItem(IDC_STATIC_ALM_IN_2)->SetWindowText("按键2：激活");
		else
			GetDlgItem(IDC_STATIC_ALM_IN_2)->SetWindowText("按键2：");

		if (m_TermState[nSel].AlmOutState & 0x1)
			GetDlgItem(IDC_BTN_OPEN_ALM_OUT_1)->SetWindowText(" 输出1：激活");
		else
			GetDlgItem(IDC_BTN_OPEN_ALM_OUT_1)->SetWindowText(" 输出1：");

		if ((m_TermState[nSel].AlmOutState >> 1) & 0x1)
			GetDlgItem(IDC_BTN_OPEN_ALM_OUT_2)->SetWindowText(" 输出2：激活");
		else
			GetDlgItem(IDC_BTN_OPEN_ALM_OUT_2)->SetWindowText(" 输出2：");
	}

	//////////////////////////////////////////////////////////////////////////////////////

	int i = m_cbxTermList.GetCurSel();
	if (i < 0)
		return;

	int nShow = m_TermState[i].bTalkbackEnable ? SW_HIDE : SW_SHOW;
	GetDlgItem(IDC_STATIC_PCM_TITLE)->ShowWindow(nShow);
	GetDlgItem(IDC_BTN_OPEN_PCM)->ShowWindow(nShow);
	GetDlgItem(IDC_BTN_CLOSE_PCM)->ShowWindow(nShow);
	GetDlgItem(IDC_STATIC_FILE_3)->ShowWindow(nShow);
	GetDlgItem(IDC_STATIC_FILE_4)->ShowWindow(nShow);
	GetDlgItem(IDC_CHK_PREVIEW_L2)->ShowWindow(nShow);
	GetDlgItem(IDC_CHK_PREVIEW_R2)->ShowWindow(nShow);
}

void CSdkMp3Dlg::OnBtnClear()
{
	m_InfoList.ResetContent();
}

void CSdkMp3Dlg::OnChkDbAnalyze()
{
	UpdateData(TRUE);

	int nSel = m_cbxTermList.GetCurSel();
	if (nSel < 0)
	{
		MessageBox("请选择终端设备！", "提示", MB_ICONWARNING);
		return;
	}

	if (m_TermState[nSel].eState == TSDK_DEV_STATE_OFFLINE)
	{
		MessageBox("设备不在线！", "提示", MB_ICONWARNING);
		return;
	}

	int nResult = TSDK_Req_DbAnalyze(m_TermList[nSel].dwTermID, m_bDbAnalyzeEnable);
	if (nResult != CERR_SUCCESS)
	{
		CString strText;
		strText.Format("请求音量分析（声道一）失败！错误代码:%d", nResult);
		MessageBox(strText, "错误", MB_ICONERROR);
	}
}

