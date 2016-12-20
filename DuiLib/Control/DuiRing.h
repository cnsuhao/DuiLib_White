#pragma once
namespace DuiLib
{
    class DUILIB_API CDuiRing
        : public CDuiLabel
    {
        DECLARE_DUICONTROL(CDuiRing)
    public:
        CDuiRing(void);
        virtual ~CDuiRing(void);
    public:
        LPCTSTR GetClass() const override;
        LPVOID GetInterface(LPCTSTR pstrName) override;

        void SetBkImage(LPCTSTR pStrImage) override;

        void PaintBkImage(HDC hDC) override;

        void DoEvent(TEventUI& event) override;
    private:
        void InitImage();
        void DeleteImage();
    private:
        float m_fCurAngle;
        Gdiplus::Image* m_pBkimage;
    };

}