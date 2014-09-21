// OptionDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "AutoLinker.h"
#include "OptionDlg.h"
#include "afxdialogex.h"

/*********************************/
#include "AutoLinkerDlg.h"

// COptionDlg �Ի���

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


// COptionDlg ��Ϣ�������

BOOL COptionDlg::OnInitDialog()
{
	LoadIniFile();					//���������ļ�
	CDialogEx::OnInitDialog();
	//���û��鷶Χ��λ��
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_SPEED))->SetRange(1, 100);
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_SPEED))->SetTicFreq(10);
	//������ʾ����λ�õ�����
	CString speed;
	speed.Format(TEXT("%d"), m_slider);
	SetDlgItemText(IDC_STATIC_SPEED, speed);
	//������Ӧcheckbox��ѡ��״̬
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
	// �쳣: OCX ����ҳӦ���� FALSE
}

/**********************************************************************************
*	�������ƣ�LoadIniFile
*	�������ܣ����������ļ�
*	������������
*	�������أ���
*	�����д��shinn-2011-05-22
***********************************************************************************/
void COptionDlg::LoadIniFile()
{
	//��ȡ�ٶ�
	m_slider = ::GetPrivateProfileInt(TEXT("AutoLink"), TEXT("Speed"), 0, TEXT(".\\USER.ini"));
	if (m_slider == 0 || m_slider > 100)
	{
		CString speed;
		m_slider = 50;
		speed.Format(TEXT("%d"), m_slider);
		::WritePrivateProfileString(TEXT("AutoLink"), TEXT("Speed"), speed, TEXT(".\\USER.ini"));
	}
	//��ȡ�Ƿ�Ҫ������ѧ
	m_teachDlg = ::GetPrivateProfileInt(TEXT("Other"), TEXT("TeachDlg"), 0, TEXT(".\\USER.ini"));
	if (m_teachDlg == 0 || (m_teachDlg != 1 && m_teachDlg != 2))
	{
		CString teachDlg;
		m_teachDlg = 1;				//1Ϊ������ѧ����
		teachDlg.Format(TEXT("%d"), m_teachDlg);
		::WritePrivateProfileString(TEXT("Other"), TEXT("TeachDlg"), teachDlg, TEXT(".\\USER.ini"));
	}
	//��ȡ�Ƿ�Ϊ���ģʽ
	m_randomLink = ::GetPrivateProfileInt(TEXT("AutoLink"), TEXT("RandomLink"), 0, TEXT(".\\USER.ini"));
	if (m_randomLink == 0 || (m_randomLink != 1 && m_randomLink != 2))
	{
		CString randomLink;
		m_randomLink = 2;				//2Ϊ�ر�����ٶ�
		randomLink.Format(TEXT("%d"), m_randomLink);
		::WritePrivateProfileString(TEXT("AutoLink"), TEXT("RandomLink"), randomLink, TEXT(".\\USER.ini"));
	}
}

/**********************************************************************************
*	�������ƣ�OnBnClickedCheckRandom
*	�������ܣ���ѡ��ȡ�����ģʽ
*	������������
*	�������أ���
*	�����д��shinn-2011-05-22
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
*	�������ƣ�OnBnClickedCheckSkip
*	�������ܣ���ѡ��ȡ��������ѧ
*	������������
*	�������أ���
*	�����д��shinn-2011-05-22
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
*	�������ƣ�OnOK
*	�������ܣ���дOnOK,����Ӧ������д�������ļ������ı��������������
*	������������
*	�������أ���
*	�����д��shinn-2011-05-22
***********************************************************************************/
void COptionDlg::OnOK()
{
	CString temp;
	//д�����ģʽ
	temp.Format(TEXT("%d"), m_randomLink);
	::WritePrivateProfileString(TEXT("AutoLink"), TEXT("RandomLink"), temp, TEXT(".\\USER.ini"));
	//д���Ƿ�Ҫ������ѧ
	temp.Format(TEXT("%d"), m_teachDlg);
	::WritePrivateProfileString(TEXT("Other"), TEXT("TeachDlg"), temp, TEXT(".\\USER.ini"));
	//д���ٶ�
	temp.Format(TEXT("%d"), m_slider);
	::WritePrivateProfileString(TEXT("AutoLink"), TEXT("Speed"), temp, TEXT(".\\USER.ini"));
	//�ı������ڵ��ٶȼ����ģʽ
	m_pDlg->SetOption(m_teachDlg, m_slider, m_randomLink);

	CDialogEx::OnOK();
}

/**********************************************************************************
*	�������ƣ�SetMainDlg
*	�������ܣ�����������ָ�룬���ڵ��������ں���SetOption��
*	����������pDlg��������thisָ��
*	�������أ���
*	�����д��shinn-2011-05-22
***********************************************************************************/
void COptionDlg::SetMainDlg(CAutoLinkerDlg* pDlg)
{
	m_pDlg = pDlg;
}

/**********************************************************************************
*	�������ƣ�OnHScroll
*	�������ܣ�����λ�øı�ʱ��Ӧ����Ϣ�����ڸı��ٶȵı���
*	������������
*	�������أ���
*	�����д��shinn-2011-05-22
***********************************************************************************/
void COptionDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	m_slider = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER_SPEED))->GetPos();
	CString speed;
	speed.Format(TEXT("%d"), m_slider);
	SetDlgItemText(IDC_STATIC_SPEED, speed);
	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}