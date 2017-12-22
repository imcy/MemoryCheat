// DialogMain.cpp : 实现文件
//

#include "stdafx.h"
#include "pvz.h"
#include "DialogMain.h"
#include "afxdialogex.h"

// 模访者I：D
#define ID_MFZ (0x30)

// CDialogMain 对话框

IMPLEMENT_DYNAMIC(CDialogMain, CDialogEx)

CDialogMain::CDialogMain(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG1, pParent)
	, m_dwSunlight(0)
	, m_dwGold(0)
{

}

CDialogMain::~CDialogMain()
{
}

void CDialogMain::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SUNLIGHT, m_dwSunlight);
	DDX_Text(pDX, IDC_EDIT_GOLD, m_dwGold);
	DDX_Control(pDX, IDC_COMBO_PLANTS, m_comboPlants);
	DDX_Control(pDX, IDC_COMBO_CROPX, m_comboCropX);
	DDX_Control(pDX, IDC_COMBO_CROPY, m_comboCropY);
	DDX_Control(pDX, IDC_COMBO_PLANTS2, m_comboPlants2);
}


BEGIN_MESSAGE_MAP(CDialogMain, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_SUNLIGHT, &CDialogMain::OnBnClickedButtonSunlight)
	ON_BN_CLICKED(IDC_BUTTON_GOLD, &CDialogMain::OnBnClickedButtonGold)
	ON_BN_CLICKED(IDC_BUTTON_CROP, &CDialogMain::OnBnClickedButtonCrop)
	ON_BN_CLICKED(IDC_BUTTON_CROP2, &CDialogMain::OnBnClickedButtonCrop2)
	ON_BN_CLICKED(IDC_BUTTON_CROP3, &CDialogMain::OnBnClickedButtonCrop3)
	ON_BN_CLICKED(IDC_BUTTON_REMOVEPLANT, &CDialogMain::OnBnClickedButtonRemoveplant)
	ON_BN_CLICKED(IDC_BUTTON_AUTO_COLLECT, &CDialogMain::OnBnClickedButtonAutoCollect)
	ON_BN_CLICKED(IDC_BUTTON_ALL_ZOMBIES, &CDialogMain::OnBnClickedButtonAllZombies)
END_MESSAGE_MAP()


// CDialogMain 消息处理程序


void CDialogMain::OnBnClickedButtonSunlight()
{
	UpdateData(TRUE);	//更新控件上的值到关联的变量
	DWORD dwValue = m_dwSunlight;
	UpdateData(FALSE);
	//阳光数量地址
	DWORD dwTemp;
	dwTemp = *(DWORD*)0x006A9EC0;	//取一级地址中的值
	dwTemp = *(DWORD*)(0x768 + dwTemp);	//便宜0x768之后，再取出该地址的值
	//内存地址
	DWORD dwAddr = 0x5560 + dwTemp;	//这是当前阳光值得“临时”地址
	*(DWORD*)(dwAddr) = dwValue;	//写入新的阳光数值
}


