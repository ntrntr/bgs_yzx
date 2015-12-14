// YzxFormView.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "bgs_yzx.h"
#include "YzxFormView.h"
#include "DragEdit.h"

// CYzxFormView

IMPLEMENT_DYNCREATE(CYzxFormView, CFormView)

CYzxFormView::CYzxFormView()
	: CFormView(CYzxFormView::IDD)
{

	m_methodName = _T("");
	m_filePath = _T("");
}

CYzxFormView::~CYzxFormView()
{
}

void CYzxFormView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LISTBOX_BGSLIST, m_BGSList);
	DDX_Control(pDX, IDC_INPUT_VIDEO, m_dragEdit);
	DDX_Control(pDX, IDC_EDIT_DELAY, m_delay);
	DDX_Control(pDX, IDC_EXEC_TIME, m_execTime);
	DDX_Control(pDX, IDC_FRAME_NUMBER, m_frameNumber);
	DDX_Control(pDX, IDC_LOG, m_log);
	DDX_Control(pDX, IDC_SAVE_MASK, m_saveMask);
	DDX_Control(pDX, IDC_EDIT_SAVE_MASK, m_saveMaskPath);
	DDX_Control(pDX, IDC_COMBO_FILTER, m_filterList);
	DDX_Control(pDX, IDC_IDC_INPUT_VIDEO_APPEND, m_videoPathAppendControl);
}

BEGIN_MESSAGE_MAP(CYzxFormView, CFormView)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BUTTON_START, &CYzxFormView::OnBnClickedButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_SUSPEND, &CYzxFormView::OnBnClickedButtonSuspend)
	ON_BN_CLICKED(IDC_BUTTON_RESUME, &CYzxFormView::OnBnClickedButtonResume)
	ON_BN_CLICKED(IDC_BUTTON_END, &CYzxFormView::OnBnClickedButtonEnd)
END_MESSAGE_MAP()


// CYzxFormView ���

#ifdef _DEBUG
void CYzxFormView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CYzxFormView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG
// CYzxFormView ��Ϣ��������


int CYzxFormView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  �ڴ�������ר�õĴ�������
	// TODO:  �ڴ����Ӷ���ĳ�ʼ��

	
	return 0;
}


BOOL CYzxFormView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	// TODO:  �ڴ�����ר�ô����/����û���

	return CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}


void CYzxFormView::addBgsList()
{
	m_BGSList.AddString(_T("vibe"));
	m_BGSList.AddString(_T("subsense"));
	m_BGSList.AddString(_T("codebook"));
	m_BGSList.AddString(_T("gaussian_mix"));
	m_BGSList.AddString(_T("gaussian_mix2"));
	m_BGSList.AddString(_T("frame_difference"));
	m_BGSList.AddString(_T("BGSubtract"));
	m_BGSList.AddString(_T("AdpativeBGSubtract"));
	m_BGSList.AddString(_T("AdpativeVibe"));
	m_BGSList.AddString(_T("AdpativeGMM"));
	m_BGSList.AddString(_T("SigmaDelta"));
	m_BGSList.AddString(_T("PBAS"));
	m_BGSList.AddString(_T("MyBGS"));
}

void CYzxFormView::initMemberVariable()
{
	m_methodName = _T("");
	m_filePath = _T("");
	m_saveMaskOutPath = _T("");
	bgs = NULL;
	isContinue = true;
	m_maskfilename = _T("bin%06d.pgm");

}

bool CYzxFormView::getBgsMethodName()
{
	int nSel;
	nSel = m_BGSList.GetCurSel();
	if (nSel != -1)
	{
		m_BGSList.GetText(nSel, m_methodName);
		return true;
	}
	else
	{
		AfxMessageBox(L"��ѡ��һ���������ķ���!");
		return false;
	}
	return false;
}

bool CYzxFormView::getInputVideoFilePath()
{
	m_dragEdit.GetWindowTextW(m_filePath);
	m_videoPathAppendControl.GetWindowTextW(m_filePathAppend);
	if (m_filePath.GetLength() > 0 && m_filePathAppend.GetLength() > 0)
	{
		m_filePath.Append(m_filePathAppend);
		return true;
	}
	else
	{
		AfxMessageBox(L"�����Ƶ�ļ��ϵ��༭�����������Ƶ���У�");
	}
	return false;
}

