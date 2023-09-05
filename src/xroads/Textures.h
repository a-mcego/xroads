#pragma once

#include "Types.h"
#include "GlIncludes.h"
#include "Containers.h"

namespace Xroads
{
    enum struct WRAP
    {
        NO,
        YES,
        N
    };

    const GLuint NO_TEXTURE = 0;

    class Textures
    {
    public:
        static void Load(const std::string& name, WRAP wrap);
        static u32 Get(const std::string& name, WRAP wrap = WRAP::YES);
        static void UnloadAll();

        static void Unload(u32& n);

        static VectorMap<std::string, u32> textures;
    };
}
