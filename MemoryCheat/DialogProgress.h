#pragma once
#include "afxcmn.h"


// CDialogProgress �Ի���

class CDialogProgress : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogProgress)

public:
	CDialogProgress(CWnd* pParent = NULL);   // ��׼���캯��
	BOOL GetProcessList();
	virtual ~CDialogProgress();

// �Ի�������S
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_PROGRESS_LIST };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_lst;
	// �����б��ͼ��
	CImageList m_imgList;
	// ��ѡѡ�еĽ��̵�ID
	static DWORD m_dwProcessId;
	// ¼��ѡ�еĽ�����
	static CString m_strProcessName;
};
