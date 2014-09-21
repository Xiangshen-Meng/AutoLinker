// ErrorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AutoLinker.h"
#include "ErrorDlg.h"
#include "afxdialogex.h"


// CErrorDlg dialog

IMPLEMENT_DYNAMIC(CErrorDlg, CDialogEx)

CErrorDlg::CErrorDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CErrorDlg::IDD, pParent)
	,m_errorType(ErrorDlg)
{

}

CErrorDlg::~CErrorDlg()
{
}

void CErrorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CErrorDlg, CDialogEx)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CErrorDlg message handlers


BOOL CErrorDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	m_errorBitmapNoWnd.LoadBitmap(IDB_BITMAP_ERROR);
	m_errorBitmapKill.LoadBitmap(IDB_BITMAP_ALLLINK);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CErrorDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	
	CDC* sysDC = GetDC();
	m_errorDC.CreateCompatibleDC(sysDC);

	if (m_errorType == ErrorDlg)
	{
		m_errorDC.SelectObject(m_errorBitmapNoWnd);
	}else
	{
		m_errorDC.SelectObject(m_errorBitmapKill);
	}



	
	dc.BitBlt(0, 0, 200, 150, &m_errorDC, 0, 0, SRCCOPY);
	// Do not call CDialogEx::OnPaint() for painting messages
}

/**********************************************************************************
*	函数名称：SetErrorType
*	函数功能：设置 m_errorType，当类型不同时窗口中的图片不一样
*	函数参数：窗口类型
*	函数返回：无用
*	代码编写：shinn-2011-05-07
***********************************************************************************/

void CErrorDlg::SetErrorType(ErrorType err)
{
	m_errorType = err;
}
