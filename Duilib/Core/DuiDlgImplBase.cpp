#include "StdAfx.h"
#include "DuiDlgImplBase.h"

namespace DuiLib
{


    CDuiDlgImplBase::CDuiDlgImplBase(void)
    {
    }


    CDuiDlgImplBase::~CDuiDlgImplBase(void)
    {
    }

    void CDuiDlgImplBase::InitWindow()
    {
    }

    DuiLib::CDuiString CDuiDlgImplBase::GetSkinType()
    {
        return _T("");
    }

    void CDuiDlgImplBase::Notify(TNotifyUI& msg)
    {
    }

    LRESULT CDuiDlgImplBase::ResponseDefaultKeyEvent(WPARAM wParam)
    {
        if(wParam == VK_RETURN)
        {
            return FALSE;
        }
        else if(wParam == VK_ESCAPE)
        {
            return TRUE;
        }
        return FALSE;
    }

    LRESULT CDuiDlgImplBase::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        if(uMsg == WM_KEYDOWN)
        {
            switch(wParam)
            {
                case VK_RETURN:
                case VK_ESCAPE:
                    return ResponseDefaultKeyEvent(wParam);
                default:
                    break;
            }
        }
        return FALSE;
    }

    LRESULT CDuiDlgImplBase::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        bHandled = FALSE;
        return 0;
    }

    LRESULT CDuiDlgImplBase::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        LRESULT lRes = 0;
        BOOL bHandled = TRUE;
        switch(uMsg)
        {
            case WM_CREATE:
                lRes = OnCreate(uMsg, wParam, lParam, bHandled);
                break;
            case WM_CLOSE:
                lRes = OnClose(uMsg, wParam, lParam, bHandled);
                break;
            case WM_DESTROY:
                lRes = OnDestroy(uMsg, wParam, lParam, bHandled);
                break;
#if defined(WIN32) && !defined(UNDER_CE)
            case WM_NCACTIVATE:
                lRes = OnNcActivate(uMsg, wParam, lParam, bHandled);
                break;
            case WM_NCCALCSIZE:
                lRes = OnNcCalcSize(uMsg, wParam, lParam, bHandled);
                break;
            case WM_NCPAINT:
                lRes = OnNcPaint(uMsg, wParam, lParam, bHandled);
                break;
            case WM_NCHITTEST:
                lRes = OnNcHitTest(uMsg, wParam, lParam, bHandled);
                break;
            case WM_GETMINMAXINFO:
                lRes = OnGetMinMaxInfo(uMsg, wParam, lParam, bHandled);
                break;
            case WM_MOUSEWHEEL:
                lRes = OnMouseWheel(uMsg, wParam, lParam, bHandled);
                break;
#endif
            case WM_SIZE:
                lRes = OnSize(uMsg, wParam, lParam, bHandled);
                break;
            case WM_CHAR:
                lRes = OnChar(uMsg, wParam, lParam, bHandled);
                break;
            case WM_SYSCOMMAND:
                lRes = OnSysCommand(uMsg, wParam, lParam, bHandled);
                break;
            case WM_KEYDOWN:
                lRes = OnKeyDown(uMsg, wParam, lParam, bHandled);
                break;
            case WM_KILLFOCUS:
                lRes = OnKillFocus(uMsg, wParam, lParam, bHandled);
                break;
            case WM_SETFOCUS:
                lRes = OnSetFocus(uMsg, wParam, lParam, bHandled);
                break;
            case WM_LBUTTONUP:
                lRes = OnLButtonUp(uMsg, wParam, lParam, bHandled);
                break;
            case WM_LBUTTONDOWN:
                lRes = OnLButtonDown(uMsg, wParam, lParam, bHandled);
                break;
            case WM_MOUSEMOVE:
                lRes = OnMouseMove(uMsg, wParam, lParam, bHandled);
                break;
            case WM_MOUSEHOVER:
                lRes = OnMouseHover(uMsg, wParam, lParam, bHandled);
                break;
            default:
                bHandled = FALSE;
                break;
        }
        if(bHandled)
        {
            return lRes;
        }
        lRes = HandleCustomMessage(uMsg, wParam, lParam, bHandled);
        if(bHandled)
        {
            return lRes;
        }
        if(m_PaintManager.MessageHandler(uMsg, wParam, lParam, lRes))
        {
            return lRes;
        }
        return __super::HandleMessage(uMsg, wParam, lParam);
    }

    LRESULT CDuiDlgImplBase::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        // 去掉对话框标题栏
        LONG styleValue = ::GetWindowLong(GetHWND(), GWL_STYLE);
        styleValue &= (~WS_CAPTION);
        ::SetWindowLong(GetHWND(), GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
        // 调整窗口尺寸
        RECT rcClient;
        ::GetClientRect(GetHWND(), &rcClient);
        ::SetWindowPos(GetHWND(), NULL, rcClient.left, rcClient.top, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top, SWP_FRAMECHANGED);
        // 关联UI管理器
        m_PaintManager.Init(GetHWND());
        // 注册PreMessage回调
        m_PaintManager.AddPreMessageFilter(this);
        // 注册Notify回调
        m_PaintManager.AddNotifier(this);
        // 创建主窗口
        CDuiControl* pRoot = NULL;
        CDuiDlgBuilder builder;
        CDuiString sSkinType = GetSkinType();
        if(!sSkinType.IsEmpty())
        {
            STRINGorID xml(_ttoi(GetSkinFile().GetData()));
            pRoot = builder.Create(xml, sSkinType, this, &m_PaintManager);
        }
        else
        {
            pRoot = builder.Create(GetSkinFile().GetData(), (UINT)0, this, &m_PaintManager);
        }
        if(pRoot == NULL)
        {
            CDuiString sError = _T("加载资源文件失败：");
            sError += GetSkinFile();
            MessageBox(NULL, sError, _T("Duilib") , MB_OK | MB_ICONERROR);
            ExitProcess(1);
            return 0;
        }
        m_PaintManager.AttachDialog(pRoot);
        InitWindow();
        return 0;
    }

    LRESULT CDuiDlgImplBase::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        bHandled = FALSE;
        return 0;
    }

    LRESULT CDuiDlgImplBase::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        PostQuitMessage(0);
        return 0;
    }

