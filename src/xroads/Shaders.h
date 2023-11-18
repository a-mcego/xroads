#pragma once

namespace Xroads
{
    struct Shaders
    {
        static VectorMap<std::string, u32> shaders;
        static u32 Get(std::string_view name);
        static void LoadShaders(std::string_view name);
        static void UnloadAll();
    };
}
