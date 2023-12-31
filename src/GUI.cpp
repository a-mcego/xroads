#include "xroads/Xroads.h"

namespace Xroads
{
    struct ButtonDef
    {
        GUIBUTTON guibutton{"NONE"_sm};
        Args arg;
        std::string text{};
        Xr::C2 pos{}, size{};
        GUI::STYLE style{GUI::STYLE::NORMAL};
        Xr::Color color{};
        enum TYPE
        {
            BUTTON,
            SLIDER,
            N
        } type = TYPE::BUTTON;
    };
    std::vector<ButtonDef> buttondefs, old_buttondefs;

    ButtonDef current_slider;

    void GUI::DrawButton(GUIBUTTON name, int argument, std::string_view text, Xr::C2 pos, Xr::C2 size, STYLE style, Xr::Color color)
    {
        buttondefs.push_back(ButtonDef{name,Args{._i64=argument},std::string(text),pos,size,style,color});
    }
    void GUI::DrawButton(GUIBUTTON name, std::string_view argument, std::string_view text, Xr::C2 pos, Xr::C2 size, STYLE style, Xr::Color color)
    {
        buttondefs.push_back(ButtonDef{name,Args{._string=std::string(argument)},std::string(text),pos,size,style,color});
    }
    void GUI::DrawSlider(GUIBUTTON name, std::string_view text, Xr::Tracked<int>& ci, Xr::C2 pos, Xr::C2 size)
    {
        buttondefs.push_back(ButtonDef{name,Args{._i64=ci.Get()},std::string(text),pos,size,STYLE::NORMAL,Xr::Color{0x53/255.f/.4f, 0x4f/255.f/.4f, 0x42/255.f/.4f}, ButtonDef::TYPE::SLIDER});
    }

    void GUI::StartFrame()
    {
        if (current_slider.guibutton != "NONE"_sm)
        {
            Xr::Rect2D<float> rect{current_slider.pos,current_slider.pos+current_slider.size};
            GetEngine().appstate.btns[current_slider.guibutton].clicked = true;
            auto rc = rect.TransformTo(GetEngine().appstate.cursorpos);
            GetEngine().appstate.btns[current_slider.guibutton].position = rc;
            GetEngine().appstate.btns[current_slider.guibutton].arg._i64 = i64((Xr::Clamp(rc.x,SLIDER_PADDING,1.0f-SLIDER_PADDING)-SLIDER_PADDING)/(1.0f-2.0f*SLIDER_PADDING)*100.9f);
        }

        old_buttondefs.swap(buttondefs);
        buttondefs.clear();
    }

    void GUI::EndFrame()
    {
        auto spr = Xr::TextureDef{Xr::Textures::Get("white")};
        auto sh = Xr::Shaders::Get("default2_color");
        for(auto& bd: buttondefs)
        {
            Xr::Rect2D<float> rect{bd.pos,bd.pos+bd.size};
            Xr::Color color = bd.color;
            if ((!rect.is_inside(GetEngine().appstate.cursorpos) && bd.style != GUI::STYLE::HIGHLIGHTED) || bd.style == GUI::STYLE::DISABLED)
                color = color*0.5f;
            else
                color = color*0.95f;
            std::vector<std::string_view> text = Xr::Explode(bd.text,'\n');
            Xr::C2 middle = rect.Center();//Xr::C2{rect.topleft.x+rect.bottomright.x,rect.topleft.y+rect.bottomright.y}*0.5f;

            if (bd.type == ButtonDef::TYPE::BUTTON)
            {
                Xr::GetEngine().renderer.Queue(Xr::Quad{rect,-4.0}, color, spr, sh, Xr::Renderer::STAGE::GUI);
                GetEngine().screen.DrawTexts(std::span(text), 4.0f, middle, Xr::Color{1.0f,1.0f,1.0f}, JUSTIFY::CENTER, 1.1f);
            }
            else if (bd.type == ButtonDef::TYPE::SLIDER)
            {
                const float SLIDER_WIDTH = 3.0f;
                Xr::C2 halfpos{2.0f*0.5f,SLIDER_WIDTH*0.5f};
                Xr::C2 widgetpos{bd.size.x*(float(bd.arg._i64)/100.9f*(1.0f-SLIDER_PADDING*2.0f)+SLIDER_PADDING-0.5f),0.0f};
                Xr::Rect2D<float> slider_widget{middle-halfpos+widgetpos,middle+halfpos+widgetpos};
                Xr::GetEngine().renderer.Queue(Xr::Quad{slider_widget,-4.0}, color, spr, sh, Xr::Renderer::STAGE::GUI);
                GetEngine().screen.DrawTexts(std::span(text), 4.0f, {middle.x,(middle.y+rect.topleft.y)*0.5f}, Xr::Color{1.0f,1.0f,1.0f}, JUSTIFY::CENTER, 1.1f);
                GetEngine().screen.DrawText(Xr::Format("{}"_sm,bd.arg._i64), 4.0f, middle, Xr::Color{1.0f,1.0f,1.0f}, JUSTIFY::CENTER);
            }
        }
    }

    bool GUI::MouseDown(int kind)
    {
        if (kind != 0)
            return false;
        bool ret=false;
        for(auto& bd: buttondefs)
        {
            Xr::Rect2D<float> rect{bd.pos,bd.pos+bd.size};
            if (rect.is_inside(GetEngine().appstate.cursorpos))
            {
                GetEngine().appstate.btns[bd.guibutton].clicked = true;
                GetEngine().appstate.btns[bd.guibutton].arg = bd.arg;
                GetEngine().appstate.btns[bd.guibutton].position = rect.TransformTo(GetEngine().appstate.cursorpos);
                ret = true;

                if (bd.type == ButtonDef::TYPE::SLIDER)
                {
                    current_slider = bd;
                }
            }
        }
        return ret;
    }
    void GUI::MouseUp(int kind)
    {
        if (kind != 0)
            return;
        if (current_slider.guibutton == "OPTIONS_SOUND_VOLUME"_sm)
            GetEngine().sound.Play("buttonclick"_sm);
        current_slider = ButtonDef{};
    }
}
