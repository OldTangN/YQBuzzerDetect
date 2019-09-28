// SdkNvr.h : main header file for the SDKNVR application
//

#if !defined(AFX_SDKNVR_H__A64F31B6_E1B7_47BB_95B7_CF09AE591665__INCLUDED_)
#define AFX_SDKNVR_H__A64F31B6_E1B7_47BB_95B7_CF09AE591665__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CSdkNvrApp:
// See SdkNvr.cpp for the implementation of this class
//

class CSdkNvrApp : public CWinApp
{
public:
	CSdkNvrApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSdkNvrApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CSdkNvrApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SDKNVR_H__A64F31B6_E1B7_47BB_95B7_CF09AE591665__INCLUDED_)