void CYzxFormView::OnBnClickedButtonStart()
{
	// TODO:  �ڴ����ӿؼ�֪ͨ�����������
	isContinue = true;
	if (started == false)
	{
		m_log.SetWindowTextW(L"�����...");
		if (!getBgsMethodName())
		{
			m_log.SetWindowTextW(L"��ȡ������ʧ�ܣ��˳���...");
			return;
		}
		if (!getInputVideoFilePath())
		{
			m_log.SetWindowTextW(L"��ȡ��ƵĿ¼ʧ�ܣ��˳���...");
			return;
		}
		GetDlgItem(IDC_BUTTON_START)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_SUSPEND)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_RESUME)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_END)->EnableWindow(TRUE);
		m_log.SetWindowTextW(L"�㷨������...");
		yzxBGSthread = AfxBeginThread((AFX_THREADPROC)CYzxFormView::BGSThread, (LPVOID) this);
	}
	else
	{
		return;
	}
}


void CYzxFormView::OnBnClickedButtonSuspend()
{
	// TODO:  �ڴ����ӿؼ�֪ͨ�����������
	GetDlgItem(IDC_BUTTON_START)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_SUSPEND)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_RESUME)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_END)->EnableWindow(FALSE);
	yzxBGSthread->SuspendThread();
}


void CYzxFormView::OnBnClickedButtonResume()
{
	// TODO:  �ڴ����ӿؼ�֪ͨ�����������
	GetDlgItem(IDC_BUTTON_START)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_SUSPEND)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_RESUME)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_END)->EnableWindow(TRUE);
	yzxBGSthread->ResumeThread();
	
}


void CYzxFormView::OnBnClickedButtonEnd()
{
	// TODO:  �ڴ����ӿؼ�֪ͨ�����������
	GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_SUSPEND)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_RESUME)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_END)->EnableWindow(FALSE);
	isContinue = false;
}
DWORD CYzxFormView::BGSThread(LPVOID* pParam)
{
	CYzxFormView* thr = (CYzxFormView*)pParam;
	thr->ThreadProcess();
	return 0;
}

void CYzxFormView::ThreadProcess()
{
	cv::VideoCapture oVideoInput;
	CStringA file_path_aux(m_filePath);
	oVideoInput.open(file_path_aux.GetBuffer(0));
	if (!oVideoInput.isOpened())
	{
		AfxMessageBox(L"���ļ�ʧ�ܣ�");
		m_log.SetWindowTextW(L"Stopped...");
		return;
	}

	if (m_methodName == "frame_difference")
	{
		bgs = new FrameDifferenceBGS;
	}
	if (m_methodName == "vibe")
	{
		bgs = new VibeBGS;
	}
	if (m_methodName == "BGSubtract")
	{
		bgs = new CAdpativeBGSubtract;
	}
	if (m_methodName == "AdpativeBGSubtract")
	{
		bgs = new CBGSubtract;
	}
	if (m_methodName == "codebook")
	{
		bgs = new CYZXCodebookBGS;
	}
	if (m_methodName == "gaussian_mix")
	{
		bgs = new CYZXGaussianMix;
	}
	if (m_methodName == "gaussian_mix2")
	{
		bgs = new CYZXGaussianMix2;
	}
	if (m_methodName == "subsense")
	{
		bgs = new CSubsense;
	}
	if (m_methodName == "AdpativeVibe")
	{
		bgs = new CAdpativeVibeBGS;
	}
	if (m_methodName == "AdpativeGMM")
	{
		bgs = new CYZXAdpativeGaussianMix;
	}
	if (m_methodName == "SigmaDelta")
	{
		bgs = new CSigmaDeltaBGS;
	}
	if (m_methodName == "PBAS")
	{
		bgs = new CPBASBGS;
	}
	if (m_methodName == "MyBGS")
	{
		bgs = new CMyBGS;
	}
	if (bgs == NULL)
	{
		AfxMessageBox(L"BGS object not defined!");
		return;
	}
	//  [11/30/2015 yzx]
	//VibeBGS* bgs1 = new VibeBGS;
	started = true;
	cv::Size default_size;
	CRect rcPicControl;
	GetDlgItem(IDC_FRAME_INPUT)->GetClientRect(&rcPicControl);
	default_size.width = rcPicControl.Width();
	default_size.height = rcPicControl.Height();
	CString strFrameNumber;
	CString strExecTime;
	cv::Mat pframe;
	CString csDelay;
	m_delay.GetWindowTextW(csDelay);
	int delay = _tstoi(csDelay);
	int count = 0;
	bool saveMask = isSaveMaskImage();
	CString filtername = getFilterName();
	clock_t start, finish;
	clock_t allStart, allFinish;
	allStart = clock();
	while (true)
	{
		m_log.SetWindowTextW(L"������...");
		count++;
		oVideoInput >> pframe;
		if (pframe.empty())
		{
			break;
		}
		
		cv::Mat img_mask;
		cv::Mat img_bkg;
		start = clock();
		bgs->process(pframe, img_mask, img_bkg);
		finish = clock();
		//  [11/30/2015 yzx]
		//cv::Mat img_mask1;
		//cv::Mat img_bkg1;
		//bgs1->process(pframe, img_mask1, img_bkg1);
		//modifyMask(img_mask1, filtername);
		//cv::resize(img_mask1, img_mask1, default_size);
		//if (img_bkg1.empty())
		//{
		//	img_bkg1 = cv::Mat::zeros(cv::Size(pframe.size().width, pframe.size().height), pframe.type());
		//}
		//cv::resize(img_bkg1, img_bkg1, default_size);
		//cv::imshow("MASK1", img_mask1);
		//cv::imshow("BKG1", img_bkg1);
		//////////////////////////////////////////////////////////////////////////



		cv::Mat img_input_aux;
		cv::resize(pframe, img_input_aux, default_size);

		cv::Mat img_mask_aux;
		if (img_mask.empty())
			img_mask = cv::Mat::zeros(cv::Size(pframe.size().width, pframe.size().height), pframe.type());
		//if (m_medianFilter.GetCheck() == BST_CHECKED)
		//	cv::medianBlur(img_mask, img_mask, 5);
		modifyMask(img_mask, filtername);
		cv::resize(img_mask, img_mask_aux, default_size);

		cv::Mat img_bgk_aux;
		if (img_bkg.empty())
			img_bkg = cv::Mat::zeros(cv::Size(pframe.size().width, pframe.size().height), pframe.type());
		cv::resize(img_bkg, img_bgk_aux, default_size);

		cv::imshow("INPUT", img_input_aux);
		cv::imshow("MASK", img_mask_aux);
		cv::imshow("BKG", img_bgk_aux);

		if (saveMask)
		{
			saveImage(img_mask, m_saveMaskOutPath, m_maskfilename, count);
		}
		strFrameNumber.Format(L"%d", count);
		m_frameNumber.SetWindowTextW(strFrameNumber);

		strExecTime.Format(L"%d", finish - start);
		m_execTime.SetWindowTextW(strExecTime);
		::Sleep(delay);

		if (!isContinue)
		{
			break;
		}
	}

	delete bgs;
	//delete bgs1;
	allFinish = clock();
	CString text(L"Finished!");
	text.AppendFormat(CString(L" �ܹ�����ʱ�� %d ��"), (allFinish - allStart) / 1000);
	m_log.SetWindowTextW(text);
	started = false;
	OnBnClickedButtonEnd();
}

