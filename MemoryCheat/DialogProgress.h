#pragma once
#include "afxcmn.h"


// CDialogProgress 对话框

class CDialogProgress : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogProgress)

public:
	CDialogProgress(CWnd* pParent = NULL);   // 标准构造函数
	BOOL GetProcessList();
	virtual ~CDialogProgress();

// 对话框数据S
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_PROGRESS_LIST };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_lst;
	// 进程列表的图标
	CImageList m_imgList;
	// 当选选中的进程的ID
	static DWORD m_dwProcessId;
	// 录脱选中的进程名
	static CString m_strProcessName;
};
