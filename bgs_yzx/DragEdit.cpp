// DragEdit.cpp : 实现文件
//

#include "stdafx.h"
#include "bgs_yzx.h"
#include "DragEdit.h"


// CDragEdit

IMPLEMENT_DYNAMIC(CDragEdit, CEdit)

CDragEdit::CDragEdit()
{

	m_filePath = _T("");
}

CDragEdit::~CDragEdit()
{
}


BEGIN_MESSAGE_MAP(CDragEdit, CEdit)
	ON_WM_CREATE()
	ON_WM_DROPFILES()
END_MESSAGE_MAP()



// CDragEdit 消息处理程序




int CDragEdit::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CEdit::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	DragAcceptFiles(TRUE);
	return 0;
}


void CDragEdit::OnDropFiles(HDROP hDropInfo)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	WORD wNumFilesDropped = DragQueryFile(hDropInfo, -1, NULL, 0);//同时拖放文件的数目
	CString firstFile = _T("");
	for (WORD x = 0; x < wNumFilesDropped; x++) //只记录第一个拖动的文件
	{
		WORD wPathnameSize = DragQueryFile(hDropInfo, x, NULL, 0);
		LPTSTR npszFile = (LPTSTR)LocalAlloc(LPTR, wPathnameSize += 1);//分配内存
		if (npszFile == NULL)
			continue;
		DragQueryFile(hDropInfo, x, npszFile, wPathnameSize);//获取拖放的文件名
		if (firstFile == "")
			firstFile = npszFile;//记录文件名
		LocalFree(npszFile);//释放内存
	}
	DragFinish(hDropInfo);//拖放结束
	SetWindowText(firstFile);//编辑框显示
	//CEdit::OnDropFiles(hDropInfo);
	m_filePath = firstFile;
	CEdit::OnDropFiles(hDropInfo);
}
