#include "xroads/Xroads.h"

namespace Xroads
{
    Engine& GetEngine()
    {
        static Engine engine;
        return engine;
    }

    void Engine::Init()
    {
        input.Init();
        screen.Init(true, "mainfont"); //TODO: get the parameters from the game
        renderer.Init();
        files.Init();
        sound.Init();
    }
    void Engine::Quit()
    {
        input.Quit();
        //screen.Quit();
        renderer.Quit();
        files.Quit();
        sound.Quit();
    }
}
