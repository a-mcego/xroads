#include "Global.h"
#include "xroads/Renderer.h"
#include <fstream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace Xroads
{
    GLuint Renderer::vertexarray_id;
    GLuint Renderer::vertexbuffer_id;
    int Renderer::trianglecount = 0;
    int Renderer::searches = 0;
    int Renderer::getvecs = 0;
    float Renderer::aberration{};
    int Renderer::n_lights_last_frame = 0;
    float Renderer::flash_amount{};
    Xr::Color Renderer::flash_color{};

    std::array<std::vector<Renderer::RenderList>,int(Renderer::STAGE::N)> Renderer::renderlists;
    std::vector<Renderer::LightDef> Renderer::lights;

    map<ModelID, vector<std::pair<Xr::Color,glm::mat4>>> Renderer::modelqueue;
    array<glm::mat4,int(Renderer::CAMERA::N)> Renderer::Vs, Renderer::Ps;

    vector<Renderer::Model> Renderer::models;
    map<string, ModelID> Renderer::model_ids;

    GLuint Renderer::gBuffer{}, Renderer::gPosition{}, Renderer::gNormal{}, Renderer::gAlbedoSpec{}, Renderer::gColorSpec{}, Renderer::rboDepth{}, Renderer::gBright{}, Renderer::gNormalOut{};
    GLuint Renderer::quadVAO{}, Renderer::quadVBO{}, Renderer::fbSecond{};
    Renderer::FBTex Renderer::pingpong[2];

    u32 Renderer::LoadTexture(string name, WRAP wrap)
    {
        string imagepath = string("textures/") + name + ".png";
        FILE *fp;
        fp = fopen(imagepath.c_str(), "rb");
        if (fp == NULL)
        {
            cout << "Texture " << imagepath << " not found" << endl;
            return -1;
        }

        int x,y,n;
        unsigned char* data_ptr = stbi_load(imagepath.c_str(), &x, &y, &n, 0);
        vector<unsigned char> data{data_ptr, data_ptr+x*y*n};

        //cout << "Loaded image " << imagepath << " with xyn " << x << " " << y << " " << n << endl;

        stbi_image_free(data_ptr);

        unsigned int components = n;

        unsigned int gl_format = 0, gl_internal_format = 0;
        switch(components)
        {
        case 3:
            gl_format = GL_RGB;
            gl_internal_format = GL_SRGB8;
            break;
        case 4:
            gl_format = GL_RGBA;
            gl_internal_format = GL_SRGB8_ALPHA8;
            break;
        }

        // Create one OpenGL texture
        GLuint textureID;
        glGenTextures(1, &textureID);

        // Bind the newly created texture : all future texture functions will modify this texture
        glBindTexture(GL_TEXTURE_2D, textureID);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        glTexImage2D(GL_TEXTURE_2D, 0, gl_internal_format, x, y, 0, gl_format, GL_UNSIGNED_BYTE, &data[0]);

        if (name == "mainfont_scifi" || name == "mainfont")
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        }
        else
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        }
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 8);
        if (wrap == WRAP::YES)
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        }
        else
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }

        //cout << "Texture \"" << name << "\" successfully loaded as ID " << textureID << endl;
        return textureID;
    }

    u32 Renderer::LoadShader(string name)
    {
        cout << "Load shader: " << name << endl;

        // Read the Vertex Shader code from the file
        std::string VertexShaderCode;
        std::ifstream VertexShaderStream("shaders/" + name + "_vertex.glsl", std::ios::in);
        if (VertexShaderStream.is_open())
        {
            std::string Line = "";
            while (getline(VertexShaderStream, Line))
                VertexShaderCode += "\n" + Line;
            VertexShaderStream.close();
        }

        // Read the Fragment Shader code from the file
        std::string FragmentShaderCode;
        std::ifstream FragmentShaderStream("shaders/" + name + "_fragment.glsl", std::ios::in);
        if (FragmentShaderStream.is_open()){
            std::string Line = "";
            while (getline(FragmentShaderStream, Line))
                FragmentShaderCode += "\n" + Line;
            FragmentShaderStream.close();
        }

        GLint Result = GL_FALSE;
        int InfoLogLength;

        GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
        // Compile Vertex Shader
        cout << "Compiling shader " << name << "_vertex" << endl;
        char const * VertexSourcePointer = VertexShaderCode.c_str();
        glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
        glCompileShader(VertexShaderID);

        // Check Vertex Shader
        glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
        glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
        std::vector<char> VertexShaderErrorMessage(InfoLogLength);
        glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
        fprintf(stdout, "%s\n", &VertexShaderErrorMessage[0]);

        GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
        // Compile Fragment Shader
        cout << "Compiling shader " << name << "_fragment" << endl;
        char const * FragmentSourcePointer = FragmentShaderCode.c_str();
        glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
        glCompileShader(FragmentShaderID);

        // Check Fragment Shader
        glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
        glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
        std::vector<char> FragmentShaderErrorMessage(InfoLogLength);
        glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
        fprintf(stdout, "%s\n", &FragmentShaderErrorMessage[0]);

        // Link the program
        fprintf(stdout, "Linking program\n");
        GLuint ProgramID = glCreateProgram();
        glAttachShader(ProgramID, VertexShaderID);
        glAttachShader(ProgramID, FragmentShaderID);
        glLinkProgram(ProgramID);

        // Check the program
        glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
        glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
        std::vector<char> ProgramErrorMessage(glm::max(InfoLogLength, int(1)));
        glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
        fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);

        glDeleteShader(VertexShaderID);
        glDeleteShader(FragmentShaderID);

        return ProgramID;
    }
}

