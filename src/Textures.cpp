#include "Global.h"
#include "xroads/Textures.h"
#include "Screen.h"

namespace Xroads
{
    VectorMap<string, u32> Textures::textures;

    GLuint Textures::Get(const std::string& name, WRAP wrap)
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
        it = textures.find("none");
        if (it != textures.end())
        {
            return it->second;
        }
        Load("none", wrap);
        return textures["none"];
    }

    void Textures::Load(const std::string& name, WRAP wrap)
    {
        u32 textureID = Renderer::LoadTexture(name, wrap);
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

