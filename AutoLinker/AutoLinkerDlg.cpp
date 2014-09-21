
// AutoLinkerDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "AutoLinker.h"
#include "AutoLinkerDlg.h"
#include "afxdialogex.h"

/*******************����ͷ�ļ���By shinn*****************/
#include "TeachDlg.h"
#include "ErrorDlg.h"
#include "OptionDlg.h"
/*******************����ͷ�ļ���By shinn*****************/

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DWORD WINAPI LinkThread(LPVOID lpParameter);
DWORD WINAPI CheckWindowThread(LPVOID lpParameter);
// CAutoLinkerDlg �Ի���




CAutoLinkerDlg::CAutoLinkerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAutoLinkerDlg::IDD, pParent)
	,m_LinkHwnd(0)
	,m_timerSpeed(0)
	,m_started(FALSE)
	,m_preSingleTime(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAutoLinkerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAutoLinkerDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_SINGLELINK, &CAutoLinkerDlg::OnBnClickedSinglelink)
	ON_BN_CLICKED(IDC_AUTOLINK, &CAutoLinkerDlg::OnBnClickedAutolink)
	ON_BN_CLICKED(IDC_ALLLINK, &CAutoLinkerDlg::OnBnClickedAlllink)
	ON_MESSAGE(WM_FINISH_LINK, &CAutoLinkerDlg::OnFinishLink)
	ON_BN_CLICKED(IDC_BUTTON_OPTION, &CAutoLinkerDlg::OnBnClickedButtonOption)
END_MESSAGE_MAP()


// CAutoLinkerDlg ��Ϣ�������

