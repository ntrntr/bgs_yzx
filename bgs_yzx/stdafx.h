
// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 从 Windows 头中排除极少使用的资料
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的

// 关闭 MFC 对某些常见但经常可放心忽略的警告消息的隐藏
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC 核心组件和标准组件
#include <afxext.h>         // MFC 扩展


#include <afxdisp.h>        // MFC 自动化类



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC 对 Internet Explorer 4 公共控件的支持
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC 对 Windows 公共控件的支持
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // 功能区和控件条的 MFC 支持




#include "precomp.hpp"
#include "opencv2/opencv.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include "time.h"
#include <direct.h>
#include <cstring>
#include <map>
#include <omp.h>
#include <fstream>

#include "FrameDifferenceBGS.h"
#include "VibeBGS.h"
#include "BGSubtract.h"
#include "AdpativeBGSubtract.h"
#include "YZXCodebookBGS.h"
#include "YZXGaussianMix.h"
#include "YZXGaussianMix2.h"
#include "Subsense.h"
#include "AdpativeVibeBGS.h"
#include "YZXAdpativeGaussianMix.h"
#include "SigmaDeltaBGS.h"
#include "PBASBGS.h"
#include "MyBGS.h"
#include "VibePlusBGS.h"
#include "CodebookOpencvBGS.h"
#include "Codebook2BGS.h"
#include "MultiscaleSTBGS.h"
#include "PAWCSBgs.h"
#include "VibeTest.h"
#include "CyzxPAWCSBGS.h"
#include "LobsterBGS.h"
#include "YzxLBSPBGS.h"
#include "AMBERBGS.h"
#include "IMBSBGS.h"
#include "VuMeterBGS.h"
#include "IWonderBGS.h"
#include "EFICBGS.h"
//if using the check of leak memory
#define _CRTDBG_MAP_ALLOC 
#include<stdlib.h>
#include<crtdbg.h>



#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


