
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
	, m_strAddressEdit(_T(""))
	, m_strDesEdit(_T(""))
	, m_strValueEdit(_T(""))
	, m_strSearchValue(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	g_pGoon = &m_bGoon;
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
	DDX_Text(pDX, IDC_EDIT_ADDRESS, m_strAddressEdit);
	DDX_Text(pDX, IDC_EDIT_DES, m_strDesEdit);
	DDX_Text(pDX, IDC_EDIT_VALUE, m_strValueEdit);
	DDX_Text(pDX, IDC_EDIT_SEARCH_VALUE, m_strSearchValue);
}

BEGIN_MESSAGE_MAP(CMemoryCheatDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_PROGRESS, &CMemoryCheatDlg::OnBnClickedButtonProgress)
	ON_BN_CLICKED(IDC_BUTTON_FIRST, &CMemoryCheatDlg::OnBnClickedButtonFirst)
	ON_BN_CLICKED(IDC_BUTTON_NEXT, &CMemoryCheatDlg::OnBnClickedButtonNext)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_ADDRESS_TEMP, &CMemoryCheatDlg::OnNMDblclkListAddressTemp)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_ADDRESS_TARGET, &CMemoryCheatDlg::OnNMDblclkListAddressTarget)
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
		// 传送的函数指针为 进度条处理函数
		m_pFinder->SetCallbackFirst(FirstSearchRoutine, &m_pProcess);
		m_pFinder->SetCallbackNext(NextSearchRoutine, &m_pProcess);
	}
	
	// 启动定时器 : 每秒更新一次 目标地址框中的数据
	SetTimer(1, 1000, nullptr);

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

