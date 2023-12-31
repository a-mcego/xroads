#pragma once

#include "xroads/Color.h"
#include "xroads/Shaders.h"
#include "xroads/Shapes.h"

#include <algorithm>
#include <span>

namespace Xroads
{
    enum struct JUSTIFY
    {
        LEFT,
        RIGHT,
        CENTER
    };
    struct Screen
    {
        int SIZE_X = 1024;
        int SIZE_Y = 768;

        float ORTHO_SIZE{};
        float ASPECT_RATIO{};

        float ORTHO_SIZE_X{};
        float ORTHO_SIZE_Y{};

        GLFWwindow* glfw_window{};
        std::vector<TextureDef> mainfont;

        bool cursor_enabled{true}; //TODO: make this work

        void Init(bool is_fullscreen, std::string_view fontname);
        void Quit();
        void SizeCallback(int width, int height);
        void CreateWindow(bool fullscreen);

        const float FONTRATIO = 12.0f/23.0f; //XGA Font x/y ratio. TODO: make this configurable by font
        //template<typename CONTAINER>//=std::string_view> requires std::convertible_to<INSIDETYPE,std::string_view>
        void DrawTexts(const std::span<std::string_view>& text, float size, C2 pos, Color color={1.f,1.f,1.f,1.f}, JUSTIFY justify = JUSTIFY::LEFT, float line_spacing=1.0f);
        void DrawText(const std::string_view& text, float size, C2 pos, Color color={1.f,1.f,1.f,1.f}, JUSTIFY justify = JUSTIFY::LEFT);
        void DrawCentered(const std::string_view& text, float font_size, C2 pos, Color color={1.f,1.f,1.f,1.f});

    };
}