#if defined(WIN32) && !defined(UNDER_CE)

    LRESULT CDuiDlgImplBase::OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        if(::IsIconic(GetHWND()))
        {
            bHandled = FALSE;
        }
        return (wParam == 0) ? TRUE : FALSE;
    }

    LRESULT CDuiDlgImplBase::OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        return 0;
    }

    LRESULT CDuiDlgImplBase::OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        return 0;
    }

    LRESULT CDuiDlgImplBase::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        POINT pt;
        pt.x = GET_X_LPARAM(lParam);
        pt.y = GET_Y_LPARAM(lParam);
        ::ScreenToClient(GetHWND(), &pt);
        RECT rcClient;
        ::GetClientRect(GetHWND(), &rcClient);
        if(!::IsZoomed(GetHWND()))
        {
            RECT rcSizeBox = m_PaintManager.GetSizeBox();
            if(pt.y < rcClient.top + rcSizeBox.top)
            {
                if(pt.x < rcClient.left + rcSizeBox.left)
                {
                    return HTTOPLEFT;
                }
                if(pt.x > rcClient.right - rcSizeBox.right)
                {
                    return HTTOPRIGHT;
                }
                return HTTOP;
            }
            else if(pt.y > rcClient.bottom - rcSizeBox.bottom)
            {
                if(pt.x < rcClient.left + rcSizeBox.left)
                {
                    return HTBOTTOMLEFT;
                }
                if(pt.x > rcClient.right - rcSizeBox.right)
                {
                    return HTBOTTOMRIGHT;
                }
                return HTBOTTOM;
            }
            if(pt.x < rcClient.left + rcSizeBox.left)
            {
                return HTLEFT;
            }
            if(pt.x > rcClient.right - rcSizeBox.right)
            {
                return HTRIGHT;
            }
        }
        RECT rcCaption = m_PaintManager.GetCaptionRect();
        if(-1 == rcCaption.bottom)
        {
            rcCaption.bottom = rcClient.bottom;
        }
        if(pt.x >= rcClient.left + rcCaption.left && pt.x < rcClient.right - rcCaption.right
                && pt.y >= rcCaption.top && pt.y < rcCaption.bottom)
        {
            CDuiControl* pControl = m_PaintManager.FindControl(pt);
            if(IsInStaticControl(pControl))
            {
                return HTCAPTION;
            }
        }
        return HTCLIENT;
    }

    LRESULT CDuiDlgImplBase::OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        MONITORINFO Monitor = {0};
        Monitor.cbSize = sizeof(Monitor);
        ::GetMonitorInfo(::MonitorFromWindow(GetHWND(), MONITOR_DEFAULTTOPRIMARY), &Monitor);
        RECT rcWork = Monitor.rcWork;
        if(Monitor.dwFlags != MONITORINFOF_PRIMARY)
        {
            ::OffsetRect(&rcWork, -rcWork.left, -rcWork.top);
        }
        LPMINMAXINFO lpMMI = (LPMINMAXINFO) lParam;
        lpMMI->ptMaxPosition.x	= rcWork.left;
        lpMMI->ptMaxPosition.y	= rcWork.top;
        lpMMI->ptMaxSize.x = rcWork.right - rcWork.left;
        lpMMI->ptMaxSize.y = rcWork.bottom - rcWork.top;
        lpMMI->ptMaxTrackSize.x = rcWork.right - rcWork.left;
        lpMMI->ptMaxTrackSize.y = rcWork.bottom - rcWork.top;
        lpMMI->ptMinTrackSize.x = m_PaintManager.GetMinInfo().cx;
        lpMMI->ptMinTrackSize.y = m_PaintManager.GetMinInfo().cy;
        bHandled = TRUE;
        return 0;
    }

    LRESULT CDuiDlgImplBase::OnMouseWheel(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        bHandled = FALSE;
        return 0;
    }

    LRESULT CDuiDlgImplBase::OnMouseHover(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        bHandled = FALSE;
        return 0;
    }

