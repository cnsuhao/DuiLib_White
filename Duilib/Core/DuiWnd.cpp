#include "StdAfx.h"
#include "DuiWnd.h"

namespace DuiLib
{

    CDuiWnd::CDuiWnd(void)
        : m_hWnd(NULL)
        , m_bSubclassed(FALSE)
        , m_OldWndProc(::DefWindowProc)
    {
    }


    CDuiWnd::~CDuiWnd(void)
    {
    }

    HWND CDuiWnd::GetHWND() const
    {
        return m_hWnd;
    }

    CDuiWnd::operator HWND() const
    {
        return m_hWnd;
    }

    HWND CDuiWnd::Create(HWND hWndParent, LPCTSTR lpWindowName, DWORD dwStyle, DWORD dwExStyle, const RECT rc, HMENU hMenu /*= NULL*/)
    {
        return Create(hWndParent, lpWindowName, dwStyle, dwExStyle, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, hMenu);
    }

    HWND CDuiWnd::Create(HWND hWndParent, LPCTSTR lpWindowName, DWORD dwStyle, DWORD dwExStyle, int x /*= CW_USEDEFAULT*/, int y /*= CW_USEDEFAULT*/, int nWidth /*= CW_USEDEFAULT*/, int nHeight /*= CW_USEDEFAULT*/, HMENU hMenu /*= NULL*/)
    {
        if(GetSuperClassName() != NULL && !RegisterSuperClass())
        {
            return NULL;
        }
        if(GetSuperClassName() == NULL && !RegisterWindowClass())
        {
            return NULL;
        }
        m_hWnd = ::CreateWindowEx(dwExStyle, GetWindowClassName(), lpWindowName, dwStyle, x, y, nWidth, nHeight, hWndParent, hMenu, CDuiPaintManager::GetInstance(), this);
        ASSERT(m_hWnd != NULL);
        return m_hWnd;
    }

    LRESULT CDuiWnd::SendMessage(UINT uMsg, WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/)
    {
        ASSERT(::IsWindow(m_hWnd));
        return ::SendMessage(m_hWnd, uMsg, wParam, lParam);
    }

    LRESULT CDuiWnd::PostMessage(UINT uMsg, WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/)
    {
        ASSERT(::IsWindow(m_hWnd));
        return ::SendMessage(m_hWnd, uMsg, wParam, lParam);
    }

