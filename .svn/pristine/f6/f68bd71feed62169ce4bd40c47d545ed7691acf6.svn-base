#pragma once


// CDragEdit

class CDragEdit : public CEdit
{
	DECLARE_DYNAMIC(CDragEdit)

public:
	CDragEdit();
	virtual ~CDragEdit();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	CString m_filePath;
};


