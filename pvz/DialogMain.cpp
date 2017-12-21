// DialogMain.cpp : 实现文件
//

#include "stdafx.h"
#include "pvz.h"
#include "DialogMain.h"
#include "afxdialogex.h"


// CDialogMain 对话框

IMPLEMENT_DYNAMIC(CDialogMain, CDialogEx)

CDialogMain::CDialogMain(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG1, pParent)
{

}

CDialogMain::~CDialogMain()
{
}

void CDialogMain::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDialogMain, CDialogEx)
END_MESSAGE_MAP()


// CDialogMain 消息处理程序
