// TeachDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "AutoLinker.h"
#include "TeachDlg.h"
#include "afxdialogex.h"


// CTeachDlg 对话框

IMPLEMENT_DYNAMIC(CTeachDlg, CDialog)

CTeachDlg::CTeachDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTeachDlg::IDD, pParent)
	,m_isAboutDlg(FALSE)
{

}

CTeachDlg::~CTeachDlg()
{
}

void CTeachDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTeachDlg, CDialog)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BUTTON_ABOUT, &CTeachDlg::OnBnClickedButtonAbout)
END_MESSAGE_MAP()


// CTeachDlg 消息处理程序


BOOL CTeachDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化

	//m_isAboutDlg用于控制使用什么样的图片，按键应该怎么样：By shinn
	CDC* sysDC = GetDC();
	m_teachDC.CreateCompatibleDC(sysDC);
	m_teachBitmapAbout.LoadBitmap(IDB_BITMAP_ABOUT);
	m_teachBitmapTech.LoadBitmap(IDB_BITMAP_TEACH);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CTeachDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	if (m_isAboutDlg)
	{
		m_teachDC.SelectObject(m_teachBitmapAbout);
	}else
	{
		m_teachDC.SelectObject(m_teachBitmapTech);
	}
	
	

	dc.StretchBlt(0, 0, 600, 400, &m_teachDC, 0, 0, 600, 400, SRCCOPY);

	// 不为绘图消息调用 CDialog::OnPaint()
}


void CTeachDlg::OnBnClickedButtonAbout()
{
	//控制相应改变
	if (m_isAboutDlg)
	{
		((CButton*)GetDlgItem(IDOK))->EnableWindow(TRUE);
		((CButton*)GetDlgItem(IDOK))->ShowWindow(SW_NORMAL);
		((CButton*)GetDlgItem(IDC_BUTTON_ABOUT))->SetWindowText(TEXT("关于作者"));

		m_isAboutDlg = FALSE;
		InvalidateRect(NULL);
	}else
	{
		((CButton*)GetDlgItem(IDOK))->EnableWindow(FALSE);
		((CButton*)GetDlgItem(IDOK))->ShowWindow(SW_HIDE);
		((CButton*)GetDlgItem(IDC_BUTTON_ABOUT))->SetWindowText(TEXT("返回"));

		m_isAboutDlg = TRUE;
		InvalidateRect(NULL);
	}
}
