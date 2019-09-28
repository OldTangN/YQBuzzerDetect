// SdkMp3Dlg.h : header file
//

#if !defined(AFX_SDKMP3DLG_H__AFF104CC_0121_4005_B49F_84DDA7897E63__INCLUDED_)
#define AFX_SDKMP3DLG_H__AFF104CC_0121_4005_B49F_84DDA7897E63__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../Include/cts_sdk.h"

typedef struct
{
	enSdkDevState eState;
	BYTE AlmInState;  // 报警输入通道状态指示码，每位对应一路报警: 0(无报警), 1(有报警)。
	BYTE AlmOutState; // 报警输出通道状态指示码，每位对应一路报警: 0(无报警), 1(有报警)。
	BOOL bTalkbackEnable;
}TDevState;

/////////////////////////////////////////////////////////////////////////////
// CSdkMp3Dlg dialog

class CSdkMp3Dlg : public CDialog
{
// Construction
public:
	CSdkMp3Dlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CSdkMp3Dlg)
	enum { IDD = IDD_SDKMP3_DIALOG };
	CComboBox	m_cbxTermList;
	CListBox	m_InfoList;
	int		m_nCapLevel;
	BOOL	m_bPreviewL;
	BOOL	m_bPreviewR;
	BOOL	m_bPreviewL2;
	BOOL	m_bPreviewR2;
	int		m_nAmpLevel;
	BOOL	m_bDbAnalyzeEnable;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSdkMp3Dlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
private:
	TSdkEventTermRegister m_TermList[MAX_TERM_COUNT];
	TDevState m_TermState[MAX_TERM_COUNT];
	int m_nTermCnt;
	int m_nRecordTermIdx;
	int m_nRecordTermIdx2;
	BOOL m_bIsBroadingVoice;
	int m_nTalkTermIdx;

	static int CALLBACK OnTzlSdkCallback(enSdkCbType eCbType, LPVOID pParam, DWORD dwSize, int usr_data);
	void UpdateTermListUI();
	CString GetExeDir();
	void LoadTermList();
	void StoreTermList();

protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CSdkMp3Dlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDestroy();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnGetLevel();
	afx_msg void OnSetLevel();
	afx_msg void OnGetDbAlm();
	afx_msg void OnBtnOpenMp3();
	afx_msg void OnBtnCloseMp3();
	afx_msg void OnChkPreviewL();
	afx_msg void OnChkPreviewR();
	afx_msg void OnBtnOpenPcm();
	afx_msg void OnBtnClosePcm();
	afx_msg void OnChkPreviewL2();
	afx_msg void OnChkPreviewR2();
	afx_msg void OnBtnOpenTalk();
	afx_msg void OnBtnCloseTalk();
	afx_msg void OnBtnOpenAlmOut1();
	afx_msg void OnBtnOpenAlmOut2();
	afx_msg void OnSelchangeCbxTermList();
	afx_msg void OnBtnClear();
	afx_msg void OnBtnOpenTalk_R();
	afx_msg void OnChkDbAnalyze();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SDKMP3DLG_H__AFF104CC_0121_4005_B49F_84DDA7897E63__INCLUDED_)
