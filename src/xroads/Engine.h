#pragma once

#include "xroads/Input.h"
#include "xroads/Screen.h"
#include "xroads/Renderer.h"
#include "xroads/Appstate.h"
#include "xroads/Config.h"
#include "xroads/FileUtil.h"
#include "xroads/Sound.h"
#include "xroads/GUI.h"

namespace Xroads
{
    struct Engine
    {
        Input input;
        Screen screen;
        Renderer renderer;
        Appstate appstate;
        Config config;
        Files files;
        Sound sound;
        GUI gui;

        void Init();
        void Quit();
    };

    //get the engine singleton
    Engine& GetEngine();
}
