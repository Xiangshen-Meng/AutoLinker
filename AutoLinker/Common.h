#pragma once
//起始位置（像素）：By shinn
#define  STARTPOS_X		12
#define  STARTPOS_Y		182
//图块大小（像素）：By shinn
#define BLOCK_HIGHT		35
#define BLOCK_WIDTH		31
//图块个数：By shinn
#define BLOCK_NUM_W	19
#define BLOCK_NUM_H	11
//取图像时的偏移：By shinn
#define OFFSET_PIXEL		5
//QQ连连看的背景色：By shinn
#define BACK_GROUND_COLOR 7359536
//延时的原子（延时时间=延时基础时间+延时的原子*原子数-m_timerSpeed）
#define TIMER_DEFAULT 10
#define TIMER_BASE			300
//线程的回发消息
#define WM_FINISH_LINK WM_USER+1
//游戏窗口名
const TCHAR windowName[] = TEXT("QQ游戏 - 连连看角色版");
//图块结构体（取出的5个像素点的color及这个点是否为背景-是否有图块）
typedef struct  BITBLOCK
{
	COLORREF pointMid;
	COLORREF pointUp;
	COLORREF pointDown;
	COLORREF pointLeft;
	COLORREF pointRight;
	BOOL		  standing;
}BitBlock;
//弹出窗口的类型枚举
typedef enum ERRORTYPE
{
	ErrorDlg,
	AllLinkDlg
}ErrorType;