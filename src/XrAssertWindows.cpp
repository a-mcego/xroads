/*#include "xroads/XrAssert.h"

//#include <stacktrace>

//#include <execinfo.h>

#include "windows.h"
#include "dbghelp.h"
#undef CreateWindow
#undef DEFAULT_PALETTE


namespace Xroads
{
    void DieWithTrace()
    {

        //void* array[30] = {};
        // get void*'s for all entries on the stack
        //size_t size = backtrace(array, 30);

        // print out all the frames to stderr
        //fprintf(stderr, "Stack trace:\n");
        //backtrace_symbols_fd(array, size, STDERR_FILENO);

        void* callstack[100] = {};
        HANDLE process = GetCurrentProcess();
        SymInitialize( process, NULL, TRUE );

        USHORT frames = CaptureStackBackTrace( 0, 100, callstack, NULL );
        SYMBOL_INFO* symbol = ( SYMBOL_INFO * )calloc( sizeof( SYMBOL_INFO ) + 256 * sizeof( char ), 1 );
        symbol->MaxNameLen = 255;
        symbol->SizeOfStruct = sizeof( SYMBOL_INFO );

        for(int i = 0; i < frames; i++)
        {
            SymFromAddr(process, DWORD64( callstack[i]), 0, symbol);
            printf("%i: %s - 0x%0llX\n", frames - i - 1, symbol->Name, symbol->Address);
        }

        CONTEXT* context = new CONTEXT;
        RtlCaptureContext(context);

        STACKFRAME_EX* stack = new STACKFRAME_EX;
        memset(stack, 0, sizeof(STACKFRAME_EX));

        stack->AddrPC.Offset    = context->Rip;
        stack->AddrPC.Mode      = AddrModeFlat;
        stack->AddrStack.Offset = context->Rsp;
        stack->AddrStack.Mode   = AddrModeFlat;
        stack->AddrFrame.Offset = context->Rbp;
        stack->AddrFrame.Mode   = AddrModeFlat;


        auto thread = GetCurrentThread();

        std::cout << "Noniin yritettään" << std::endl;
        while(StackWalk64(IMAGE_FILE_MACHINE_AMD64, process, thread, (LPSTACKFRAME64)stack, context, nullptr, nullptr, nullptr, nullptr))
        {
            auto result = SymFromAddr(process, DWORD64(stack->AddrPC.Offset), 0, symbol);
            if (result == false)
                std::cout << "ERRROR! " << GetLastError() << std::endl;
            else
                std::cout << symbol->Name << " " << std::hex << symbol->Address << "/" << stack->AddrPC.Offset << std::dec << " result:" << result << std::endl;
        }
        std::cout << "siin oli." << std::endl;

        free(symbol);

        delete stack;
        delete context;

        exit(1);
    }
}
*/
