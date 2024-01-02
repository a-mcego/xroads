#include "xroads/Xroads.h"

namespace Xroads
{
    void Screen::Init(bool is_fullscreen, std::string_view fontname)
    {
        if (!glfwInit())
            Kill("Failed to initialize GLFW");

        CreateWindow(GetEngine().config.GetValue<bool>("fullscreen",true));
        mainfont = CutAtlas(Textures::Get(fontname), {32,8});
        Log(ToString(mainfont.size())+" glyphs loaded from main font");
    }

    void window_size_callback([[maybe_unused]] GLFWwindow* window, int width, int height)
    {
        GetEngine().screen.SizeCallback(width,height);
    }
    void window_focus_callback([[maybe_unused]] GLFWwindow* window, int focused)
    {
        Xr::GetEngine().appstate.has_focus = focused;
        if (focused)
        {
            Xr::GetEngine().sound.SetSoundVolume(Xr::GetEngine().appstate.sound_volume.Get());
            Xr::GetEngine().sound.SetMusicVolume(Xr::GetEngine().appstate.music_volume.Get());
        }
        else
        {
            Xr::GetEngine().sound.SetSoundVolume(0);
            Xr::GetEngine().sound.SetMusicVolume(0);
            if (Xr::GetEngine().appstate.state == "INGAME"_sm)
                Xr::GetEngine().appstate.state = "INGAME_PAUSE"_sm;
        }
    }

