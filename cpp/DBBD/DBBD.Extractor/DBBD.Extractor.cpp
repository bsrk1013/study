#include <iostream>
#include <filesystem>
#include <fstream>
#include "CppExtractor.h"
#include "tinyxml2.h"

inline bool XMLCheckResult(tinyxml2::XMLError result) {
	if (result != tinyxml2::XML_SUCCESS) {
		printf("Error: %i\n", result);
	}

	return result != tinyxml2::XML_SUCCESS;
}

namespace fs = std::filesystem;

int main()
{
	std::vector<std::string> fileList;
	auto path = fs::current_path() / "xml";
	for (const auto entry : fs::directory_iterator(path)) {
		auto status = entry.status();
		auto fileName = entry.path().filename();
		std::cout << fileName << std::endl;
		size_t pos = fileName.u8string().find(".");
		if (pos > SCHAR_MAX) { continue; }
		fileList.push_back(entry.path().filename().u8string());
	}

	CppExtractor cppExtractor(path, fileList);
	cppExtractor.start();

	std::cout << "~~ done ~~\n";
}