//添加目标地址列表数据
void CMemoryCheatDlg::AddTargetListData(const TupleTargetLineData & info)
{
	int index = m_lstAddressTarget.GetItemCount();
	// 插入新行
	index = m_lstAddressTarget.InsertItem(index, std::get<0>(info));
	// 设置地址
	SetListItemValueFormat(m_lstAddressTarget, index, EListTargetIndexAddress, _T("%08X"), std::get<1>(info));
	// 设置数据类型
	m_lstAddressTarget.SetItemText(index, EListTargetIndexType, std::get<2>(info));
	// 设置 值
	m_lstAddressTarget.SetItemText(index, EListTargetIndexValue, std::get<3>(info));
}
//更新目标地址列表数据
void CMemoryCheatDlg::UpdTargetListData(int index, const TupleTargetLineData & info)
{
	// 设置描述
	m_lstAddressTarget.SetItemText(index, EListTargetIndexDescription, std::get<0>(info));
	// 设置地址
	SetListItemValueFormat(m_lstAddressTarget, index, EListTargetIndexAddress, _T("%08X"), std::get<1>(info));
	// 设置 值类型
	m_lstAddressTarget.SetItemText(index, EListTargetIndexType, std::get<2>(info));
	// 设置 值
	m_lstAddressTarget.SetItemText(index, EListTargetIndexValue, std::get<3>(info));
}
// 获得某行数据
CMemoryCheatDlg::TupleTargetLineData CMemoryCheatDlg::GetListTargetData(int index)
{
	return std::make_tuple(
		m_lstAddressTarget.GetItemText(index, EListTargetIndexDescription),
		GetListItemValueDWORD(m_lstAddressTarget, index, EListTargetIndexAddress, 0x10),
		m_lstAddressTarget.GetItemText(index, EListTargetIndexType),
		m_lstAddressTarget.GetItemText(index, EListTargetIndexValue)
	);
}
// 更新某行数据
void CMemoryCheatDlg::UpdateTargetListValueByAddress()
{
	using namespace std;
	for (int i = 0; i < m_lstAddressTarget.GetItemCount(); ++i) {
		// 获得行数据
		TupleTargetLineData dt = GetListTargetData(i);
		// 内存地址
		DWORD dwAddr = get<1>(dt);
		CString  valueType = get<2>(dt);
		// 读取 该内存处的值
		CString value = ReadValue(valueType, dwAddr);
		// 更新本行内容:变化的只是 值
		UpdTargetListData(i, std::make_tuple(get<0>(dt), dwAddr, valueType, value));
	}
}
// 获得 dword 类型的值
DWORD CMemoryCheatDlg::GetListItemValueDWORD(CListCtrl & lst, int nItem, int nSubItem, int _Radix)
{
	CString s;
	// 获得该列的文本
	s = lst.GetItemText(nItem, nSubItem);
	// 转换成 DWORD 类型
	TCHAR *szEndPtr = nullptr;
	DWORD d = _tcstoul(s.GetString(), &szEndPtr, _Radix);
	return d;
}
// 根据传入的格式设置列 文本, 使用方法类似 printf
void CMemoryCheatDlg::SetListItemValueFormat(CListCtrl & lst, int nItem, int nSubItem, PCTSTR szFormat, ...)
{
	va_list ap;
	va_start(ap, szFormat);
	TCHAR szBuf[1024] = { 0 };
	// 将不定数量参数按照 szFormat 指定的格式,打印到 szBuf中
	_vstprintf_s(szBuf, szFormat, ap);
	// 设置列文本
	lst.SetItemText(nItem, nSubItem, szBuf);
	va_end(ap);
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
	// 根据选择的不同数据类型,进行不同方式的查找
	switch (iIdx) {
	case 0: { // 1字节
		TCHAR *szEndPtr = nullptr;
		unsigned long ul = _tcstoul(sTarget.GetString(), &szEndPtr, 10);
		BYTE bb = (BYTE)(ul & 0x000000FF);
		bFind = m_pFinder->FindFirst(m_dwProcessId, dwLimitBegin, dwLimitEnd, bb);
		break;
	}
	case 1: { // 2字节
		TCHAR *szEndPtr = nullptr;
		unsigned long ul = _tcstoul(sTarget.GetString(), &szEndPtr, 10);
		SHORT bb = (SHORT)(ul & 0x0000FFFF);
		bFind = m_pFinder->FindFirst(m_dwProcessId, dwLimitBegin, dwLimitEnd, bb);
		break;
	}
	case 2: { // 4字节
		TCHAR *szEndPtr = nullptr;
		unsigned long ul = _tcstoul(sTarget.GetString(), &szEndPtr, 10);
		DWORD bb = (DWORD)(ul & 0xFFFFFFFF);
		bFind = m_pFinder->FindFirst(m_dwProcessId, dwLimitBegin, dwLimitEnd, bb);
		break;
	}
	case 3: { // float
		TCHAR *szEndPtr = nullptr;
		float f = _tcstof(sTarget.GetString(), &szEndPtr);
		bFind = m_pFinder->FindFirst(m_dwProcessId, dwLimitBegin, dwLimitEnd, f);
		break;
	}
	case 4: { // double
		TCHAR *szEndPtr = nullptr;
		double d = _tcstod(sTarget.GetString(), &szEndPtr);
		bFind = m_pFinder->FindFirst(m_dwProcessId, dwLimitBegin, dwLimitEnd, d);
		break;
	}
	default:
		break;
	}
	// 如果 搜索结果返回 FALSE,退出
	if (!bFind) {
		goto __End;
	}

	// 设置结果内容
	{
		const std::list<DWORD> &lst = m_pFinder->GetResults();
		int index = 1024; //最多显示1024结果
		for (auto addr : lst) {
			if (index-- <= 0) {
				break;
			}
			int index = m_lstAddressTemp.InsertItem(0, _T(""));
			CString s;
			s.Format(_T("%08X"), addr);
			m_lstAddressTemp.SetItemText(index, 0, s);
		}
	}

	// 进度条清0
	m_pProcess.SetPos(0);

__End:
	// 恢复按钮 可用状态
	pBtnFirst->EnableWindow(TRUE);
	// 继续搜索标置(false)
	m_bGoon = false;
	UpdateData(FALSE);
}

void CMemoryCheatDlg::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent) {
	case 1: {
		// 更新当前地址处的值
		UpdateTargetListValueByAddress();
		break;
	}
	default:
		break;
	}
	CDialogEx::OnTimer(nIDEvent);
}

