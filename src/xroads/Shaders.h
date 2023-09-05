#pragma once

namespace Xroads
{
    struct Shaders
    {
        static VectorMap<std::string, u32> shaders;
        static u32 Get(const std::string& name);
        static void LoadShaders(const std::string& name);
        static void UnloadAll();
    };
}
