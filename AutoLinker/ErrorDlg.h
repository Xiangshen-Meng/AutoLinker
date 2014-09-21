#pragma once


// CErrorDlg dialog

class CErrorDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CErrorDlg)

public:
	CErrorDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CErrorDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_ERROR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

private:
	CDC		m_errorDC;
	CBitmap	m_errorBitmapNoWnd;
	CBitmap	m_errorBitmapKill;

	ErrorType	m_errorType;
public:
	afx_msg void OnPaint();
	void SetErrorType(ErrorType err);
};