    HWND CDuiWnd::SubClass(HWND hWnd)
    {
        ASSERT(::IsWindow(hWnd));
        ASSERT(m_hWnd == NULL);
        m_OldWndProc = SubclassWindow(hWnd, __WndProc);
        if(m_OldWndProc == NULL)
        {
            return NULL;
        }
        m_bSubclassed = TRUE;
        m_hWnd = hWnd;
        ::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LPARAM>(this));
        return m_hWnd;
    }

    void CDuiWnd::UnSubClass()
    {
        ASSERT(::IsWindow(m_hWnd));
        if(!::IsWindow(m_hWnd) || !m_bSubclassed)
        {
            return;
        }
        SubclassWindow(m_hWnd, m_OldWndProc);
        m_OldWndProc = ::DefWindowProc;
        m_bSubclassed = FALSE;
    }

    void CDuiWnd::ShowWindow(BOOL bShow /*= TRUE*/, BOOL bTakeFocus /*= TRUE*/)
    {
        ASSERT(::IsWindow(m_hWnd));
        if(!::IsWindow(m_hWnd))
        {
            return;
        }
        ::ShowWindow(m_hWnd, bShow ? (bTakeFocus ? SW_SHOWNORMAL : SW_SHOWNOACTIVATE) : SW_HIDE);
    }

    void CDuiWnd::CenterWindow()
    {
        ASSERT(::IsWindow(m_hWnd));
        ASSERT((GetWindowStyle(m_hWnd)&WS_CHILD) == 0);
        RECT rcDlg = { 0 };
        ::GetWindowRect(m_hWnd, &rcDlg);
        HWND hWnd = *this;
        HWND hWndCenter = ::GetWindowOwner(m_hWnd);
        if(hWndCenter != NULL)
        {
            hWnd = hWndCenter;
        }
        // ��������ʾ��ģʽ����Ļ����
        RECT rcArea = {0};
        RECT rcCenter = {0};
        MONITORINFO oMonitor = {0};
        oMonitor.cbSize = sizeof(oMonitor);
        ::GetMonitorInfo(::MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST), &oMonitor);
        rcArea = oMonitor.rcWork;
        if(hWndCenter == NULL)
        {
            rcCenter = rcArea;
        }
        else
        {
            ::GetWindowRect(hWndCenter, &rcCenter);
        }
        int DlgWidth = rcDlg.right - rcDlg.left;
        int DlgHeight = rcDlg.bottom - rcDlg.top;
        // Find dialog's upper left based on rcCenter
        int xLeft = (rcCenter.left + rcCenter.right) / 2 - DlgWidth / 2;
        int yTop = (rcCenter.top + rcCenter.bottom) / 2 - DlgHeight / 2;
        // The dialog is outside the screen, move it inside
        if(xLeft < rcArea.left)
        {
            xLeft = rcArea.left;
        }
        else if(xLeft + DlgWidth > rcArea.right)
        {
            xLeft = rcArea.right - DlgWidth;
        }
        if(yTop < rcArea.top)
        {
            yTop = rcArea.top;
        }
        else if(yTop + DlgHeight > rcArea.bottom)
        {
            yTop = rcArea.bottom - DlgHeight;
        }
        ::SetWindowPos(m_hWnd, NULL, xLeft, yTop, -1, -1, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    }

    UINT CDuiWnd::ShowModal()
    {
        ASSERT(::IsWindow(m_hWnd));
        UINT nRet = 0;
        HWND hWndParent = GetWindowOwner(m_hWnd);
        ::ShowWindow(m_hWnd, SW_SHOWNORMAL);
        ::EnableWindow(hWndParent, FALSE);
        MSG msg = { 0 };
        while(::IsWindow(m_hWnd) && ::GetMessage(&msg, NULL, 0, 0))
        {
            if(msg.message == WM_CLOSE && msg.hwnd == m_hWnd)
            {
                nRet = msg.wParam;
                ::EnableWindow(hWndParent, TRUE);
                ::SetFocus(hWndParent);
            }
            if(!CDuiPaintManager::TranslateMessage(&msg))
            {
                ::TranslateMessage(&msg);
                ::DispatchMessage(&msg);
            }
            if(msg.message == WM_QUIT)
            {
                break;
            }
        }
        ::EnableWindow(hWndParent, TRUE);
        ::SetFocus(hWndParent);
        if(msg.message == WM_QUIT)
        {
            ::PostQuitMessage(msg.wParam);
        }
        return nRet;
    }

    void CDuiWnd::Close(UINT nRet /*= IDOK*/)
    {
        ASSERT(::IsWindow(m_hWnd));
        if(!::IsWindow(m_hWnd))
        {
            return;
        }
        PostMessage(WM_CLOSE, (WPARAM)nRet, 0);
    }

    void CDuiWnd::SetIcon(UINT nRes)
    {
        HICON hIcon = (HICON)::LoadImage(CDuiPaintManager::GetInstance(), MAKEINTRESOURCE(nRes), IMAGE_ICON,
                                         (::GetSystemMetrics(SM_CXICON) + 15) & ~15, (::GetSystemMetrics(SM_CYICON) + 15) & ~15,	// ��ֹ��DPI��ͼ��ģ��
                                         LR_DEFAULTCOLOR);
        ASSERT(hIcon);
        SendMessage(WM_SETICON, (WPARAM) TRUE, (LPARAM) hIcon);
        hIcon = (HICON)::LoadImage(CDuiPaintManager::GetInstance(), MAKEINTRESOURCE(nRes), IMAGE_ICON,
                                   (::GetSystemMetrics(SM_CXICON) + 15) & ~15, (::GetSystemMetrics(SM_CYICON) + 15) & ~15,	// ��ֹ��DPI��ͼ��ģ��
                                   LR_DEFAULTCOLOR);
        ASSERT(hIcon);
        SendMessage(WM_SETICON, (WPARAM) FALSE, (LPARAM) hIcon);
    }

    void CDuiWnd::ResizeClient(int cx /*= -1*/, int cy /*= -1*/)
    {
        ASSERT(::IsWindow(m_hWnd));
        RECT rc = { 0 };
        if(!::GetClientRect(m_hWnd, &rc))
        {
            return;
        }
        if(cx != -1)
        {
            rc.right = cx;
        }
        if(cy != -1)
        {
            rc.bottom = cy;
        }
        if(!::AdjustWindowRectEx(&rc, GetWindowStyle(m_hWnd), (!(GetWindowStyle(m_hWnd) & WS_CHILD) && (::GetMenu(m_hWnd) != NULL)), GetWindowExStyle(m_hWnd)))
        {
            return;
        }
        ::SetWindowPos(m_hWnd, NULL, 0, 0, rc.right - rc.left, rc.bottom - rc.top, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
    }

    UINT CDuiWnd::GetClassStyle() const
    {
        return 0;
    }

    LPCTSTR CDuiWnd::GetSuperClassName() const
    {
        return NULL;
    }

    LRESULT CDuiWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        return ::CallWindowProc(m_OldWndProc, m_hWnd, uMsg, wParam, lParam);
    }

    void CDuiWnd::OnFinalMessage(HWND hWnd)
    {
    }

    BOOL CDuiWnd::RegisterWindowClass()
    {
        WNDCLASS wc = { 0 };
        wc.style = GetClassStyle();
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hIcon = NULL;
        wc.lpfnWndProc = CDuiWnd::__WndProc;
        wc.hInstance = CDuiPaintManager::GetInstance();
        wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = NULL;
        wc.lpszMenuName  = NULL;
        wc.lpszClassName = GetWindowClassName();
        ATOM ret = ::RegisterClass(&wc);
        ASSERT(ret != NULL || ::GetLastError() == ERROR_CLASS_ALREADY_EXISTS);
        return ret != NULL || ::GetLastError() == ERROR_CLASS_ALREADY_EXISTS;
    }

    BOOL CDuiWnd::RegisterSuperClass()
    {
        // Get the class information from an existing
        // window so we can subclass it later on...
        WNDCLASSEX wc = { 0 };
        wc.cbSize = sizeof(WNDCLASSEX);
        if(!::GetClassInfoEx(NULL, GetSuperClassName(), &wc))
        {
            if(!::GetClassInfoEx(CDuiPaintManager::GetInstance(), GetSuperClassName(), &wc))
            {
                ASSERT(!"Unable to locate window class");
                return FALSE;
            }
        }
        m_OldWndProc = wc.lpfnWndProc;
        wc.lpfnWndProc = CDuiWnd::__ControlProc;
        wc.hInstance = CDuiPaintManager::GetInstance();
        wc.lpszClassName = GetWindowClassName();
        ATOM ret = ::RegisterClassEx(&wc);
        ASSERT(ret != NULL || ::GetLastError() == ERROR_CLASS_ALREADY_EXISTS);
        return ret != NULL || ::GetLastError() == ERROR_CLASS_ALREADY_EXISTS;
    }

    LRESULT CALLBACK CDuiWnd::__WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        CDuiWnd* pThis = NULL;
        if(uMsg == WM_NCCREATE)
        {
            LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
            pThis = static_cast<CDuiWnd*>(lpcs->lpCreateParams);
            pThis->m_hWnd = hWnd;
            ::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LPARAM>(pThis));
        }
        else
        {
            pThis = reinterpret_cast<CDuiWnd*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
            if(uMsg == WM_NCDESTROY && pThis != NULL)
            {
                LRESULT lRes = ::CallWindowProc(pThis->m_OldWndProc, hWnd, uMsg, wParam, lParam);
                ::SetWindowLongPtr(pThis->m_hWnd, GWLP_USERDATA, 0L);
                if(pThis->m_bSubclassed)
                {
                    pThis->UnSubClass();
                }
                pThis->m_hWnd = NULL;
                pThis->OnFinalMessage(hWnd);
                return lRes;
            }
        }
        if(pThis != NULL)
        {
            return pThis->HandleMessage(uMsg, wParam, lParam);
        }
        else
        {
            return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
        }
    }

    LRESULT CALLBACK CDuiWnd::__ControlProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        CDuiWnd* pThis = NULL;
        if(uMsg == WM_NCCREATE)
        {
            LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
            pThis = static_cast<CDuiWnd*>(lpcs->lpCreateParams);
            ::SetProp(hWnd, _T("WndX"), (HANDLE) pThis);
            pThis->m_hWnd = hWnd;
        }
        else
        {
            pThis = reinterpret_cast<CDuiWnd*>(::GetProp(hWnd, _T("WndX")));
            if(uMsg == WM_NCDESTROY && pThis != NULL)
            {
                LRESULT lRes = ::CallWindowProc(pThis->m_OldWndProc, hWnd, uMsg, wParam, lParam);
                if(pThis->m_bSubclassed)
                {
                    pThis->UnSubClass();
                }
                ::SetProp(hWnd, _T("WndX"), NULL);
                pThis->m_hWnd = NULL;
                pThis->OnFinalMessage(hWnd);
                return lRes;
            }
        }
        if(pThis != NULL)
        {
            return pThis->HandleMessage(uMsg, wParam, lParam);
        }
        else
        {
            return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
        }
    }

}