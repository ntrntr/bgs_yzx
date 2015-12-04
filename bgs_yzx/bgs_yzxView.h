
// bgs_yzxView.h : Cbgs_yzxView 类的接口
//

#pragma once
#include "MyGBSDialog.h"

class Cbgs_yzxView : public CView
{
protected: // 仅从序列化创建
	Cbgs_yzxView();
	DECLARE_DYNCREATE(Cbgs_yzxView)

// 特性
public:
	Cbgs_yzxDoc* GetDocument() const;

// 操作
public:

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// 实现
public:
	virtual ~Cbgs_yzxView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);


public:
	CMyGBSDialog m_MyGBSDialog;
	afx_msg void OnSize(UINT nType, int cx, int cy);
};

#ifndef _DEBUG  // bgs_yzxView.cpp 中的调试版本
inline Cbgs_yzxDoc* Cbgs_yzxView::GetDocument() const
   { return reinterpret_cast<Cbgs_yzxDoc*>(m_pDocument); }
#endif

