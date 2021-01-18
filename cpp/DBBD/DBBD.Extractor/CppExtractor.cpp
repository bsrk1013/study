#include "CppExtractor.h"

CppExtractor::CppExtractor(std::filesystem::path basePath, std::vector<std::string>& fileList)
	: BaseExtractor(ExtractorType::Cpp, basePath, fileList) {

}

void CppExtractor::writeHeader(std::ofstream& ofs) {
	ofs << "#pragma once" << std::endl;
	ofs << "#include \"DBBD/Cell.h\"" << std::endl;
	ofs << "#include \"DBBD/Request.h\"" << std::endl;
	ofs << "#include \"DBBD/Response.h\"" << std::endl;
	ofs << "#include \"ProtocolType.hpp\"" << std::endl;
	ofs << std::endl;
}

void CppExtractor::writeCell(std::ofstream& ofs) {
	writeContentsHeader(ofs);
	writeCellContents(ofs);
	ofs << "}" << std::endl;
}

void CppExtractor::writeProtocol(std::ofstream& ofs) {
	writeContentsHeader(ofs);

	for (auto info : headerInfoList) {
		if (info.fileType == XmlElementType::Protocol) {
			writeProtocolContents(ofs, info.base);
			break;
		}
	}

	ofs << "};" << std::endl;
}

void CppExtractor::writeConst(std::ofstream& ofs, std::string fileName) {
	size_t pos = fileName.find(".");
	auto namespaceName = fileName.substr(0, pos);
	ofs << "#include <map>" << std::endl << std::endl;
	ofs << "namespace " << namespaceName << " {" << std::endl;
	ofs << "\tenum class Value {" << std::endl;
	for (auto info : headerInfoList) {
		ofs << "\t\t" << info.name << " = " << info.value << "," << std::endl;
	}
	ofs << "\t};" << std::endl;

	ofs << "\tstd::map<Value, std::string> stringMap = {" << std::endl;
	for (auto info : headerInfoList) {
		ofs << "\t\t{ Value::" << info.name << ", \"" << info.name << "\" }," << std::endl;
	}
	ofs << "\t};" << std::endl;

	ofs << "\tstd::string Get(Value value) {" << std::endl;
	ofs << "\t\tauto iter = " << namespaceName << "::stringMap.find(value);" << std::endl;
	ofs << "\t\tif (iter == " << namespaceName << "::stringMap.end()) {" << std::endl;
	ofs << "\t\t\treturn \"\";" << std::endl;
	ofs << "\t\t}" << std::endl;
	ofs << "\t\treturn iter->second;" << std::endl;
	ofs << "\t}" << std::endl;
	ofs << "}" << std::endl;
}

void CppExtractor::writeContentsHeader(std::ofstream& ofs) {
	for (auto info : headerInfoList) {
		switch (info.fileType) {
		case XmlElementType::Comment: {
			ofs << "// " << info.value << std::endl;
			break;
		}
		case XmlElementType::Protocol:
		case XmlElementType::Cell: {
			ofs << "class " << info.name << " : public DBBD::" << info.base << " {" << std::endl;
			ofs << "public:" << std::endl;
			if (info.fileType == XmlElementType::Cell) {
				ofs << "\t" << info.name << "() {}" << std::endl;
			}
			else if (info.fileType == XmlElementType::Protocol) {
				ofs << "\t" << info.name << "() {" << std::endl;
				ofs << "\t\ttypeId = ProtocolType::" << info.type << ";" << std::endl;
				ofs << "\t}";
			}

			std::vector<FileInfo> realContents;
			for (auto contentsInfo : contentsInfoList) {
				if (contentsInfo.fileType != XmlElementType::Property) { continue; }
				realContents.push_back(contentsInfo);
			}
			if (realContents.size() > 0) {
				ofs << "\t" << info.name << "(";
				for (size_t i = 0; i < realContents.size(); i++) {
					if (i > 0) { ofs << ", "; }
					auto propertyInfo = realContents[i];
					ofs << getPropertyType(propertyInfo.type) << " " << propertyInfo.name;
				}
				ofs << ")" << std::endl;
				ofs << "\t\t: ";
				for (size_t i = 0; i < realContents.size(); i++) {
					if (i > 0) { ofs << ", "; }
					auto propertyInfo = realContents[i];
					ofs << propertyInfo.name << "(" << propertyInfo.name << ")";
				}
				ofs << std::endl << "\t{}" << std::endl;
			}

			ofs << "\t" << "virtual ~" << info.name << "() {}" << std::endl;
			break;
		}
		}
	}

	ofs << std::endl;
}

