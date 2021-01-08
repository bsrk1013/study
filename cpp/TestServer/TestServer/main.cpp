#pragma once
#include <boost/asio.hpp>
#include <iostream>
#include "SessionServer.h"
#include <Psapi.h>
#include <mutex>
#if _DEBUG
#include <vld.h>
#endif

static size_t prevUsedVirtualMem = 0;
static size_t prevUsedPhysMem = 0;

static void PrintUsageInfo() {
	PROCESS_MEMORY_COUNTERS_EX pmc;
	GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
	size_t usedVirtualMem = pmc.PrivateUsage / 1024;
	size_t usedPhysMem = pmc.WorkingSetSize / 1024;
	
	bool needSplit = false;
	if (prevUsedVirtualMem != usedVirtualMem) {
		std::cout << "현재 사용중인 가상 메모리 : " << usedVirtualMem << "kb, 변화량 : " << ((int)usedVirtualMem - (int)prevUsedVirtualMem) << "kb" << std::endl;
		prevUsedVirtualMem = usedVirtualMem;
		needSplit = true;
	}

	if (prevUsedPhysMem != usedPhysMem) {
		std::cout << "현재 사용중인 물리 메모리 : " << usedPhysMem << "kb, 변화량 : " << ((int)usedPhysMem - (int)prevUsedPhysMem) << "kb" << std::endl;
		prevUsedPhysMem = usedPhysMem;
		needSplit = true;
	}

	if (needSplit) {
		std::cout << "------------------------------------------------------------------------------------------------------------------" << std::endl << std::endl;
	}
}

int main() {
	bool isRunprintUsageInfo = true;
	int* number = nullptr;

	try {
		SessionServer server("Session", "127.0.0.1", 8100);
		std::thread printing([&]() {
			auto start = std::chrono::system_clock::now();
			while (isRunprintUsageInfo) {
				auto end = std::chrono::system_clock::now();
				std::chrono::duration<double> elapsed = end - start;
				auto elapsedSecond = elapsed.count();

				if (elapsedSecond >= 1) {
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
			else if (a == "d") {
				if (number) {
					delete[] number;
					number = nullptr;
				}
			}
			else {
				int num = atoi(a.c_str());
				if (!number) {
					number = new int[num]{ 0 };
				}
			}
		}
	}
	catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}

	return 0;
}