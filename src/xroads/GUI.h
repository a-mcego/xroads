#pragma once

namespace Xroads
{
    using GUIBUTTON = Sm32;

    //arguments for GUI buttons
    struct Args
    {
        i64 _i64{};
        std::string _string{};
    };

    struct Browser
    {
        int total=0, perpage=0;
        int start=0, end=0;

        Browser() {}
        Browser(int total_items, int items_per_page):total(total_items),perpage(items_per_page),start(0),end(items_per_page){}

        bool CanNext() { return start+perpage < total; }
        bool CanPrev() { return start-perpage >= 0; }

        void Next()
        {
            if (!CanNext())
                return;
            start += perpage;
            end += perpage;
        }
        void Prev()
        {
            if (!CanPrev())
                return;
            start -= perpage;
            end -= perpage;
        }
        void Message(int msg)
        {
            if (msg == 0)
                Prev();
            else if (msg == 1)
                Next();
            else
                Kill("Browser got invalid message "+ToString(msg));
        }

        int Start()
        {
            return start;
        }
        int End()
        {
            return end;
        }
    };

    struct GUI
    {
        const float SLIDER_PADDING = 0.05f; //how much, on each side, we pad to the slider.

        enum struct STYLE
        {
            NORMAL,
            HIGHLIGHTED,
            DISABLED
        };
        float physical_Y, physical_X, logical_Y, logical_X;
        C2 LogToPhysPos(const C2& p)
        {
            C2 ret;
            ret.x = (p.x+logical_X)/(2.0*logical_X)*physical_X;
            ret.y = (p.y+logical_Y)/(2.0*logical_Y)*physical_Y;
            return ret;
        }

        C2 LogToPhysSize(const C2& p)
        {
            C2 ret;
            ret.x = (p.x)/(2.0*logical_X)*physical_X;
            ret.y = (p.y)/(2.0*logical_Y)*physical_Y;
            return ret;
        }

        void DrawPager(Browser& browser, GUIBUTTON button, C2 prevpos, C2 nextpos, C2 size)
        {
            if(browser.CanPrev())
                GUI::DrawButton(button, 0, "<<\nPrev", prevpos,size);
            if(browser.CanNext())
                GUI::DrawButton(button, 1, ">>\nNext", nextpos,size);
        }

        bool MouseDown(int kind);
        void MouseUp(int kind);

        void DrawButton(GUIBUTTON name, int argument, std::string_view text, C2 pos, C2 size, STYLE style=STYLE::NORMAL, Color color = Color{0x53/255.f/.4f, 0x4f/255.f/.4f, 0x42/255.f/.4f});
        void DrawButton(GUIBUTTON name, std::string_view argument, std::string_view text, C2 pos, C2 size, STYLE style=STYLE::NORMAL, Color color = Color{0x53/255.f/.4f, 0x4f/255.f/.4f, 0x42/255.f/.4f});

        void DrawImageButton(GUIBUTTON name, int argument, GLuint texture, C2 pos, C2 size, STYLE style=STYLE::NORMAL, Color color = Color{1.0f,1.0f,1.0f});
        void DrawSlider(GUIBUTTON name, std::string_view text, Tracked<int>& ci, C2 pos, C2 size);

        void StartFrame();
        void EndFrame();

        void SetLogicalSize(float y, float x)
        {
            logical_Y = y, logical_X = x;
        }
    };
}
