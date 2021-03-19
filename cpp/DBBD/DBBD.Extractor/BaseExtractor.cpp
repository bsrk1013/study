#include "BaseExtractor.h"

using namespace tinyxml2;

BaseExtractor::BaseExtractor(ExtractorType type, 
	std::filesystem::path basePath, const std::vector<std::string>& fileList)
	: type(type), basePath(basePath), fileList(fileList) {
	baseOutputPath = basePath / "output";
}

void BaseExtractor::start() {
	for (auto fileName : fileList) {
		parseXml(fileName);
	}
}

std::filesystem::path BaseExtractor::getOutputPath() {
	std::string outputName = "";
	switch (type) {
	case ExtractorType::Cpp:
		outputName = "cpp";
		break;
	case ExtractorType::Csharp:
		outputName = "csharp";
		break;
	}
	return baseOutputPath / outputName;
}

std::filesystem::path BaseExtractor::getOutputFileName(std::string fileName) {
	std::string outputName = "";
	std::string newFileName = "";

	size_t pos = fileName.find(".");
	auto baseName = fileName.substr(0, pos);

	switch (type) {
	case ExtractorType::Cpp:
		outputName = "cpp";
		newFileName = baseName + ".hpp";
		break;
	case ExtractorType::Csharp:
		outputName = "csharp";
		newFileName = baseName + ".cs";
		break;
	}
	return baseOutputPath / outputName / newFileName;
}

std::string BaseExtractor::getPropertyType(std::string type) {
	switch (HashCode(type.c_str())) {
	case HashCode("string"):
		if (this->type == ExtractorType::Cpp) { return "std::wstring"; }
		else if (this->type == ExtractorType::Csharp) { return "string"; }
		break;
	case HashCode("int64"):
		if (this->type == ExtractorType::Cpp) { return "__int64"; }
		else if (this->type == ExtractorType::Csharp) { return "long"; }
		break;
	case HashCode("uint64"):
		if (this->type == ExtractorType::Cpp) { return "unsigned __int64"; }
		else if (this->type == ExtractorType::Csharp) { return "ulong"; }
		break;
	case HashCode("int32"):
		if (this->type == ExtractorType::Cpp) { return "int"; }
		else if (this->type == ExtractorType::Csharp) { return "int"; }
		break;
	case HashCode("uint32"):
		if (this->type == ExtractorType::Cpp) { return "unsigned int"; }
		else if (this->type == ExtractorType::Csharp) { return "uint"; }
		break;
	case HashCode("int16"):
		if (this->type == ExtractorType::Cpp) { return "short"; }
		else if (this->type == ExtractorType::Csharp) { return "short"; }
		break;
	case HashCode("uint16"):
		if (this->type == ExtractorType::Cpp) { return "unsigned short"; }
		else if (this->type == ExtractorType::Csharp) { return "ushort"; }
		break;
	case HashCode("double"):
		if (this->type == ExtractorType::Cpp) { return "double"; }
		else if (this->type == ExtractorType::Csharp) { return "double"; }
		break;
	case HashCode("float"):
		if (this->type == ExtractorType::Cpp) { return "float"; }
		else if (this->type == ExtractorType::Csharp) { return "float"; }
		break;
	case HashCode("bool"):
		if (this->type == ExtractorType::Cpp) { return "bool"; }
		else if (this->type == ExtractorType::Csharp) { return "bool"; }
		break;
	case HashCode("char"):
		if (this->type == ExtractorType::Cpp) { return "wchar_t"; }
		else if (this->type == ExtractorType::Csharp) { return "char"; }
		break;
	case HashCode("byte"):
		if (this->type == ExtractorType::Cpp) { return "unsigned char"; }
		else if (this->type == ExtractorType::Csharp) { return "byte"; }
		break;
	case HashCode("sbyte"):
		if (this->type == ExtractorType::Cpp) { return "char"; }
		else if (this->type == ExtractorType::Csharp) { return "sbyte"; }
		break;
	default:
		break;
	}
}

void BaseExtractor::parseXml(std::string fileName) {
	auto outputPath = getOutputPath();
	std::filesystem::create_directories(outputPath);
	auto outputFileName = getOutputFileName(fileName);
	std::ofstream ofs(outputFileName);

	XMLDocument doc;
	auto error = doc.LoadFile((basePath / fileName).u8string().c_str());

	if (error != XML_SUCCESS) {
		new std::exception(("not exists file, fileName: " + fileName).c_str());
	}

	auto node = doc.FirstChild();
	auto childElement = node->FirstChildElement();

	writeHeader(ofs);
	parseRoot(ofs, childElement, fileName);
	
	ofs.close();
}

