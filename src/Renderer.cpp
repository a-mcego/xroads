#include "Global.h"
#include "xroads/Renderer.h"
#include <fstream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace Xroads
{

    GLuint Renderer::vertexarray_id;
    GLuint Renderer::vertexbuffer_id;
    i64 Renderer::trianglecount = 0;
    int Renderer::searches = 0;
    int Renderer::getvecs = 0;
    //int Renderer::n_lights_last_frame = 0;
    Renderer::Options Renderer::options{};
    CENTERING Renderer::current_centering = CENTERING::XYZ;
    //std::vector<std::string> Renderer::light_uniform_names;

    std::array<std::vector<Renderer::RenderList>,int(Renderer::STAGE::N)> Renderer::renderlists;
    //std::array<std::vector<Renderer::LightDef>,3> Renderer::lights;

    std::map<ModelID, Renderer::ModelQueueData> Renderer::modelqueue, Renderer::modelqueueUI, Renderer::modelqueue_lightvolume;
    std::array<glm::mat4,int(Renderer::CAMERA::N)> Renderer::Vs, Renderer::Ps;
    std::array<C3,int(Renderer::CAMERA::N)> Renderer::Vs_normal;
    GLuint Renderer::modelVBO[2];

    std::vector<Renderer::Model> Renderer::models;
    std::map<std::string, ModelID, StringLessThan> Renderer::model_ids;

    GLuint Renderer::gBuffer{}, Renderer::gPosition{}, Renderer::gNormal{}, Renderer::gAlbedoSpec{}, Renderer::gColorSpec{}, Renderer::rboDepth{}, Renderer::gBright{}, Renderer::gNormalOut{}, Renderer::gLighting;
    GLuint Renderer::quadVAO{}, Renderer::quadVBO{}, Renderer::fbSecond{}, Renderer::fbLighting{};
    Renderer::FBTex Renderer::pingpong[2];

    C2i Renderer::current_resolution{1,1};

    void Renderer::QueueLight(const C3& pos, const Color& color, f32 intensity)
    {
        //lights.at(priority).push_back({pos, color*2.0f, intensity});
        float extent = std::sqrt((1.0f/0.025f-1.0f)/intensity)*2.0f; //2.0 for obj loading scaling
        glm::mat4 Mmat(1.0f);
        Mmat[0][0] = extent;
        Mmat[1][1] = extent;
        Mmat[2][2] = extent;
        Mmat[3] = glm::vec4(pos.x, pos.y, pos.z, intensity);
        QueueModel(modelqueue_lightvolume, "lightvolume"_sm, 0, Mmat, color);
    }

    u32 Renderer::LoadTexture(std::string_view name, WRAP wrap)
    {
        std::string imagepath = std::string("textures/") + std::string(name) + ".png";
        FILE* fp = fopen(imagepath.c_str(), "rb");
        if (fp == NULL)
        {
            Log("Texture "+imagepath+" not found");
            return -1;
        }
        fclose(fp);

        int x{},y{},n{};
        unsigned char* data_ptr = stbi_load(imagepath.c_str(), &x, &y, &n, 0);
        if(data_ptr == nullptr)
        {
            Log("stbi_load() failed with image " + imagepath);
            return -1;
        }

        //cout << "Loaded image " << imagepath << " with xyn " << x << " " << y << " " << n << endl;

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

        glTexImage2D(GL_TEXTURE_2D, 0, gl_internal_format, x, y, 0, gl_format, GL_UNSIGNED_BYTE, data_ptr);
        stbi_image_free(data_ptr);

        if (name == "_mainfont_scifi" || name == "mainfont")
        {
            glGenerateMipmap(GL_TEXTURE_2D);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, -1.7f);
        }
        else
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
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

    u32 Renderer::LoadShader(std::string_view name)
    {
        Log("Load shader: "+std::string(name));

        // Read the Vertex Shader code from the file
        std::string VertexShaderCode = "#version 430\n"+options.GetShaderDefines();
        //std::ifstream VertexShaderStream("shaders/" + std::string(name) + "_vertex.glsl", std::ios::in);
        std::ifstream VertexShaderStream(std::format("shaders/{}_vertex.glsl",name), std::ios::in);
        if (VertexShaderStream.is_open())
        {
            std::string Line = "";
            while (getline(VertexShaderStream, Line))
                VertexShaderCode += "\n" + Line;
            VertexShaderStream.close();
        }
        else
            Kill("Shader " + std::string(name) + " not found!");

        // Read the Fragment Shader code from the file
        std::string FragmentShaderCode = "#version 430\n"+options.GetShaderDefines();
        std::ifstream FragmentShaderStream(std::format("shaders/{}_fragment.glsl",name), std::ios::in);
        if (FragmentShaderStream.is_open()){
            std::string Line = "";
            while (getline(FragmentShaderStream, Line))
                FragmentShaderCode += "\n" + Line;
            FragmentShaderStream.close();
        }
        else
            Kill("Shader " + std::string(name) + " not found!");

        GLint Result = GL_FALSE;
        int InfoLogLength;

        GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
        // Compile Vertex Shader
        Log(std::format("Compiling shader {}_vertex",name));
        char const * VertexSourcePointer = VertexShaderCode.c_str();
        glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
        glCompileShader(VertexShaderID);

        // Check Vertex Shader
        glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
        glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);

        if (InfoLogLength > 0)
        {
            std::vector<char> VertexShaderErrorMessage(InfoLogLength);
            glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
            Log(std::string("Vertex shader log: ") + &VertexShaderErrorMessage[0]);
        }
        //fprintf(stdout, "%s\n", &VertexShaderErrorMessage[0]);

        GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
        // Compile Fragment Shader
        Log("Compiling shader "+std::string(name)+"_fragment");
        char const * FragmentSourcePointer = FragmentShaderCode.c_str();
        glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
        glCompileShader(FragmentShaderID);

        // Check Fragment Shader
        glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
        glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
        if (InfoLogLength > 0)
        {
            std::vector<char> FragmentShaderErrorMessage(InfoLogLength);
            glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
            Log(std::string("Fragment shader log: ") + &FragmentShaderErrorMessage[0]);
        }

        // Link the program
        Log("Linking program");
        GLuint ProgramID = glCreateProgram();
        glAttachShader(ProgramID, VertexShaderID);
        glAttachShader(ProgramID, FragmentShaderID);
        glLinkProgram(ProgramID);

        // Check the program
        glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
        glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
        if (InfoLogLength > 0)
        {
            std::vector<char> ProgramErrorMessage(glm::max(InfoLogLength, int(1)));
            glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
            Log(std::string("Linking program log: ") + &ProgramErrorMessage[0]);
        }

        glDeleteShader(VertexShaderID);
        glDeleteShader(FragmentShaderID);

        return ProgramID;
    }
}

