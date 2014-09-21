
// AutoLinkerDlg.h : 头文件
//

#pragma once

#include "TaskbarNotifier.h"
#include "afxcmn.h"
// CAutoLinkerDlg 对话框
class CAutoLinkerDlg : public CDialogEx
{
// 构造
public:
	CAutoLinkerDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_AUTOLINKER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	void LoadBitmap();									//加载图块区域位图

	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg LRESULT OnFinishLink(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedSinglelink();
	afx_msg void OnBnClickedAutolink();
	afx_msg void OnBnClickedAlllink();


private:
	HWND m_LinkHwnd;

	CDC			m_backDC;
	CBitmap	m_backBitmap;
	int		m_teachDlg;

	

	BitBlock map[BLOCK_NUM_H][BLOCK_NUM_W];

	BOOL CheckLinkHwnd();
	BOOL SearchConnectable(int line, int row, int& desLine, int& desRow);
	BOOL CheckSame(BitBlock& b1, BitBlock& b2);
	BOOL UpLineCheck(int line, int row, int& desLine, int& desRow);
	BOOL DownLineCheck(int line, int row, int& desLine, int& desRow);
	BOOL LeftRowCheck(int line, int row, int& desLine, int& desRow);
	BOOL RightRowCheck(int line, int row, int& desLine, int& desRow);
	void	  ClickPos(int line, int row, int desLine, int desRow);
	BOOL LinkSingle();
	BOOL LinkAll();
	BOOL CheckDone();
	BOOL QuickLink();
	void	   LoadIniFile();
	

public:
	int		m_timerSpeed;
	BOOL	m_started;
	DWORD	m_preSingleTime;
		int		m_randomLink;

	BOOL InitalMap();
	BOOL LinkOne();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void SetLinkHwnd(HWND hwnd);
	void SetOption(int teachDlg, int timerSpeed, int randomLink);
	HWND GetLinkHwnd();
	void NotifyBox(CString text);

private:
	CTaskbarNotifier m_taskBar;
public:
	afx_msg void OnBnClickedButtonOption();
};
