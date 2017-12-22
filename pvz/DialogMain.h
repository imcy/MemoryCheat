#pragma once
#include "afxwin.h"


// CDialogMain 对话框

class CDialogMain : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogMain)

public:
	CDialogMain(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDialogMain();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	HICON m_hIcon;
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonSunlight();
	DWORD m_dwSunlight;
	DWORD m_dwGold;
	afx_msg void OnBnClickedButtonGold();
	virtual BOOL OnInitDialog();
	CComboBox m_comboPlants;
	CComboBox m_comboCropX;
	CComboBox m_comboCropY;
	afx_msg void OnBnClickedButtonCrop();
	CComboBox m_comboPlants2;
	// 获得被模仿植物的ID
	DWORD GetPlants2Id();
	afx_msg void OnBnClickedButtonCrop2();
	afx_msg void OnBnClickedButtonCrop3();
	afx_msg void OnBnClickedButtonRemoveplant();
	afx_msg void OnBnClickedButtonAutoCollect();
	afx_msg void OnBnClickedButtonAllZombies();
};
