
// AutoLinkerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "AutoLinker.h"
#include "AutoLinkerDlg.h"
#include "afxdialogex.h"

/*******************引入头文件：By shinn*****************/
#include "TeachDlg.h"
#include "ErrorDlg.h"
#include "OptionDlg.h"
/*******************引入头文件：By shinn*****************/

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DWORD WINAPI LinkThread(LPVOID lpParameter);
DWORD WINAPI CheckWindowThread(LPVOID lpParameter);
// CAutoLinkerDlg 对话框




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


// CAutoLinkerDlg 消息处理程序

BOOL CAutoLinkerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	LoadIniFile();							//加载配置文件

	//建立教学Dialog（1为启动教学窗口，2为不启动）：By shinn
	if(m_teachDlg == 1)
	{
		CTeachDlg teach;
		teach.DoModal();
	}

	//初始化背景图片：By shinn
	CDC* sysDC = GetDC();

	m_backDC.CreateCompatibleDC(sysDC);
	m_backBitmap.LoadBitmap(IDB_BITMAP_BACKGROUND);
	m_backDC.SelectObject(m_backBitmap);
	//初始化右下角小窗口：By shinn
	m_taskBar.Create(this);
	m_taskBar.SetSkin(IDB_BITMAP_NOTIFY, 255, 0, 255);
	m_taskBar.SetTextFont(TEXT("Arial Black"),120,TN_TEXT_NORMAL,TN_TEXT_UNDERLINE | TN_TEXT_BOLD);
	m_taskBar.SetTextColor(RGB(0,0,0),RGB(0,0,200));
	m_taskBar.SetTextRect(CRect(10, 30, m_taskBar.m_nSkinWidth-10, m_taskBar.m_nSkinHeight-30));
	//记录当前时间：By shinn
	m_preSingleTime = GetTickCount();

	CreateThread(0, 0, CheckWindowThread, this, 0, NULL);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CAutoLinkerDlg::OnPaint()
{
	CPaintDC dc(this); // 用于绘制的设备上下文

	if (IsIconic())
	{
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		//绘制背景图片：By shinn
		dc.StretchBlt(0, 0, 300, 240, &m_backDC, 0, 0, 292, 228, SRCCOPY);

		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CAutoLinkerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

/**********************************************************************************
*	函数名称：CheckLinkHwnd
*	函数功能：判断当前句柄是否为空，如果为空
*	函数参数：无
*	函数返回：有无句柄
*	代码编写：shinn-2011-05-20
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
*	函数名称：InitalMap
*	函数功能：通过句柄得到DC，进行图像的采取及分析，存入地图中
*	函数参数：无
*	函数返回：是否成功
*	代码编写：shinn-2011-05-07
***********************************************************************************/

BOOL CAutoLinkerDlg::InitalMap()
{
	int line = 0;
	int row = 0;
	//使游戏窗口为最上，方便截图（但是并不好用。。。）：By shinn
	::SetWindowPos(m_LinkHwnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	HDC hDC = ::GetDC(m_LinkHwnd);



	if (hDC == NULL)
	{
		return FALSE;
	}
	//得到关键点的像素以生成地图：By shinn
	for (line = 0; line <BLOCK_NUM_H; line++)
	{
		for (row = 0; row < BLOCK_NUM_W; row++)
		{
			map[line][row].pointMid				= GetPixel(hDC, STARTPOS_X+row*BLOCK_WIDTH+BLOCK_WIDTH/2, STARTPOS_Y+line*BLOCK_HIGHT+BLOCK_HIGHT/2);
			map[line][row].pointUp				= GetPixel(hDC, STARTPOS_X+row*BLOCK_WIDTH+BLOCK_WIDTH/2, STARTPOS_Y+line*BLOCK_HIGHT+BLOCK_HIGHT/2-OFFSET_PIXEL);
			map[line][row].pointDown			= GetPixel(hDC, STARTPOS_X+row*BLOCK_WIDTH+BLOCK_WIDTH/2, STARTPOS_Y+line*BLOCK_HIGHT+BLOCK_HIGHT/2+OFFSET_PIXEL);
			map[line][row].pointLeft			= GetPixel(hDC, STARTPOS_X+row*BLOCK_WIDTH+BLOCK_WIDTH/2-OFFSET_PIXEL, STARTPOS_Y+line*BLOCK_HIGHT+BLOCK_HIGHT/2);
			map[line][row].pointRight			= GetPixel(hDC, STARTPOS_X+row*BLOCK_WIDTH+BLOCK_WIDTH/2+OFFSET_PIXEL, STARTPOS_Y+line*BLOCK_HIGHT+BLOCK_HIGHT/2);
			
			//如果都是背景色的话就是无图块，否则设置为有图块，便于以后判断路径可通：By shinn
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
*	函数名称：SearchConnectable
*	函数功能：通过输入一个点在地图中的位置，
*		可以得到能和它连上的一个点并通过desLine和desRow返回
*	函数参数：line：	给出的点在地图中的纵坐标
*			  row：		给出的点在地图中的横坐标
*			  desLine：	用于返回找到的可连的点在地图中的纵坐标
*			  desRow：	用于返回找到的可连的点在地图中的横坐标
*	函数返回：是否成功
*	代码编写：shinn-2011-05-07
***********************************************************************************/

BOOL CAutoLinkerDlg::SearchConnectable(int line, int row, int& desLine, int& desRow)
{
	//当前的传入位置根本无图块时直接返回：By shinn
	if (map[line][row].standing == FALSE)
	{
		return FALSE;
	}
	//向上判断是否能找到一样并可通的图块：By shinn
	if (UpLineCheck(line, row, desLine, desRow))
	{
		return TRUE;
	}
	//向下判断是否能找到一样并可通的图块：By shinn
	if (DownLineCheck(line, row, desLine, desRow))
	{
		return TRUE;
	}
	//向左判断是否能找到一样并可通的图块：By shinn
	if (LeftRowCheck(line, row, desLine, desRow))
	{
		return TRUE;
	}
	//向右判断是否能找到一样并可通的图块：By shinn
	if (RightRowCheck(line, row, desLine, desRow))
	{
		return TRUE;
	}

	return FALSE;
}

/**********************************************************************************
*	函数名称：UpLineCheck
*	函数功能：向上部分的查找功能
*	函数参数：line：	给出的点在地图中的纵坐标
*			  row：		给出的点在地图中的横坐标
*			  desLine：	用于返回找到的可连的点在地图中的纵坐标
*			  desRow：	用于返回找到的可连的点在地图中的横坐标
*	函数返回：是否成功
*	代码编写：shinn-2011-05-07
***********************************************************************************/

BOOL CAutoLinkerDlg::UpLineCheck(int line, int row, int& desLine, int& desRow)
{
	//分别代表三条线：By shinn
	int upFirst = 0, upSecond = 0, upThird = 0;
	/********************************向上找*****************************************/
	//向上进行查找：By shinn
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
		/********************************向上后向两边找中的向左*************************************/
		//向上后向左转：By shinn
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
			//向上后向左转再向上找：By shinn
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
			//向上后向左转再向下找：By shinn
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
		/********************************向上后向两边找中的向右*************************************/
		//向上后向右转：By shinn
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
			//向上后向右转再向上找：By shinn
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
			//向上后向右转再向下找：By shinn
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
*	函数名称：DownLineCheck
*	函数功能：向下部分的查找功能
*	函数参数：line：	给出的点在地图中的纵坐标
*			  row：		给出的点在地图中的横坐标
*			  desLine：	用于返回找到的可连的点在地图中的纵坐标
*			  desRow：	用于返回找到的可连的点在地图中的横坐标
*	函数返回：是否成功
*	代码编写：shinn-2011-05-07
***********************************************************************************/

BOOL CAutoLinkerDlg::DownLineCheck(int line, int row, int& desLine, int& desRow)
{
	//分别代表三条线：By shinn
	int downFirst = 0, downSecond = 0, downThird = 0;
	/********************************向下找*****************************************/
	//向下进行查找：By shinn
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
		/********************************向下后向两边找中的向左*************************************/
		//向下后向左转：By shinn
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
			//向下后向左转再向上找：By shinn
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
			//向下后向左转再向下找：By shinn
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
		/********************************向下后向两边找中的向右*************************************/
		//向下后向右转：By shinn
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
			//向下后向右转再向上找：By shinn
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
			//向下后向右转再向下找：By shinn
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
*	函数名称：LeftRowCheck
*	函数功能：向左部分的查找功能
*	函数参数：line：	给出的点在地图中的纵坐标
*			  row：		给出的点在地图中的横坐标
*			  desLine：	用于返回找到的可连的点在地图中的纵坐标
*			  desRow：	用于返回找到的可连的点在地图中的横坐标
*	函数返回：是否成功
*	代码编写：shinn-2011-05-07
***********************************************************************************/

BOOL CAutoLinkerDlg::LeftRowCheck(int line, int row, int& desLine, int& desRow)
{
	//分别代表三条线：By shinn
	int leftFirst = 0, leftSecond = 0, leftThird = 0;
	/********************************向左找*****************************************/
	//向左进行查找：By shinn
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
		/********************************向左后向上下找中的向上*************************************/
		//向左后向上转：By shinn
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
			//向左后向上转再向左找：By shinn
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
			//向左后向上转再向右找：By shinn
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
		/********************************向左后向上下找中的向下*************************************/
		//向左后向下转：By shinn
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
			//向左后向下转再向左找：By shinn
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
			//向左后向下转再向右找：By shinn
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
*	函数名称：RightRowCheck
*	函数功能：向右部分的查找功能
*	函数参数：line：	给出的点在地图中的纵坐标
*			  row：		给出的点在地图中的横坐标
*			  desLine：	用于返回找到的可连的点在地图中的纵坐标
*			  desRow：	用于返回找到的可连的点在地图中的横坐标
*	函数返回：是否成功
*	代码编写：shinn-2011-05-07
***********************************************************************************/

BOOL CAutoLinkerDlg::RightRowCheck(int line, int row, int& desLine, int& desRow)
{
	//分别代表三条线：By shinn
	int rightFirst = 0, rightSecond = 0, rightThird = 0;
	/********************************向右找*****************************************/
	//向右进行查找：By shinn
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
		/********************************向右后向上下找中的向上*************************************/
		//向右后向上转：By shinn
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
			//向右后向上转再向左找：By shinn
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
			//向右后向上转再向右找：By shinn
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
		/********************************向右后向上下找中的向下*************************************/
		//向右后向下转：By shinn
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
			//向左后向下转再向左找：By shinn
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
			//向左后向下转再向右找：By shinn
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
*	函数名称：CheckSame
*	函数功能：判断是否为一样的图块
*	函数参数：b1：		第一个图块
*			  b2：		第二个图块
*	函数返回：是否一样
*	代码编写：shinn-2011-05-07
***********************************************************************************/

BOOL CAutoLinkerDlg::CheckSame(BitBlock& b1, BitBlock& b2)
{
	//颜色全一样就是一种图块：By shinn
	if (b1.pointDown == b2.pointDown && b1.pointLeft == b2.pointLeft && b1.pointMid == b2.pointMid && b1.pointRight == b2.pointRight && b1.pointUp == b2.pointUp)
	{
		return TRUE;
	}else
	{
		return FALSE;
	}
}

/**********************************************************************************
*	函数名称：LinkSingle
*	函数功能：点击单个消除时，如果得到QQ句柄的操作
*	函数参数：无
*	函数返回：是否成功（无太大用处）
*	代码编写：shinn-2011-05-07
***********************************************************************************/

BOOL CAutoLinkerDlg::LinkSingle()
{
	if (CheckLinkHwnd() == TRUE)
	{
		//判断2次点击单个消除的间隔是不是大于1s秒，大于1s则生成地图
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
*	函数名称：ClickPos
*	函数功能：通过输入的2个点，模拟鼠标事件进行消去
*	函数参数：line：	第一个点在地图中的纵坐标
*			  row：		第一个点在地图中的横坐标
*			  desLine：	第二个点在地图中的纵坐标
*			  desRow：	第二个点在地图中的横坐标
*	函数返回：无
*	代码编写：shinn-2011-05-07
***********************************************************************************/

void CAutoLinkerDlg::ClickPos(int line, int row, int desLine, int desRow)
{
	POINT clickPos;
	POINT p;
	//这置游戏窗口在最上：By shinn
	::SetWindowPos(m_LinkHwnd, 0, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	//将地图中的位置转化为窗口坐标位置：By shinn
	clickPos.x = STARTPOS_X+row*BLOCK_WIDTH+BLOCK_WIDTH/2;
	clickPos.y = STARTPOS_Y+line*BLOCK_HIGHT+BLOCK_HIGHT/2;
	//将窗口坐标转化为screen坐标：By shinn
	::ClientToScreen(m_LinkHwnd, &clickPos);

	//移动到指定位置模拟点击：By shinn
	GetCursorPos(&p);
	::SetCursorPos(clickPos.x, clickPos.y);
	mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);

	//第二个点和第一个点相同的操作：By shinn
	clickPos.x = STARTPOS_X+desRow*BLOCK_WIDTH+BLOCK_WIDTH/2;
	clickPos.y = STARTPOS_Y+desLine*BLOCK_HIGHT+BLOCK_HIGHT/2;

	::ClientToScreen(m_LinkHwnd, &clickPos);
	::SetCursorPos(clickPos.x, clickPos.y);
	mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
	::SetCursorPos(p.x, p.y);
}

/**********************************************************************************
*	函数名称：LinkAll
*	函数功能：点击秒杀的时候，成功得到句柄时进行的操作
*	函数参数：无
*	函数返回：是否成功（无用）
*	代码编写：shinn-2011-05-07
***********************************************************************************/

BOOL CAutoLinkerDlg::LinkAll()
{
	if (m_LinkHwnd == NULL)
	{
		return FALSE;
	}
	InitalMap();
	//还有图块的时候没被消除时，进行一次快速连接：By shinn
	while (!CheckDone())
	{
		//当快速连接已经没有可以连接的图块时结束：By shinn
		if (QuickLink() == FALSE)
		{
			break;
		}
	}
	return TRUE;
}

/**********************************************************************************
*	函数名称：OnBnClickedSinglelink
*	函数功能：按键点击事件
*	函数参数：无
*	函数返回：无
*	代码编写：shinn-2011-05-07
***********************************************************************************/

void CAutoLinkerDlg::OnBnClickedSinglelink()
{
	if (CheckLinkHwnd() == TRUE)
	{
		LinkSingle();
	}
}

/**********************************************************************************
*	函数名称：OnBnClickedAutolink
*	函数功能：按键点击事件
*	函数参数：无
*	函数返回：无
*	代码编写：shinn-2011-05-07
***********************************************************************************/

void CAutoLinkerDlg::OnBnClickedAutolink()
{
	if (CheckLinkHwnd() == TRUE)
	{
		//m_started用于控制自动消除线程是否进行当m_started为FALSE时，线程结束，现时向主窗口发送消息通知窗口改变按键等状态：By shinn
		if (m_started == FALSE)
		{
			//当m_started为假时说明当前用户希望启动自动消除，所以启动线程并对相应视图进行修改：By shinn
			((CButton*)GetDlgItem(IDC_AUTOLINK))->SetWindowText(TEXT("停止消除"));
			((CButton*)GetDlgItem(IDC_BUTTON_OPTION))->EnableWindow(FALSE);
			((CButton*)GetDlgItem(IDC_ALLLINK))->EnableWindow(FALSE);
			((CButton*)GetDlgItem(IDC_SINGLELINK))->EnableWindow(FALSE);

			m_started = TRUE;

			InitalMap();
			//启动线程
			CreateThread(0, 0, LinkThread, this, 0, NULL);
		}else
		{
			//如果为真，说明用户希望关闭自动连接，相m_started设置为FALSE并改变相应视图：By shinn
			((CButton*)GetDlgItem(IDC_AUTOLINK))->SetWindowText(TEXT("自动消除"));
			((CButton*)GetDlgItem(IDC_ALLLINK))->EnableWindow(TRUE);
			((CButton*)GetDlgItem(IDC_SINGLELINK))->EnableWindow(TRUE);
			((CButton*)GetDlgItem(IDC_BUTTON_OPTION))->EnableWindow(TRUE);

			m_started = FALSE;
		}
	}
}

/**********************************************************************************
*	函数名称：OnBnClickedAlllink
*	函数功能：按键点击事件
*	函数参数：无
*	函数返回：无
*	代码编写：shinn-2011-05-07
***********************************************************************************/

void CAutoLinkerDlg::OnBnClickedAlllink()
{
	if (CheckLinkHwnd() == TRUE)
	{
		LinkAll();
		//提示用记少用秒杀。。。容易被发现。。。：By shinn
		CErrorDlg err;
		err.SetErrorType(AllLinkDlg);
		err.DoModal();
	}
}

/**********************************************************************************
*	函数名称：CheckDone
*	函数功能：检测地图是否完成
*	函数参数：无
*	函数返回：是否完成
*	代码编写：shinn-2011-05-07
***********************************************************************************/

BOOL CAutoLinkerDlg::CheckDone()
{
	//检测完成：By shinn
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
*	函数名称：QuickLink
*	函数功能：秒杀功能中的核心，快速连接
*	函数参数：无
*	函数返回：只要有点个可消除的图块被消除就返回真
*	代码编写：shinn-2011-05-07
***********************************************************************************/

BOOL CAutoLinkerDlg::QuickLink()
{
	//快速连接的算法：从头开始找能连上的图块，如果能边就模拟点击消除，并将地图中的相应位置的standing设为FALSE
	//但是不退出循环而是继续向下找，直到将地图全扫一遍。这时地图中的有些图块还没有被消除，因为当扫它的时候它无法被消除，
	//但在消除其它块后，可以被消除了，此时需要CheckDone一下，然后再进行一次QuickLink也就是LinkAll中实现的：By shinn
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
*	函数名称：LinkThread
*	函数功能：自动连接时，需要启动的线程
*	函数参数：lpParameter：实为运行的CAutoLinkerDlg的this指针，用以调用其函数
*	函数返回：无
*	代码编写：shinn-2011-05-07
***********************************************************************************/

DWORD WINAPI LinkThread(LPVOID lpParameter)
{
	CAutoLinkerDlg* pMainDlg = ((CAutoLinkerDlg*)lpParameter);
	//m_started为开关，当为真时线程进行，当为假时线程结束：By shinn
	while(pMainDlg->LinkOne() && pMainDlg->m_started)
	{
		//sleep延时的时间，如果为随机睡眠模式则睡随机的时间：By shinn
		if (pMainDlg->m_randomLink == 1)
		{
			Sleep(TIMER_DEFAULT*(rand()%100)+TIMER_BASE);
		}else
		{
			Sleep((100-pMainDlg->m_timerSpeed)*TIMER_DEFAULT+TIMER_BASE);
		}
	}

	//线程结束通知主线程做相应改变：By shinn
	::SendMessage(pMainDlg->m_hWnd, WM_FINISH_LINK, NULL, NULL);
	return 0;
}

/**********************************************************************************
*	函数名称：OnFinishLink
*	函数功能：用以处理自动连接线程结束时，向主进程发送的消息，
*			其作用在于改变主进行相关按钮的状态
*	函数参数：无用
*	函数返回：无
*	代码编写：shinn-2011-05-07
***********************************************************************************/

LRESULT CAutoLinkerDlg::OnFinishLink(WPARAM wParam, LPARAM lParam)
{
	//用于响应子线程的结束通知，如果由于线程算法完成而结束，则m_started应该还是真，模拟再点击一下“停止消除”：By shinn
	if (m_started == TRUE)
	{
		OnBnClickedAutolink();
	}
	//否则是用户主动关闭的，不用做处理：By shinn
	return 0;
}

/**********************************************************************************
*	函数名称：LinkOne
*	函数功能：由于自动连接时不需要每次都进行生成地图，
*		但为了实现每次消除不同的，而设计的函数。不同于单个消除时的单个消除
*	函数参数：无用
*	函数返回：有可以消除的，消除并返回真
*	代码编写：shinn-2011-05-07
***********************************************************************************/

BOOL CAutoLinkerDlg::LinkOne()
{
	int line = 0, row = 0;
	int desLine = 0, desRow = 0;
	//自动连接时不需要每次都进行生成地图，但是要将相应位置从地图中去掉：By shinn
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
*	函数名称：PreTranslateMessage
*	函数功能：重写函数用于使ESC键不退出主Dialog
*	函数参数：消息
*	函数返回：无用
*	代码编写：shinn-2011-05-07
***********************************************************************************/

BOOL CAutoLinkerDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_ESCAPE)
		{
			//不解析ESC的消息：By shinn
			return TRUE;
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}

/**********************************************************************************
*	函数名称：CheckWindowThread
*	函数功能：检测连连看窗口线程，当qq窗口出现时弹出小窗口，当qq窗口消失时也弹出小窗口
*	函数参数：lpParameter：当前窗口指针
*	函数返回：无用
*	代码编写：shinn-2011-05-21
***********************************************************************************/
DWORD WINAPI CheckWindowThread(LPVOID lpParameter)
{
	CAutoLinkerDlg* pWnd = (CAutoLinkerDlg*)lpParameter;
	BOOL preStatus = FALSE;
	while(1)
	{
		//得到窗口句柄：By shinn
		HWND hwnd = FindWindow(NULL, windowName);
		if (NULL != hwnd)
		{
			if (preStatus == FALSE)
			{
				//如果状态改变（先前无窗口现在有窗口），启动小窗口
				preStatus = TRUE;
				pWnd->NotifyBox(TEXT("已发现QQ游戏窗口"));
			}
			pWnd->SetLinkHwnd(hwnd);
			//得到当前screen的大小：By shinn
			int cx = GetSystemMetrics(SM_CXSCREEN);
			int cy = GetSystemMetrics(SM_CYSCREEN);

			//设置外挂窗口和游戏窗口位置使其不被挡住：By shinn
			::SetWindowPos(pWnd->m_hWnd, NULL, cx/10+800, cy/10+150, 292, 228, SWP_NOSIZE);
			::SetWindowPos(hwnd, NULL, cx/10, cy/10, 800, 600, SWP_NOSIZE);
		}else
		{
			if (preStatus == TRUE)
			{
				//如果状态改变（先前有窗口现在无窗口），启动小窗口
				preStatus = FALSE;
				pWnd->NotifyBox(TEXT("QQ游戏窗口已关闭"));
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
*	函数名称：NotifyBox
*	函数功能：弹出小窗口函数
*	函数参数：text：要显示的文本
*	函数返回：无用
*	代码编写：shinn-2011-05-21
***********************************************************************************/
void CAutoLinkerDlg::NotifyBox(CString text)
{
	int cx = GetSystemMetrics(SM_CXSCREEN);
	int cy = GetSystemMetrics(SM_CYSCREEN);
	::SetWindowPos(this->m_hWnd, HWND_TOPMOST, cx/10+800, cy/10+150, 292, 228, SWP_NOSIZE);
	m_taskBar.Show(text);
}

/**********************************************************************************
*	函数名称：GetLinkHwnd
*	函数功能：返回当前对象保存的连连看窗口句柄
*	函数参数：无
*	函数返回：窗口句柄
*	代码编写：shinn-2011-05-21
***********************************************************************************/
HWND CAutoLinkerDlg::GetLinkHwnd()
{
	return m_LinkHwnd;
}

/**********************************************************************************
*	函数名称：SetLinkHwnd
*	函数功能：设置当前对象的连连看窗口句柄
*	函数参数：hwnd：窗口句柄
*	函数返回：无
*	代码编写：shinn-2011-05-21
***********************************************************************************/
void CAutoLinkerDlg::SetLinkHwnd(HWND hwnd)
{
	m_LinkHwnd = hwnd;
}

/**********************************************************************************
*	函数名称：OnBnClickedButtonOption
*	函数功能：点击设置按钮
*	函数参数：无
*	函数返回：无
*	代码编写：shinn-2011-05-22
***********************************************************************************/
void CAutoLinkerDlg::OnBnClickedButtonOption()
{
	COptionDlg option;
	option.SetMainDlg(this);
	option.DoModal();
}
/**********************************************************************************
*	函数名称：LoadIniFile
*	函数功能：加载配置文件
*	函数参数：无
*	函数返回：无
*	代码编写：shinn-2011-05-22
***********************************************************************************/
void CAutoLinkerDlg::LoadIniFile()
{
	//读取速度
	m_timerSpeed = ::GetPrivateProfileInt(TEXT("AutoLink"), TEXT("Speed"), 0, TEXT(".\\USER.ini"));
	if (m_timerSpeed == 0 || m_timerSpeed > 100)
	{
		CString speed;
		m_timerSpeed = 50;
		speed.Format(TEXT("%d"), m_timerSpeed);
		::WritePrivateProfileString(TEXT("AutoLink"), TEXT("Speed"), speed, TEXT(".\\USER.ini"));
	}
	//读取是否要跳过教学
	m_teachDlg = ::GetPrivateProfileInt(TEXT("Other"), TEXT("TeachDlg"), 0, TEXT(".\\USER.ini"));
	if (m_teachDlg == 0 || (m_teachDlg != 1 && m_teachDlg != 2))
	{
		CString teachDlg;
		m_teachDlg = 1;				//1为开启教学窗口
		teachDlg.Format(TEXT("%d"), m_teachDlg);
		::WritePrivateProfileString(TEXT("Other"), TEXT("TeachDlg"), teachDlg, TEXT(".\\USER.ini"));
	}
	//读取是否为随机模式
	m_randomLink = ::GetPrivateProfileInt(TEXT("AutoLink"), TEXT("RandomLink"), 0, TEXT(".\\USER.ini"));
	if (m_randomLink == 0 || (m_randomLink != 1 && m_randomLink != 2))
	{
		CString randomLink;
		m_randomLink = 2;				//2为关闭随机速度
		randomLink.Format(TEXT("%d"), m_randomLink);
		::WritePrivateProfileString(TEXT("AutoLink"), TEXT("RandomLink"), randomLink, TEXT(".\\USER.ini"));
	}
}
/**********************************************************************************
*	函数名称：SetOption
*	函数功能：设置相关属性
*	函数参数：teachDlg：是否要开启教学窗口
*					timerSpeed：自动连接的速度
*					randomLink：是否开启了随机模式
*	函数返回：无
*	代码编写：shinn-2011-05-22
***********************************************************************************/
void CAutoLinkerDlg::SetOption(int teachDlg, int timerSpeed, int randomLink)
{
	m_teachDlg		= teachDlg;
	m_timerSpeed	= timerSpeed;
	m_randomLink	= randomLink;
}