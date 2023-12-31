#pragma once

#include "xroads/Cloque.h"

#include <source_location>
#include <iostream>
#include <sstream>

namespace Xroads
{
    //TODO: move to .cpp file
    static std::string FormatTime(i64 time_ms)
    {
        int hours = time_ms / (1000 * 60 * 60);
        int minutes = (time_ms / (1000 * 60)) % 60;
        int seconds = (time_ms / 1000) % 60;
        int milliseconds = time_ms % 1000;

        std::stringstream formattedTime;

        if (hours < 10)
            formattedTime << '0';
        formattedTime << hours << ':';

        if (minutes < 10)
            formattedTime << '0';
        formattedTime << minutes << ':';

        if (seconds < 10)
            formattedTime << '0';
        formattedTime << seconds << '.';

        if (milliseconds < 100)
            formattedTime << '0';
        if (milliseconds < 10)
            formattedTime << '0';
        formattedTime << milliseconds;

        return formattedTime.str();
    }

    //TODO: log levels
    static void Log(std::string_view message, const std::source_location location=std::source_location::current())
    {
        std::stringstream msg;
        std::string filename = location.file_name();
        //  << " @ " << location.function_name()
        msg << "[" << FormatTime(Cloque().n) << "] " << filename.substr(39) << ":" << location.line() << ": " << message << std::endl;
        std::cout << msg.str();
    }
}