void BaseExtractor::parseRoot(std::ofstream& ofs, XMLElement* elem, std::string fileName) {
	while (elem) {
		switch (HashCode(elem->Name())) {
		case HashCode("cells"):
			parseContents(ofs, elem, fileName);
			break;
		case HashCode("protocols"):
			parseContents(ofs, elem, fileName);
			break;
		case HashCode("consts"):
		{
			auto find = elem->FindAttribute("name");
			if (!find)
				throw std::exception("consts name is empty");
			auto constsName = find->Value();
			parseContents(ofs, elem, fileName);
			writeConst(ofs, constsName);
			headerInfoList.clear();
			break;
		}
		default:
			break;
		}

		elem = elem->NextSiblingElement();
	}
}

void BaseExtractor::parseContents(std::ofstream& ofs, XMLElement* root, std::string fileName) {
	int index = 0;
	auto a = root->FindAttribute("name");
	if (a)
	{
		auto b = a->Value();
		std::string c = b;
	}
	auto child = root->FirstChild();
	XmlElementType type = XmlElementType::None;
	while (child) {
		FileInfo info;
		if (child->ToComment()) {
			info.fileType = XmlElementType::Comment;
			std::string value(child->Value());
			info.value = trim(value);
			headerInfoList.push_back(info);
		}
		else if (child->ToElement()) {
			auto elem = child->ToElement();
			auto name = elem->Name();
			
			if (strcmp(elem->Name(), "cell") == 0
				|| strcmp(elem->Name(), "protocol") == 0
				|| strcmp(elem->Name(), "const") == 0) {
				if(strcmp(elem->Name(), "cell") == 0){
					type = info.fileType = XmlElementType::Cell;

				} else if (strcmp(elem->Name(), "protocol") == 0) {
					type = info.fileType = XmlElementType::Protocol;
				}
				else if (strcmp(elem->Name(), "const") == 0) {
					type = info.fileType = XmlElementType::Const;
				}

				auto find = elem->FindAttribute("name");
				if (find) {
					std::string name = find->Value();
					info.name = name;
				}

				find = elem->FindAttribute("type");
				if (find) {
					std::string type = find->Value();
					info.type = type;
				}

				find = elem->FindAttribute("base");
				if (find) {
					std::string base = find->Value();
					info.base = base;
				}

				auto valueChild = child->FirstChild();
				if (valueChild && valueChild->ToText()) {
					auto valueText = valueChild->ToText();
					std::string value = valueText->Value();
					info.value = value;
				}

				std::string value = elem->Value();

				headerInfoList.push_back(info);
			}

			auto cellChild = child->FirstChild();
			while (cellChild) {
				FileInfo info2;
				if (cellChild->ToComment()) {
					info2.fileType = XmlElementType::Comment;
					std::string value(cellChild->Value());
					info2.value = trim(value);
					contentsInfoList.push_back(info2);
				}
				else if (cellChild->ToElement()) {
					auto elemChild = cellChild->ToElement();
					info2.fileType = XmlElementType::Property;

					auto find = elemChild->FindAttribute("name");
					if (find) {
						std::string name = find->Value();
						info2.name = name;
					}

					find = elemChild->FindAttribute("base");
					if (find) {
						std::string base = find->Value();
						info2.base = base;
					}

					find = elemChild->FindAttribute("type");
					if (find) {
						std::string type = find->Value();
						info2.type = type;
					}

					std::string value = elemChild->Value();
					info2.value = value;

					auto valueChild = elemChild->FirstChild();
					if (valueChild && valueChild->ToText()) {
						auto valueText = valueChild->ToText();
						std::string value = valueText->Value();
						info2.value = value;
					}

					contentsInfoList.push_back(info2);
				}

				cellChild = cellChild->NextSibling();
			}
		}

		bool needInfoClear = false;
		switch (type) {
		case XmlElementType::Cell:
			writeCell(ofs);
			needInfoClear = true;
			break;
		case XmlElementType::Protocol:
			writeProtocol(ofs);
			needInfoClear = true;
			break;
		}

		if (needInfoClear) {
			headerInfoList.clear();
			contentsInfoList.clear();
			ofs << std::endl;
		}

		child = child->NextSibling();
	}
}