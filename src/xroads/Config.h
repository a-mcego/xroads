#pragma once

namespace Xroads
{
    class Config
    {
    private:
        std::map<std::string, std::string> config;
    public:

        void Save();
        void Load();

        template<typename T>
        void SetValue(std::string key, T val)
        {
            if constexpr(std::is_same_v<T,bool>)
                config[key] = ToString(int(val));
            else
                config[key] = ToString(val);
        }

        template<typename T>
        T GetValue(std::string key, const T& default_value)
        {
            if (config.find(key) == config.end())
                SetValue(key, default_value);
            return FromString<T>(config[key]);
        }
    };
}
