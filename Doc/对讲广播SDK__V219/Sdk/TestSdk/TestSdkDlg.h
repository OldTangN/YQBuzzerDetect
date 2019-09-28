// TestSdkDlg.h : header file
//

#if !defined(AFX_TESTSDKDLG_H__C5772887_C5BB_4189_A3F7_842295E6B63B__INCLUDED_)
#define AFX_TESTSDKDLG_H__C5772887_C5BB_4189_A3F7_842295E6B63B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../Include/cts_sdk.h"

#define MAX_TERM_CNT   500

typedef struct
{
	enSdkDevState eState;
	BYTE AlmInState;  // 报警输入通道状态指示码，每位对应一路报警: 0(无报警), 1(有报警)。
	BYTE AlmOutState; // 报警输出通道状态指示码，每位对应一路报警: 0(无报警), 1(有报警)。
}TDevState;

/////////////////////////////////////////////////////////////////////////////
// CTestSdkDlg dialog

class CTestSdkDlg : public CDialog
{
// Construction
public:
	CTestSdkDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CTestSdkDlg)
	enum { IDD = IDD_TESTSDK_DIALOG };
	CListCtrl	m_lstTerm;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestSdkDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	TSdkEventTermConnect m_TermList[MAX_TERM_CNT];
	TDevState m_TermState[MAX_TERM_COUNT];

private:
	static int CALLBACK OnTzlSdkCallback(enSdkCbType eCbType, LPVOID pParam, DWORD dwSize, int usr_data);

protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CTestSdkDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDestroy();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

extern CTestSdkDlg * g_pMainWnd;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTSDKDLG_H__C5772887_C5BB_4189_A3F7_842295E6B63B__INCLUDED_)
