
// AutoLinkerDlg.h : ͷ�ļ�
//

#pragma once

#include "TaskbarNotifier.h"
#include "afxcmn.h"
// CAutoLinkerDlg �Ի���
class CAutoLinkerDlg : public CDialogEx
{
// ����
public:
	CAutoLinkerDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_AUTOLINKER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	void LoadBitmap();									//����ͼ������λͼ

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
