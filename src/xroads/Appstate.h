#pragma once

#include "Containers.h"
#include "GUI.h"

namespace Xroads
{

    struct Appstate
    {
        //TODO: make functions to populate these
        std::wstring wgamename{L"gamename"};
        std::string gamename{"gamename"};

        Sm32 state{"MENU_MAIN"_sm};
        int gamepad_id{-1};
        C2 cursorpos{}; //current mouse cursor position in the GUI coordinates
        bool has_focus{true};

        Tracked<int> sound_volume{10};
        Tracked<int> music_volume{10};

        struct ButtonData
        {
            bool clicked{false};
            Args arg{};
            C2 position{};
        };
        Hashmap<GUIBUTTON, ButtonData> btns;

        bool is_in_menu() { return state.startswith("MENU_"_sm); }
    };
}
