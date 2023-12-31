#include "xroads/Xroads.h"

namespace Xroads
{
    VectorMap<std::string, u32> Textures::textures;

    bool Textures::Has(std::string_view name)
    {
        return textures.find(name) != textures.end();
    }

    GLuint Textures::Get(std::string_view name, WRAP wrap)
    {
        auto it = textures.find(name);
        if (it != textures.end())
        {
            return it->second;
        }
        Load(name, wrap);
        it = textures.find(name);
        if (it != textures.end())
        {
            return it->second;
        }
        it = textures.find("_none");
        if (it != textures.end())
        {
            return it->second;
        }
        Load("_none", wrap);
        return textures["_none"];
    }

    void Textures::Load(std::string_view name, WRAP wrap)
    {
        u32 textureID = GetEngine().renderer.LoadTexture(name, wrap);
        if (textureID == -1)
            return;
        textures[name] = textureID;
    }

    void Textures::Unload(u32& n)
    {
        if (n != -1)
        {
            glDeleteTextures(1, &n);
            n = -1;
        }
    }

    void Textures::UnloadAll()
    {
        for (auto it : textures)
            glDeleteTextures(1, &it.second);
        textures.clear();
    }
}
