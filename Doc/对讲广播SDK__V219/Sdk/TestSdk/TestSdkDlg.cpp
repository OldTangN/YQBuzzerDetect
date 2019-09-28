// TestSdkDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TestSdk.h"
#include "TestSdkDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

CTestSdkDlg * g_pMainWnd = NULL;

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
// CTestSdkDlg dialog

CTestSdkDlg::CTestSdkDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestSdkDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTestSdkDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	memset(m_TermList, 0, sizeof(m_TermList));
	memset(m_TermState, 0, sizeof(m_TermState));

	g_pMainWnd = this;
}

void CTestSdkDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTestSdkDlg)
	DDX_Control(pDX, IDC_TERM_LIST, m_lstTerm);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTestSdkDlg, CDialog)
	//{{AFX_MSG_MAP(CTestSdkDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestSdkDlg message handlers

BOOL CTestSdkDlg::OnInitDialog()
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

	DWORD dwStyle = m_lstTerm.GetExtendedStyle();
	dwStyle |= (LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	m_lstTerm.SetExtendedStyle(dwStyle);
	m_lstTerm.InsertColumn(0, _T("���"), LVCFMT_LEFT, 50);
	m_lstTerm.InsertColumn(1, _T("IP ��ַ"), LVCFMT_LEFT, 110);
	m_lstTerm.InsertColumn(2, _T("�ն�����"), LVCFMT_LEFT, 115);
	m_lstTerm.InsertColumn(3, _T("�ն�ID"), LVCFMT_LEFT, 60);
	m_lstTerm.InsertColumn(4, _T("MAC��ַ"), LVCFMT_LEFT, 130);
	m_lstTerm.InsertColumn(5, _T("�ն�����"), LVCFMT_LEFT, 160);
	m_lstTerm.InsertColumn(6, _T("�ն�״̬"), LVCFMT_LEFT, 100);

	CString strText;
	for(int i=0; i<MAX_TERM_CNT; i++)
	{
		strText.Format("%d", i+1);
		m_lstTerm.InsertItem(i, strText);
	}

	int nResult = TSDK_Init(OnTzlSdkCallback, TRUE, 0x12345678);
	if(nResult != CERR_SUCCESS)
	{
		strText.Format("SDK ��ʼ��ʧ�ܣ��������:%d", nResult);
		MessageBox(strText, "���� ������", MB_ICONERROR);
		EndDialog(IDCANCEL);
		return FALSE;
	}
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTestSdkDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CTestSdkDlg::OnPaint() 
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
HCURSOR CTestSdkDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CTestSdkDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	TSDK_DeInit();
}

void CTestSdkDlg::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}

void CTestSdkDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

static LPCSTR GetStateText(enSdkDevState eState)
{
	switch(eState)
	{
	case TSDK_DEV_STATE_OFFLINE:
		return "������";
	case TSDK_DEV_STATE_ONLINE:
		return "����";
	case TSDK_DEV_STATE_PLAYING:
		return "��������";
	case TSDK_DEV_STATE_TALKING:
		return "ͨ����";
	case TSDK_DEV_STATE_TALK_PLAY:
		return "ͨ���в���������";
	}

	return "δ֪״̬";
}

static LPCSTR GetTermType(enSdkDevType eTermType)
{
	switch(eTermType)
	{
	case TSDK_DEV_TYPE_TALK:
		return "[�Խ��㲥�ն�]";
	case TSDK_DEV_TYPE_BROAD:
		return "[�㲥�ն�]";
	case TSDK_DEV_TYPE_MP3:
		return "[����ʰ����]";
	}

	return "[δ֪�����ն�]";
}