void CDialogMain::OnBnClickedButtonGold()
{
	UpdateData(TRUE);	//更新控件上的值到关联的变量
	DWORD dwValue = m_dwGold;
	UpdateData(FALSE);
	//阳光数量地址
	DWORD dwTemp;
	dwTemp = *(DWORD*)0x006A9EC0;	//取一级地址中的值
	dwTemp = *(DWORD*)(0x82C + dwTemp);	//便宜0x768之后，再取出该地址的值
										//内存地址
	DWORD dwAddr = 0x28 + dwTemp;	//这是当前阳光值得“临时”地址
	*(DWORD*)(dwAddr) = dwValue;	//写入新的阳光数值
}
// 调用种植CALL
static void _CropCall(DWORD dwType, DWORD dwX, DWORD dwY, DWORD dwIDTarget = 0xFFFFffff)
{
	__try {
		__asm {
			pushad;
			pushfd;
			push dwIDTarget; // 如果模访者，则是被模访的植物的ID
			push dwType;      // 植物类型
			push dwX;      // x坐标

			mov eax, 0x006A9EC0;
			mov eax, dword ptr[eax];
			add eax, 0x768;
			mov eax, dword ptr[eax];
			push eax;

			mov eax, dwY; // y坐标

			mov esi, 0x0040D120; // 调用CALL
			call esi;

			popfd;
			popad;
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {

	}
}
static void CropCall(DWORD dwType, DWORD dwX, DWORD dwY, DWORD dwIDTarget = 0xFFFFffff)
{
	// 如果是模访者
	if (dwType == ID_MFZ) {
		_CropCall(dwType, dwX, dwY, dwIDTarget);
	}
	else {
		_CropCall(dwType, dwX, dwY);
	}
}
// 获得某位置的植物
static DWORD GetHitPlants(int row, int col)
{
	int a = row, b = col;
	DWORD dwArr[10] = { 0 };
	__asm {
		pushfd;
		pushad;

		push b;
		push a;
		mov edx, 0x6A9EC0;
		mov edx, dword ptr[edx];
		mov edx, dword ptr[edx + 0x768];
		mov esi, edx;
		lea ebx, dwArr;
		mov eax, 0x0040D2A0;
		call eax;

		popad;
		popfd;
	};
	return dwArr[3];
}

// 铲除植物
static void RemovePlant(int row, int col)
{
	__try {
		DWORD dwBase = GetHitPlants(row, col);
		if (dwBase) {
			__asm {
				pushfd;
				pushad;

				mov ebp, dwBase;
				push ebp;
				mov eax, 0x004679B0;
				call eax;

				popad;
				popfd;

			}
		}
		else {
			// 指定位置没有植物，什么也不做
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {

	}
}
// 写内存:修改代码
static bool WriteCode(DWORD dwAddr, const unsigned char code[], size_t len)
{
	DWORD dwOld;
	if (!VirtualProtect((LPVOID)dwAddr, len, PAGE_EXECUTE_READWRITE, &dwOld)) {
		return false;
	}

	memcpy_s((void *)dwAddr, len, code, len);

	DWORD dwNew;
	if (VirtualProtect((LPVOID)dwAddr, len, dwOld, &dwNew)) {
		return true;
	}
	return false;
}
BOOL CDialogMain::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);
	// TODO:  在此添加额外的初始化
	// 初始化所有植物
	{
		CString str[] = { _T(" 0:豌豆射手"), _T(" 1:向日葵"), _T(" 2:樱桃炸弹"), _T(" 3:坚果墙"),
			_T(" 4:土豆雷"), _T(" 5:寒冰射手"), _T(" 6:大嘴花"), _T(" 7:双发射手"),

			_T(" 8:小喷菇"), _T(" 9:阳光菇"), _T("10:大喷菇"), _T("11:墓碑吞噬者"),
			_T("12:魅惑菇"), _T("13:胆小菇"), _T("14:寒冰菇"), _T("15:毁灭菇"),

			_T("16:睡莲"), _T("17:窝瓜"), _T("18:三线射手"), _T("19:缠绕海草"),
			_T("20:火爆辣椒"), _T("21:地刺"), _T("22:火炬树桩"), _T("23:高坚果"),

			_T("24:海蘑菇"), _T("25:路灯花"), _T("26:仙人掌"), _T("27:三叶草"),
			_T("28:裂荚射手"), _T("29:杨桃"), _T("30:南瓜头"), _T("31:磁力菇"),

			_T("32:卷心菜投手"), _T("33:花盆"), _T("34:玉米投手"), _T("35:咖啡豆"),
			_T("36:大蒜"), _T("37:叶子保护伞"), _T("38:金盏花"), _T("39:西瓜投手"),

			_T("40:机枪射手"), _T("41:双子向日葵"), _T("42:忧郁蘑菇"), _T("43:香蒲"),
			_T("44:“冰”瓜"), _T("45:吸金磁"), _T("46:地刺王"), _T("47:玉米加农炮"),

			_T("48:模仿者"), _T("49:爆炸坚果"), _T("50:巨大坚果墙")
		};
		for (auto s : str) {
			m_comboPlants.AddString(s);
		}

		m_comboPlants.SetCurSel(0);
	}
	// 被模仿植物
	{
		CString str[] = { _T(" 0:豌豆射手"), _T(" 1:向日葵"), _T(" 2:樱桃炸弹"), _T(" 3:坚果墙"),
			_T(" 4:土豆雷"), _T(" 5:寒冰射手"), _T(" 6:大嘴花"), _T(" 7:双发射手"),

			/*_T(" 8:小喷菇"), _T(" 9:阳光菇"), _T("10:大喷菇"), _T("11:墓碑吞噬者"),
			_T("12:魅惑菇"), _T("13:胆小菇"), _T("14:寒冰菇"), _T("15:毁灭菇"),*/

			_T("16:睡莲"), _T("17:窝瓜"), _T("18:三线射手"), _T("19:缠绕海草"),
			_T("20:火爆辣椒"), _T("21:地刺"), _T("22:火炬树桩"), _T("23:高坚果"),

			/*_T("24:海蘑菇"), _T("25:路灯花"), */_T("26:仙人掌"), _T("27:三叶草"),
			_T("28:裂荚射手"), _T("29:杨桃"), _T("30:南瓜头"), /*_T("31:磁力菇"),*/

			_T("32:卷心菜投手"), /*_T("33:花盆"),*/ _T("34:玉米投手"), _T("35:咖啡豆"),
			_T("36:大蒜"), _T("37:叶子保护伞"), _T("38:金盏花"), _T("39:西瓜投手")/*,

																	_T("40:机枪射手"), _T("41:双子向日葵"), _T("42:忧郁蘑菇"), _T("43:香蒲"),
																	_T("44:“冰”瓜"), _T("45:吸金磁"), _T("46:地刺王"), _T("47:玉米加农炮"),

																	_T("48:模仿者"), _T("49:爆炸坚果"), _T("50:巨大坚果墙")*/
		};
		// 所有植物
		for (auto s : str) {
			m_comboPlants2.AddString(s);
		}
		m_comboPlants2.SetCurSel(0);
	}
	// 行:最多6行
	for (int x = 0; x <= 5; ++x) {
		CString str;
		str.Format(_T("%d"), x);
		m_comboCropX.AddString(str.GetString());
	}
	m_comboCropX.SetCurSel(0);

	// 列:最多9列
	for (int y = 0; y <= 8; ++y) {
		CString str;
		str.Format(_T("%d"), y);
		m_comboCropY.AddString(str.GetString());
	}
	m_comboCropY.SetCurSel(0);


	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CDialogMain::OnBnClickedButtonCrop()
{
	UpdateData(TRUE);
	
	if (m_comboCropY.GetCurSel() >= 0 && m_comboCropX.GetCurSel() >= 0) {
		CropCall(m_comboPlants.GetCurSel(), m_comboCropY.GetCurSel(),
			m_comboCropX.GetCurSel(), GetPlants2Id());
	}
	UpdateData(FALSE);
}

DWORD CDialogMain::GetPlants2Id()
{
	CString str;
	int index = m_comboPlants2.GetCurSel();
	m_comboPlants2.GetLBText(index, str);
	if (str.IsEmpty()) {
		return 0xFFFFffff;
	}
	DWORD dwId = 0xFFFFffff;
	{
		// 查找数字, 转成 植物ID
		int index = str.Find(_T(":"));
		if (index < 0) {
			return 0xFFFFffff;
		}
		PTSTR szEndPtr;
		dwId = _tcstol(str.Mid(0, index).GetString(), &szEndPtr, 0xA);
	}
	return dwId;
}


void CDialogMain::OnBnClickedButtonCrop2()
{
	UpdateData(TRUE);
	DWORD dwId = GetPlants2Id();
	for (DWORD col = 0; col < 9; ++col) {
		CropCall(m_comboPlants.GetCurSel(), col, m_comboCropX.GetCurSel(), dwId);
	}
	UpdateData(FALSE);
}


void CDialogMain::OnBnClickedButtonCrop3()
{
	UpdateData(TRUE);
	DWORD dwId = GetPlants2Id();
	for (DWORD row = 0; row < 5; ++row) {
		CropCall(m_comboPlants.GetCurSel(), m_comboCropY.GetCurSel(), row, dwId);
	}
	UpdateData(FALSE);
}


void CDialogMain::OnBnClickedButtonRemoveplant()
{
	RemovePlant(m_comboCropX.GetCurSel(), m_comboCropY.GetCurSel());
}


void CDialogMain::OnBnClickedButtonAutoCollect()
{
	const DWORD dwBaseAddr = 0x0043158F;

	CString str;
	GetDlgItemText(IDC_BUTTON_AUTO_COLLECT, str);
	if (str == _T("自动收集阳光金钱")) {
		// 判断是否点击阳光，金钱，
		{
			const unsigned char code[] = { 0x74, 0x08 };
			WriteCode(dwBaseAddr, code, _countof(code));
		}
		SetDlgItemText(IDC_BUTTON_AUTO_COLLECT, _T("自动收集(恢复)"));
	}
	else {
		// 判断是否点击阳光，金钱，
		{
			const unsigned char code[] = { 0x75, 0x08 };
			WriteCode(dwBaseAddr, code, _countof(code));
		}
		SetDlgItemText(IDC_BUTTON_AUTO_COLLECT, _T("自动收集阳光金钱"));
	}
}


void CDialogMain::OnBnClickedButtonAllZombies()
{
	const DWORD dwBaseAddr = 0x00413FD9;
	CString str;
	GetDlgItemText(IDC_BUTTON_ALL_ZOMBIES, str);
	if (str == _T("僵尸全部出动")) {
		{
			const unsigned char code[] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
			WriteCode(dwBaseAddr, code, _countof(code));
		}
		SetDlgItemText(IDC_BUTTON_ALL_ZOMBIES, _T("僵尸全部出动(恢复)"));
	}
	else {
		{
			const unsigned char code[] = { 0x0F, 0x85, 0x0D, 0x01, 0x00, 0x00 };
			WriteCode(dwBaseAddr, code, _countof(code));
		}
		SetDlgItemText(IDC_BUTTON_ALL_ZOMBIES, _T("僵尸全部出动"));
	}
}
