
// MemoryCheatDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MemoryCheat.h"
#include "MemoryCheatDlg.h"
#include "afxdialogex.h"
#include "DialogProgress.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// �ص����� ָʾ �Ƿ���� ����
static bool *g_pGoon = nullptr;
// �����б�ķ�Χ(0 ~ rage)
static int range = 100;

// �״�ɨ��ص�����:
bool __stdcall FirstSearchRoutine(void *pArgs, size_t nPageCount, size_t index)
{
	CProgressCtrl *p = (CProgressCtrl *)pArgs;
	if (nPageCount == 0) {
		return *g_pGoon;
	}

	p->SetPos(static_cast<int>(index / (nPageCount / float(range))));
	return *g_pGoon;
}

// �´�ɨ��ص�����
bool __stdcall NextSearchRoutine(void *pArgs, size_t addrCount, size_t index)
{
	CProgressCtrl *p = (CProgressCtrl *)pArgs;
	if (addrCount == 0) {
		return *g_pGoon;
	}

	p->SetPos(static_cast<int>(index / (addrCount / float(range))));
	return *g_pGoon;
}
// CMemoryCheatDlg �Ի���
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


// CMemoryCheatDlg ��Ϣ�������

BOOL CMemoryCheatDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	// �����ʼ��
	{
		
		// ��ʱ���� �б�
		{
			CListCtrl &m_lst = m_lstAddressTemp;
			LONG lStyle = GetWindowLong(m_lst.m_hWnd, GWL_STYLE);
			lStyle &= ~LVS_TYPEMASK;
			lStyle |= LVS_REPORT;
			SetWindowLong(m_lst.GetSafeHwnd(), GWL_STYLE, lStyle);
			DWORD dwStyle = m_lst.GetExtendedStyle();
			dwStyle |= LVS_EX_FULLROWSELECT; //ѡ���� ��������
			dwStyle |= LVS_EX_GRIDLINES; //������
			m_lst.SetExtendedStyle(dwStyle);
			// ������,�����ô�С
			{
				CRect rc;
				m_lst.GetClientRect(rc);
				m_lst.InsertColumn(EListTempIndexAddress, _T("��ַ"), LVCFMT_LEFT, 80);
			}
		}
	
		// ���� �����б�
		{
			CListCtrl &m_lst = m_lstAddressTarget;
			LONG lStyle = GetWindowLong(m_lst.m_hWnd, GWL_STYLE);
			lStyle &= ~LVS_TYPEMASK;
			lStyle |= LVS_REPORT;
			SetWindowLong(m_lst.GetSafeHwnd(), GWL_STYLE, lStyle);
			DWORD dwStyle = m_lst.GetExtendedStyle();
			dwStyle |= LVS_EX_FULLROWSELECT; //ѡ���� ��������
			dwStyle |= LVS_EX_GRIDLINES; //������
			m_lst.SetExtendedStyle(dwStyle);
			// ������,�����ô�С
			{
				CRect rc;
				m_lst.GetClientRect(rc);
				m_lst.InsertColumn(EListTargetIndexDescription, _T("˵��"), LVCFMT_LEFT, 80);
				m_lst.InsertColumn(EListTargetIndexAddress, _T("��ַ"), LVCFMT_LEFT, 80);
				m_lst.InsertColumn(EListTargetIndexType, _T("����"), LVCFMT_LEFT, 80);
				m_lst.InsertColumn(EListTargetIndexValue, _T("ֵ"), LVCFMT_LEFT, 80);
			}
		}
		
		// ֵ����
		{
			auto &ss = m_indexValueTypeArray;
			ss[0].index = 0, ss[0].strValueType.LoadString(IDS_STRING_1BYTE);
			ss[1].index = 1, ss[1].strValueType.LoadString(IDS_STRING_2BYTE);
			ss[2].index = 2, ss[2].strValueType.LoadString(IDS_STRING_4BYTE);
			ss[3].index = 3, ss[3].strValueType.LoadString(IDS_STRING_FLOAT);
			ss[4].index = 4, ss[4].strValueType.LoadString(IDS_STRING_DOUBLE);
		}
		
		// ���� ֵ����
		{
			for (auto km : m_indexValueTypeArray) {
				m_cbbValueType.AddString(km.strValueType);
			}
			m_cbbValueType.SetCurSel(2);
		}
		
		// �༭ֵ����
		{
			for (auto km : m_indexValueTypeArray) {
				m_cbbValueTypeEdit.AddString(km.strValueType);
			}
			m_cbbValueTypeEdit.SetCurSel(2);
		}
		
		// ���������ص�����
		// ���͵�ָ��Ϊ ������
		m_pFinder->SetCallbackFirst(FirstSearchRoutine, &m_pProcess);
		m_pFinder->SetCallbackNext(NextSearchRoutine, &m_pProcess);
	}
	
	// ������ʱ�� : ÿ�����һ�� Ŀ���ַ���е�����
	//SetTimer(1, 1000, nullptr);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CMemoryCheatDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CMemoryCheatDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMemoryCheatDlg::OnBnClickedButtonProgress()
{
	// ��ʾ �����б�Ի���
	CDialogProgress dlg(this);
	if (IDOK != dlg.DoModal()) {
		// ȡ���������,��0
		m_dwProcessId = 0;
		return;
	}
	// ���ѡ��Ľ���id
	m_dwProcessId = CDialogProgress::m_dwProcessId;
	// �ѱ���������Ϊ Ŀ�������
	SetWindowText(CDialogProgress::m_strProcessName);
	// �򿪽���
	m_pFinder->OpenProcess(m_dwProcessId);
}


