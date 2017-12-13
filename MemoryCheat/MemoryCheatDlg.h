
// MemoryCheatDlg.h : ͷ�ļ�
//

#pragma once
#include "MemFinder.h"
#include "afxwin.h"
#include "afxcmn.h"

// CMemoryCheatDlg �Ի���
class CMemoryCheatDlg : public CDialogEx
{
// ����
public:
	CMemoryCheatDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MEMORYCHEAT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonProgress();
public:
	/*******************************************************************************/
	// ��ʱ��ַ�б��
	enum {
		EListTempIndexAddress = 0,  // ��ַ
	};
	/*******************************************************************************/
	// Ŀ���ֹ�б����
	enum {
		EListTargetIndexDescription = 0, // ����
		EListTargetIndexAddress, // �ڴ��ַ
		EListTargetIndexType,  // ��������
		EListTargetIndexValue, // ����ֵ : ��������� �������ͱ仯
	};
	// ��ǰ���� �����Ľ���ID
	DWORD m_dwProcessId{ 0 };
	// Ҫ������ֵ
	CString m_strSearchValue;
	// ����������ָ��
	std::shared_ptr<CMemFinder>  m_pFinder{ std::make_shared<CMemFinder>() };
	afx_msg void OnBnClickedButtonFirst();
	// Ҫ������ֵ������
	CComboBox m_cbbValueType;
	CString m_strValueType;
	// �Ƿ��������
	bool m_bGoon{ false };
	/******************** ���� ********************/
	struct {
		int index;
		CString strValueType;
	} m_indexValueTypeArray[5];
	// ֵ����
	CComboBox m_cbbValueTypeEdit;
	CString m_strValueTypeEdit;
	CProgressCtrl m_pProcess;
	// ��ʱ����ֵ�б�
	CListCtrl m_lstAddressTemp;
	// Ŀ���ַ�б�
	CListCtrl m_lstAddressTarget;
public:
	void GetLimit(DWORD &dwLimitBegin, DWORD &dwLimitEnd);
	// //�״�������ʼ
	CString m_strLimitBegin;
	// �״�����������ַ
	CString m_strLimitEnd;
};
