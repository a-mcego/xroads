#include "xroads/Xroads.h"

namespace Xroads
{
    enum struct JOYSTICK
    {
        NONE,
        JOYSTICK,
        GAMEPAD
    };
    Xr::Array<JOYSTICK,16> joysticks{};
    const float DEADZONE = 0.04f;
    Xr::Array<int,GLFW_GAMEPAD_BUTTON_LAST+1> gp_buttonstate{};
    Xr::Array<int,GLFW_GAMEPAD_AXIS_LAST+1> gp_axisstate{};

    float Input::GetAxis(int axis_id)
    {
        return float(gp_axisstate[axis_id])/1048576.0f;
    }

    bool Input::HasJoystick(int joy_id)
    {
        if (joy_id < 0 || joy_id >= joysticks.size())
            return false;
        return joysticks[joy_id] != JOYSTICK::NONE;
    }
    std::string Input::JoystickName(int joy_id)
    {
        const char* gpname = glfwGetGamepadName(joy_id);
        if (gpname != nullptr)
            return gpname;
        const char* jname = glfwGetJoystickName(joy_id);
        if (jname != nullptr)
            return jname;

        return "No gamepad";
    }

    InputEvent::STATE convert(int action)
    {
        if (action == GLFW_PRESS)
            return InputEvent::STATE::DOWN;
        else if (action == GLFW_RELEASE)
            return InputEvent::STATE::UP;
        return InputEvent::STATE::NOTHING;
    }

    void keyfun([[maybe_unused]] GLFWwindow* window, int key, [[maybe_unused]] int scancode, int action, [[maybe_unused]] int mods)
    {
        InputEvent ie;
        ie.type = InputEvent::TYPE::KEY;
        ie.state = convert(action);
        if (ie.state == InputEvent::STATE::NOTHING)
            return;
        ie.data = key;
        GetEngine().input.inputqueue.push(ie);
    }
    void gamepadbuttonfun(int joy_id, int key_id, InputEvent::STATE action)
    {
        if (key_id == GLFW_GAMEPAD_BUTTON_START || key_id == GLFW_GAMEPAD_BUTTON_GUIDE)
        {
            GetEngine().appstate.gamepad_id = joy_id;
        }

        InputEvent ie;
        ie.type = InputEvent::TYPE::GAMEPAD_BUTTON;
        ie.state = action;
        ie.data = key_id;
        ie.data2 = joy_id;
        GetEngine().input.inputqueue.push(ie);
    }
    void mousebuttonfun([[maybe_unused]] GLFWwindow* window, int button, int action, [[maybe_unused]] int mods)
    {
        InputEvent ie;
        ie.type = InputEvent::TYPE::MOUSE_BUTTON;
        ie.state = convert(action);
        if (ie.state == InputEvent::STATE::NOTHING)
            return;
        ie.data = button;
        GetEngine().input.inputqueue.push(ie);
    }
    void gamepadaxisfun([[maybe_unused]] int joy_id, int axis_id, int amount)
    {
        InputEvent ie;
        ie.type = InputEvent::TYPE::GAMEPAD_AXISMOVE;
        ie.data = axis_id;
        ie.data2 = amount;
        GetEngine().input.inputqueue.push(ie);
    }
    void cursorposfun([[maybe_unused]] GLFWwindow* window, double x, double y)
    {
        InputEvent ie;
        ie.type = InputEvent::TYPE::MOUSE_MOVE;
        ie.data = (int)x;
        ie.data2 = (int)y;
        GetEngine().input.inputqueue.push(ie);
    }
    //TODO: maybe support the x axis as well?
    //need to make two different events
    //Like MOUSE_WHEEL_Y and MOUSE_WHEEL_X
    void scrollfun([[maybe_unused]] GLFWwindow* window, [[maybe_unused]] double x, double y)
    {
        InputEvent ie;
        ie.type = InputEvent::TYPE::MOUSE_WHEEL;
        ie.data = Xr::Sign((int)y);
        ie.state = InputEvent::STATE::DOWN;
        GetEngine().input.inputqueue.push(ie);
    }
    void cursorenterfun(GLFWwindow* window, [[maybe_unused]] int entered)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    void joystickfun(int jid, int event)
    {
        if (event == GLFW_CONNECTED)
        {
            if (glfwJoystickIsGamepad(jid))
            {
                joysticks[jid] = JOYSTICK::GAMEPAD;
                std::string gamepadname;
                //avoid assigning possible nullptr
                if (auto name = glfwGetGamepadName(jid); name!=nullptr)
                    gamepadname = name;
                Log("Gamepad found! Name: "+gamepadname);
            }
            else
                joysticks[jid] = JOYSTICK::JOYSTICK;
        }
        else if (event == GLFW_DISCONNECTED)
        {
            joysticks[jid] = JOYSTICK::NONE;
            // The joystick was disconnected

            Xr::SetToZero(gp_buttonstate);
            Xr::SetToZero(gp_axisstate);
            if (GetEngine().appstate.state == "INGAME"_sm)
                GetEngine().appstate.state = "INGAME_PAUSE"_sm;
            if (GetEngine().appstate.gamepad_id == jid)
                GetEngine().appstate.gamepad_id = -1;
        }

        GetEngine().input.n_joysticks = 0;
        for(int jid_counter=GLFW_JOYSTICK_1; jid_counter<GLFW_JOYSTICK_LAST; ++jid_counter)
        {
            if (glfwJoystickPresent(jid_counter))
            {
                ++GetEngine().input.n_joysticks;
            }
        }
    }

