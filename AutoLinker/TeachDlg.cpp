// TeachDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "AutoLinker.h"
#include "TeachDlg.h"
#include "afxdialogex.h"


// CTeachDlg �Ի���

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


// CTeachDlg ��Ϣ�������


BOOL CTeachDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��

	//m_isAboutDlg���ڿ���ʹ��ʲô����ͼƬ������Ӧ����ô����By shinn
	CDC* sysDC = GetDC();
	m_teachDC.CreateCompatibleDC(sysDC);
	m_teachBitmapAbout.LoadBitmap(IDB_BITMAP_ABOUT);
	m_teachBitmapTech.LoadBitmap(IDB_BITMAP_TEACH);
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
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

	// ��Ϊ��ͼ��Ϣ���� CDialog::OnPaint()
}


void CTeachDlg::OnBnClickedButtonAbout()
{
	//������Ӧ�ı�
	if (m_isAboutDlg)
	{
		((CButton*)GetDlgItem(IDOK))->EnableWindow(TRUE);
		((CButton*)GetDlgItem(IDOK))->ShowWindow(SW_NORMAL);
		((CButton*)GetDlgItem(IDC_BUTTON_ABOUT))->SetWindowText(TEXT("��������"));

		m_isAboutDlg = FALSE;
		InvalidateRect(NULL);
	}else
	{
		((CButton*)GetDlgItem(IDOK))->EnableWindow(FALSE);
		((CButton*)GetDlgItem(IDOK))->ShowWindow(SW_HIDE);
		((CButton*)GetDlgItem(IDC_BUTTON_ABOUT))->SetWindowText(TEXT("����"));

		m_isAboutDlg = TRUE;
		InvalidateRect(NULL);
	}
}
