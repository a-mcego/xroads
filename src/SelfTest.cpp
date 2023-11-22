#include "xroads/Xroads.h"

namespace Xroads
{
    void SelfTest()
    {
        //test everything here.

        //test Xr::Explode
        {
            auto str = Explode("abc abc abc", ' ');
            XrAssert(str.size(),==,3);
            XrAssert(str[0],==,"abc");
            XrAssert(str[1],==,"abc");
            XrAssert(str[2],==,"abc");
        }
        {
            auto str = Explode("abc abc abc", 'a');
            XrAssert(str.size(),==,4);
            XrAssert(str[0],==,"");
            XrAssert(str[1],==,"bc ");
            XrAssert(str[2],==,"bc ");
            XrAssert(str[3],==,"bc");
        }
        {
            auto str = Explode("abc abc abc", 'c');
            XrAssert(str.size(),==,4);
            XrAssert(str[0],==,"ab");
            XrAssert(str[1],==," ab");
            XrAssert(str[2],==," ab");
            XrAssert(str[3],==,"");
        }
    }
}
