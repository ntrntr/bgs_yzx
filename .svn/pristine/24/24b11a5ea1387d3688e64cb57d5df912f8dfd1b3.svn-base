#pragma once
#include "afxwin.h"
#include "DragEdit.h"
#include "IBGS.h"


// CMyGBSDialog 对话框

class CMyGBSDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CMyGBSDialog)

public:
	CMyGBSDialog(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMyGBSDialog();

// 对话框数据
	enum { IDD = IDD_MYGBSDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	CListBox m_BGSList;
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnInitDialog();
	void addBgsList();
	CString m_methodName;
	void initMemberVariable();
	CDragEdit m_dragEdit;
	CEdit m_delay;
	CStatic m_execTime;
	CStatic m_frameNumber;
	bool started;
	// getBgsMethodName
	bool getBgsMethodName();
	// bool CDlg::getInputVideoFilePath()
	bool getInputVideoFilePath();
	CString m_filePath;
	CString m_filePathAppend;
	CString m_saveMaskOutPath;
	afx_msg void OnBnClickedButtonStart();
	CStatic m_log;
	CWinThread* yzxBGSthread;
	static DWORD BGSThread(LPVOID* pParam);
	void ThreadProcess();
	IBGS* bgs;
	bool isContinue;
	afx_msg void OnBnClickedButtonSuspend();
	afx_msg void OnBnClickedButtonResume();
	afx_msg void OnBnClickedButtonEnd();
	CButton m_saveMask;
	// 保存文件
	void saveImage(cv::Mat& image, CString outpath, CString filename, int count);
	CEdit m_saveMaskPath;
	bool isSaveMaskImage();
	CString m_maskfilename;
	CComboBox m_filterList;
	void addFilterList();
	void modifyMask(cv::Mat& image, CString& filtername);
	CString getFilterName();
	CComboBox m_videoPathAppendControl;
	void addVideoPathAppend();
};