    void Input::Init()
    {
        auto* glfw_window = GetEngine().screen.glfw_window;
        glfwSetKeyCallback(glfw_window, keyfun);
        glfwSetMouseButtonCallback(glfw_window, mousebuttonfun);
        glfwSetCursorPosCallback(glfw_window, cursorposfun);
        glfwSetScrollCallback(glfw_window, scrollfun);
        glfwSetCursorEnterCallback(glfw_window, cursorenterfun);

        for(int jid=GLFW_JOYSTICK_1; jid<GLFW_JOYSTICK_LAST; ++jid)
        {
            if (glfwJoystickPresent(jid))
            {
                joystickfun(jid, GLFW_CONNECTED);
            }
        }
        glfwSetJoystickCallback(joystickfun);
    }

    bool Input::KeyPressed(int key)
    {
        return glfwGetKey(GetEngine().screen.glfw_window, key) == GLFW_PRESS;
    }

    void Input::UpdateJoysticks()
    {
        for(int j_id=0; j_id<joysticks.size(); ++j_id)
        {
             if (joysticks[j_id] != JOYSTICK::GAMEPAD)
                continue;
            GLFWgamepadstate state;
            if (!glfwGetGamepadState(j_id, &state))
                continue;
            for(int b_id=0; b_id<=GLFW_GAMEPAD_BUTTON_LAST; ++b_id)
            {
                int b_new = state.buttons[b_id];
                if (gp_buttonstate[b_id] != b_new)
                {
                    gp_buttonstate[b_id] = b_new;
                    gamepadbuttonfun(j_id, b_id, b_new?InputEvent::STATE::DOWN:InputEvent::STATE::UP);
                }
            }

            for(int a_id=0; a_id<=GLFW_GAMEPAD_AXIS_LAST; ++a_id)
            {
                int a_new = int(state.axes[a_id]*1048576.0f);
                if (Xr::Abs(a_new) > int(DEADZONE*1048576.0f))
                {
                    gp_axisstate[a_id] = a_new;
                    gamepadaxisfun(j_id, a_id, a_new);
                }
                else
                {
                    gp_axisstate[a_id] = 0;
                    gamepadaxisfun(j_id, a_id, 0);
                }
            }
        }
    }

}

