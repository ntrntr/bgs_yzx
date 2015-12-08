
// bgs_yzxView.cpp : Cbgs_yzxView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "bgs_yzx.h"
#endif

#include "bgs_yzxDoc.h"
#include "bgs_yzxView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Cbgs_yzxView

IMPLEMENT_DYNCREATE(Cbgs_yzxView, CView)

BEGIN_MESSAGE_MAP(Cbgs_yzxView, CView)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

// Cbgs_yzxView 构造/析构

Cbgs_yzxView::Cbgs_yzxView()
{
	// TODO:  在此处添加构造代码

}

Cbgs_yzxView::~Cbgs_yzxView()
{
}

BOOL Cbgs_yzxView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO:  在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式
	return CView::PreCreateWindow(cs);
}

// Cbgs_yzxView 绘制

void Cbgs_yzxView::OnDraw(CDC* /*pDC*/)
{
	Cbgs_yzxDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO:  在此处为本机数据添加绘制代码
}


// Cbgs_yzxView 诊断

#ifdef _DEBUG
void Cbgs_yzxView::AssertValid() const
{
	CView::AssertValid();
}

void Cbgs_yzxView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

Cbgs_yzxDoc* Cbgs_yzxView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(Cbgs_yzxDoc)));
	return (Cbgs_yzxDoc*)m_pDocument;
}
#endif //_DEBUG


// Cbgs_yzxView 消息处理程序


int Cbgs_yzxView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	//SetWindowPos(NULL, 0, 0, 500, 500, SWP_NOMOVE);
	m_MyGBSDialog.Create(IDD_MYGBSDIALOG, this);
	m_MyGBSDialog.ShowWindow(SW_SHOW);
	//CRect rect;
	//m_MyGBSDialog.GetClientRect(&rect);
	//m_MyGBSDialog.SetWindowPos(this, 100, 100, rect.Width(), rect.Height(), SWP_NOZORDER);
	
	//SetWindowPos(NULL, 5000, 200, rect.Width(), rect.Height(), SWP_NOZORDER);
	//AfxGetMainWnd()->MoveWindow(500, 100, rect.Width(), rect.Height(), TRUE);
	//this->MoveWindow();
	//ShellExecute(NULL, _T("open"), _T("./config/MixtureOfGaussianV1BGS.xml"), NULL, NULL, SW_SHOW);
	return 0;
}


void Cbgs_yzxView::OnSize(UINT nType, int cx, int cy)
{
	CRect rc;
	m_MyGBSDialog.GetClientRect(&rc);
	MoveWindow(&rc);
	CView::OnSize(nType, cx, cy);

	// TODO:  在此处添加消息处理程序代码
}
