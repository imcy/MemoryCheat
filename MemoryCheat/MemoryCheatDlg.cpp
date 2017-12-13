
// MemoryCheatDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MemoryCheat.h"
#include "MemoryCheatDlg.h"
#include "afxdialogex.h"
#include "DialogProgress.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 回调函数 指示 是否继续 搜索
static bool *g_pGoon = nullptr;
// 进度列表的范围(0 ~ rage)
static int range = 100;

// 首次扫描回调函数:
bool __stdcall FirstSearchRoutine(void *pArgs, size_t nPageCount, size_t index)
{
	CProgressCtrl *p = (CProgressCtrl *)pArgs;
	if (nPageCount == 0) {
		return *g_pGoon;
	}

	p->SetPos(static_cast<int>(index / (nPageCount / float(range))));
	return *g_pGoon;
}

// 下次扫描回调函数
bool __stdcall NextSearchRoutine(void *pArgs, size_t addrCount, size_t index)
{
	CProgressCtrl *p = (CProgressCtrl *)pArgs;
	if (addrCount == 0) {
		return *g_pGoon;
	}

	p->SetPos(static_cast<int>(index / (addrCount / float(range))));
	return *g_pGoon;
}
// CMemoryCheatDlg 对话框
CMemoryCheatDlg::CMemoryCheatDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MEMORYCHEAT_DIALOG, pParent)
	, m_strValueType(_T(""))
	, m_strValueTypeEdit(_T(""))
	, m_strLimitBegin(_T("0x00000000"))
	, m_strLimitEnd(_T("0x7FFFffff"))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMemoryCheatDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_VALUE_TYPE, m_cbbValueType);
	DDX_CBString(pDX, IDC_COMBO_VALUE_TYPE, m_strValueType);
	DDX_Control(pDX, IDC_COMBO3, m_cbbValueTypeEdit);
	DDX_CBString(pDX, IDC_COMBO3, m_strValueTypeEdit);
	DDX_Control(pDX, IDC_PROGRESS_SEARCH, m_pProcess);
	DDX_Control(pDX, IDC_LIST_ADDRESS_TEMP, m_lstAddressTemp);
	DDX_Control(pDX, IDC_LIST_ADDRESS_TARGET, m_lstAddressTarget);
	DDX_Text(pDX, IDC_EDIT_LIMIT_START, m_strLimitBegin);
	DDX_Text(pDX, IDC_EDIT_LIMIT_END, m_strLimitEnd);
}

BEGIN_MESSAGE_MAP(CMemoryCheatDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_PROGRESS, &CMemoryCheatDlg::OnBnClickedButtonProgress)
	ON_BN_CLICKED(IDC_BUTTON_FIRST, &CMemoryCheatDlg::OnBnClickedButtonFirst)
END_MESSAGE_MAP()


// CMemoryCheatDlg 消息处理程序

