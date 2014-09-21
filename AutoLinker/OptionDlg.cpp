// OptionDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "AutoLinker.h"
#include "OptionDlg.h"
#include "afxdialogex.h"

/*********************************/
#include "AutoLinkerDlg.h"

// COptionDlg 对话框

IMPLEMENT_DYNAMIC(COptionDlg, CDialogEx)

COptionDlg::COptionDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(COptionDlg::IDD, pParent)
	, m_slider(0)
{
}

COptionDlg::~COptionDlg()
{
}

void COptionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Slider(pDX, IDC_SLIDER_SPEED, m_slider);
}


BEGIN_MESSAGE_MAP(COptionDlg, CDialogEx)
	ON_BN_CLICKED(IDC_CHECK_RANDOM, &COptionDlg::OnBnClickedCheckRandom)
	ON_BN_CLICKED(IDC_CHECK_SKIP, &COptionDlg::OnBnClickedCheckSkip)
	ON_WM_HSCROLL()
END_MESSAGE_MAP()


// COptionDlg 消息处理程序

BOOL COptionDlg::OnInitDialog()
{
	LoadIniFile();					//加载配置文件
	CDialogEx::OnInitDialog();
	//设置滑块范围和位置
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_SPEED))->SetRange(1, 100);
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_SPEED))->SetTicFreq(10);
	//设置显示滑块位置的区域
	CString speed;
	speed.Format(TEXT("%d"), m_slider);
	SetDlgItemText(IDC_STATIC_SPEED, speed);
	//设置相应checkbox的选中状态
	if (m_teachDlg == 2)
	{
		((CButton*)GetDlgItem(IDC_CHECK_SKIP))->SetCheck(1);
	}
	if (m_randomLink == 1)
	{
		((CButton*)GetDlgItem(IDC_CHECK_RANDOM))->SetCheck(1);
		GetDlgItem(IDC_SLIDER_SPEED)->EnableWindow(FALSE);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

/**********************************************************************************
*	函数名称：LoadIniFile
*	函数功能：加载配置文件
*	函数参数：无
*	函数返回：无
*	代码编写：shinn-2011-05-22
***********************************************************************************/
void COptionDlg::LoadIniFile()
{
	//读取速度
	m_slider = ::GetPrivateProfileInt(TEXT("AutoLink"), TEXT("Speed"), 0, TEXT(".\\USER.ini"));
	if (m_slider == 0 || m_slider > 100)
	{
		CString speed;
		m_slider = 50;
		speed.Format(TEXT("%d"), m_slider);
		::WritePrivateProfileString(TEXT("AutoLink"), TEXT("Speed"), speed, TEXT(".\\USER.ini"));
	}
	//读取是否要跳过教学
	m_teachDlg = ::GetPrivateProfileInt(TEXT("Other"), TEXT("TeachDlg"), 0, TEXT(".\\USER.ini"));
	if (m_teachDlg == 0 || (m_teachDlg != 1 && m_teachDlg != 2))
	{
		CString teachDlg;
		m_teachDlg = 1;				//1为开启教学窗口
		teachDlg.Format(TEXT("%d"), m_teachDlg);
		::WritePrivateProfileString(TEXT("Other"), TEXT("TeachDlg"), teachDlg, TEXT(".\\USER.ini"));
	}
	//读取是否为随机模式
	m_randomLink = ::GetPrivateProfileInt(TEXT("AutoLink"), TEXT("RandomLink"), 0, TEXT(".\\USER.ini"));
	if (m_randomLink == 0 || (m_randomLink != 1 && m_randomLink != 2))
	{
		CString randomLink;
		m_randomLink = 2;				//2为关闭随机速度
		randomLink.Format(TEXT("%d"), m_randomLink);
		::WritePrivateProfileString(TEXT("AutoLink"), TEXT("RandomLink"), randomLink, TEXT(".\\USER.ini"));
	}
}

/**********************************************************************************
*	函数名称：OnBnClickedCheckRandom
*	函数功能：勾选或取消随机模式
*	函数参数：无
*	函数返回：无
*	代码编写：shinn-2011-05-22
***********************************************************************************/
void COptionDlg::OnBnClickedCheckRandom()
{
	if (m_randomLink == 2)
	{
		m_randomLink = 1;
		((CButton*)GetDlgItem(IDC_CHECK_RANDOM))->SetCheck(1);
		GetDlgItem(IDC_SLIDER_SPEED)->EnableWindow(FALSE);
	}else
	{
		m_randomLink = 2;
		((CButton*)GetDlgItem(IDC_CHECK_RANDOM))->SetCheck(0);
		GetDlgItem(IDC_SLIDER_SPEED)->EnableWindow(TRUE);
	}
}

/**********************************************************************************
*	函数名称：OnBnClickedCheckSkip
*	函数功能：勾选或取消跳过教学
*	函数参数：无
*	函数返回：无
*	代码编写：shinn-2011-05-22
***********************************************************************************/
void COptionDlg::OnBnClickedCheckSkip()
{
	if (m_teachDlg == 2)
	{
		m_teachDlg = 1;
		((CButton*)GetDlgItem(IDC_CHECK_SKIP))->SetCheck(0);
	}else
	{
		m_teachDlg = 2;
		((CButton*)GetDlgItem(IDC_CHECK_SKIP))->SetCheck(1);
	}
}

/**********************************************************************************
*	函数名称：OnOK
*	函数功能：重写OnOK,将相应的数据写入配置文件，并改变主窗口相关数据
*	函数参数：无
*	函数返回：无
*	代码编写：shinn-2011-05-22
***********************************************************************************/
void COptionDlg::OnOK()
{
	CString temp;
	//写入随机模式
	temp.Format(TEXT("%d"), m_randomLink);
	::WritePrivateProfileString(TEXT("AutoLink"), TEXT("RandomLink"), temp, TEXT(".\\USER.ini"));
	//写入是否要跳过教学
	temp.Format(TEXT("%d"), m_teachDlg);
	::WritePrivateProfileString(TEXT("Other"), TEXT("TeachDlg"), temp, TEXT(".\\USER.ini"));
	//写入速度
	temp.Format(TEXT("%d"), m_slider);
	::WritePrivateProfileString(TEXT("AutoLink"), TEXT("Speed"), temp, TEXT(".\\USER.ini"));
	//改变主窗口的速度及相关模式
	m_pDlg->SetOption(m_teachDlg, m_slider, m_randomLink);

	CDialogEx::OnOK();
}

/**********************************************************************************
*	函数名称：SetMainDlg
*	函数功能：传递主窗口指针，用于调用主窗口函数SetOption等
*	函数参数：pDlg：主窗口this指针
*	函数返回：无
*	代码编写：shinn-2011-05-22
***********************************************************************************/
void COptionDlg::SetMainDlg(CAutoLinkerDlg* pDlg)
{
	m_pDlg = pDlg;
}

/**********************************************************************************
*	函数名称：OnHScroll
*	函数功能：滑块位置改变时响应该消息，用于改变速度的变量
*	函数参数：无
*	函数返回：无
*	代码编写：shinn-2011-05-22
***********************************************************************************/
void COptionDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	m_slider = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER_SPEED))->GetPos();
	CString speed;
	speed.Format(TEXT("%d"), m_slider);
	SetDlgItemText(IDC_STATIC_SPEED, speed);
	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}