// Version.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "bgs_yzx.h"
#include "Version.h"
#include "afxdialogex.h"


// CVersion �Ի���

IMPLEMENT_DYNAMIC(CVersion, CDialogEx)

CVersion::CVersion(CWnd* pParent /*=NULL*/)
	: CDialogEx(CVersion::IDD, pParent)
{

	vector<CString> tmp{ _T("//[12/20/2015 yzx]\r\n"), _T("����Multiscale ST Backround, ��������opencvʵ�ֵ�codebook�� ����vibe+���룬����my backround Subtract\r\n "),_T("[12/14/2015 yzx]\r\n"), _T("�޸�subsense���ܴ�\r\n") };
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


// CVersion ��Ϣ�������


//void CVersion::OnVersion()
//{
//	// TODO:  �ڴ���������������
//}


BOOL CVersion::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	//versionText.SetWindowText(text);
	versionText.SetWindowText(text);
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}