void CppExtractor::writeCellContents(std::ofstream& ofs) {
	std::vector<FileInfo> realContents;
	for (auto contentsInfo : contentsInfoList) {
		if (contentsInfo.fileType != XmlElementType::Property) { continue; }
		realContents.push_back(contentsInfo);
	}

	ofs << "public:" << std::endl;
	ofs << "\tvirtual void serialize(DBBD::Buffer& buffer) {" << std::endl;
	for (auto info : realContents) {
		ofs << "\t\t" << getDeSerialize(info.base, info.type, info.name, true) << std::endl;
	}
	ofs << "\t}" << std::endl;;

	ofs << "\tvirtual void deserialize(DBBD::Buffer& buffer) {" << std::endl;
	for (auto info : realContents) {
		ofs << "\t\t" << getDeSerialize(info.base, info.type, info.name, false) << std::endl;
	}
	ofs << "\t}" << std::endl;;

	ofs << "\tvirtual size_t getLength() {" << std::endl;
	ofs << "\t\treturn ";
	for (size_t i = 0; i < realContents.size(); i++) {
		if (i > 0) {
			ofs << " + ";
		}
		auto info = realContents[i];
		ofs << getLength(info.type, info.name);
	}
	ofs << ";" << std::endl;
	ofs << "\t}" << std::endl << std::endl;

	if (realContents.size() <= 0) { return; }

	ofs << "public:" << std::endl;
	for (auto info : realContents) {
		std::string newName = "";
		for (size_t i = 0; i < info.name.size(); i++) {
			char c = info.name[i];
			if (i == 0) {
				newName += toupper(c);
			}
			else {
				newName += c;
			}
		}

		ofs << "\t" << getPropertyType(info.type) << " get" << newName << "() { return " << info.name << "; }" << std::endl;
		ofs << "\tvoid set" << newName << "(" << getPropertyType(info.type) << " value) { " << info.name << " = value; }" << std::endl;
	}
	ofs << std::endl;

	ofs << "protected:" << std::endl;
	for (auto info : contentsInfoList) {
		switch (info.fileType) {
		case XmlElementType::Comment:
			ofs << "\t// " << info.value << std::endl;
			break;
		case XmlElementType::Property:
			ofs << "\t" << getPropertyType(info.type) << " " << info.name << ";" << std::endl;
			break;
		}
	}
}

void CppExtractor::writeProtocolContents(std::ofstream& ofs, std::string base) {
	std::vector<FileInfo> realContents;
	for (auto contentsInfo : contentsInfoList) {
		if (contentsInfo.fileType != XmlElementType::Property) { continue; }
		realContents.push_back(contentsInfo);
	}

	ofs << "public:" << std::endl;
	ofs << "\tvirtual void serialize(DBBD::Buffer& buffer) {" << std::endl;
	ofs << "\t\tDBBD::" << base << "::writeHeader(buffer, getLength());" << std::endl;
	for (auto info : realContents) {
		ofs << "\t\t" << getDeSerialize(info.base, info.type, info.name, true) << std::endl;
	}
	ofs << "\t}" << std::endl;;

	ofs << "\tvirtual void deserialize(DBBD::Buffer& buffer) {" << std::endl;
	ofs << "\t\tDBBD::" << base << "::readHeader(buffer);" << std::endl;
	for (auto info : realContents) {
		ofs << "\t\t" << getDeSerialize(info.base, info.type, info.name, false) << std::endl;
	}
	ofs << "\t}" << std::endl;;

	ofs << "\tvirtual size_t getLength() {" << std::endl;
	ofs << "\t\treturn DBBD::" << base << "::getLength()";
	for (size_t i = 0; i < realContents.size(); i++) {
		if (i > 0) {
			ofs << " + ";
		}
		auto info = realContents[i];
		ofs << getLength(info.type, info.name);
	}
	ofs << ";" << std::endl;
	ofs << "\t}" << std::endl << std::endl;

	if (realContents.size() <= 0) { return; }

	ofs << "public:" << std::endl;
	for (auto info : realContents) {
		std::string newName = "";
		for (size_t i = 0; i < info.name.size(); i++) {
			char c = info.name[i];
			if (i == 0) {
				newName += toupper(c);
			}
			else {
				newName += c;
			}
		}

		ofs << "\t" << getPropertyType(info.type) << " get" << newName << "() { return " << info.name << "; }" << std::endl;
		ofs << "\tvoid set" << newName << "(" << getPropertyType(info.type) << " value) { " << info.name << " = value; }" << std::endl;
	}
	ofs << std::endl;

	ofs << "protected:" << std::endl;
	for (auto info : contentsInfoList) {
		switch (info.fileType) {
		case XmlElementType::Comment:
			ofs << "\t// " << info.value << std::endl;
			break;
		case XmlElementType::Property:
			ofs << "\t" << getPropertyType(info.type) << " " << info.name << ";" << std::endl;
			break;
		}
	}
}

std::string CppExtractor::getDeSerialize(std::string base, std::string type, std::string name, bool isSerialize) {
	std::string baseProcess = isSerialize ? "DBBD::Serialize::write" : "DBBD::Deserialize::read";
	switch (HashCode(type.c_str())) {
	case HashCode("string"):
	case HashCode("int64"):
		return baseProcess + "(buffer, " + name + ");";
		//return ofs << "\t\t" << base << "(buffer, " << name << ");" << std::endl;
	default:
		if (strcmp(base.c_str(), "cell") == 0
			|| strcmp(base.c_str(), "Cell") == 0) {
			return baseProcess + "(buffer, dynamic_cast<DBBD::Cell*>(&" + name + ");";
		}
		else {
			std::string msg = "illegal type and base, type: " + type + ", base: " + base;
			new std::exception(msg.c_str());
		}
	}
}

std::string CppExtractor::getLength(std::string type, std::string name) {
	switch (HashCode(type.c_str())) {
	case HashCode("int64"):
		return "sizeof(" + getPropertyType(type) + ")";
		break;
	case HashCode("string"):
		return "sizeof(size_t) + " + name + ".length()";
	default:
		return name + ".getLength()";
	}
}