int CALLBACK CTestSdkDlg::OnTzlSdkCallback(enSdkCbType eCbType, LPVOID pParam, DWORD dwSize, int usr_data)
{
	switch(eCbType)
	{
		case CB_Event_TermRegister:
		{
			ASSERT(dwSize == sizeof(TSdkEventTermRegister));
			TSdkEventTermRegister * pEventTermRegister = (TSdkEventTermRegister*)pParam;
			LPBYTE p = pEventTermRegister->TermMac;
			CString strText;

			for(int i=0; i<MAX_TERM_CNT; i++)
			{
				if(g_pMainWnd->m_TermList[i].dwTermID == pEventTermRegister->dwTermID)
					break;
			}

			if(i >= MAX_TERM_CNT)
			{
				for(i=0; i<MAX_TERM_CNT; i++)
				{
					if(g_pMainWnd->m_TermList[i].dwTermID == 0)
						break;
				}
			}

			if(i >= MAX_TERM_CNT)
			{
				ASSERT(FALSE);
				break;
			}

			strText.Format("%s", "0.0.0.0");
			strcpy(g_pMainWnd->m_TermList[i].TermIp, strText);
			g_pMainWnd->m_lstTerm.SetItemText(i, 1, strText);

			g_pMainWnd->m_TermList[i].eTermType = pEventTermRegister->eTermType;
			strText = GetTermType(pEventTermRegister->eTermType);
			g_pMainWnd->m_lstTerm.SetItemText(i, 2, strText);

			g_pMainWnd->m_TermList[i].dwTermID = pEventTermRegister->dwTermID;
			strText.Format("0x%X", pEventTermRegister->dwTermID);
			g_pMainWnd->m_lstTerm.SetItemText(i, 3, strText);

			memcpy(g_pMainWnd->m_TermList[i].TermMac, p, 6);
			strText.Format("%02X-%02X-%02X-%02X-%02X-%02X", p[0],p[1],p[2],p[3],p[4],p[5]);
			g_pMainWnd->m_lstTerm.SetItemText(i, 4, strText);

			strcpy(g_pMainWnd->m_TermList[i].TermName, pEventTermRegister->TermName);
			strText.Format("%s", pEventTermRegister->TermName);
			g_pMainWnd->m_lstTerm.SetItemText(i, 5, strText);

			g_pMainWnd->m_TermState[i].eState = TSDK_DEV_STATE_OFFLINE;
			strText.Format("%s", "ע��");
			g_pMainWnd->m_lstTerm.SetItemText(i, 6, strText);
			
			return CERR_SUCCESS; // ���� 0 ����ע��ɹ����������ע��ʧ�� !!!
		}

		case CB_Event_TermConnect:
		{
			ASSERT(dwSize == sizeof(TSdkEventTermConnect));
			TSdkEventTermConnect * pEventTermConnect = (TSdkEventTermConnect*)pParam;
			LPBYTE p = pEventTermConnect->TermMac;
			CString strText;
			
			for(int i=0; i<MAX_TERM_CNT; i++)
			{
				if(g_pMainWnd->m_TermList[i].dwTermID == pEventTermConnect->dwTermID)
					break;
			}

			if(i >= MAX_TERM_CNT)
			{
				for(i=0; i<MAX_TERM_CNT; i++)
				{
					if(g_pMainWnd->m_TermList[i].dwTermID == 0)
						break;
				}
			}

			if(i >= MAX_TERM_CNT)
			{
				ASSERT(FALSE);
				return -1; // ���� 0 �����������ӣ������������ !!!
			}

			strcpy(g_pMainWnd->m_TermList[i].TermIp, pEventTermConnect->TermIp);
			strText.Format("%s", pEventTermConnect->TermIp);
			g_pMainWnd->m_lstTerm.SetItemText(i, 1, strText);

			g_pMainWnd->m_TermList[i].eTermType = pEventTermConnect->eTermType;
			strText = GetTermType(pEventTermConnect->eTermType);
			g_pMainWnd->m_lstTerm.SetItemText(i, 2, strText);

			g_pMainWnd->m_TermList[i].dwTermID = pEventTermConnect->dwTermID;
			strText.Format("0x%X", pEventTermConnect->dwTermID);
			g_pMainWnd->m_lstTerm.SetItemText(i, 3, strText);

			memcpy(g_pMainWnd->m_TermList[i].TermMac, p, 6);
			strText.Format("%02X-%02X-%02X-%02X-%02X-%02X", p[0],p[1],p[2],p[3],p[4],p[5]);
			g_pMainWnd->m_lstTerm.SetItemText(i, 4, strText);

			strcpy(g_pMainWnd->m_TermList[i].TermName, pEventTermConnect->TermName);
			strText.Format("%s", pEventTermConnect->TermName);
			g_pMainWnd->m_lstTerm.SetItemText(i, 5, strText);

			g_pMainWnd->m_TermState[i].eState = TSDK_DEV_STATE_ONLINE;
			strText = GetStateText(g_pMainWnd->m_TermState[i].eState);
			g_pMainWnd->m_lstTerm.SetItemText(i, 6, strText);

			return CERR_SUCCESS; // ���� 0 �����������ӣ������������ !!!
		}

		case CB_Event_TermCnnLost:
		{
			ASSERT(dwSize == sizeof(TSdkEventTermCnnLost));
			TSdkEventTermCnnLost * pEventTermCnnLost = (TSdkEventTermCnnLost*)pParam;
			CString strText;

			for(int i=0; i<MAX_TERM_CNT; i++)
			{
				if(g_pMainWnd->m_TermList[i].dwTermID == pEventTermCnnLost->dwTermID)
					break;
			}

			if(i >= MAX_TERM_CNT)
			{
				for(i=0; i<MAX_TERM_CNT; i++)
				{
					if(g_pMainWnd->m_TermList[i].dwTermID == 0)
						break;
				}
			}

			if(i >= MAX_TERM_CNT)
			{
				ASSERT(FALSE);
				return -1;
			}

			g_pMainWnd->m_TermState[i].eState = TSDK_DEV_STATE_OFFLINE;
			strText = GetStateText(g_pMainWnd->m_TermState[i].eState);
			g_pMainWnd->m_lstTerm.SetItemText(i, 6, strText);

			break;
		}
/*		
		case CB_Asw_OpenTermAudVid:
		{
			ASSERT(dwSize == sizeof(TSdkAswOpenTermAudVid));
			TSdkAswOpenTermAudVid * pAswOpenTermAudVid = (TSdkAswOpenTermAudVid*)pParam;
			CString strText;
			if(pAswOpenTermAudVid->nResult == CERR_SUCCESS)
				strText.Format("���ն���Ƶ�ɹ����ն�ID:%X ", pAswOpenTermAudVid->dwTermID);
			else
				strText.Format("���ն���Ƶʧ�ܣ��ն�ID:%X ���������:%d", pAswOpenTermAudVid->dwTermID, pAswOpenTermAudVid->nResult);
			g_pMainWnd->m_InfoList.AddString(strText);
			break;
		}

		case CB_Asw_CloseTermAudVid:
		{
			ASSERT(dwSize == sizeof(TSdkAswCloseTermAudVid));
			TSdkAswCloseTermAudVid * pAswCloseTermAudVid = (TSdkAswCloseTermAudVid*)pParam;
			CString strText;
			if(pAswCloseTermAudVid->nResult == CERR_SUCCESS)
				strText.Format("�ر��ն���Ƶ�ɹ����ն�ID:%X ", pAswCloseTermAudVid->dwTermID);
			else
				strText.Format("�ر��ն���Ƶʧ�ܣ��ն�ID:%X ���������:%d", pAswCloseTermAudVid->dwTermID, pAswCloseTermAudVid->nResult);
			
			break;
		}

		case CB_Asw_OpenTermTalk:
		{
			ASSERT(dwSize == sizeof(TSdkAswOpenTermTalk));
			TSdkAswOpenTermTalk * pAswOpenTermTalk = (TSdkAswOpenTermTalk*)pParam;
			CString strText;
			if(pAswOpenTermTalk->nResult == CERR_SUCCESS)
				strText.Format("���ն˶Խ��ɹ����ն�ID:%X ", pAswOpenTermTalk->dwTermID);
			else
				strText.Format("���ն˶Խ�ʧ�ܣ��ն�ID:%X ���������:%d", pAswOpenTermTalk->dwTermID, pAswOpenTermTalk->nResult);
			g_pMainWnd->m_InfoList.AddString(strText);
			break;
		}

		case CB_Asw_CloseTermTalk:
		{
			ASSERT(dwSize == sizeof(TSdkAswCloseTermTalk));
			TSdkAswCloseTermTalk * pAswCloseTermTalk = (TSdkAswCloseTermTalk*)pParam;
			CString strText;
			if(pAswCloseTermTalk->nResult == CERR_SUCCESS)
				strText.Format("�ر��ն˶Խ��ɹ����ն�ID:%X ", pAswCloseTermTalk->dwTermID);
			else
				strText.Format("�ر��ն˶Խ�ʧ�ܣ��ն�ID:%X ���������:%d", pAswCloseTermTalk->dwTermID, pAswCloseTermTalk->nResult);
			g_pMainWnd->m_InfoList.AddString(strText);
			break;
		}

		case CB_Asw_GetAudLevel:
		{
			ASSERT(dwSize == sizeof(TSdkAswGetAudLevel));
			TSdkAswGetAudLevel * pAswGetAudLevel = (TSdkAswGetAudLevel*)pParam;
			g_pMainWnd->m_nCapLevel = pAswGetAudLevel->Level.eCapLevel;
			g_pMainWnd->m_nAmpLevel = pAswGetAudLevel->Level.eAmpLevel;
			g_pMainWnd->m_nCapTalkLevel = pAswGetAudLevel->Level.eCapTalkLevel;
			((CComboBox*)g_pMainWnd->GetDlgItem(IDC_CMB_CAP))->SetCurSel(g_pMainWnd->m_nCapLevel);
			((CComboBox*)g_pMainWnd->GetDlgItem(IDC_CMB_AMP))->SetCurSel(g_pMainWnd->m_nAmpLevel);
			((CComboBox*)g_pMainWnd->GetDlgItem(IDC_CMB_CAP_TALK))->SetCurSel(g_pMainWnd->m_nCapTalkLevel);
			g_pMainWnd->m_AmpSlider.SetPos(pAswGetAudLevel->nAmpValue);
			CString strText;
			strText.Format("��ǰ:%d", pAswGetAudLevel->nAmpValue);
			g_pMainWnd->GetDlgItem(IDC_STATIC_CUR)->SetWindowText(strText);
			break;
		}

		case CB_Asw_SetAudLevel:
		{
			ASSERT(dwSize == sizeof(TSdkAswSetAudLevel));
			TSdkAswSetAudLevel * pAswSetAudLevel = (TSdkAswSetAudLevel*)pParam;
			CString strText;
			if(pAswSetAudLevel->nResult == CERR_SUCCESS)
			{
				strText.Format("�����ն������ɹ����ն�ID:%X ", pAswSetAudLevel->dwTermID);

				if(pAswSetAudLevel->bIsEx)
				{
					g_pMainWnd->m_nAmpLevel = pAswSetAudLevel->eAmpLevel;
					((CComboBox*)g_pMainWnd->GetDlgItem(IDC_CMB_AMP))->SetCurSel(g_pMainWnd->m_nAmpLevel);
				}
				else
				{
					g_pMainWnd->m_AmpSlider.SetPos(pAswSetAudLevel->nAmpValue);
					CString strText;
					strText.Format("��ǰ:%d", pAswSetAudLevel->nAmpValue);
					g_pMainWnd->GetDlgItem(IDC_STATIC_CUR)->SetWindowText(strText);
				}
			}
			else
			{
				strText.Format("�����ն�����ʧ�ܣ��ն�ID:%X ���������:%d", pAswSetAudLevel->dwTermID, pAswSetAudLevel->nResult);
			}
			g_pMainWnd->m_InfoList.AddString(strText);
			break;
		}

		case CB_Asw_DismissTerm:
		{
			ASSERT(dwSize == sizeof(TSdkAswDismissTerm));
			TSdkAswDismissTerm * pAswDismissTerm = (TSdkAswDismissTerm*)pParam;
			CString strText;
			if(pAswDismissTerm->nResult == CERR_SUCCESS)
			{
				g_pMainWnd->m_stcTips.SetWindowText("������ʾ��");
				strText.Format("������гɹ����ն�ID:%X ", pAswDismissTerm->dwTermID);
			}
			else
			{
				strText.Format("�������ʧ�ܣ��ն�ID:%X ���������:%d", pAswDismissTerm->dwTermID, pAswDismissTerm->nResult);
			}
			g_pMainWnd->m_InfoList.AddString(strText);
			break;
		}

		case CB_Asw_GetDbAlmCfg:
		{
			ASSERT(dwSize == sizeof(TSdkAswGetDbAlmCfg));
			TSdkAswGetDbAlmCfg * pAswGetDbAlmCfg = (TSdkAswGetDbAlmCfg*)pParam;
			g_pMainWnd->m_bDbAlarm = pAswGetDbAlmCfg->Cfg.bDbAlmEnable;
			((CButton*)g_pMainWnd->GetDlgItem(IDC_CHK_DB_ALARM))->SetCheck(pAswGetDbAlmCfg->Cfg.bDbAlmEnable);
			g_pMainWnd->m_DbValSpin.SetPos(pAswGetDbAlmCfg->Cfg.nDbAlmVal);
			break;
		}

		case CB_Asw_SetDbAlmCfg:
		{
			ASSERT(dwSize == sizeof(TSdkAswSetDbAlmCfg));
			TSdkAswSetDbAlmCfg * pAswSetDbAlmCfg = (TSdkAswSetDbAlmCfg*)pParam;
			CString strText;
			if(pAswSetDbAlmCfg->nResult == CERR_SUCCESS)
				strText.Format("�޸��ն����óɹ����ն�ID:%X ", pAswSetDbAlmCfg->dwTermID);
			else
				strText.Format("�޸��ն�����ʧ�ܣ��ն�ID:%X ���������:%d", pAswSetDbAlmCfg->dwTermID, pAswSetDbAlmCfg->nResult);
			g_pMainWnd->m_InfoList.AddString(strText);
			break;
		}

		case CB_Post_TermSos:
		{
			ASSERT(dwSize == sizeof(TSdkPostTermSos));
			TSdkPostTermSos * pPostTermSos = (TSdkPostTermSos*)pParam;
			CString strText;
			if(pPostTermSos->nSosType == 1)
				strText.Format("�ն˽������� !!!!!! \n�ն�ID:%X", pPostTermSos->dwTermID);
			else
				strText.Format("�ն���ѯ���� !!!!!! \n�ն�ID:%X", pPostTermSos->dwTermID);
			g_pMainWnd->m_InfoList.AddString(strText);
			g_pMainWnd->m_stcTips.SetWindowText(strText);

			for(int i=0; i<g_pMainWnd->m_nTermCnt; i++)
			{
				if(g_pMainWnd->m_TermList[i].dwTermID == pPostTermSos->dwTermID)
				{
					g_pMainWnd->m_cbxTermList.SetCurSel(i);
					break;
				}
			}			

			break;
		}
*/
		case CB_Post_TermState:
		{
			ASSERT(dwSize == sizeof(TSdkPostTermState));
			TSdkPostTermState * pPostTermState = (TSdkPostTermState*)pParam;
			CString strText;

			for(int i=0; i<MAX_TERM_CNT; i++)
			{
				if(g_pMainWnd->m_TermList[i].dwTermID == pPostTermState->dwTermID)
					break;
			}

			if(i >= MAX_TERM_CNT)
			{
				for(i=0; i<MAX_TERM_CNT; i++)
				{
					if(g_pMainWnd->m_TermList[i].dwTermID == 0)
						break;
				}
			}

			if(i >= MAX_TERM_CNT)
			{
				ASSERT(FALSE);
				return -1;
			}

			g_pMainWnd->m_TermState[i].eState = pPostTermState->eTermState;
			strText = GetStateText(g_pMainWnd->m_TermState[i].eState);
			g_pMainWnd->m_lstTerm.SetItemText(i, 6, strText);

			break;
		}
/*
		case CB_Post_485PipeData:
		{
			ASSERT(dwSize == sizeof(TSdkPost485PipeData));
			TSdkPost485PipeData * pPost485PipeData = (TSdkPost485PipeData*)pParam;
			CString strText;
			strText.Format("===> �յ�485���ݣ��ն�ID:%X , ����:%d , ����:%s",
				pPost485PipeData->dwTermID, pPost485PipeData->nFrmLen, pPost485PipeData->FrmData);
			g_pMainWnd->m_InfoList.AddString(strText);

			g_pMainWnd->m_str485PipeData += pPost485PipeData->FrmData;
			g_pMainWnd->m_str485PipeData += "\r\n";
			g_pMainWnd->m_edt485PipeData.SetWindowText(g_pMainWnd->m_str485PipeData);
			break;
		}

		case CB_Post_Mp3PlayFinish:
		{
			ASSERT(dwSize == sizeof(TSdkPostMp3PlayFinish));
			TSdkPostMp3PlayFinish * pPostMp3PlayFinish = (TSdkPostMp3PlayFinish*)pParam;
			CString strText;
			if(pPostMp3PlayFinish->nResult == CERR_SUCCESS)
				strText.Format("����%d : MP3�ļ����ţ�������������", pPostMp3PlayFinish->nBroadNum);
			else
				strText.Format("����%d : MP3�ļ����ţ����ų����������:%d", pPostMp3PlayFinish->nBroadNum, pPostMp3PlayFinish->nResult);
			g_pMainWnd->m_InfoList.AddString(strText);
			break;
		}

		case CB_Post_AlmNotify:
		{
			ASSERT(dwSize == sizeof(TSdkPostAlmNotify));
			TSdkPostAlmNotify * pPostAlmNotify = (TSdkPostAlmNotify*)pParam;
			CString strText;
			if((pPostAlmNotify->eAlmType & SDK_ALMTYPE_DB_ALARM) == SDK_ALMTYPE_DB_ALARM)
				strText.Format("�ն��������� !!!    �ն�ID:%X", pPostAlmNotify->dwTermID);
			else
				strText.Format("�ն˱�������: 0x%X  !!!    �ն�ID:%X", pPostAlmNotify->eAlmType);
			g_pMainWnd->m_InfoList.AddString(strText);
			break;
		}

		case CB_Data_TermAudio:
		{
			ASSERT(dwSize == sizeof(TSdkDataTermAudio));
			TSdkDataTermAudio * pDataTermAudio = (TSdkDataTermAudio*)pParam;
//			TRACE("== �ն���Ƶ ID:%X ����С %d �ֽ�\n", pDataTermAudio->dwTermID,pDataTermAudio->nDataSize);
			TSDK_Player_AudVidAdd(1, TSDK_AUDIO, pDataTermAudio->pAudData, pDataTermAudio->nDataSize);
			break;
		}

		case CB_Data_TermVideo:
		{
			ASSERT(dwSize == sizeof(TSdkDataTermVideo));
			TSdkDataTermVideo * pDataTermVideo = (TSdkDataTermVideo*)pParam;
			if(pDataTermVideo->bIsKeyFrm)
				TRACE("== �ն�I֡��Ƶ ID:%X ����С %d �ֽ�\n", pDataTermVideo->dwTermID,pDataTermVideo->nFrmSize);
//			else
//				TRACE("== �ն�P֡��Ƶ ID:%X ����С %d �ֽ�\n", pDataTermVideo->dwTermID,pDataTermVideo->nFrmSize);

			TSDK_Player_AudVidAdd(1, TSDK_VIDEO, pDataTermVideo->pVidFrm, pDataTermVideo->nFrmSize);
			break;
		}

		case CB_Data_PcMicAudio:
		{
			ASSERT(dwSize == sizeof(TSdkDataPcMicAudio));
			TSdkDataPcMicAudio * pDataPcAudio = (TSdkDataPcMicAudio*)pParam;
//			TRACE("== ������˷���Ƶ����С %d �ֽ�\n", pDataPcAudio->nDataSize);

			if(g_pMainWnd->m_nTalkTermIdx != -1)
			{
				TSDK_Send_TalkAudio(g_pMainWnd->m_TermList[g_pMainWnd->m_nTalkTermIdx].dwTermID,
					pDataPcAudio->pPcmData, pDataPcAudio->nDataSize);
			}

			if(g_pMainWnd->m_bIsBroadingVoice)
			{
				TSDK_BroadPcmVoice(g_MyGroupNum, pDataPcAudio->pPcmData, pDataPcAudio->nDataSize);

				/////////////////////////////////////
				// ����ж������
				TSDK_BroadPcmVoice(xx, pDataPcAudio->pPcmData, pDataPcAudio->nDataSize);
				TSDK_BroadPcmVoice(xxx, pDataPcAudio->pPcmData, pDataPcAudio->nDataSize);
				.......................
				.......................
				/////////////////////////////////////
			}
			break;
		}

		case CB_Data_BypassAudio:
		{
			ASSERT(dwSize == sizeof(TSdkDataBypassAudio));
			TSdkDataBypassAudio * pDataBypassAudio = (TSdkDataBypassAudio*)pParam;
//			TRACE("== ��·��Ƶ ID:%X ����С %d �ֽ�\n", pDataBypassAudio->dwTermID,pDataBypassAudio->nDataSize);
			TSDK_Player_PcmAdd(1, pDataBypassAudio->pPcmData, pDataBypassAudio->nDataSize);
			break;
		}

		case CB_Data_TermMp3L:
		{
			ASSERT(dwSize == sizeof(TSdkDataTermMp3L));
			TSdkDataTermMp3L * pDataTermMp3L = (TSdkDataTermMp3L*)pParam;
//			TRACE("== MP3��Ƶ ID:%X ����С %d �ֽ�\n", pDataTermMp3L->dwTermID,pDataTermMp3L->nDataSize);
			if(g_pMp3File1)
				fwrite(pDataTermMp3L->pMp3Data, 1, pDataTermMp3L->nDataSize, g_pMp3File1);
			break;
		}

		case CB_Data_TermMp3R:
		{
			ASSERT(dwSize == sizeof(TSdkDataTermMp3R));
			TSdkDataTermMp3R * pDataTermMp3R = (TSdkDataTermMp3R*)pParam;
//			TRACE("== MP3��Ƶ ID:%X ����С %d �ֽ�\n", pDataTermMp3R->dwTermID,pDataTermMp3R->nDataSize);
			if(g_pMp3File2)
				fwrite(pDataTermMp3R->pMp3Data, 1, pDataTermMp3R->nDataSize, g_pMp3File2);
			break;
		}
*/
		default:
		{
//			ASSERT(FALSE);
			break;
		}
	}

	return CERR_SUCCESS;
}

