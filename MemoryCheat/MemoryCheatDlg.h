
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
	// Ŀ���ַ�б�� ÿ������
	typedef std::tuple<CString, DWORD, CString, CString> TupleTargetLineData;
	// ��������
	void AddTargetListData(const TupleTargetLineData &info);
	// ��������
	void UpdTargetListData(int index, const TupleTargetLineData &info);
	// ���ĳ������
	TupleTargetLineData GetListTargetData(int index);


	/*******************************************************************************/
	// ��� dword ���͵�ֵ
	DWORD GetListItemValueDWORD(CListCtrl &lst, int nItem, int nSubItem, int _Radix = 0x10);
	// ����ֵ
	void SetListItemValueFormat(CListCtrl &lst, int nItem, int nSubItem, PCTSTR szFormat, ...);

	// ��ǰ���� �����Ľ���ID
	DWORD m_dwProcessId{ 0 };
	// ����������ָ��
	std::shared_ptr<CMemFinder>  m_pFinder{ std::make_shared<CMemFinder>() };
	afx_msg void OnBnClickedButtonFirst();
	// ��ʱ��, ��ʱ���� Ŀ���б�����г����ڴ��ַ����ֵ
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	// �����б���е�ֵ
	void UpdateTargetListValueByAddress();
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

	// ����CComboBox��ͬѡ����, ���в�ͬ�� ��ȡ/д������
	CString ReadValue(int index, DWORD dwAddr);
	CString ReadValue(const CString &strValueType, DWORD dwAddr);
	CString ReadValue(CComboBox &cbb, DWORD dwAddr);
	void WriteValue(CComboBox &cbb, DWORD dwAddr, const CString &value);
	CString ReadValue(CComboBox &cbb, const CString &strAddr);
	void WriteValue(CComboBox &cbb, const CString &strAddr, const CString &value);
	
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
	afx_msg void OnBnClickedButtonNext();
	afx_msg void OnNMDblclkListAddressTemp(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkListAddressTarget(NMHDR *pNMHDR, LRESULT *pResult);
	// ��ַ
	CString m_strAddressEdit;
	// ����
	CString m_strDesEdit;
	// ֵ
	CString m_strValueEdit;
	//Ҫ������ֵ
	CString m_strSearchValue;
};
