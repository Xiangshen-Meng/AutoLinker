
// AutoLinker.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CAutoLinkerApp:
// �йش����ʵ�֣������ AutoLinker.cpp
//

class CAutoLinkerApp : public CWinApp
{
public:
	CAutoLinkerApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CAutoLinkerApp theApp;
