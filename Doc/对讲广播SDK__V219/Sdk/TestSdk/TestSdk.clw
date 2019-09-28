; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CTestSdkDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "TestSdk.h"

ClassCount=3
Class1=CTestSdkApp
Class2=CTestSdkDlg
Class3=CAboutDlg

ResourceCount=3
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Resource3=IDD_TESTSDK_DIALOG

[CLS:CTestSdkApp]
Type=0
HeaderFile=TestSdk.h
ImplementationFile=TestSdk.cpp
Filter=N

[CLS:CTestSdkDlg]
Type=0
HeaderFile=TestSdkDlg.h
ImplementationFile=TestSdkDlg.cpp
Filter=D
BaseClass=CDialog
VirtualFilter=dWC

[CLS:CAboutDlg]
Type=0
HeaderFile=TestSdkDlg.h
ImplementationFile=TestSdkDlg.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_TESTSDK_DIALOG]
Type=1
Class=CTestSdkDlg
ControlCount=1
Control1=IDC_TERM_LIST,SysListView32,1350631429