void CMemoryCheatDlg::OnBnClickedButtonFirst()
{
	// ��� û��ѡ�����,��ѡ��
	if (0 == m_dwProcessId) {
		OnBnClickedButtonProgress();
	}
	// �������0,˵��û��ѡ�����,ֱ�ӷ���
	if (0 == m_dwProcessId) {
		return;
	}

	// �ص������п��������Ƿ���� �ı�־
	m_bGoon = true;

	// �������
	m_lstAddressTemp.DeleteAllItems();

	// ���¿ؼ����ݵ�����
	UpdateData(TRUE);

	// Ŀ��ֵ
	CString sTarget;
	GetDlgItemText(IDC_EDIT_SEARCH_VALUE, sTarget);
	if (sTarget.IsEmpty()) {
		AfxMessageBox(_T("����������ֵ"));
		// ���� Ŀ��ֵ �ؼ� ��ý���
		GetDlgItem(IDC_EDIT_SEARCH_VALUE)->SetFocus();
		UpdateData(FALSE);
		m_bGoon = false;
		return;
	}
	// ��õ�ǰ��ť
	auto pBtnFirst = (CButton *)GetDlgItem(IDC_BUTTON_FIRST);
	pBtnFirst->EnableWindow(FALSE);

	// ��ȡ ���ҷ�Χ
	DWORD dwLimitBegin, dwLimitEnd;
	GetLimit(dwLimitBegin, dwLimitEnd);

	// ��ȡ��ǰѡ����
	int iIdx = m_cbbValueType.GetCurSel();

	// ��¼ �������
	bool bFind = false;
}

// ��� ɨ�跶Χ
void CMemoryCheatDlg::GetLimit(DWORD &dwLimitBegin, DWORD &dwLimitEnd)
{
	// ��� �ı��� ����
	CString s0, s1;
	GetDlgItemText(IDC_EDIT_LIMIT_START, s0);
	GetDlgItemText(IDC_EDIT_LIMIT_END, s1);
	// ת���� DWORD ����
	_TCHAR *szEnd = NULL;
	dwLimitBegin = _tcstol(s0.GetString(), &szEnd, 0x10);
	dwLimitEnd = _tcstol(s1.GetString(), &szEnd, 0x10);
}