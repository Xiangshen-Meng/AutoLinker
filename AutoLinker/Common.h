#pragma once
//��ʼλ�ã����أ���By shinn
#define  STARTPOS_X		12
#define  STARTPOS_Y		182
//ͼ���С�����أ���By shinn
#define BLOCK_HIGHT		35
#define BLOCK_WIDTH		31
//ͼ�������By shinn
#define BLOCK_NUM_W	19
#define BLOCK_NUM_H	11
//ȡͼ��ʱ��ƫ�ƣ�By shinn
#define OFFSET_PIXEL		5
//QQ�������ı���ɫ��By shinn
#define BACK_GROUND_COLOR 7359536
//��ʱ��ԭ�ӣ���ʱʱ��=��ʱ����ʱ��+��ʱ��ԭ��*ԭ����-m_timerSpeed��
#define TIMER_DEFAULT 10
#define TIMER_BASE			300
//�̵߳Ļط���Ϣ
#define WM_FINISH_LINK WM_USER+1
//��Ϸ������
const TCHAR windowName[] = TEXT("QQ��Ϸ - ��������ɫ��");
//ͼ��ṹ�壨ȡ����5�����ص��color��������Ƿ�Ϊ����-�Ƿ���ͼ�飩
typedef struct  BITBLOCK
{
	COLORREF pointMid;
	COLORREF pointUp;
	COLORREF pointDown;
	COLORREF pointLeft;
	COLORREF pointRight;
	BOOL		  standing;
}BitBlock;
//�������ڵ�����ö��
typedef enum ERRORTYPE
{
	ErrorDlg,
	AllLinkDlg
}ErrorType;