#pragma once

// COptionDlg 对话框
class CAutoLinkerDlg;

class COptionDlg : public CDialogEx
{
	DECLARE_DYNAMIC(COptionDlg)

public:
	COptionDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~COptionDlg();



// 对话框数据
	enum { IDD = IDD_DIALOG_OPTION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

private:
	int m_teachDlg;
	int m_randomLink;
	CAutoLinkerDlg* m_pDlg;

	void LoadIniFile();
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedCheckRandom();
	afx_msg void OnBnClickedCheckSkip();
	virtual void OnOK();
	void SetMainDlg(CAutoLinkerDlg* pDlg);
	int m_slider;
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};
