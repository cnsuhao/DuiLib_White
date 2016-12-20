#pragma once
namespace DuiLib
{

#define UILIST_MAX_COLUMNS 32

    typedef struct tagListInfo
    {
        BOOL bAlternateBk;
        BOOL bShowRowLine;
        BOOL bShowColumnLine;
        BOOL bShowHtml;
        BOOL bMultiExpandable;
        DWORD dwTextColor;
        DWORD dwBkColor;
        DWORD dwSelectedTextColor;
        DWORD dwSelectedBkColor;
        DWORD dwHotTextColor;
        DWORD dwHotBkColor;
        DWORD dwDisabledTextColor;
        DWORD dwDisabledBkColor;
        DWORD dwLineColor;
        int nColumns;
        int nFont;
        UINT uTextStyle;
        CDuiRect rcTextPadding;
        CDuiRect rcColumn[UILIST_MAX_COLUMNS];
        CDuiString sBkImage;
        CDuiString sSelectedImage;
        CDuiString sHotImage;
        CDuiString sDisabledImage;
    } ListInfo;

    class IListOwner
    {
    public:
        virtual UINT GetListType() = 0;
        virtual ListInfo* GetListInfo() = 0;
        virtual int GetCurSel() const = 0;
        virtual BOOL SelectItem(int iIndex, BOOL bTakeFocus = FALSE) = 0;
        virtual BOOL SelectMultiItem(int iIndex, BOOL bTakeFocus = FALSE) = 0;
        virtual BOOL UnSelectItem(int iIndex, BOOL bOthers = FALSE) = 0;
        virtual void DoEvent(TEventUI& event) = 0;
    };

    class IListItem
    {
    public:
        virtual int GetIndex() const = 0;
        virtual void SetIndex(int iIndex) = 0;
        virtual IListOwner* GetOwner() = 0;
        virtual void SetOwner(CDuiControl* pOwner) = 0;
        virtual BOOL IsSelected() const = 0;
        virtual BOOL Select(BOOL bSelect = TRUE) = 0;
        virtual BOOL SelectMulti(BOOL bSelect = TRUE) = 0;
        virtual BOOL IsExpanded() const = 0;
        virtual BOOL Expand(BOOL bExpand = TRUE) = 0;
    };

    class DUILIB_API CDuiListElement
        : public CDuiControl
        , public IListItem
    {
        DECLARE_DUICONTROL(CDuiListElement)
    public:
        CDuiListElement(void);
        virtual ~CDuiListElement(void);
    public:
        LPCTSTR GetClass() const override;
        LPVOID GetInterface(LPCTSTR pstrName) override;
        UINT GetControlFlags() const override;

        void Invalidate(); // 直接CControl::Invalidate会导致滚动条刷新，重写减少刷新区域
        BOOL Activate() override;

        int GetIndex() const override;
        void SetIndex(int iIndex) override;
        IListOwner* GetOwner();
        void SetOwner(CDuiControl* pOwner) override;
        BOOL IsSelected() const override;
        BOOL Select(BOOL bSelect = TRUE) override;
        BOOL SelectMulti(BOOL bSelect = TRUE) override;
        BOOL IsExpanded() const override;
        BOOL Expand(BOOL bExpand = TRUE) override;

        void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) override;

