#include "xroads/Xroads.h"

#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <iterator>

#include <dwarfstack.h>

namespace Xroads
{
    const CloqueVal starttime{std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count()};

    inline void stdoutPrint(uint64_t addr, const char *filename_cstr, int lineno, const char *funcname, void *context, int columnno)
    {
        int* count = reinterpret_cast<int*>(context);

        std::string filename = filename_cstr;
        if (size_t pos = filename.find_last_of("/\\"); pos != std::string::npos)
            filename = filename.substr(pos + 1);

        std::string output;
        uintptr_t ptr = (uintptr_t)addr;
        switch( lineno )
        {
        case DWST_BASE_ADDR:
            output += std::format("base address: {:#x} ({})", ptr, filename);
            break;

        case DWST_NOT_FOUND:
            output += std::format("   not found: {:#x} ({})", ptr, filename);
            break;

        case DWST_NO_DBG_SYM:
        case DWST_NO_SRC_FILE:
            output += std::format("    stack {:02}: {:#x} ({})", *count, ptr, filename);
            (*count) += 1;
            break;
        default:
            if(ptr)
            {
                output += std::format("    stack {:02}: {:#x}", *count ,ptr );
                (*count) += 1;
            }
            else
                output += "                                ";
            output += std::format(" ({}:{}", filename, lineno);
            if(columnno>0)
                output += std::format(":{}", columnno);
            output += ')';
            if(funcname)
                output += std::format(" [{}]", funcname);//printf( " [%s]",funcname );

        }

        std::cout << output << std::endl;
    }

    void BuildStackTrace()
    {
        int count=0;
        dwstOfLocation( stdoutPrint,&count );
    }
}