#endif

    LRESULT CDuiDlgImplBase::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        SIZE szRoundCorner = m_PaintManager.GetRoundCorner();
#if defined(WIN32) && !defined(UNDER_CE)
        if(!::IsIconic(GetHWND()))
        {
            CDuiRect rcWnd;
            ::GetWindowRect(GetHWND(), &rcWnd);
            rcWnd.Offset(-rcWnd.left, -rcWnd.top);
            rcWnd.right++;
            rcWnd.bottom++;
            HRGN hRgn = ::CreateRoundRectRgn(rcWnd.left, rcWnd.top, rcWnd.right, rcWnd.bottom, szRoundCorner.cx, szRoundCorner.cy);
            ::SetWindowRgn(GetHWND(), hRgn, TRUE);
            ::DeleteObject(hRgn);
        }
#endif
        bHandled = FALSE;
        return 0;
    }


    LRESULT CDuiDlgImplBase::OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        bHandled = FALSE;
        return 0;
    }

    LRESULT CDuiDlgImplBase::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        if(wParam == SC_CLOSE)
        {
            bHandled = TRUE;
            SendMessage(WM_CLOSE);
            return 0;
        }
#if defined(WIN32) && !defined(UNDER_CE)
        BOOL bZoomed = ::IsZoomed(*this);
        LRESULT lRes = __super::HandleMessage(uMsg, wParam, lParam);
        if(::IsZoomed(*this) != bZoomed)
        {
            if(!bZoomed)
            {
                CDuiControl* pControl = static_cast<CDuiControl*>(m_PaintManager.FindControl(_T("maxbtn")));
                if(pControl)
                {
                    pControl->SetVisible(FALSE);
                }
                pControl = static_cast<CDuiControl*>(m_PaintManager.FindControl(_T("restorebtn")));
                if(pControl)
                {
                    pControl->SetVisible(TRUE);
                }
            }
            else
            {
                CDuiControl* pControl = static_cast<CDuiControl*>(m_PaintManager.FindControl(_T("maxbtn")));
                if(pControl)
                {
                    pControl->SetVisible(TRUE);
                }
                pControl = static_cast<CDuiControl*>(m_PaintManager.FindControl(_T("restorebtn")));
                if(pControl)
                {
                    pControl->SetVisible(FALSE);
                }
            }
        }
#else
        LRESULT lRes = __super::HandleMessage(uMsg, wParam, lParam);
#endif
        return lRes;
    }

    LRESULT CDuiDlgImplBase::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        bHandled = FALSE;
        Close();
        return 0;
    }

    LRESULT CDuiDlgImplBase::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        bHandled = FALSE;
        return 0;
    }

    LRESULT CDuiDlgImplBase::OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        bHandled = FALSE;
        return 0;
    }

    LRESULT CDuiDlgImplBase::OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        bHandled = FALSE;
        return 0;
    }

    LRESULT CDuiDlgImplBase::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        bHandled = FALSE;
        return 0;
    }

    LRESULT CDuiDlgImplBase::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        bHandled = FALSE;
        return 0;
    }

    BOOL CDuiDlgImplBase::IsInStaticControl(CDuiControl* pControl)
    {
        BOOL bRet = FALSE;
        if(!pControl)
        {
            return bRet;
        }
        CDuiString strClassName;
        std::vector<CDuiString> vctStaticName;
        strClassName = pControl->GetClass();
        strClassName.MakeLower();
        vctStaticName.push_back(_T("control"));
        vctStaticName.push_back(_T("text"));
        vctStaticName.push_back(_T("label"));
        vctStaticName.push_back(_T("container"));
        vctStaticName.push_back(_T("horizontallayout"));
        vctStaticName.push_back(_T("verticallayout"));
        vctStaticName.push_back(_T("tablayout"));
        vctStaticName.push_back(_T("childlayout"));
        vctStaticName.push_back(_T("dialoglayout"));
        vctStaticName.push_back(_T("progresscontainer"));
        std::vector<CDuiString>::iterator it = std::find(vctStaticName.begin(), vctStaticName.end(), strClassName);
        if(vctStaticName.end() != it)
        {
            CDuiControl* pParent = pControl->GetParent();
            while(pParent)
            {
                strClassName = pParent->GetClass();
                strClassName.MakeLower();
                it = std::find(vctStaticName.begin(), vctStaticName.end(), strClassName);
                if(vctStaticName.end() == it)
                {
                    return bRet;
                }
                pParent = pParent->GetParent();
            }
            bRet = TRUE;
        }
        return bRet;
    }

}