BOOL CMemoryCheatDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	// 界面初始化
	{
		
		// 临时数据 列表
		{
			CListCtrl &m_lst = m_lstAddressTemp;
			LONG lStyle = GetWindowLong(m_lst.m_hWnd, GWL_STYLE);
			lStyle &= ~LVS_TYPEMASK;
			lStyle |= LVS_REPORT;
			SetWindowLong(m_lst.GetSafeHwnd(), GWL_STYLE, lStyle);
			DWORD dwStyle = m_lst.GetExtendedStyle();
			dwStyle |= LVS_EX_FULLROWSELECT; //选中行 整征高亮
			dwStyle |= LVS_EX_GRIDLINES; //网络线
			m_lst.SetExtendedStyle(dwStyle);
			// 设置列,并设置大小
			{
				CRect rc;
				m_lst.GetClientRect(rc);
				m_lst.InsertColumn(EListTempIndexAddress, _T("地址"), LVCFMT_LEFT, 80);
			}
		}
	
		// 保留 数据列表
		{
			CListCtrl &m_lst = m_lstAddressTarget;
			LONG lStyle = GetWindowLong(m_lst.m_hWnd, GWL_STYLE);
			lStyle &= ~LVS_TYPEMASK;
			lStyle |= LVS_REPORT;
			SetWindowLong(m_lst.GetSafeHwnd(), GWL_STYLE, lStyle);
			DWORD dwStyle = m_lst.GetExtendedStyle();
			dwStyle |= LVS_EX_FULLROWSELECT; //选中行 整征高亮
			dwStyle |= LVS_EX_GRIDLINES; //网络线
			m_lst.SetExtendedStyle(dwStyle);
			// 设置列,并设置大小
			{
				CRect rc;
				m_lst.GetClientRect(rc);
				m_lst.InsertColumn(EListTargetIndexDescription, _T("说明"), LVCFMT_LEFT, 80);
				m_lst.InsertColumn(EListTargetIndexAddress, _T("地址"), LVCFMT_LEFT, 80);
				m_lst.InsertColumn(EListTargetIndexType, _T("类型"), LVCFMT_LEFT, 80);
				m_lst.InsertColumn(EListTargetIndexValue, _T("值"), LVCFMT_LEFT, 80);
			}
		}
		
		// 值类型
		{
			auto &ss = m_indexValueTypeArray;
			ss[0].index = 0, ss[0].strValueType.LoadString(IDS_STRING_1BYTE);
			ss[1].index = 1, ss[1].strValueType.LoadString(IDS_STRING_2BYTE);
			ss[2].index = 2, ss[2].strValueType.LoadString(IDS_STRING_4BYTE);
			ss[3].index = 3, ss[3].strValueType.LoadString(IDS_STRING_FLOAT);
			ss[4].index = 4, ss[4].strValueType.LoadString(IDS_STRING_DOUBLE);
		}
		
		// 搜索 值类型
		{
			for (auto km : m_indexValueTypeArray) {
				m_cbbValueType.AddString(km.strValueType);
			}
			m_cbbValueType.SetCurSel(2);
		}
		
		// 编辑值类型
		{
			for (auto km : m_indexValueTypeArray) {
				m_cbbValueTypeEdit.AddString(km.strValueType);
			}
			m_cbbValueTypeEdit.SetCurSel(2);
		}
		
		// 设置搜索回调函数
		// 传送的指针为 进度条
		m_pFinder->SetCallbackFirst(FirstSearchRoutine, &m_pProcess);
		m_pFinder->SetCallbackNext(NextSearchRoutine, &m_pProcess);
	}
	
	// 启动定时器 : 每秒更新一次 目标地址框中的数据
	//SetTimer(1, 1000, nullptr);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMemoryCheatDlg::OnPaint()
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
HCURSOR CMemoryCheatDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMemoryCheatDlg::OnBnClickedButtonProgress()
{
	// 显示 进程列表对话框
	CDialogProgress dlg(this);
	if (IDOK != dlg.DoModal()) {
		// 取消的情况下,清0
		m_dwProcessId = 0;
		return;
	}
	// 获得选择的进程id
	m_dwProcessId = CDialogProgress::m_dwProcessId;
	// 把本窗口设置为 目标进程名
	SetWindowText(CDialogProgress::m_strProcessName);
	// 打开进程
	m_pFinder->OpenProcess(m_dwProcessId);
}


void CMemoryCheatDlg::OnBnClickedButtonFirst()
{
	// 如果 没有选择进程,先选择
	if (0 == m_dwProcessId) {
		OnBnClickedButtonProgress();
	}
	// 如果还是0,说明没有选择进程,直接返回
	if (0 == m_dwProcessId) {
		return;
	}

	// 回调函数中控制搜索是否继续 的标志
	m_bGoon = true;

	// 清空内容
	m_lstAddressTemp.DeleteAllItems();

	// 更新控件内容到变量
	UpdateData(TRUE);

	// 目标值
	CString sTarget;
	GetDlgItemText(IDC_EDIT_SEARCH_VALUE, sTarget);
	if (sTarget.IsEmpty()) {
		AfxMessageBox(_T("请输入搜索值"));
		// 设置 目标值 控件 获得焦点
		GetDlgItem(IDC_EDIT_SEARCH_VALUE)->SetFocus();
		UpdateData(FALSE);
		m_bGoon = false;
		return;
	}
	// 获得当前按钮
	auto pBtnFirst = (CButton *)GetDlgItem(IDC_BUTTON_FIRST);
	pBtnFirst->EnableWindow(FALSE);

	// 获取 查找范围
	DWORD dwLimitBegin, dwLimitEnd;
	GetLimit(dwLimitBegin, dwLimitEnd);

	// 获取当前选择项
	int iIdx = m_cbbValueType.GetCurSel();

	// 记录 搜索结果
	bool bFind = false;
}

// 获得 扫描范围
void CMemoryCheatDlg::GetLimit(DWORD &dwLimitBegin, DWORD &dwLimitEnd)
{
	// 获得 文本框 内容
	CString s0, s1;
	GetDlgItemText(IDC_EDIT_LIMIT_START, s0);
	GetDlgItemText(IDC_EDIT_LIMIT_END, s1);
	// 转化成 DWORD 类型
	_TCHAR *szEnd = NULL;
	dwLimitBegin = _tcstol(s0.GetString(), &szEnd, 0x10);
	dwLimitEnd = _tcstol(s1.GetString(), &szEnd, 0x10);
}