    void APIENTRY glDebugOutput(GLenum source,
                                GLenum type,
                                unsigned int id,
                                GLenum severity,
                                [[maybe_unused]] GLsizei length,
                                const char *message,
                                [[maybe_unused]] const void *userParam)
    {
        // ignore non-significant error/warning codes
        if(id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

        std::cout << "---------------" << std::endl;
        std::cout << "Debug message (" << id << "): " <<  message << std::endl;

        switch (source)
        {
            case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
            case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
            case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
            case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
            case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
            case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
        } std::cout << std::endl;

        switch (type)
        {
            case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
            case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
            case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break;
            case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
            case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
            case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
            case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
            case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
            case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
        } std::cout << std::endl;

        switch (severity)
        {
            case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
            case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
            case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
            case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
        } std::cout << std::endl;
        std::cout << std::endl;
    }

    void Screen::CreateWindow(bool fullscreen)
    {
        const bool DEBUG_OPENGL = true;
        if (glfw_window == nullptr)
        {
            glfwDefaultWindowHints();
            glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, DEBUG_OPENGL);
            glfwWindowHint(GLFW_SAMPLES, 1); //Antialiasing?
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // We want OpenGL 4.3
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //We don't want the old OpenGL
            GLFWmonitor* monitor = nullptr;
            if (fullscreen)
            {
                monitor = glfwGetPrimaryMonitor();
                const GLFWvidmode* mode = glfwGetVideoMode(monitor);
                SIZE_X = GetEngine().config.GetValue<int>("fullscreen_x",mode->width);
                SIZE_Y = GetEngine().config.GetValue<int>("fullscreen_y",mode->height);
                glfwWindowHint(GLFW_RED_BITS, mode->redBits);
                glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
                glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
                glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
                glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);

            }
            else
            {
                SIZE_X = GetEngine().config.GetValue<int>("windowed_x",1024);
                SIZE_Y = GetEngine().config.GetValue<int>("windowed_y",768);
                glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
            }

            glfw_window = glfwCreateWindow(SIZE_X, SIZE_Y, GetEngine().appstate.gamename.c_str(), monitor, nullptr);
            if (glfw_window == nullptr)
            {
                glfwTerminate();
                Xr::Kill("GPU is not OpenGL 4.3 compatible.");
            }

            GetEngine().input.Init();
            glfwSetWindowSizeCallback(glfw_window, window_size_callback);
            glfwSetWindowFocusCallback(glfw_window, window_focus_callback);
            glfwMakeContextCurrent(glfw_window); // Initialize GLEW

            int result = gladLoadGL((GLADloadfunc)glfwGetProcAddress);
            //TODO: check result

            if (DEBUG_OPENGL)
            {
                glEnable(GL_DEBUG_OUTPUT);
                glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
                glDebugMessageCallback(glDebugOutput, nullptr);
                glDebugMessageControl(GL_DONT_CARE, GL_DEBUG_TYPE_ERROR, GL_DONT_CARE, 0, nullptr, GL_TRUE);
            }
        }
        else
        {
            if (fullscreen)
            {
                const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
                SIZE_X = GetEngine().config.GetValue<int>("fullscreen_x",mode->width);
                SIZE_Y = GetEngine().config.GetValue<int>("fullscreen_y",mode->height);
                glfwSetWindowMonitor(glfw_window, glfwGetPrimaryMonitor(), 0, 0, SIZE_X, SIZE_Y, GLFW_DONT_CARE);
            }
            else
            {
                SIZE_X = GetEngine().config.GetValue<int>("windowed_x",1024);
                SIZE_Y = GetEngine().config.GetValue<int>("windowed_y",768);
                glfwSetWindowMonitor(glfw_window, NULL, 100, 100, SIZE_X, SIZE_Y, GLFW_DONT_CARE);
            }
        }
        SizeCallback(SIZE_X, SIZE_Y);
    }

    void Screen::SizeCallback(int width, int height)
    {
        if (width == 0 or height == 0)
            return;
        if (glfwGetWindowMonitor(glfw_window) == nullptr)
        {
            GetEngine().config.SetValue("windowed_x", width);
            GetEngine().config.SetValue("windowed_y", height);
        }

        if (cursor_enabled)
            glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        else
            glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        SIZE_X = width, SIZE_Y = height;

        ORTHO_SIZE = 50.0f;
        ASPECT_RATIO = float(SIZE_X) / float(SIZE_Y);

        ORTHO_SIZE_X = ORTHO_SIZE*ASPECT_RATIO;
        ORTHO_SIZE_Y = ORTHO_SIZE;

        const double X_WANTED = 800.0/9.0, Y_WANTED = 50.0;

        if (ORTHO_SIZE_X < X_WANTED)
        {
            ORTHO_SIZE_Y = ORTHO_SIZE_Y*(X_WANTED/ORTHO_SIZE_X);
            ORTHO_SIZE_X = X_WANTED;
        }
        if (ORTHO_SIZE_Y < Y_WANTED)
        {
            ORTHO_SIZE_X = ORTHO_SIZE_X*(Y_WANTED/ORTHO_SIZE_Y);
            ORTHO_SIZE_Y = Y_WANTED;
        }

        GetEngine().gui.SetLogicalSize(ORTHO_SIZE_Y, ORTHO_SIZE_X);

        ORTHO_SIZE_X = -ORTHO_SIZE_X;

        glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

        int display_x, display_y;
        glfwGetFramebufferSize(glfw_window, &display_x, &display_y);
        GetEngine().renderer.ResolutionChange({display_x, display_y});

        InputEvent ie;
        ie.type = InputEvent::TYPE::RESOLUTION_CHANGE;
        ie.data = display_x;
        ie.data2 = display_y;
        GetEngine().input.inputqueue.push(ie);
    }

    //TODO: combine DrawTexts and DrawText
    void Screen::DrawTexts(const std::span<std::string_view>& text, float size, C2 pos, Color color, JUSTIFY justify, float line_spacing)
    {
        GLuint shader_id = Shaders::Get("default2_color");

        //u32 longest = std::max_element(text.begin(), text.end(), [](auto& a, auto& b){return a.size() < b.size(); })->size();
        u32 longest = std::ranges::max_element(text,[](auto& a, auto& b){return a.size() < b.size(); })->size();

        float x_delta = size*FONTRATIO; //HACK: XGA FONT for new rule!
        if (justify == JUSTIFY::RIGHT)
        {
            x_delta = -x_delta;
            pos.x += x_delta;
        }

        float curr_y = pos.y;
        if (justify == JUSTIFY::CENTER)
        {
            curr_y -= (size + (text.size()-1)*size*line_spacing)*0.5f;
        }
        curr_y += size*12.0f/138.0f;//offset for the XGA font. the exact value is 3/138 but it's still too high
        for(std::string_view str: text)
        {
            float curr_x = pos.x;
            if (justify == JUSTIFY::CENTER)
            {
                curr_x += (longest-str.size())*0.5*x_delta-(longest)*0.5*x_delta;
            }

            if (justify == JUSTIFY::LEFT or justify == JUSTIFY::CENTER)
            {
                for(u8 c: str)
                {
                    GetEngine().renderer.Queue(Quad{{{curr_x,curr_y},{curr_x+x_delta,curr_y+size}},-2.0}, color, mainfont[c], shader_id, Renderer::STAGE::GUITEXT);
                    curr_x += x_delta;
                }
            }
            else
            {
                for(int i=str.size()-1; i>=0; --i)
                {
                    u8 c = str[i];
                    GetEngine().renderer.Queue(Quad{{{curr_x,curr_y},{curr_x-x_delta,curr_y+size}},-2.0}, color, mainfont[c], shader_id, Renderer::STAGE::GUITEXT);
                    curr_x += x_delta;
                }
            }
            curr_y += size*line_spacing;
        }
    }

    void Screen::DrawText(const std::string_view& text, float size, Xr::C2 pos, Xr::Color color, JUSTIFY justify)
    {
        GLuint shader_id = Xr::Shaders::Get("default2_color");

        u32 longest = text.size();

        float x_delta = size*FONTRATIO; //HACK: XGA FONT for new rule!
        if (justify == JUSTIFY::RIGHT)
        {
            x_delta = -x_delta;
            pos.x += x_delta;
        }

        float curr_y = pos.y;
        if (justify == JUSTIFY::CENTER)
        {
            curr_y -= size*0.5f;
        }
        curr_y += size*12.0f/138.0f;//offset for the XGA font. the exact value is 3/138 but it's still too high

        float curr_x = pos.x;
        if (justify == JUSTIFY::CENTER)
        {
            curr_x += (longest-text.size())*0.5*x_delta-(longest)*0.5*x_delta;
        }

        if (justify == JUSTIFY::LEFT or justify == JUSTIFY::CENTER)
        {
            for(u8 c: text)
            {
                Xr::GetEngine().renderer.Queue(Xr::Quad{{{curr_x,curr_y},{curr_x+x_delta,curr_y+size}},-2.0f-size*0.01f}, color, mainfont[c], shader_id, Xr::Renderer::STAGE::GUITEXT);
                curr_x += x_delta;
            }
        }
        else
        {
            for(int i=text.size()-1; i>=0; --i)
            {
                u8 c = text[i];
                Xr::GetEngine().renderer.Queue(Xr::Quad{{{curr_x,curr_y},{curr_x-x_delta,curr_y+size}},-2.0f-size*0.01f}, color, mainfont[c], shader_id, Xr::Renderer::STAGE::GUITEXT);
                curr_x += x_delta;
            }
        }
    }

    void Screen::DrawCentered(const std::string_view& text, float font_size, Xr::C2 pos, Xr::Color color)
    {
        DrawText(text, font_size, Xr::C2{pos.x-font_size*FONTRATIO*0.5f*text.size(),pos.y}, color, JUSTIFY::LEFT);
    }
}