void CYzxFormView::saveImage(cv::Mat& image, CString outpath, CString filename, int count)
{
	outpath.AppendFormat(filename, count);
	// Convert a TCHAR string to a LPCSTR
	CT2CA fileType2(outpath);
	// construct a std::string using the LPCSTR input
	std::string str_file(fileType2);
	cv::imwrite(str_file, image);
}

bool CYzxFormView::isSaveMaskImage()
{
	m_saveMaskPath.GetWindowTextW(m_saveMaskOutPath);

	if (m_saveMaskOutPath.GetLength() > 0 && m_saveMask.GetCheck() == BST_CHECKED)
	{
		return true;
	}
	else
	{
		return false;
	}
	return false;
}

void CYzxFormView::addFilterList()
{
	m_filterList.AddString(L"none");
	m_filterList.AddString(L"modify");
	m_filterList.AddString(L"median filter");
}

void CYzxFormView::modifyMask(cv::Mat& image, CString& filtername)
{
	if (filtername == "none")
	{
		return;
	}
	
	if (filtername == "median filter")
	{
		cv::medianBlur(image, image, 5);
		return;
	}
	if (filtername == "modify")
	{
		cv::morphologyEx(image, image, cv::MORPH_CLOSE, cv::Mat(), cv::Point(0, 0), 1);
		cv::erode(image, image, cv::Mat(), cv::Point(0, 0), 1);
		cv::medianBlur(image, image, 5);
	}
}

CString CYzxFormView::getFilterName()
{
	CString res(L"none");
	int nIndex = m_filterList.GetCurSel();
	if (nIndex != CB_ERR)
	{
		m_filterList.GetLBText(nIndex, res);
	}
	return res;
}

