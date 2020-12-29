#pragma once
#include <boost/asio.hpp>
#include <iostream>
#include "SessionServer.h"
#include <Psapi.h>
#if _DEBUG
#include <vld.h>
#endif
//#include <crtdbg.h>
//#if _DEBUG
//#define new new(_CLIENT_BLOCK, __FILE__, __LINE__)
//#define malloc(s) _malloc_dbg(s, _NORMAL_BLOCK, __FILE__, __LINE__)
//#endif

static void PrintUsageInfo() {
	PROCESS_MEMORY_COUNTERS_EX pmc;
	GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
	size_t usedVirtualMem = pmc.PrivateUsage / 1024;
	size_t usedPhysMem = pmc.WorkingSetSize / 1024;

	std::cout << "Used Virtual Mem : " << usedVirtualMem << "kb" << std::endl;
	//std::cout << "Used Phys Mem : " << usedPhysMem << "kb" << std::endl;

	/*static MEMORYSTATUSEX memInfo;
	memInfo.dwLength = sizeof(MEMORYSTATUSEX);

	GlobalMemoryStatusEx(&memInfo);

	static DWORDLONG totalPhysMem = memInfo.ullTotalPhys;
	static DWORDLONG usedPhysMem = totalPhysMem - memInfo.ullAvailPhys;

	std::cout << "Total Ram : " << totalPhysMem << std::endl;
	std::cout << "Used Ram : " << usedPhysMem << std::endl;*/
}

int main() {
	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//int* a = new int(10);
	
	bool isRunprintUsageInfo = true;

	try {
		SessionServer server("Session", "127.0.0.1", 8100);
		std::thread printing([&]() {
			auto start = std::chrono::system_clock::now();
			while (isRunprintUsageInfo) {
				auto end = std::chrono::system_clock::now();
				std::chrono::duration<double> elapsed = end - start;
				auto elapsedSecond = elapsed.count();

				if (elapsedSecond >= 5) {
					start = std::chrono::system_clock::now();
					PrintUsageInfo();
				}
			}
			});

		while (true) {
			std::string a;
			std::getline(std::cin, a);

			if (a == "exit") {
				server.stop();
				isRunprintUsageInfo = false;
				printing.join();
				break;
			}
		}
	}
	catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}

	return 0;
}