#pragma once
#define _WIN32_WINNT 0x0501
#define WIN32_LEAN_AND_MEAN

#include "Singleton"
#include "Common.hpp"

#include <windows.h>
#include <iostream>

#pragma warning(push)
#pragma warning(disable: 4091)  // 'typedef ': ignored on left of '' when no variable is declared
#include <dbghelp.h>
#pragma warning(pop)

#pragma comment(lib, "Dbghelp.lib")

namespace DBBD
{
	class ExceptionHandler : public Singleton<ExceptionHandler>
	{
	public:
		DWORD init(
            __in const std::string& dumpFileName,
			//__in LPCTSTR dumpFileName,
			__in const MINIDUMP_TYPE dump_type = MINIDUMP_TYPE::MiniDumpNormal
		)
		{
			DWORD error = ERROR_SUCCESS;

			do {

				if (dumpFileName.empty()) {
					error = ERROR_INVALID_PARAMETER;
					break;
				}

                this->dumpFileName = dumpFileName;
                dumpType = dump_type;

			} while (false);

          /*  if (error == ERROR_SUCCESS) {
                run();
            }*/

			return error;
		}

        virtual void release() override {}

		DWORD run()
		{
            prevFilter = ::SetUnhandledExceptionFilter(exceptionCallback);
			return ERROR_SUCCESS;
		}

		static LONG WINAPI exceptionCallback(
			__in struct _EXCEPTION_POINTERS* exceptioninfo
		)
		{

            do {

                if (nullptr == exceptioninfo) {
                    break;
                }

                SYSTEMTIME st = { 0 };
                ::GetLocalTime(&st);

                auto nowStr = getNowString();
                std::string fileName = strFormat("{}-{}.dmp", nowStr, ExceptionHandler::Instance()->dumpFileName);
                std::wstring wstring = strconv(fileName);

                //
                // create dump file.
                //

                HANDLE dump_file_handle = ::CreateFile(
                    wstring.c_str(),
                    GENERIC_WRITE,
                    0,
                    nullptr,
                    CREATE_ALWAYS,
                    FILE_ATTRIBUTE_NORMAL,
                    nullptr
                );

                if (INVALID_HANDLE_VALUE == dump_file_handle) {
                    break;
                }

                MINIDUMP_EXCEPTION_INFORMATION ex_info = { 0 };

                ex_info.ThreadId = ::GetCurrentThreadId(); // Threae ID 설정
                ex_info.ExceptionPointers = exceptioninfo; // Exception 정보 설정
                ex_info.ClientPointers = FALSE;

                //
                // write dump file.
                //

                if (FALSE == ::MiniDumpWriteDump(
                    ::GetCurrentProcess(),
                    ::GetCurrentProcessId(),
                    dump_file_handle,
                    ExceptionHandler::Instance()->dumpType,
                    &ex_info,
                    nullptr, nullptr
                )) {

                    break;
                }

            } while (false);

            return (ExceptionHandler::Instance()->prevFilter) ? ExceptionHandler::Instance()->prevFilter(exceptioninfo) : EXCEPTION_EXECUTE_HANDLER;
		}

	private:
		std::string dumpFileName;
		MINIDUMP_TYPE dumpType;
		LPTOP_LEVEL_EXCEPTION_FILTER prevFilter;
	};
}