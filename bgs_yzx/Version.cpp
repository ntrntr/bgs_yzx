// Version.cpp : 实现文件
//

#include "stdafx.h"
#include "bgs_yzx.h"
#include "Version.h"
#include "afxdialogex.h"


// CVersion 对话框

IMPLEMENT_DYNAMIC(CVersion, CDialogEx)

CVersion::CVersion(CWnd* pParent /*=NULL*/)
	: CDialogEx(CVersion::IDD, pParent)
{

	vector<CString> tmp{ _T("//[12/20/2015 yzx]\r\n"), _T("增加Multiscale ST Backround, 增加来自opencv实现的codebook， 增加vibe+代码，增加my backround Subtract\r\n "),_T("[12/14/2015 yzx]\r\n"), _T("修复subsense不能打开\r\n") };
	text = _T("");
	for (auto i:tmp)
	{
		text.Append(i);
	}
}

CVersion::~CVersion()
{
}

void CVersion::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_VERSION_TEXT, versionText);
}


BEGIN_MESSAGE_MAP(CVersion, CDialogEx)
//	ON_COMMAND(ID_VERSION, &CVersion::OnVersion)
END_MESSAGE_MAP()


// CVersion 消息处理程序


//void CVersion::OnVersion()
//{
//	// TODO:  在此添加命令处理程序代码
//}


BOOL CVersion::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	//versionText.SetWindowText(text);
	versionText.SetWindowText(text);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}
