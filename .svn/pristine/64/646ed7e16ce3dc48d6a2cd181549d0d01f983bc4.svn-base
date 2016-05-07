#pragma once
#include "afxwin.h"


// CVersion 对话框

class CVersion : public CDialogEx
{
	DECLARE_DYNAMIC(CVersion)

public:
	CVersion(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CVersion();

// 对话框数据
	enum { IDD = IDD_VERSION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
//	afx_msg void OnVersion();
	virtual BOOL OnInitDialog();
	CString text;
	CEdit versionText;
};
