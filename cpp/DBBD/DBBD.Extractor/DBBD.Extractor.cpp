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

//inline constexpr unsigned int HashCode(const char* str)
//{
//	return str[0] ? static_cast<unsigned int>(str[0]) + 0xEDB8832Full * HashCode(str + 1) : 8603;
//}

//#ifndef XMLCheckResult
//#define XMLCheckResult(a_eResult) if (a_eResult != tinyxml2::XML_SUCCESS) { printf("Error: %i\n", a_eResult); return a_eResult != tinyxml2::XML_SUCCESS; }
//#endif

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

	//for (const auto fileName : fileList) {
	//	size_t pos = fileName.find(".");
	//	auto baseName = fileName.substr(0, pos);
	//	std::cout << baseName << std::endl;

	//	auto newPath = path / (baseName + ".cpp");

	//	//fs::create_directory(newPath);
	//	std::ofstream ofs(newPath);
	//	ofs << baseName << std::endl;
	//	ofs.close();
	//}

	/*tinyxml2::XMLDocument document;
	auto error = document.LoadFile("xml\\test.xml");

	if (XMLCheckResult(error)) {
		return 0;
	}
	auto node = document.FirstChild();
	auto element = node->FirstChildElement();

	while (element) {
		switch (HashCode(element->Name())) {
		case HashCode("cells"): {
			auto cellElement = element->FirstChildElement();
			while (cellElement) {
				std::string cellName = cellElement->Attribute("name");
				std::string cellBase = cellElement->Attribute("base");

				std::cout << "cell, name: " << cellName << ", base: " << cellBase << std::endl;

				auto cellChild = cellElement->FirstChild();
				while (cellChild) {
					if (cellChild->ToComment()) {
						auto comment = cellChild->ToComment();
						std::cout << "\tcomment: " << comment->Value() << std::endl;
					}
					else if (cellChild->ToElement()) {
						auto propertyElement = cellChild->ToElement();
						std::string propertyName = propertyElement->Attribute("name");
						std::string propertyType = propertyElement->Attribute("type");

						std::cout << "\tcellProperty, name: " << propertyName << ", type:" << propertyType << std::endl;
					}
					cellChild = cellChild->NextSibling();
				}
				
				cellElement = cellElement->NextSiblingElement();
			}

			break;
		}
		case HashCode("protocols"): {
			auto protocolElement = element->FirstChildElement();
			while (protocolElement) {
				std::string protocolName = protocolElement->Attribute("name");
				std::string protocolType = protocolElement->Attribute("type");
				std::string protocolBase = protocolElement->Attribute("base");

				std::cout << "protocol, name: " << protocolName << ", type: " << protocolType << ", base: " << protocolBase << std::endl;

				auto propertyElement = protocolElement->FirstChildElement();
				while (propertyElement) {
					auto propertyComment = propertyElement->ToComment();
					std::string propertyName = propertyElement->Attribute("name");
					std::string propertyType = propertyElement->Attribute("type");

					std::cout << "protocolProperty, name: " << propertyName << ", type:" << propertyType << std::endl;

					propertyElement = propertyElement->NextSiblingElement();
				}

				protocolElement = protocolElement->NextSiblingElement();
			}
			break;
		}
		}

		std::cout << std::endl;

		element = element->NextSiblingElement();
	}*/

	std::cout << "Hello World!\n";
}