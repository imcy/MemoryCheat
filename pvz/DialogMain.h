#pragma once


// CDialogMain �Ի���

class CDialogMain : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogMain)

public:
	CDialogMain(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDialogMain();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
};
