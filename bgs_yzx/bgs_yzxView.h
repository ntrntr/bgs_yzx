
// bgs_yzxView.h : Cbgs_yzxView ��Ľӿ�
//

#pragma once
#include "MyGBSDialog.h"

class Cbgs_yzxView : public CView
{
protected: // �������л�����
	Cbgs_yzxView();
	DECLARE_DYNCREATE(Cbgs_yzxView)

// ����
public:
	Cbgs_yzxDoc* GetDocument() const;

// ����
public:

// ��д
public:
	virtual void OnDraw(CDC* pDC);  // ��д�Ի��Ƹ���ͼ
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// ʵ��
public:
	virtual ~Cbgs_yzxView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);


public:
	CMyGBSDialog m_MyGBSDialog;
	afx_msg void OnSize(UINT nType, int cx, int cy);
};

#ifndef _DEBUG  // bgs_yzxView.cpp �еĵ��԰汾
inline Cbgs_yzxDoc* Cbgs_yzxView::GetDocument() const
   { return reinterpret_cast<Cbgs_yzxDoc*>(m_pDocument); }
#endif

