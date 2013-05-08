
// SQL_InjectorDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SQL_Injector.h"
#include "SQL_InjectorDlg.h"
#include "afxdialogex.h"
#include <string>
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CSQL_InjectorDlg 对话框




CSQL_InjectorDlg::CSQL_InjectorDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSQL_InjectorDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSQL_InjectorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_COOKIE, m_bInjectionCookie);
	DDX_Control(pDX, IDC_CHECK_POST, m_bInjectionGet);
	DDX_Control(pDX, IDC_CHECK_GET, m_bInjectionPost);
	DDX_Control(pDX, IDC_CHECK_IEPROXY, m_bInjectionIEProxy);
	DDX_Control(pDX, IDC_EDITURL, m_ceditURL);
	DDX_Control(pDX, IDC_EDIT_GET, m_ceditGetPara);
	DDX_Control(pDX, IDC_EDIT_POST, m_cePostPara);
	DDX_Control(pDX, IDC_EDIT_COOKIE, m_ceCookiePara);
	//DDX_Control(pDX, IDC_EDIT1, m_cdCompareString);
	DDX_Control(pDX, IDC_EDIT1, m_ceCompare);
	DDX_Control(pDX, IDC_STATIC_USER, m_csDBUser);
	DDX_Control(pDX, IDC_STATIC_DBNAME, m_csDBName);
}

BEGIN_MESSAGE_MAP(CSQL_InjectorDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_EN_CHANGE(IDC_EDIT1, &CSQL_InjectorDlg::OnEnChangeEdit1)
	ON_BN_CLICKED(IDC_BTNSCAN, &CSQL_InjectorDlg::OnBnClickedBtnscan)
END_MESSAGE_MAP()


// CSQL_InjectorDlg 消息处理程序

BOOL CSQL_InjectorDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	m_ceCompare.SetWindowText(_T("query success"));
	m_cePostPara.SetWindowText(_T("cols=100&submit=submit"));
	m_ceditURL.SetWindowText(_T("http://after1990s.info/php/Sql_inject.php"));
	m_bInjectionPost.SetCheck(true);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CSQL_InjectorDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CSQL_InjectorDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CSQL_InjectorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CSQL_InjectorDlg::OnEnChangeEdit1()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}


void CSQL_InjectorDlg::OnBnClickedBtnproxy()
{
	// TODO: Add your control notification handler code here
}


void CSQL_InjectorDlg::OnBnClickedRadio1()
{
	// TODO: Add your control notification handler code here
}


void CSQL_InjectorDlg::OnBnClickedBtnscan()
{
	// TODO: Add your control notification handler code here
	char temp[1024]={0};
	//获得对比字符串
	

	if (m_ceditURL.GetLine(0, temp, 1024) == 0)
	{
		MessageBox(_T("请输入URL"));
		return;
	}
	else
	{
		m_stringRawURL = temp;
		ZeroMemory(temp, sizeof(temp));
	}
	if ((m_bInjectionCookie.GetCheck()==BST_CHECKED || m_bInjectionGet.GetCheck()==BST_CHECKED || m_bInjectionPost.GetCheck()==BST_CHECKED) == false)
	{
		MessageBox(_T("请选择注入方式"));
		return;
	}

	if (m_bInjectionPost.GetCheck()==BST_CHECKED)
	{
		if (m_cePostPara.GetLine(0, temp, sizeof(temp)) == 0){
			MessageBox(_T("请填入Post注入参数，以&分割"));
			return;
		}
		m_stringRawPost = temp;
		ZeroMemory(temp, sizeof(temp));
	}
	if (m_bInjectionCookie.GetCheck()==BST_CHECKED)
	{
		if (m_ceCookiePara.GetLine(1, temp, sizeof(temp)) == 0){
			MessageBox(_T("请填入cookie注入参数，以&分割"));
			return;
		}
		m_stringRawCookie = temp;
		ZeroMemory(temp, sizeof(temp));
	}
	m_ceCompare.GetLine(0, temp, sizeof(temp));
	m_CompareString = temp;
	ZeroMemory(temp, sizeof(temp));
	m_InjectClass = 0;
	if (m_bInjectionCookie.GetCheck())
		m_InjectClass |= COOKIEMASK;
	if (m_bInjectionGet.GetCheck())
		m_InjectClass |= GETMASK;
	if (m_bInjectionPost.GetCheck())
		m_InjectClass |= POSTMASK;

	_beginthread(beginInjectThread, 0, (void*)this);

}
void beginInjectThread(void *p)
{/*分割参数，测试是否有注入，实施注入*/
	CSQL_InjectorDlg *dlg = (CSQL_InjectorDlg*)p;
	Injector *injector = new Injector(dlg->m_stringRawURL, dlg->m_InjectClass, dlg);
	injector->TryInject();
	
}
