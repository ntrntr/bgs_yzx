
// bgs_yzxView.cpp : Cbgs_yzxView ���ʵ��
//

#include "stdafx.h"
// SHARED_HANDLERS ������ʵ��Ԥ��������ͼ������ɸѡ�������
// ATL ��Ŀ�н��ж��壬�����������Ŀ�����ĵ����롣
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

// Cbgs_yzxView ����/����

Cbgs_yzxView::Cbgs_yzxView()
{
	// TODO:  �ڴ˴���ӹ������

}

Cbgs_yzxView::~Cbgs_yzxView()
{
}

BOOL Cbgs_yzxView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO:  �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ
	return CView::PreCreateWindow(cs);
}

// Cbgs_yzxView ����

void Cbgs_yzxView::OnDraw(CDC* /*pDC*/)
{
	Cbgs_yzxDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO:  �ڴ˴�Ϊ����������ӻ��ƴ���
}


// Cbgs_yzxView ���

#ifdef _DEBUG
void Cbgs_yzxView::AssertValid() const
{
	CView::AssertValid();
}

void Cbgs_yzxView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

Cbgs_yzxDoc* Cbgs_yzxView::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(Cbgs_yzxDoc)));
	return (Cbgs_yzxDoc*)m_pDocument;
}
#endif //_DEBUG


// Cbgs_yzxView ��Ϣ�������


int Cbgs_yzxView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  �ڴ������ר�õĴ�������
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

	// TODO:  �ڴ˴������Ϣ����������
}
