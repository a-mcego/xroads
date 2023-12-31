#include "xroads/Xroads.h"

#include <fstream>

namespace Xroads
{
    const std::wstring CONFIG_FILENAME = L"config.txt";

    void Config::Load()
    {
        std::vector<std::byte> filudata_byte = GetEngine().files.ReadTextFile(CONFIG_FILENAME);

        std::string filudata;
        for(int i=0; i<filudata_byte.size(); ++i)
            filudata.push_back(char(filudata_byte[i]));

        //std::cout << "Loaded config file: " << filudata.size() << " bytes." << std::endl;

        std::stringstream filu(filudata);

        while (!filu.eof())
        {
            std::string prop;
            filu >> prop;

            std::string value;
            filu >> value;

            config[prop] = value;
        }
    }

    void Config::Save()
    {
        std::stringstream filu;
        for (std::map<std::string, std::string>::iterator it = config.begin(); it != config.end(); ++it)
        {
            filu << it->first << " " << it->second << std::endl;
        }

        std::string filustr = filu.str();
        std::vector<std::byte> bytes;
        for(int i=0; i<filustr.size(); ++i)
        {
            bytes.push_back(std::byte(filustr[i]));
        }

        GetEngine().files.WriteTextFile(CONFIG_FILENAME, bytes);
    }
}