BOOL CAutoLinkerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	LoadIniFile();							//���������ļ�

	//������ѧDialog��1Ϊ������ѧ���ڣ�2Ϊ����������By shinn
	if(m_teachDlg == 1)
	{
		CTeachDlg teach;
		teach.DoModal();
	}

	//��ʼ������ͼƬ��By shinn
	CDC* sysDC = GetDC();

	m_backDC.CreateCompatibleDC(sysDC);
	m_backBitmap.LoadBitmap(IDB_BITMAP_BACKGROUND);
	m_backDC.SelectObject(m_backBitmap);
	//��ʼ�����½�С���ڣ�By shinn
	m_taskBar.Create(this);
	m_taskBar.SetSkin(IDB_BITMAP_NOTIFY, 255, 0, 255);
	m_taskBar.SetTextFont(TEXT("Arial Black"),120,TN_TEXT_NORMAL,TN_TEXT_UNDERLINE | TN_TEXT_BOLD);
	m_taskBar.SetTextColor(RGB(0,0,0),RGB(0,0,200));
	m_taskBar.SetTextRect(CRect(10, 30, m_taskBar.m_nSkinWidth-10, m_taskBar.m_nSkinHeight-30));
	//��¼��ǰʱ�䣺By shinn
	m_preSingleTime = GetTickCount();

	CreateThread(0, 0, CheckWindowThread, this, 0, NULL);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CAutoLinkerDlg::OnPaint()
{
	CPaintDC dc(this); // ���ڻ��Ƶ��豸������

	if (IsIconic())
	{
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		//���Ʊ���ͼƬ��By shinn
		dc.StretchBlt(0, 0, 300, 240, &m_backDC, 0, 0, 292, 228, SRCCOPY);

		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CAutoLinkerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

/**********************************************************************************
*	�������ƣ�CheckLinkHwnd
*	�������ܣ��жϵ�ǰ����Ƿ�Ϊ�գ����Ϊ��
*	������������
*	�������أ����޾��
*	�����д��shinn-2011-05-20
***********************************************************************************/

BOOL CAutoLinkerDlg::CheckLinkHwnd()
{
	if (NULL != m_LinkHwnd)
	{
		return TRUE;
	}else
	{
		CErrorDlg errorDlg;
		errorDlg.SetErrorType(ErrorDlg);
		errorDlg.DoModal();

		return FALSE;
	}
}

/**********************************************************************************
*	�������ƣ�InitalMap
*	�������ܣ�ͨ������õ�DC������ͼ��Ĳ�ȡ�������������ͼ��
*	������������
*	�������أ��Ƿ�ɹ�
*	�����д��shinn-2011-05-07
***********************************************************************************/

BOOL CAutoLinkerDlg::InitalMap()
{
	int line = 0;
	int row = 0;
	//ʹ��Ϸ����Ϊ���ϣ������ͼ�����ǲ������á���������By shinn
	::SetWindowPos(m_LinkHwnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	HDC hDC = ::GetDC(m_LinkHwnd);



	if (hDC == NULL)
	{
		return FALSE;
	}
	//�õ��ؼ�������������ɵ�ͼ��By shinn
	for (line = 0; line <BLOCK_NUM_H; line++)
	{
		for (row = 0; row < BLOCK_NUM_W; row++)
		{
			map[line][row].pointMid				= GetPixel(hDC, STARTPOS_X+row*BLOCK_WIDTH+BLOCK_WIDTH/2, STARTPOS_Y+line*BLOCK_HIGHT+BLOCK_HIGHT/2);
			map[line][row].pointUp				= GetPixel(hDC, STARTPOS_X+row*BLOCK_WIDTH+BLOCK_WIDTH/2, STARTPOS_Y+line*BLOCK_HIGHT+BLOCK_HIGHT/2-OFFSET_PIXEL);
			map[line][row].pointDown			= GetPixel(hDC, STARTPOS_X+row*BLOCK_WIDTH+BLOCK_WIDTH/2, STARTPOS_Y+line*BLOCK_HIGHT+BLOCK_HIGHT/2+OFFSET_PIXEL);
			map[line][row].pointLeft			= GetPixel(hDC, STARTPOS_X+row*BLOCK_WIDTH+BLOCK_WIDTH/2-OFFSET_PIXEL, STARTPOS_Y+line*BLOCK_HIGHT+BLOCK_HIGHT/2);
			map[line][row].pointRight			= GetPixel(hDC, STARTPOS_X+row*BLOCK_WIDTH+BLOCK_WIDTH/2+OFFSET_PIXEL, STARTPOS_Y+line*BLOCK_HIGHT+BLOCK_HIGHT/2);
			
			//������Ǳ���ɫ�Ļ�������ͼ�飬��������Ϊ��ͼ�飬�����Ժ��ж�·����ͨ��By shinn
			if (map[line][row].pointMid != BACK_GROUND_COLOR || map[line][row].pointDown != BACK_GROUND_COLOR || map[line][row].pointUp != BACK_GROUND_COLOR || map[line][row].pointLeft != BACK_GROUND_COLOR || map[line][row].pointRight != BACK_GROUND_COLOR)
			{
				map[line][row].standing = TRUE;
			}else
			{
				map[line][row].standing = FALSE;
			}
		}
	}

	::ReleaseDC(m_LinkHwnd, hDC);
	return TRUE;
}

/**********************************************************************************
*	�������ƣ�SearchConnectable
*	�������ܣ�ͨ������һ�����ڵ�ͼ�е�λ�ã�
*		���Եõ��ܺ������ϵ�һ���㲢ͨ��desLine��desRow����
*	����������line��	�����ĵ��ڵ�ͼ�е�������
*			  row��		�����ĵ��ڵ�ͼ�еĺ�����
*			  desLine��	���ڷ����ҵ��Ŀ����ĵ��ڵ�ͼ�е�������
*			  desRow��	���ڷ����ҵ��Ŀ����ĵ��ڵ�ͼ�еĺ�����
*	�������أ��Ƿ�ɹ�
*	�����д��shinn-2011-05-07
***********************************************************************************/

BOOL CAutoLinkerDlg::SearchConnectable(int line, int row, int& desLine, int& desRow)
{
	//��ǰ�Ĵ���λ�ø�����ͼ��ʱֱ�ӷ��أ�By shinn
	if (map[line][row].standing == FALSE)
	{
		return FALSE;
	}
	//�����ж��Ƿ����ҵ�һ������ͨ��ͼ�飺By shinn
	if (UpLineCheck(line, row, desLine, desRow))
	{
		return TRUE;
	}
	//�����ж��Ƿ����ҵ�һ������ͨ��ͼ�飺By shinn
	if (DownLineCheck(line, row, desLine, desRow))
	{
		return TRUE;
	}
	//�����ж��Ƿ����ҵ�һ������ͨ��ͼ�飺By shinn
	if (LeftRowCheck(line, row, desLine, desRow))
	{
		return TRUE;
	}
	//�����ж��Ƿ����ҵ�һ������ͨ��ͼ�飺By shinn
	if (RightRowCheck(line, row, desLine, desRow))
	{
		return TRUE;
	}

	return FALSE;
}

/**********************************************************************************
*	�������ƣ�UpLineCheck
*	�������ܣ����ϲ��ֵĲ��ҹ���
*	����������line��	�����ĵ��ڵ�ͼ�е�������
*			  row��		�����ĵ��ڵ�ͼ�еĺ�����
*			  desLine��	���ڷ����ҵ��Ŀ����ĵ��ڵ�ͼ�е�������
*			  desRow��	���ڷ����ҵ��Ŀ����ĵ��ڵ�ͼ�еĺ�����
*	�������أ��Ƿ�ɹ�
*	�����д��shinn-2011-05-07
***********************************************************************************/

BOOL CAutoLinkerDlg::UpLineCheck(int line, int row, int& desLine, int& desRow)
{
	//�ֱ���������ߣ�By shinn
	int upFirst = 0, upSecond = 0, upThird = 0;
	/********************************������*****************************************/
	//���Ͻ��в��ң�By shinn
	for (upFirst = line-1; upFirst >=0; upFirst--)
	{
		if (map[upFirst][row].standing == TRUE)
		{
			if(CheckSame(map[line][row], map[upFirst][row]))
			{
				desLine = upFirst;
				desRow = row;
				return TRUE;
			}else
			{
				break;
			}
		}
		/********************************���Ϻ����������е�����*************************************/
		//���Ϻ�����ת��By shinn
		for (upSecond = row-1; upSecond >=0; upSecond--)
		{
			if (map[upFirst][upSecond].standing == TRUE)
			{
				if (CheckSame(map[line][row], map[upFirst][upSecond]))
				{
					desLine = upFirst;
					desRow = upSecond;
					return TRUE;
				}else
				{
					break;
				}
			}
			//���Ϻ�����ת�������ң�By shinn
			for (upThird = upFirst-1; upThird >= 0; upThird--)
			{
				if (map[upThird][upSecond].standing == TRUE)
				{
					if (CheckSame(map[line][row], map[upThird][upSecond]))
					{
						desLine = upThird;
						desRow = upSecond;
						return TRUE;
					}else
					{
						break;
					}
				}
			}
			//���Ϻ�����ת�������ң�By shinn
			for (upThird = upFirst+1; upThird < BLOCK_NUM_H; upThird++)
			{
				if (map[upThird][upSecond].standing == TRUE)
				{
					if (CheckSame(map[line][row], map[upThird][upSecond]))
					{
						desLine = upThird;
						desRow = upSecond;
						return TRUE;
					}else
					{
						break;
					}
				}
			}
		}
		/********************************���Ϻ����������е�����*************************************/
		//���Ϻ�����ת��By shinn
		for (upSecond = row+1; upSecond < BLOCK_NUM_W; upSecond++)
		{
			if (map[upFirst][upSecond].standing == TRUE)
			{
				if (CheckSame(map[line][row], map[upFirst][upSecond]))
				{
					desLine = upFirst;
					desRow = upSecond;
					return TRUE;
				}else
				{
					break;
				}
			}
			//���Ϻ�����ת�������ң�By shinn
			for (upThird = upFirst-1; upThird >= 0; upThird--)
			{
				if (map[upThird][upSecond].standing == TRUE)
				{
					if (CheckSame(map[line][row], map[upThird][upSecond]))
					{
						desLine = upThird;
						desRow = upSecond;
						return TRUE;
					}else
					{
						break;
					}
				}
			}
			//���Ϻ�����ת�������ң�By shinn
			for (upThird = upFirst+1; upThird < BLOCK_NUM_H; upThird++)
			{
				if (map[upThird][upSecond].standing == TRUE)
				{
					if (CheckSame(map[line][row], map[upThird][upSecond]))
					{
						desLine = upThird;
						desRow = upSecond;
						return TRUE;
					}else
					{
						break;
					}
				}
			}
		}
	}

	return FALSE;
}

/**********************************************************************************
*	�������ƣ�DownLineCheck
*	�������ܣ����²��ֵĲ��ҹ���
*	����������line��	�����ĵ��ڵ�ͼ�е�������
*			  row��		�����ĵ��ڵ�ͼ�еĺ�����
*			  desLine��	���ڷ����ҵ��Ŀ����ĵ��ڵ�ͼ�е�������
*			  desRow��	���ڷ����ҵ��Ŀ����ĵ��ڵ�ͼ�еĺ�����
*	�������أ��Ƿ�ɹ�
*	�����д��shinn-2011-05-07
***********************************************************************************/

BOOL CAutoLinkerDlg::DownLineCheck(int line, int row, int& desLine, int& desRow)
{
	//�ֱ���������ߣ�By shinn
	int downFirst = 0, downSecond = 0, downThird = 0;
	/********************************������*****************************************/
	//���½��в��ң�By shinn
	for (downFirst = line+1; downFirst < BLOCK_NUM_H; downFirst++)
	{
		if (map[downFirst][row].standing == TRUE)
		{
			if(CheckSame(map[line][row], map[downFirst][row]))
			{
				desLine = downFirst;
				desRow = row;
				return TRUE;
			}else
			{
				break;
			}
		}
		/********************************���º����������е�����*************************************/
		//���º�����ת��By shinn
		for (downSecond = row-1; downSecond >=0; downSecond--)
		{
			if (map[downFirst][downSecond].standing == TRUE)
			{
				if (CheckSame(map[line][row], map[downFirst][downSecond]))
				{
					desLine = downFirst;
					desRow = downSecond;
					return TRUE;
				}else
				{
					break;
				}
			}
			//���º�����ת�������ң�By shinn
			for (downThird = downFirst-1; downThird >= 0; downThird--)
			{
				if (map[downThird][downSecond].standing == TRUE)
				{
					if (CheckSame(map[line][row], map[downThird][downSecond]))
					{
						desLine = downThird;
						desRow = downSecond;
						return TRUE;
					}else
					{
						break;
					}
				}
			}
			//���º�����ת�������ң�By shinn
			for (downThird = downFirst+1; downThird < BLOCK_NUM_H; downThird++)
			{
				if (map[downThird][downSecond].standing == TRUE)
				{
					if (CheckSame(map[line][row], map[downThird][downSecond]))
					{
						desLine = downThird;
						desRow = downSecond;
						return TRUE;
					}else
					{
						break;
					}
				}
			}
		}
		/********************************���º����������е�����*************************************/
		//���º�����ת��By shinn
		for (downSecond = row+1; downSecond < BLOCK_NUM_W; downSecond++)
		{
			if (map[downFirst][downSecond].standing == TRUE)
			{
				if (CheckSame(map[line][row], map[downFirst][downSecond]))
				{
					desLine = downFirst;
					desRow = downSecond;
					return TRUE;
				}else
				{
					break;
				}
			}
			//���º�����ת�������ң�By shinn
			for (downThird = downFirst-1; downThird >= 0; downThird--)
			{
				if (map[downThird][downSecond].standing == TRUE)
				{
					if (CheckSame(map[line][row], map[downThird][downSecond]))
					{
						desLine = downThird;
						desRow = downSecond;
						return TRUE;
					}else
					{
						break;
					}
				}
			}
			//���º�����ת�������ң�By shinn
			for (downThird = downFirst+1; downThird < BLOCK_NUM_H; downThird++)
			{
				if (map[downThird][downSecond].standing == TRUE)
				{
					if (CheckSame(map[line][row], map[downThird][downSecond]))
					{
						desLine = downThird;
						desRow = downSecond;
						return TRUE;
					}else
					{
						break;
					}
				}
			}
		}
	}

	return FALSE;
}

/**********************************************************************************
*	�������ƣ�LeftRowCheck
*	�������ܣ����󲿷ֵĲ��ҹ���
*	����������line��	�����ĵ��ڵ�ͼ�е�������
*			  row��		�����ĵ��ڵ�ͼ�еĺ�����
*			  desLine��	���ڷ����ҵ��Ŀ����ĵ��ڵ�ͼ�е�������
*			  desRow��	���ڷ����ҵ��Ŀ����ĵ��ڵ�ͼ�еĺ�����
*	�������أ��Ƿ�ɹ�
*	�����д��shinn-2011-05-07
***********************************************************************************/

BOOL CAutoLinkerDlg::LeftRowCheck(int line, int row, int& desLine, int& desRow)
{
	//�ֱ���������ߣ�By shinn
	int leftFirst = 0, leftSecond = 0, leftThird = 0;
	/********************************������*****************************************/
	//������в��ң�By shinn
	for (leftFirst = row-1; leftFirst >= 0; leftFirst--)
	{
		if (map[line][leftFirst].standing == TRUE)
		{
			if(CheckSame(map[line][row], map[line][leftFirst]))
			{
				desLine = line;
				desRow = leftFirst;
				return TRUE;
			}else
			{
				break;
			}
		}
		/********************************��������������е�����*************************************/
		//���������ת��By shinn
		for (leftSecond = line-1; leftSecond >=0; leftSecond--)
		{
			if (map[leftSecond][leftFirst].standing == TRUE)
			{
				if (CheckSame(map[line][row], map[leftSecond][leftFirst]))
				{
					desLine = leftSecond;
					desRow = leftFirst;
					return TRUE;
				}else
				{
					break;
				}
			}
			//���������ת�������ң�By shinn
			for (leftThird = leftFirst-1; leftThird >= 0; leftThird--)
			{
				if (map[leftSecond][leftThird].standing == TRUE)
				{
					if (CheckSame(map[line][row], map[leftSecond][leftThird]))
					{
						desLine = leftSecond;
						desRow = leftThird;
						return TRUE;
					}else
					{
						break;
					}
				}
			}
			//���������ת�������ң�By shinn
			for (leftThird = leftFirst+1; leftThird < BLOCK_NUM_W; leftThird++)
			{
				if (map[leftSecond][leftThird].standing == TRUE)
				{
					if (CheckSame(map[line][row], map[leftSecond][leftThird]))
					{
						desLine = leftSecond;
						desRow = leftThird;
						return TRUE;
					}else
					{
						break;
					}
				}
			}
		}
		/********************************��������������е�����*************************************/
		//���������ת��By shinn
		for (leftSecond = line+1; leftSecond < BLOCK_NUM_H; leftSecond++)
		{
			if (map[leftSecond][leftFirst].standing == TRUE)
			{
				if (CheckSame(map[line][row], map[leftSecond][leftFirst]))
				{
					desLine = leftSecond;
					desRow = leftFirst;
					return TRUE;
				}else
				{
					break;
				}
			}
			//���������ת�������ң�By shinn
			for (leftThird = leftFirst-1; leftThird >= 0; leftThird--)
			{
				if (map[leftSecond][leftThird].standing == TRUE)
				{
					if (CheckSame(map[line][row], map[leftSecond][leftThird]))
					{
						desLine = leftSecond;
						desRow = leftThird;
						return TRUE;
					}else
					{
						break;
					}
				}
			}
			//���������ת�������ң�By shinn
			for (leftThird = leftFirst+1; leftThird < BLOCK_NUM_W; leftThird++)
			{
				if (map[leftSecond][leftThird].standing == TRUE)
				{
					if (CheckSame(map[line][row], map[leftSecond][leftThird]))
					{
						desLine = leftSecond;
						desRow = leftThird;
						return TRUE;
					}else
					{
						break;
					}
				}
			}
		}
	}

	return FALSE;
}

/**********************************************************************************
*	�������ƣ�RightRowCheck
*	�������ܣ����Ҳ��ֵĲ��ҹ���
*	����������line��	�����ĵ��ڵ�ͼ�е�������
*			  row��		�����ĵ��ڵ�ͼ�еĺ�����
*			  desLine��	���ڷ����ҵ��Ŀ����ĵ��ڵ�ͼ�е�������
*			  desRow��	���ڷ����ҵ��Ŀ����ĵ��ڵ�ͼ�еĺ�����
*	�������أ��Ƿ�ɹ�
*	�����д��shinn-2011-05-07
***********************************************************************************/

BOOL CAutoLinkerDlg::RightRowCheck(int line, int row, int& desLine, int& desRow)
{
	//�ֱ���������ߣ�By shinn
	int rightFirst = 0, rightSecond = 0, rightThird = 0;
	/********************************������*****************************************/
	//���ҽ��в��ң�By shinn
	for (rightFirst = row+1; rightFirst < BLOCK_NUM_W; rightFirst++)
	{
		if (map[line][rightFirst].standing == TRUE)
		{
			if(CheckSame(map[line][row], map[line][rightFirst]))
			{
				desLine = line;
				desRow = rightFirst;
				return TRUE;
			}else
			{
				break;
			}
		}
		/********************************���Һ����������е�����*************************************/
		//���Һ�����ת��By shinn
		for (rightSecond = line-1; rightSecond >=0; rightSecond--)
		{
			if (map[rightSecond][rightFirst].standing == TRUE)
			{
				if (CheckSame(map[line][row], map[rightSecond][rightFirst]))
				{
					desLine = rightSecond;
					desRow = rightFirst;
					return TRUE;
				}else
				{
					break;
				}
			}
			//���Һ�����ת�������ң�By shinn
			for (rightThird = rightFirst-1; rightThird >= 0; rightThird--)
			{
				if (map[rightSecond][rightThird].standing == TRUE)
				{
					if (CheckSame(map[line][row], map[rightSecond][rightThird]))
					{
						desLine = rightSecond;
						desRow = rightThird;
						return TRUE;
					}else
					{
						break;
					}
				}
			}
			//���Һ�����ת�������ң�By shinn
			for (rightThird = rightFirst+1; rightThird < BLOCK_NUM_W; rightThird++)
			{
				if (map[rightSecond][rightThird].standing == TRUE)
				{
					if (CheckSame(map[line][row], map[rightSecond][rightThird]))
					{
						desLine = rightSecond;
						desRow = rightThird;
						return TRUE;
					}else
					{
						break;
					}
				}
			}
		}
		/********************************���Һ����������е�����*************************************/
		//���Һ�����ת��By shinn
		for (rightSecond = line+1; rightSecond < BLOCK_NUM_H; rightSecond++)
		{
			if (map[rightSecond][rightFirst].standing == TRUE)
			{
				if (CheckSame(map[line][row], map[rightSecond][rightFirst]))
				{
					desLine = rightSecond;
					desRow = rightFirst;
					return TRUE;
				}else
				{
					break;
				}
			}
			//���������ת�������ң�By shinn
			for (rightThird = rightFirst-1; rightThird >= 0; rightThird--)
			{
				if (map[rightSecond][rightThird].standing == TRUE)
				{
					if (CheckSame(map[line][row], map[rightSecond][rightThird]))
					{
						desLine = rightSecond;
						desRow = rightThird;
						return TRUE;
					}else
					{
						break;
					}
				}
			}
			//���������ת�������ң�By shinn
			for (rightThird = rightFirst+1; rightThird < BLOCK_NUM_W; rightThird++)
			{
				if (map[rightSecond][rightThird].standing == TRUE)
				{
					if (CheckSame(map[line][row], map[rightSecond][rightThird]))
					{
						desLine = rightSecond;
						desRow = rightThird;
						return TRUE;
					}else
					{
						break;
					}
				}
			}
		}
	}

	return FALSE;
}

/**********************************************************************************
*	�������ƣ�CheckSame
*	�������ܣ��ж��Ƿ�Ϊһ����ͼ��
*	����������b1��		��һ��ͼ��
*			  b2��		�ڶ���ͼ��
*	�������أ��Ƿ�һ��
*	�����д��shinn-2011-05-07
***********************************************************************************/

BOOL CAutoLinkerDlg::CheckSame(BitBlock& b1, BitBlock& b2)
{
	//��ɫȫһ������һ��ͼ�飺By shinn
	if (b1.pointDown == b2.pointDown && b1.pointLeft == b2.pointLeft && b1.pointMid == b2.pointMid && b1.pointRight == b2.pointRight && b1.pointUp == b2.pointUp)
	{
		return TRUE;
	}else
	{
		return FALSE;
	}
}

/**********************************************************************************
*	�������ƣ�LinkSingle
*	�������ܣ������������ʱ������õ�QQ����Ĳ���
*	������������
*	�������أ��Ƿ�ɹ�����̫���ô���
*	�����д��shinn-2011-05-07
***********************************************************************************/

BOOL CAutoLinkerDlg::LinkSingle()
{
	if (CheckLinkHwnd() == TRUE)
	{
		//�ж�2�ε�����������ļ���ǲ��Ǵ���1s�룬����1s�����ɵ�ͼ
		DWORD curTime = GetTickCount();
		if ((curTime-m_preSingleTime) > 1000)
		{
			InitalMap();
		}
		m_preSingleTime = curTime;
		return LinkOne();
	}
	return FALSE;
}

/**********************************************************************************
*	�������ƣ�ClickPos
*	�������ܣ�ͨ�������2���㣬ģ������¼�������ȥ
*	����������line��	��һ�����ڵ�ͼ�е�������
*			  row��		��һ�����ڵ�ͼ�еĺ�����
*			  desLine��	�ڶ������ڵ�ͼ�е�������
*			  desRow��	�ڶ������ڵ�ͼ�еĺ�����
*	�������أ���
*	�����д��shinn-2011-05-07
***********************************************************************************/

void CAutoLinkerDlg::ClickPos(int line, int row, int desLine, int desRow)
{
	POINT clickPos;
	POINT p;
	//������Ϸ���������ϣ�By shinn
	::SetWindowPos(m_LinkHwnd, 0, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	//����ͼ�е�λ��ת��Ϊ��������λ�ã�By shinn
	clickPos.x = STARTPOS_X+row*BLOCK_WIDTH+BLOCK_WIDTH/2;
	clickPos.y = STARTPOS_Y+line*BLOCK_HIGHT+BLOCK_HIGHT/2;
	//����������ת��Ϊscreen���꣺By shinn
	::ClientToScreen(m_LinkHwnd, &clickPos);

	//�ƶ���ָ��λ��ģ������By shinn
	GetCursorPos(&p);
	::SetCursorPos(clickPos.x, clickPos.y);
	mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);

	//�ڶ�����͵�һ������ͬ�Ĳ�����By shinn
	clickPos.x = STARTPOS_X+desRow*BLOCK_WIDTH+BLOCK_WIDTH/2;
	clickPos.y = STARTPOS_Y+desLine*BLOCK_HIGHT+BLOCK_HIGHT/2;

	::ClientToScreen(m_LinkHwnd, &clickPos);
	::SetCursorPos(clickPos.x, clickPos.y);
	mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
	::SetCursorPos(p.x, p.y);
}

/**********************************************************************************
*	�������ƣ�LinkAll
*	�������ܣ������ɱ��ʱ�򣬳ɹ��õ����ʱ���еĲ���
*	������������
*	�������أ��Ƿ�ɹ������ã�
*	�����д��shinn-2011-05-07
***********************************************************************************/

BOOL CAutoLinkerDlg::LinkAll()
{
	if (m_LinkHwnd == NULL)
	{
		return FALSE;
	}
	InitalMap();
	//����ͼ���ʱ��û������ʱ������һ�ο������ӣ�By shinn
	while (!CheckDone())
	{
		//�����������Ѿ�û�п������ӵ�ͼ��ʱ������By shinn
		if (QuickLink() == FALSE)
		{
			break;
		}
	}
	return TRUE;
}

/**********************************************************************************
*	�������ƣ�OnBnClickedSinglelink
*	�������ܣ���������¼�
*	������������
*	�������أ���
*	�����д��shinn-2011-05-07
***********************************************************************************/

void CAutoLinkerDlg::OnBnClickedSinglelink()
{
	if (CheckLinkHwnd() == TRUE)
	{
		LinkSingle();
	}
}

/**********************************************************************************
*	�������ƣ�OnBnClickedAutolink
*	�������ܣ���������¼�
*	������������
*	�������أ���
*	�����д��shinn-2011-05-07
***********************************************************************************/

void CAutoLinkerDlg::OnBnClickedAutolink()
{
	if (CheckLinkHwnd() == TRUE)
	{
		//m_started���ڿ����Զ������߳��Ƿ���е�m_startedΪFALSEʱ���߳̽�������ʱ�������ڷ�����Ϣ֪ͨ���ڸı䰴����״̬��By shinn
		if (m_started == FALSE)
		{
			//��m_startedΪ��ʱ˵����ǰ�û�ϣ�������Զ����������������̲߳�����Ӧ��ͼ�����޸ģ�By shinn
			((CButton*)GetDlgItem(IDC_AUTOLINK))->SetWindowText(TEXT("ֹͣ����"));
			((CButton*)GetDlgItem(IDC_BUTTON_OPTION))->EnableWindow(FALSE);
			((CButton*)GetDlgItem(IDC_ALLLINK))->EnableWindow(FALSE);
			((CButton*)GetDlgItem(IDC_SINGLELINK))->EnableWindow(FALSE);

			m_started = TRUE;

			InitalMap();
			//�����߳�
			CreateThread(0, 0, LinkThread, this, 0, NULL);
		}else
		{
			//���Ϊ�棬˵���û�ϣ���ر��Զ����ӣ���m_started����ΪFALSE���ı���Ӧ��ͼ��By shinn
			((CButton*)GetDlgItem(IDC_AUTOLINK))->SetWindowText(TEXT("�Զ�����"));
			((CButton*)GetDlgItem(IDC_ALLLINK))->EnableWindow(TRUE);
			((CButton*)GetDlgItem(IDC_SINGLELINK))->EnableWindow(TRUE);
			((CButton*)GetDlgItem(IDC_BUTTON_OPTION))->EnableWindow(TRUE);

			m_started = FALSE;
		}
	}
}

/**********************************************************************************
*	�������ƣ�OnBnClickedAlllink
*	�������ܣ���������¼�
*	������������
*	�������أ���
*	�����д��shinn-2011-05-07
***********************************************************************************/

void CAutoLinkerDlg::OnBnClickedAlllink()
{
	if (CheckLinkHwnd() == TRUE)
	{
		LinkAll();
		//��ʾ�ü�������ɱ���������ױ����֡�������By shinn
		CErrorDlg err;
		err.SetErrorType(AllLinkDlg);
		err.DoModal();
	}
}

/**********************************************************************************
*	�������ƣ�CheckDone
*	�������ܣ�����ͼ�Ƿ����
*	������������
*	�������أ��Ƿ����
*	�����д��shinn-2011-05-07
***********************************************************************************/

BOOL CAutoLinkerDlg::CheckDone()
{
	//�����ɣ�By shinn
	int line = 0, row = 0;

	for (line = 0; line< BLOCK_NUM_H; line++)
	{
		for (row = 0; row< BLOCK_NUM_W; row++)
		{
			if (map[line][row].standing == TRUE)
			{
				return FALSE;
			}
		}
	}
	return TRUE;
}

/**********************************************************************************
*	�������ƣ�QuickLink
*	�������ܣ���ɱ�����еĺ��ģ���������
*	������������
*	�������أ�ֻҪ�е����������ͼ�鱻�����ͷ�����
*	�����д��shinn-2011-05-07
***********************************************************************************/

BOOL CAutoLinkerDlg::QuickLink()
{
	//�������ӵ��㷨����ͷ��ʼ�������ϵ�ͼ�飬����ܱ߾�ģ����������������ͼ�е���Ӧλ�õ�standing��ΪFALSE
	//���ǲ��˳�ѭ�����Ǽ��������ң�ֱ������ͼȫɨһ�顣��ʱ��ͼ�е���Щͼ�黹û�б���������Ϊ��ɨ����ʱ�����޷���������
	//��������������󣬿��Ա������ˣ���ʱ��ҪCheckDoneһ�£�Ȼ���ٽ���һ��QuickLinkҲ����LinkAll��ʵ�ֵģ�By shinn
	int line = 0, row = 0;
	int desLine = 0, desRow = 0;
	BOOL ret = FALSE;

	for (line = 0; line< BLOCK_NUM_H; line++)
	{
		for (row = 0; row< BLOCK_NUM_W; row++)
		{
			if (map[line][row].standing == FALSE)
			{
				continue;
			}

			if (SearchConnectable(line, row, desLine, desRow))
			{
				ClickPos(line, row, desLine, desRow);
				ret = TRUE;
				map[line][row].standing = FALSE;
				map[desLine][desRow].standing = FALSE;
			}
		}
	}
	return ret;
}

/**********************************************************************************
*	�������ƣ�LinkThread
*	�������ܣ��Զ�����ʱ����Ҫ�������߳�
*	����������lpParameter��ʵΪ���е�CAutoLinkerDlg��thisָ�룬���Ե����亯��
*	�������أ���
*	�����д��shinn-2011-05-07
***********************************************************************************/

DWORD WINAPI LinkThread(LPVOID lpParameter)
{
	CAutoLinkerDlg* pMainDlg = ((CAutoLinkerDlg*)lpParameter);
	//m_startedΪ���أ���Ϊ��ʱ�߳̽��У���Ϊ��ʱ�߳̽�����By shinn
	while(pMainDlg->LinkOne() && pMainDlg->m_started)
	{
		//sleep��ʱ��ʱ�䣬���Ϊ���˯��ģʽ��˯�����ʱ�䣺By shinn
		if (pMainDlg->m_randomLink == 1)
		{
			Sleep(TIMER_DEFAULT*(rand()%100)+TIMER_BASE);
		}else
		{
			Sleep((100-pMainDlg->m_timerSpeed)*TIMER_DEFAULT+TIMER_BASE);
		}
	}

	//�߳̽���֪ͨ���߳�����Ӧ�ı䣺By shinn
	::SendMessage(pMainDlg->m_hWnd, WM_FINISH_LINK, NULL, NULL);
	return 0;
}

/**********************************************************************************
*	�������ƣ�OnFinishLink
*	�������ܣ����Դ����Զ������߳̽���ʱ���������̷��͵���Ϣ��
*			���������ڸı���������ذ�ť��״̬
*	��������������
*	�������أ���
*	�����д��shinn-2011-05-07
***********************************************************************************/

LRESULT CAutoLinkerDlg::OnFinishLink(WPARAM wParam, LPARAM lParam)
{
	//������Ӧ���̵߳Ľ���֪ͨ����������߳��㷨��ɶ���������m_startedӦ�û����棬ģ���ٵ��һ�¡�ֹͣ��������By shinn
	if (m_started == TRUE)
	{
		OnBnClickedAutolink();
	}
	//�������û������رյģ�����������By shinn
	return 0;
}

/**********************************************************************************
*	�������ƣ�LinkOne
*	�������ܣ������Զ�����ʱ����Ҫÿ�ζ��������ɵ�ͼ��
*		��Ϊ��ʵ��ÿ��������ͬ�ģ�����Ƶĺ�������ͬ�ڵ�������ʱ�ĵ�������
*	��������������
*	�������أ��п��������ģ�������������
*	�����д��shinn-2011-05-07
***********************************************************************************/

BOOL CAutoLinkerDlg::LinkOne()
{
	int line = 0, row = 0;
	int desLine = 0, desRow = 0;
	//�Զ�����ʱ����Ҫÿ�ζ��������ɵ�ͼ������Ҫ����Ӧλ�ôӵ�ͼ��ȥ����By shinn
	if (m_LinkHwnd == NULL)
	{
		return FALSE;
	}

	for (line = 0; line< BLOCK_NUM_H; line++)
	{
		for (row = 0; row< BLOCK_NUM_W; row++)
		{
			if (map[line][row].standing == FALSE)
			{
				continue;
			}

			if (SearchConnectable(line, row, desLine, desRow))
			{
				ClickPos(line, row, desLine, desRow);
				map[line][row].standing = FALSE;
				map[desLine][desRow].standing = FALSE;
				return TRUE;
			}
		}
	}
	return FALSE;
}

/**********************************************************************************
*	�������ƣ�PreTranslateMessage
*	�������ܣ���д��������ʹESC�����˳���Dialog
*	������������Ϣ
*	�������أ�����
*	�����д��shinn-2011-05-07
***********************************************************************************/

BOOL CAutoLinkerDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_ESCAPE)
		{
			//������ESC����Ϣ��By shinn
			return TRUE;
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}

/**********************************************************************************
*	�������ƣ�CheckWindowThread
*	�������ܣ���������������̣߳���qq���ڳ���ʱ����С���ڣ���qq������ʧʱҲ����С����
*	����������lpParameter����ǰ����ָ��
*	�������أ�����
*	�����д��shinn-2011-05-21
***********************************************************************************/
DWORD WINAPI CheckWindowThread(LPVOID lpParameter)
{
	CAutoLinkerDlg* pWnd = (CAutoLinkerDlg*)lpParameter;
	BOOL preStatus = FALSE;
	while(1)
	{
		//�õ����ھ����By shinn
		HWND hwnd = FindWindow(NULL, windowName);
		if (NULL != hwnd)
		{
			if (preStatus == FALSE)
			{
				//���״̬�ı䣨��ǰ�޴��������д��ڣ�������С����
				preStatus = TRUE;
				pWnd->NotifyBox(TEXT("�ѷ���QQ��Ϸ����"));
			}
			pWnd->SetLinkHwnd(hwnd);
			//�õ���ǰscreen�Ĵ�С��By shinn
			int cx = GetSystemMetrics(SM_CXSCREEN);
			int cy = GetSystemMetrics(SM_CYSCREEN);

			//������Ҵ��ں���Ϸ����λ��ʹ�䲻����ס��By shinn
			::SetWindowPos(pWnd->m_hWnd, NULL, cx/10+800, cy/10+150, 292, 228, SWP_NOSIZE);
			::SetWindowPos(hwnd, NULL, cx/10, cy/10, 800, 600, SWP_NOSIZE);
		}else
		{
			if (preStatus == TRUE)
			{
				//���״̬�ı䣨��ǰ�д��������޴��ڣ�������С����
				preStatus = FALSE;
				pWnd->NotifyBox(TEXT("QQ��Ϸ�����ѹر�"));
			}

			if (pWnd->GetLinkHwnd() != NULL)
			{
				pWnd->SetLinkHwnd(NULL);
			}
			Sleep(500);
			continue;
		}
	}
}
/**********************************************************************************
*	�������ƣ�NotifyBox
*	�������ܣ�����С���ں���
*	����������text��Ҫ��ʾ���ı�
*	�������أ�����
*	�����д��shinn-2011-05-21
***********************************************************************************/
void CAutoLinkerDlg::NotifyBox(CString text)
{
	int cx = GetSystemMetrics(SM_CXSCREEN);
	int cy = GetSystemMetrics(SM_CYSCREEN);
	::SetWindowPos(this->m_hWnd, HWND_TOPMOST, cx/10+800, cy/10+150, 292, 228, SWP_NOSIZE);
	m_taskBar.Show(text);
}

/**********************************************************************************
*	�������ƣ�GetLinkHwnd
*	�������ܣ����ص�ǰ���󱣴�����������ھ��
*	������������
*	�������أ����ھ��
*	�����д��shinn-2011-05-21
***********************************************************************************/
HWND CAutoLinkerDlg::GetLinkHwnd()
{
	return m_LinkHwnd;
}

/**********************************************************************************
*	�������ƣ�SetLinkHwnd
*	�������ܣ����õ�ǰ��������������ھ��
*	����������hwnd�����ھ��
*	�������أ���
*	�����д��shinn-2011-05-21
***********************************************************************************/
void CAutoLinkerDlg::SetLinkHwnd(HWND hwnd)
{
	m_LinkHwnd = hwnd;
}

/**********************************************************************************
*	�������ƣ�OnBnClickedButtonOption
*	�������ܣ�������ð�ť
*	������������
*	�������أ���
*	�����д��shinn-2011-05-22
***********************************************************************************/
void CAutoLinkerDlg::OnBnClickedButtonOption()
{
	COptionDlg option;
	option.SetMainDlg(this);
	option.DoModal();
}
/**********************************************************************************
*	�������ƣ�LoadIniFile
*	�������ܣ����������ļ�
*	������������
*	�������أ���
*	�����д��shinn-2011-05-22
***********************************************************************************/
void CAutoLinkerDlg::LoadIniFile()
{
	//��ȡ�ٶ�
	m_timerSpeed = ::GetPrivateProfileInt(TEXT("AutoLink"), TEXT("Speed"), 0, TEXT(".\\USER.ini"));
	if (m_timerSpeed == 0 || m_timerSpeed > 100)
	{
		CString speed;
		m_timerSpeed = 50;
		speed.Format(TEXT("%d"), m_timerSpeed);
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
*	�������ƣ�SetOption
*	�������ܣ������������
*	����������teachDlg���Ƿ�Ҫ������ѧ����
*					timerSpeed���Զ����ӵ��ٶ�
*					randomLink���Ƿ��������ģʽ
*	�������أ���
*	�����д��shinn-2011-05-22
***********************************************************************************/
void CAutoLinkerDlg::SetOption(int teachDlg, int timerSpeed, int randomLink)
{
	m_teachDlg		= teachDlg;
	m_timerSpeed	= timerSpeed;
	m_randomLink	= randomLink;
}