void CYzxFormView::addVideoPathAppend()
{
m_videoPathAppendControl.AddString(_T("baseline//highway//input//in%06d.jpg"));
	m_videoPathAppendControl.AddString(_T("baseline//office//input//in%06d.jpg"));
	m_videoPathAppendControl.AddString(_T("baseline//pedestrians//input//in%06d.jpg"));
	m_videoPathAppendControl.AddString(_T("baseline//PETS2006//input//in%06d.jpg"));
	m_videoPathAppendControl.AddString(_T("cameraJitter//badminton//input//in%06d.jpg"));
	m_videoPathAppendControl.AddString(_T("cameraJitter//boulevard//input//in%06d.jpg"));
	m_videoPathAppendControl.AddString(_T("cameraJitter//sidewalk//input//in%06d.jpg"));
	m_videoPathAppendControl.AddString(_T("cameraJitter//traffic//input//in%06d.jpg"));
	m_videoPathAppendControl.AddString(_T("dynamicBackground//boats//input//in%06d.jpg"));
	m_videoPathAppendControl.AddString(_T("dynamicBackground//canoe//input//in%06d.jpg"));
	m_videoPathAppendControl.AddString(_T("dynamicBackground//fall//input//in%06d.jpg"));
	m_videoPathAppendControl.AddString(_T("dynamicBackground//fountain01//input//in%06d.jpg"));
	m_videoPathAppendControl.AddString(_T("dynamicBackground//fountain02//input//in%06d.jpg"));
	m_videoPathAppendControl.AddString(_T("dynamicBackground//overpass//input//in%06d.jpg"));
	m_videoPathAppendControl.AddString(_T("intermittentObjectMotion//abandonedBox//input//in%06d.jpg"));
	m_videoPathAppendControl.AddString(_T("intermittentObjectMotion//parking//input//in%06d.jpg"));
	m_videoPathAppendControl.AddString(_T("intermittentObjectMotion//sofa//input//in%06d.jpg"));
	m_videoPathAppendControl.AddString(_T("intermittentObjectMotion//streetLight//input//in%06d.jpg"));
	m_videoPathAppendControl.AddString(_T("intermittentObjectMotion//tramstop//input//in%06d.jpg"));
	m_videoPathAppendControl.AddString(_T("intermittentObjectMotion//winterDriveway//input//in%06d.jpg"));
	m_videoPathAppendControl.AddString(_T("shadow//backdoor//input//in%06d.jpg"));
	m_videoPathAppendControl.AddString(_T("shadow//bungalows//input//in%06d.jpg"));
	m_videoPathAppendControl.AddString(_T("shadow//busStation//input//in%06d.jpg"));
	m_videoPathAppendControl.AddString(_T("shadow//copyMachine//input//in%06d.jpg"));
	m_videoPathAppendControl.AddString(_T("shadow//cubicle//input//in%06d.jpg"));
	m_videoPathAppendControl.AddString(_T("shadow//peopleInShade//input//in%06d.jpg"));
	m_videoPathAppendControl.AddString(_T("thermal//corridor//input//in%06d.jpg"));
	m_videoPathAppendControl.AddString(_T("thermal//diningRoom//input//in%06d.jpg"));
	m_videoPathAppendControl.AddString(_T("thermal//lakeSide//input//in%06d.jpg"));
	m_videoPathAppendControl.AddString(_T("thermal//library//input//in%06d.jpg"));
	m_videoPathAppendControl.AddString(_T("thermal//park//input//in%06d.jpg"));
}







void CYzxFormView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO:  �ڴ�����ר�ô����/����û���
	initMemberVariable();
	addBgsList();
	m_BGSList.SetCurSel(m_BGSList.GetCount() - 5);
	addFilterList();
	addVideoPathAppend();
	m_videoPathAppendControl.SetCurSel(10);
	m_dragEdit.SetWindowTextW(L"D://dataset//dataset//");
	m_saveMaskPath.SetWindowTextW(L"D://dataset//results//dynamicBackground//fall//");
	m_delay.SetWindowTextW(L"1");
	m_frameNumber.SetWindowTextW(L"-");
	m_execTime.SetWindowTextW(L"-");
	started = false;
	if (started == false)
	{
		cv::namedWindow("INPUT", CV_WINDOW_AUTOSIZE);
		HWND hWnd = (HWND)cvGetWindowHandle("INPUT");
		HWND hParent = ::GetParent(hWnd);
		::SetParent(hWnd, GetDlgItem(IDC_FRAME_INPUT)->m_hWnd);
		::ShowWindow(hParent, SW_HIDE);
	}
	if (started == false)
	{
		cv::namedWindow("MASK", CV_WINDOW_AUTOSIZE);
		HWND hWnd = (HWND)cvGetWindowHandle("MASK");
		HWND hParent = ::GetParent(hWnd);
		::SetParent(hWnd, GetDlgItem(IDC_FRAME_MASK)->m_hWnd);
		::ShowWindow(hParent, SW_HIDE);
	}
	if (started == false)
	{
		cv::namedWindow("BKG", CV_WINDOW_AUTOSIZE);
		HWND hWnd = (HWND)cvGetWindowHandle("BKG");
		HWND hParent = ::GetParent(hWnd);
		::SetParent(hWnd, GetDlgItem(IDC_FRAME_BKG)->m_hWnd);
		::ShowWindow(hParent, SW_HIDE);
	}
}