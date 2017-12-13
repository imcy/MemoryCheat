
// MemoryCheatDlg.h : 头文件
//

#pragma once
#include "MemFinder.h"
#include "afxwin.h"
#include "afxcmn.h"

// CMemoryCheatDlg 对话框
class CMemoryCheatDlg : public CDialogEx
{
// 构造
public:
	CMemoryCheatDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MEMORYCHEAT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonProgress();
public:
	/*******************************************************************************/
	// 临时地址列表框
	enum {
		EListTempIndexAddress = 0,  // 地址
	};
	/*******************************************************************************/
	// 目标地止列表框列
	enum {
		EListTargetIndexDescription = 0, // 描述
		EListTargetIndexAddress, // 内存地址
		EListTargetIndexType,  // 数据类型
		EListTargetIndexValue, // 数据值 : 根据上面的 数据类型变化
	};
	// 当前正在 操作的进程ID
	DWORD m_dwProcessId{ 0 };
	// 要搜索的值
	CString m_strSearchValue;
	// 搜索工具类指针
	std::shared_ptr<CMemFinder>  m_pFinder{ std::make_shared<CMemFinder>() };
	afx_msg void OnBnClickedButtonFirst();
	// 要搜索的值的类型
	CComboBox m_cbbValueType;
	CString m_strValueType;
	// 是否继续搜索
	bool m_bGoon{ false };
	/******************** 辅助 ********************/
	struct {
		int index;
		CString strValueType;
	} m_indexValueTypeArray[5];
	// 值类型
	CComboBox m_cbbValueTypeEdit;
	CString m_strValueTypeEdit;
	CProgressCtrl m_pProcess;
	// 临时变量值列表
	CListCtrl m_lstAddressTemp;
	// 目标地址列表
	CListCtrl m_lstAddressTarget;
public:
	void GetLimit(DWORD &dwLimitBegin, DWORD &dwLimitEnd);
	// //首次搜索开始
	CString m_strLimitBegin;
	// 首次搜索结束地址
	CString m_strLimitEnd;
};
