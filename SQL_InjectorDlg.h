
// SQL_InjectorDlg.h : 头文件
//

#pragma once
#include "Injectior.h"
#include "afxwin.h"
#include <string>
#include "resource.h"
using namespace std;

void beginInjectThread(void *p);
// CSQL_InjectorDlg 对话框
class CSQL_InjectorDlg : public CDialogEx
{
// 构造
public:
	CSQL_InjectorDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	//           IDD_SQL_INJECTOR_DIALOG
	enum { IDD = IDD_SQL_INJECTOR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	enum INJECTCLASS{GET=0x001, POST=0x010, COOKIE=0x100};
	static const int GETMASK=0x001, POSTMASK=0x010, COOKIEMASK=0x100;
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnBnClickedBtnproxy();
	afx_msg void OnBnClickedRadio1();
	CButton m_bInjectionCookie;
	CButton m_bInjectionGet;
	CButton m_bInjectionPost;
	CButton m_bInjectionIEProxy;
	afx_msg void OnBnClickedBtnscan();

	string m_stringRawURL;//注入窗口的url值
	string m_stringRawPost;
	string m_stringRawCookie;
	CEdit m_ceditURL;
	CEdit m_ceditGetPara;
	CEdit m_cePostPara;
	CEdit m_ceCookiePara;
	CEdit m_ceCompareString;

	string m_CompareString;

	int m_InjectClass;
	CEdit m_ceCompare;
	
};