        void DoEvent(TEventUI& event) override;
    private:
        BOOL m_bSelected;
        int m_iIndex;
        IListOwner* m_pOwner;
    };

    class DUILIB_API CDuiListLabelElement : public CDuiListElement
    {
        DECLARE_DUICONTROL(CDuiListLabelElement)
    public:
        CDuiListLabelElement(void);
        virtual ~CDuiListLabelElement(void);
    public:
        LPCTSTR GetClass() const override ;
        LPVOID GetInterface(LPCTSTR pstrName) override ;

        SIZE EstimateSize(SIZE szAvailable) override;

        virtual void DrawItemBk(HDC hDC, const RECT& rcItem) ;
        virtual void DrawItemText(HDC hDC, const RECT& rcItem) ;
        void DoPaint(HDC hDC, const RECT& rcPaint) override;

        void DoEvent(TEventUI& event) override;
    private:
        UINT m_uButtonState;
    };


    class CDuiCombo;

    class CDuiComboWnd
        : public CDuiWnd
        , public INotifyUI
    {
    public:
        CDuiComboWnd(void);
        virtual ~CDuiComboWnd(void);
    public:
        void Init(CDuiCombo* pOwner);

        LPCTSTR GetWindowClassName() const override;
        void OnFinalMessage(HWND hWnd) override;
#if(_WIN32_WINNT >= 0x0501)
        UINT GetClassStyle() const override;
#endif
        BOOL IsHitItem(POINT ptMouse);

        void Scroll(int dx, int dy);
        void EnsureVisible(int iIndex);

        virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
        virtual LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
        virtual LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
        virtual LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
        virtual LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
        virtual LRESULT OnMouseWheel(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
        virtual LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
        LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

        void Notify(TNotifyUI& msg) override;
    private:
        BOOL m_bHitItem;
        int m_iOldSel;

        CDuiCombo* m_pOwner;
        CDuiVerticalLayout* m_pLayout;

        CDuiPaintManager m_PaintManager;
    };

    class DUILIB_API CDuiCombo
        : public CDuiContainer
        , public IListOwner
    {
        DECLARE_DUICONTROL(CDuiCombo)
    public:
        CDuiCombo(void);
        virtual ~CDuiCombo(void);
    public:
        LPCTSTR GetClass() const override;
        LPVOID GetInterface(LPCTSTR pstrName) override;

        CDuiString GetText() const override;
        void EmptyComboWnd();

        UINT GetButtonState() const;
        void SetButtonState(UINT uButtonState);

        UINT GetListType() override;
        ListInfo* GetListInfo() override;
        int GetCurSel() const override;
        BOOL SelectItem(int iIndex, BOOL bTakeFocus = FALSE) override;
        BOOL SelectMultiItem(int iIndex, BOOL bTakeFocus = FALSE) override;
        BOOL UnSelectItem(int iIndex, BOOL bOthers = FALSE) override;

        BOOL SetItemIndex(CDuiControl* pControl, int iIndex) override;
        BOOL Add(CDuiControl* pControl) override;
        BOOL AddAt(CDuiControl* pControl, int iIndex) override;
        BOOL Remove(CDuiControl* pControl) override;
        BOOL RemoveAt(int iIndex) override;
        void RemoveAll() override;

        int GetFont() const;
        void SetFont(int index);

        DWORD GetTextColor() const;
        void SetTextColor(DWORD dwTextColor);

        DWORD GetDisabledTextColor() const;
        void SetDisabledTextColor(DWORD dwTextColor);

        CDuiRect GetTextPadding() const;
        void SetTextPadding(CDuiRect rc);

        BOOL IsShowHtml() const;
        void SetShowHtml(BOOL bShowHtml = TRUE);

        LPCTSTR GetNormalImage() const;
        void SetNormalImage(LPCTSTR pStrImage);

        LPCTSTR GetHotImage() const;
        void SetHotImage(LPCTSTR pStrImage);

        LPCTSTR GetPushedImage() const;
        void SetPushedImage(LPCTSTR pStrImage);

        LPCTSTR GetFocusedImage() const;
        void SetFocusedImage(LPCTSTR pStrImage);

        LPCTSTR GetDisabledImage() const;
        void SetDisabledImage(LPCTSTR pStrImage);

        BOOL GetScrollSelect() const;
        void SetScrollSelect(BOOL bScrollSelect);

        CDuiString GetDropBoxAttributeList() const;
        void SetDropBoxAttributeList(LPCTSTR pstrList);

        SIZE GetDropBoxSize() const;
        void SetDropBoxSize(SIZE szDropBox);

        RECT GetItemTextPadding() const;
        void SetItemTextPadding(RECT rc);

        DWORD GetItemTextColor() const;
        void SetItemTextColor(DWORD dwTextColor);

        DWORD GetItemBkColor() const;
        void SetItemBkColor(DWORD dwBkColor);

        LPCTSTR GetItemBkImage() const;
        void SetItemBkImage(LPCTSTR pStrImage);

        BOOL IsAlternateBk() const;
        void SetAlternateBk(BOOL bAlternateBk);

        DWORD GetSelectedItemTextColor() const;
        void SetSelectedItemTextColor(DWORD dwTextColor);

        DWORD GetSelectedItemBkColor() const;
        void SetSelectedItemBkColor(DWORD dwBkColor);

        LPCTSTR GetSelectedItemImage() const;
        void SetSelectedItemImage(LPCTSTR pStrImage);

        DWORD GetHotItemTextColor() const;
        void SetHotItemTextColor(DWORD dwTextColor);

        DWORD GetHotItemBkColor() const;
        void SetHotItemBkColor(DWORD dwBkColor);

        LPCTSTR GetHotItemImage() const;
        void SetHotItemImage(LPCTSTR pStrImage);

        DWORD GetDisabledItemTextColor() const;
        void SetDisabledItemTextColor(DWORD dwTextColor);

        DWORD GetDisabledItemBkColor() const;
        void SetDisabledItemBkColor(DWORD dwBkColor);

        LPCTSTR GetDisabledItemImage() const;
        void SetDisabledItemImage(LPCTSTR pStrImage);

        DWORD GetItemLineColor() const;
        void SetItemLineColor(DWORD dwLineColor);

        BOOL IsItemShowHtml();
        void SetItemShowHtml(BOOL bShowHtml = TRUE);

        void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) override;

        SIZE EstimateSize(SIZE szAvailable) override;
        void SetPos(RECT rc, BOOL bNeedInvalidate = TRUE) override;

        void DoPaint(HDC hDC, const RECT& rcPaint) override;
        void PaintText(HDC hDC) override;
        void PaintStatusImage(HDC hDC) override;

        BOOL Activate() override;
        void DoEvent(TEventUI& event) override;
    private:
        CDuiComboWnd* m_pComboWnd;
        BOOL m_bShowHtml;
        BOOL m_bScrollSelect;
        int m_iCurSel;
        int	m_iFont;
        UINT m_uTextStyle;
        UINT m_uButtonState;
        DWORD m_dwTextColor;
        DWORD m_dwDisabledTextColor;

        ListInfo m_ListInfo;

        CDuiSize m_szDropBox;

        CDuiRect m_rcTextPadding;

        CDuiString m_sNormalImage;
        CDuiString m_sHotImage;
        CDuiString m_sPushedImage;
        CDuiString m_sFocusedImage;
        CDuiString m_sDisabledImage;

        CDuiString m_sDropBoxAttributes;
    };

}