#pragma once


// CTeachDlg �Ի���

class CTeachDlg : public CDialog
{
	DECLARE_DYNAMIC(CTeachDlg)

public:
	CTeachDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CTeachDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_TEACH };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

private:
	CDC			m_teachDC;
	CBitmap	m_teachBitmapAbout;
	CBitmap	m_teachBitmapTech;

	BOOL	m_isAboutDlg;
public:
	afx_msg void OnPaint();
	afx_msg void OnBnClickedButtonAbout();
};
