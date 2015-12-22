#pragma once
#include "afxwin.h"
#include "DragEdit.h"
#include "IBGS.h"


// CYzxFormView 窗体视图

class CYzxFormView : public CFormView
{
	DECLARE_DYNCREATE(CYzxFormView)

protected:
	CYzxFormView();           // 动态创建所使用的受保护的构造函数
	virtual ~CYzxFormView();

public:
	enum { IDD = IDD_YZXFORMVIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	CListBox m_BGSList;
	//afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//virtual BOOL OnInitDialog();
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
	//afx_msg void OnBnClickedButtonStart();
	CStatic m_log;
	CWinThread* yzxBGSthread;
	static DWORD BGSThread(LPVOID* pParam);
	void ThreadProcess();
	IBGS* bgs;
	bool isContinue;
	//afx_msg void OnBnClickedButtonSuspend();
	//afx_msg void OnBnClickedButtonResume();
	//afx_msg void OnBnClickedButtonEnd();
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
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnBnClickedButtonSuspend();
	afx_msg void OnBnClickedButtonResume();
	afx_msg void OnBnClickedButtonEnd();
	virtual void OnInitialUpdate();
	afx_msg void OnLbnSelchangeListboxBgslist();
};


