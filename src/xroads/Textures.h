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
        static void Load(std::string_view name, WRAP wrap);
        static bool Has(std::string_view name);
        static C2i GetTextureSize(u32 id);
        static u32 Get(std::string_view, WRAP wrap = WRAP::YES);
        static void UnloadAll();

        static void Unload(u32& n);

        static VectorMap<std::string, u32> textures;
    };
}
