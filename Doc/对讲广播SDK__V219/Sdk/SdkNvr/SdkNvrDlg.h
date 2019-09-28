// SdkNvrDlg.h : header file
//

#if !defined(AFX_SDKNVRDLG_H__AFF104CC_0121_4005_B49F_84DDA7897E63__INCLUDED_)
#define AFX_SDKNVRDLG_H__AFF104CC_0121_4005_B49F_84DDA7897E63__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../Include/cts_sdk.h"

typedef struct
{
	enSdkDevState eState;
	BYTE AlmInState;  // 报警输入通道状态指示码，每位对应一路报警: 0(无报警), 1(有报警)。
	BYTE AlmOutState; // 报警输出通道状态指示码，每位对应一路报警: 0(无报警), 1(有报警)。
}TDevState;

/////////////////////////////////////////////////////////////////////////////
// CSdkNvrDlg dialog

class CSdkNvrDlg : public CDialog
{
// Construction
public:
	CSdkNvrDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CSdkNvrDlg)
	enum { IDD = IDD_SDKNVR_DIALOG };
	CButton	m_btnPlayFile;
	CButton	m_btnPausePb;
	CButton	m_btnStopPb;
	CSliderCtrl	m_sldProgress;
	CStatic	m_stcTimeEnd;
	CStatic	m_stcTimePlay;
	CStatic	m_stcTimeBegin;
	CListCtrl	m_lstRecFile;
	CDateTimeCtrl	m_SrchFileDate;
	CListCtrl	m_ChanList;
	CComboBox	m_cbxTermList;
	CListBox	m_InfoList;
	BOOL	m_bSdkPlaySound;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSdkNvrDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
private:
	TSdkEventTermRegister m_TermList[MAX_TERM_COUNT];
	TDevState m_TermState[MAX_TERM_COUNT];
	int m_nTermCnt;
	TSdkNvrChanList m_NvrChanList;

	TSdkTime m_TTimeBeg;
	TSdkTime m_TTimeEnd;
	time_t mBeginTime;
	int mTimeLen;
	time_t mEndTime;
	int mFileSize;
	BOOL m_bIsPause;

	static int CALLBACK OnTzlSdkCallback(enSdkCbType eCbType, LPVOID pParam, DWORD dwSize, int usr_data);
	void UpdateTermListUI();
	CString GetExeDir();
	void LoadTermList();
	void StoreTermList();

protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CSdkNvrDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDestroy();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnBtnOpenChanMp3();
	afx_msg void OnBtnCloseChanMp3();
	afx_msg void OnSelchangeCbxTermList();
	afx_msg void OnBtnGetAllChan();
	afx_msg void OnBtnOpenChanPcm();
	afx_msg void OnBtnCloseChanPcm();
	afx_msg void OnBtnSearch();
	afx_msg void OnBtnPlayFile();
	afx_msg void OnBtnPause();
	afx_msg void OnBtnStop();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnReleasedcaptureSliderProgress(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SDKNVRDLG_H__AFF104CC_0121_4005_B49F_84DDA7897E63__INCLUDED_)
