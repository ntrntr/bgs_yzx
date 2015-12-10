// MyGBSDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "bgs_yzx.h"
#include "MyGBSDialog.h"
#include "afxdialogex.h"
#include "DragEdit.h"

// CMyGBSDialog 对话框

IMPLEMENT_DYNAMIC(CMyGBSDialog, CDialogEx)

CMyGBSDialog::CMyGBSDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMyGBSDialog::IDD, pParent)
{

	m_methodName = _T("");
	m_filePath = _T("");
}

CMyGBSDialog::~CMyGBSDialog()
{
}

void CMyGBSDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LISTBOX_BGSLIST, m_BGSList);
	DDX_Control(pDX, IDC_INPUT_VIDEO, m_dragEdit);
	DDX_Control(pDX, IDC_EDIT_DELAY, m_delay);
	DDX_Control(pDX, IDC_EXEC_TIME, m_execTime);
	DDX_Control(pDX, IDC_FRAME_NUMBER, m_frameNumber);
	DDX_Control(pDX, IDC_LOG, m_log);
	DDX_Control(pDX, IDC_SAVE_MASK, m_saveMask);
	DDX_Control(pDX, IDC_EDIT_SAVE_MASK, m_saveMaskPath);
	DDX_Control(pDX, IDC_COMBO_FILTER, m_filterList);
}


BEGIN_MESSAGE_MAP(CMyGBSDialog, CDialogEx)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BUTTON_START, &CMyGBSDialog::OnBnClickedButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_SUSPEND, &CMyGBSDialog::OnBnClickedButtonSuspend)
	ON_BN_CLICKED(IDC_BUTTON_RESUME, &CMyGBSDialog::OnBnClickedButtonResume)
	ON_BN_CLICKED(IDC_BUTTON_END, &CMyGBSDialog::OnBnClickedButtonEnd)
END_MESSAGE_MAP()


// CMyGBSDialog 消息处理程序


int CMyGBSDialog::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码

	return 0;
}


BOOL CMyGBSDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	initMemberVariable();
	addBgsList();
	addFilterList();
	m_dragEdit.SetWindowTextW(L"D://dataset//dataset//dynamicBackground//fall//input//in%06d.jpg");
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
	//  [11/30/2015 yzx]
	//cv::namedWindow("MASK1", CV_WINDOW_AUTOSIZE);
	//cv::namedWindow("BKG1", CV_WINDOW_AUTOSIZE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}


void CMyGBSDialog::addBgsList()
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
}


void CMyGBSDialog::initMemberVariable()
{
	m_methodName = _T("");
	m_filePath = _T("");
	m_saveMaskOutPath = _T("");
	bgs = NULL;
	isContinue = true;
	m_maskfilename = _T("bin%06d.pgm");

}


// getBgsMethodName
bool CMyGBSDialog::getBgsMethodName()
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
		AfxMessageBox(L"请选择一个背景检测的方法!");
		return false;
	}
	return false;
}


// bool getInputVideoFilePath()
bool CMyGBSDialog::getInputVideoFilePath()
{
	m_dragEdit.GetWindowTextW(m_filePath);

	if (m_filePath.GetLength() > 0)
	{
		return true;
	}
	else
	{
		AfxMessageBox(L"请把视频文件拖到编辑框或者输入视频序列！");
	}
	return false;
}


void CMyGBSDialog::OnBnClickedButtonStart()
{
	// TODO:  在此添加控件通知处理程序代码

	isContinue = true;
	if (started == false)
	{
		m_log.SetWindowTextW(L"检查中...");
		if (!getBgsMethodName())
		{
			m_log.SetWindowTextW(L"获取方法名失败，退出中...");
			return;
		}
		if (!getInputVideoFilePath())
		{
			m_log.SetWindowTextW(L"获取视频目录失败，退出中...");
			return;
		}
		GetDlgItem(IDC_BUTTON_START)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_SUSPEND)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_RESUME)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_END)->EnableWindow(TRUE);
		m_log.SetWindowTextW(L"算法运行中...");
		yzxBGSthread = AfxBeginThread((AFX_THREADPROC)CMyGBSDialog::BGSThread, (LPVOID) this);
	}
	else
	{
		return;
	}
}


DWORD CMyGBSDialog::BGSThread(LPVOID* pParam)
{
	CMyGBSDialog* thr = (CMyGBSDialog*)pParam;
	thr->ThreadProcess();
	return 0;
}


void CMyGBSDialog::ThreadProcess()
{
	cv::VideoCapture oVideoInput;
	CStringA file_path_aux(m_filePath);
	oVideoInput.open(file_path_aux.GetBuffer(0));
	if (!oVideoInput.isOpened())
	{
		AfxMessageBox(L"打开文件失败！");
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
		m_log.SetWindowTextW(L"运行中...");
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
	text.AppendFormat(CString(L" 总共运行时间 %d 秒"), (allFinish - allStart) / 1000);
	m_log.SetWindowTextW(text);
	started = false;
	OnBnClickedButtonEnd();

}


void CMyGBSDialog::OnBnClickedButtonSuspend()
{
	// TODO:  在此添加控件通知处理程序代码
	GetDlgItem(IDC_BUTTON_START)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_SUSPEND)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_RESUME)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_END)->EnableWindow(FALSE);
	yzxBGSthread->SuspendThread();
}


void CMyGBSDialog::OnBnClickedButtonResume()
{
	// TODO:  在此添加控件通知处理程序代码
	GetDlgItem(IDC_BUTTON_START)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_SUSPEND)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_RESUME)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_END)->EnableWindow(TRUE);
	yzxBGSthread->ResumeThread();
	
}


void CMyGBSDialog::OnBnClickedButtonEnd()
{
	// TODO:  在此添加控件通知处理程序代码
	GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_SUSPEND)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_RESUME)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_END)->EnableWindow(FALSE);
	isContinue = false;
}


// 保存文件
void CMyGBSDialog::saveImage(cv::Mat& image, CString outpath, CString filename, int count)
{
	outpath.AppendFormat(filename, count);
	// Convert a TCHAR string to a LPCSTR
	CT2CA fileType2(outpath);
	// construct a std::string using the LPCSTR input
	std::string str_file(fileType2);
	cv::imwrite(str_file, image);
}


bool CMyGBSDialog::isSaveMaskImage()
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


void CMyGBSDialog::addFilterList()
{
	m_filterList.AddString(L"none");
	m_filterList.AddString(L"modify");
	m_filterList.AddString(L"median filter");
}


void CMyGBSDialog::modifyMask(cv::Mat& image, CString& filtername)
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


CString CMyGBSDialog::getFilterName()
{
	CString res(L"none");
	int nIndex = m_filterList.GetCurSel();
	if (nIndex != CB_ERR)
	{
		m_filterList.GetLBText(nIndex, res);
	}
	return res;
}
