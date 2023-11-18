#include "Global.h"
#include "xroads/Shaders.h"
#include "xroads/Renderer.h"

namespace Xroads
{
    VectorMap<std::string, GLuint> Shaders::shaders;

    GLuint Shaders::Get(std::string_view name)
    {
        auto it = shaders.find(name);
        if (it != shaders.end())
        {
            return it->second;
        }
        LoadShaders(name);
        it = shaders.find(name);
        if (it != shaders.end())
        {
            return it->second;
        }
        it = shaders.find("none");
        if (it != shaders.end())
        {
            return it->second;
        }
        LoadShaders("none");
        return shaders["none"];
    }

    void Shaders::LoadShaders(std::string_view name)
    {
        shaders[name] = Renderer::LoadShader(name);
    }

    void Shaders::UnloadAll()
    {
        for (auto& it : shaders)
            glDeleteProgram(it.second);
        shaders.clear();
    }
}

