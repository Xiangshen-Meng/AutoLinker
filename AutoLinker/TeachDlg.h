#pragma once


// CTeachDlg 对话框

class CTeachDlg : public CDialog
{
	DECLARE_DYNAMIC(CTeachDlg)

public:
	CTeachDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CTeachDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_TEACH };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

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