// 根据CComboBox不同选择项, 进行不同的 读取/写入内容
CString CMemoryCheatDlg::ReadValue(const CString &strValueType, DWORD dwAddr)
{
	// 遍历 数组,获得类型信息
	for (auto km : m_indexValueTypeArray) {
		if (km.strValueType == strValueType) {
			//根据类型信息和内存地址,读取数据
			return ReadValue(km.index, dwAddr);
		}
	}
	// 不会跑到这里来的.
	assert(false);
	return _T("");
}
CString CMemoryCheatDlg::ReadValue(int index, DWORD dwAddr)
{
	/*
	对于 内存地址,该处的值 会根据数据类型不同,而产生不同的内容
	CComboBox 中选择的 数据类型不同,则读取的方式不同
	如: 选择 下标为0,代表 要把此处的值 解释为 BYTE类型,只读取一字节
	下面的 switch根据不同的 下标, 调用不同模板函数
	*/
	CString strValue;
	switch (index) {
	case 0: { // 1字节
		BYTE bValue;
		if (m_pFinder->Read(dwAddr, bValue)) {
			strValue.Format(_T("%d"), bValue);
		}
		break;
	}
	case 1: { // 2字节
		SHORT bValue;
		if (m_pFinder->Read(dwAddr, bValue)) {
			strValue.Format(_T("%d"), bValue);
		}
		break;
	}
	case 2: { // 4字节
		DWORD bValue;
		if (m_pFinder->Read(dwAddr, bValue)) {
			strValue.Format(_T("%d"), bValue);
		}
		break;
	}
	case 3: { // float
		float bValue;
		if (m_pFinder->Read(dwAddr, bValue)) {
			strValue.Format(_T("%E"), bValue);
		}
		break;
	}
	case 4: {
		double bValue;
		if (m_pFinder->Read(dwAddr, bValue)) {
			strValue.Format(_T("%E"), bValue);
		}
		break;
	}
	default:
		break;
	}
	return strValue;
}
CString CMemoryCheatDlg::ReadValue(CComboBox &cbb, DWORD dwAddr)
{
	int iIdx = cbb.GetCurSel();
	return ReadValue(iIdx, dwAddr);
}
CString CMemoryCheatDlg::ReadValue(CComboBox &cbb, const CString &strAddr)
{
	TCHAR *szPtr;
	DWORD dwAddr = (DWORD)_tcstoul(strAddr.GetString(), &szPtr, 0x10);
	return ReadValue(cbb, dwAddr);
}
void CMemoryCheatDlg::WriteValue(CComboBox &cbb, const CString &strAddr, const CString &value)
{
	TCHAR *szPtr;
	DWORD dwAddr = (DWORD)_tcstoul(strAddr.GetString(), &szPtr, 0x10);
	WriteValue(cbb, dwAddr, value);
}
void CMemoryCheatDlg::WriteValue(CComboBox &cbb, DWORD dwAddr, const CString &value)
{
	/*
	对于 内存地址,该处的值 会根据数据类型不同,而产生不同的内容
	CComboBox 中选择的 数据类型不同,则写入的的方式不同(字节顺序,字节数量等)
	如: 选择 下标为0,代表 要把此处的值 解释为 BYTE类型,只需写入1字节
	下面的 switch根据不同的 下标, 调用不同模板函数
	*/
	int iIdx = cbb.GetCurSel();
	const CString sTarget = value;
	switch (iIdx) {
	case 0: { // 1字节
		TCHAR *szEndPtr = nullptr;
		unsigned long ul = _tcstoul(sTarget.GetString(), &szEndPtr, 10);
		BYTE bb = (BYTE)(ul & 0x000000FF);
		m_pFinder->Write(dwAddr, bb);
		break;
	}
	case 1: { // 2字节
		TCHAR *szEndPtr = nullptr;
		unsigned long ul = _tcstoul(sTarget.GetString(), &szEndPtr, 10);
		SHORT bb = (SHORT)(ul & 0x0000FFFF);
		m_pFinder->Write(dwAddr, bb);
		break;
	}
	case 2: { // 4字节
		TCHAR *szEndPtr = nullptr;
		unsigned long ul = _tcstoul(sTarget.GetString(), &szEndPtr, 10);
		DWORD bb = (DWORD)(ul & 0xFFFFFFFF);
		m_pFinder->Write(dwAddr, bb);
		break;
	}
	case 3: { // float
		TCHAR *szEndPtr = nullptr;
		float f = _tcstof(sTarget.GetString(), &szEndPtr);
		m_pFinder->Write(dwAddr, f);
		break;
	}
	case 4: { // double
		TCHAR *szEndPtr = nullptr;
		double d = _tcstod(sTarget.GetString(), &szEndPtr);
		m_pFinder->Write(dwAddr, d);
		break;
	}
	default:
		break;
	}

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

void CMemoryCheatDlg::OnBnClickedButtonNext()
{
	// 回调函数中控件搜索是否继续 的标志
	m_bGoon = true;
	UpdateData(TRUE);
	// 目标值
	CString sTarget;
	GetDlgItemText(IDC_EDIT_SEARCH_VALUE, sTarget);
	if (sTarget.IsEmpty()) {
		AfxMessageBox(_T("请输入目标值"));
		UpdateData(FALSE);
		m_bGoon = false;
		return;
	}

	// 获得当前按钮
	auto pBtnNext = (CButton *)GetDlgItem(IDC_BUTTON_NEXT);
	pBtnNext->EnableWindow(FALSE);

	//清空内容
	m_lstAddressTemp.DeleteAllItems();

	// 获取当前选择项
	int iIdx = m_cbbValueType.GetCurSel();

	// 记录 搜索结果
	bool bFind = false;

	// 根据选择的不同数据类型,进行不同方式的查找
	switch (iIdx) {
	case 0: { // 1字节
		TCHAR *szEndPtr = nullptr;
		unsigned long ul = _tcstoul(sTarget.GetString(), &szEndPtr, 10);
		BYTE bb = (BYTE)(ul & 0x000000FF);
		bFind = m_pFinder->FindNext(bb);
		break;
	}
	case 1: { // 2字节
		TCHAR *szEndPtr = nullptr;
		unsigned long ul = _tcstoul(sTarget.GetString(), &szEndPtr, 10);
		SHORT bb = (SHORT)(ul & 0x0000FFFF);
		bFind = m_pFinder->FindNext(bb);
		break;
	}
	case 2: { // 4字节
		TCHAR *szEndPtr = nullptr;
		unsigned long ul = _tcstoul(sTarget.GetString(), &szEndPtr, 10);
		DWORD bb = (DWORD)(ul & 0xFFFFFFFF);
		bFind = m_pFinder->FindNext(bb);
		break;
	}
	case 3: { // float
		TCHAR *szEndPtr = nullptr;
		float f = _tcstof(sTarget.GetString(), &szEndPtr);
		bFind = m_pFinder->FindNext(f);
		break;
	}
	case 4: { // double
		TCHAR *szEndPtr = nullptr;
		double d = _tcstod(sTarget.GetString(), &szEndPtr);
		bFind = m_pFinder->FindNext(d);
		break;
	}
	default:
		break;
	}

	// 如果 搜索结果返回 FALSE,退出
	if (!bFind) {
		goto __End;
	}

	// 设置结果内容
	{
		const std::list<DWORD> &lst = m_pFinder->GetResults();
		int index = 1024;
		for (auto addr : lst) {
			if (index-- <= 0) {
				break;
			}
			int index = m_lstAddressTemp.InsertItem(0, _T(""));
			CString s;
			s.Format(_T("%08X"), addr);
			m_lstAddressTemp.SetItemText(index, 0, s);
		}
	}

	// 进度条清零
	m_pProcess.SetPos(0);

__End:
	// 恢复按钮 可用状态
	pBtnNext->EnableWindow(TRUE);
	// 继续搜索标置(false)
	m_bGoon = false;
	UpdateData(FALSE);
}

// 双击临时地址列表: 把选中行的数的加入到 目标地址列表
void CMemoryCheatDlg::OnNMDblclkListAddressTemp(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	int nItem = pNMItemActivate->iItem;
	int iSubItem = pNMItemActivate->iSubItem;
	if (nItem >= 0 && iSubItem >= 0) {
		UpdateData(TRUE);
		// 获得地址
		DWORD dwAddr = GetListItemValueDWORD(m_lstAddressTemp, nItem, iSubItem, 0x10);

		// 获得值类型,和值
		CString strValue, strValueType;

		// 获取 内存内容
		strValue = ReadValue(m_cbbValueType, dwAddr);

		// 获得 值类型
		int iIdx = m_cbbValueType.GetCurSel();
		m_cbbValueType.GetLBText(iIdx, strValueType);

		// 增加到 目标地址列表框
		AddTargetListData(std::make_tuple(_T(""), dwAddr, strValueType, strValue));

		UpdateData(FALSE);
	}
	*pResult = 0;
}


void CMemoryCheatDlg::OnNMDblclkListAddressTarget(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	int nItem = pNMItemActivate->iItem;
	int iSubItem = pNMItemActivate->iSubItem;
	if (nItem >= 0 && iSubItem >= 0) {
		CListCtrl &m_lst = m_lstAddressTarget;
		// 获得列表中的选中行的各列值,并设置到编辑区
		m_strDesEdit = m_lst.GetItemText(nItem, EListTargetIndexDescription);
		m_strAddressEdit = m_lst.GetItemText(nItem, EListTargetIndexAddress);
		m_strValueTypeEdit = m_lst.GetItemText(nItem, EListTargetIndexType);
		m_strValueEdit = m_lst.GetItemText(nItem, EListTargetIndexValue);
		UpdateData(FALSE);
	}
	*pResult = 0;
}
