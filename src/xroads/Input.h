#pragma once

#include <queue>
#include <string>

namespace Xroads
{
    //TODO: rename these structs to something like "Event" and "EventHandler",
    //      because they handle more than inputs now
    struct InputEvent
    {
        enum struct TYPE
        {
            NONE,
            KEY,
            MOUSE_BUTTON,
            MOUSE_MOVE,
            MOUSE_WHEEL,
            GAMEPAD_BUTTON,
            GAMEPAD_AXISMOVE,
            RESOLUTION_CHANGE,
            N
        };
        enum struct STATE
        {
            NOTHING,
            UP,
            DOWN,
            N
        };
        TYPE type{TYPE::NONE};
        STATE state{STATE::NOTHING};

        //TODO: rewrite the data/data2 descriptions per TYPE
        int data{}; //if KEY, has key. if MOUSE_BUTTON, has mouse button, if MOUSE_MOVE, has x coord
        int data2{}; //only used if MOUSE_MOVE. In which case, has y coord
    };

    class Input
    {
    public:
        void Init();
        void Quit(){}

        std::queue<InputEvent> inputqueue;
        bool KeyPressed(int key);

        void UpdateJoysticks();

        bool HasJoystick(int joy_id);
        std::string JoystickName(int joy_id);

        float GetAxis(int axis_id);

        int n_joysticks{};

        int GetNJoysticks() { return n_joysticks; }
    };
}

