#include "CppExtractor.h"
#include <string>

using namespace std;

CppExtractor::CppExtractor(std::filesystem::path basePath, const std::vector<std::string>& fileList)
	: BaseExtractor(ExtractorType::Cpp, basePath, fileList) {

}

void CppExtractor::writeHeader(ofstream& ofs) {
	ofs << "#pragma once" << endl;
	ofs << "#include \"DBBD/Cell.h\"" << endl;
	ofs << "#include \"DBBD/Request.h\"" << endl;
	ofs << "#include \"DBBD/Response.h\"" << endl;
	ofs << "#include \"DBBD/Common.hpp\"" << endl;
	ofs << "#include \"ProtocolType.hpp\"" << endl;
	ofs << endl;
}

void CppExtractor::writeCell(ofstream& ofs) {
	writeContentsHeader(ofs);
	writeCellContents(ofs);
	ofs << "};" << endl;
}

void CppExtractor::writeProtocol(ofstream& ofs) {
	writeContentsHeader(ofs);

	for (auto info : headerInfoList) {
		if (info.fileType == XmlElementType::Protocol) {
			writeProtocolContents(ofs, info.base);
			break;
		}
	}

	ofs << "};" << endl;
}

void CppExtractor::writeConst(ofstream& ofs, string fileName) {
	size_t pos = fileName.find(".");
	auto namespaceName = fileName.substr(0, pos);
	ofs << "#include <map>" << endl;
	ofs << "#include <string>" << endl << endl;
	ofs << "namespace " << namespaceName << " {" << endl;
	ofs << "\tenum Value {" << endl;
	for (auto info : headerInfoList) {
		ofs << "\t\t" << info.name << " = " << info.value << "," << endl;
	}
	ofs << "\t};" << endl;

	ofs << "\tstd::map<Value, std::wstring> stringMap = {" << endl;
	for (auto info : headerInfoList) {
		ofs << "\t\t{ Value::" << info.name << ", L\"" << info.name << "\" }," << endl;
	}
	ofs << "\t};" << endl;

	ofs << "\tstd::wstring Get(Value value) {" << endl;
	ofs << "\t\tauto iter = " << namespaceName << "::stringMap.find(value);" << endl;
	ofs << "\t\tif (iter == " << namespaceName << "::stringMap.end()) {" << endl;
	ofs << "\t\t\treturn \"\";" << endl;
	ofs << "\t\t}" << endl;
	ofs << "\t\treturn iter->second;" << endl;
	ofs << "\t}" << endl;
	ofs << "}" << endl;
}

void CppExtractor::writeContentsHeader(ofstream& ofs) {
	for (auto info : headerInfoList) {
		switch (info.fileType) {
		case XmlElementType::Comment: {
			ofs << "// " << info.value << endl;
			break;
		}
		case XmlElementType::Protocol:
		case XmlElementType::Cell: {
			ofs << "class " << info.name << " : public DBBD::" << info.base << " {" << endl;
			ofs << "public:" << endl;
			if (info.fileType == XmlElementType::Cell) {
				ofs << "\t" << info.name << "() {}" << endl;
			}
			else if (info.fileType == XmlElementType::Protocol) {
				ofs << "\t" << info.name << "() {" << endl;
				ofs << "\t\ttypeId = ProtocolType::" << info.type << ";" << endl;
				ofs << "\t}" << endl;
			}
			ofs << endl;

			//vector<FileInfo> realContents;
			//for (auto contentsInfo : contentsInfoList) {
			//	if (contentsInfo.fileType != XmlElementType::Property) { continue; }
			//	realContents.push_back(contentsInfo);
			//}
			//if (realContents.size() > 0) {
			//	ofs << "\t" << info.name << "(";
			//	for (size_t i = 0; i < realContents.size(); i++) {
			//		if (i > 0) { ofs << ", "; }
			//		auto propertyInfo = realContents[i];
			//		ofs << getPropertyType(propertyInfo.type) << " " << propertyInfo.name;
			//	}
			//	ofs << ")" << endl;
			//	ofs << "\t\t: ";
			//	for (size_t i = 0; i < realContents.size(); i++) {
			//		if (i > 0) { ofs << ", "; }
			//		auto propertyInfo = realContents[i];
			//		ofs << propertyInfo.name << "(" << propertyInfo.name << ")";
			//	}
			//	ofs << endl << "\t{}" << endl;
			//}

			ofs << "\t" << "virtual ~" << info.name << "() {}" << endl << endl;
			break;
		}
		}
	}

	ofs << endl;
}

void CppExtractor::writeCellContents(ofstream& ofs) {
	vector<FileInfo> realContents;
	for (auto contentsInfo : contentsInfoList) {
		if (contentsInfo.fileType != XmlElementType::Property) { continue; }
		realContents.push_back(contentsInfo);
	}
	vector<FileInfo> header;
	for (auto headerInfo : headerInfoList) {
		if (headerInfo.fileType == XmlElementType::Cell) { header.push_back(headerInfo); break; }
	}

	ofs << "public:" << endl;
	ofs << "\tvirtual void serialize(DBBD::Buffer& buffer) {" << endl;
	if (realContents.size() > 0) {
		ofs << "\t\tDBBD::Serialize::writeArray(buffer, fingerPrinter);" << endl;
		for (size_t i = 0; i < realContents.size(); i++) {
			auto info = realContents[i];
			ofs << "\t\tif (fingerPrinter[" << i << "]) { " << getDeSerialize(info.base, info.type, info.name, true) << " }" << endl;
		}
	}
	ofs << "\t}" << endl << endl;

	ofs << "\tvirtual void deserialize(DBBD::Buffer& buffer) {" << endl;
	if (realContents.size() > 0) {
		ofs << "\t\tDBBD::Deserialize::readArray(buffer, fingerPrinter);" << endl;
		for (size_t i = 0; i < realContents.size(); i++) {
			auto info = realContents[i];
			ofs << "\t\tif (fingerPrinter[" << i << "]) { " << getDeSerialize(info.base, info.type, info.name, false) << " }" << endl;
		}
	}
	ofs << "\t}" << endl << endl;

	ofs << "\tvirtual unsigned int getLength() {" << endl;
	ofs << "\t\tunsigned int totalLength = 0;" << endl;
	if (realContents.size() > 0) {
		ofs << "\t\ttotalLength += sizeof(unsigned int) + sizeof(fingerPrinter);" << endl;
		for (size_t i = 0; i < realContents.size(); i++) {
			auto info = realContents[i];
			ofs << "\t\tif (fingerPrinter[" << i << "]) { totalLength += " << getLength(info.type, info.name) << "; }" << endl;
		}
	}
	ofs << "\t\treturn totalLength;" << endl;
	ofs << "\t}" << endl << endl;

	// FIXME 내용물 toString 해줘야함
	ofs << "\tvirtual std::string toString() { " << endl;
	if (header.size() <= 0) { ofs << "return \"\";"; }
	else { 
		ofs << "\t\treturn \"[" << header[0].name << "] { ";
		/*for (size_t i = 0; i < realContents.size(); i++) {
			auto info = realContents[i];
			ofs << "";
		}*/
		ofs << " }\";" << endl;
	}
	ofs << "\t}" << endl << endl;

	ofs << "\tstd::string toJson() {" << endl;
	ofs << "\t\tnlohmann::json j;" << endl;
	for (size_t i = 0; i < realContents.size(); i++) {
		auto info = realContents[i];
		ofs << "\t\tif (fingerPrinter[" << i << "]) { j[\"" << info.name << "\"] = ";
		if (strcmp(info.type.c_str(), "string") == 0) {
			ofs << "DBBD::uniToUtf8(" << info.name << ")";
		}
		else {
			ofs << info.name;
		}
		ofs << "; }" << endl;
	}
	ofs << "\t\treturn j.dump();" << endl;
	ofs << "\t}" << endl << endl;

	ofs << "\tvoid fromJson(std::string rawJson) {" << endl;
	ofs << "\t\tnlohmann::json j = nlohmann::json::parse(rawJson);" << endl;
	for (size_t i = 0; i < realContents.size(); i++) {
		auto info = realContents[i];
		ofs << "\t\tif (!j[\"" << info.name << "\"].is_null()) {" << endl;
		if (strcmp(info.type.c_str(), "string") == 0) {
			ofs << "\t\t\tstd::string utf8 = j[\"" << info.name << "\"].get<std::string>();" << endl;
			ofs << "\t\t\t" << info.name << " = DBBD::utf8ToUni(utf8);" << endl;
		}
		else {
			ofs << "\t\t\t" << info.name << " = " << "j[\"" << info.name << "\"].get<" << getPropertyType(info.type) << ">();" << endl;
		}
		ofs << "\t\t}" << endl;
	}
	ofs << "\t}" << endl;

	if (realContents.size() <= 0) { return; }

	ofs << endl;

	ofs << "public:" << endl;
	for (size_t i = 0; i < realContents.size(); i++) {
		auto info = realContents[i];

		string newName = "";
		for (size_t i = 0; i < info.name.size(); i++) {
			char c = info.name[i];
			if (i == 0) {
				newName += toupper(c);
			}
			else {
				newName += c;
			}
		}

		ofs << "\t" << getPropertyType(info.type) << " get" << newName << "() { return " << info.name << "; }" << endl;
		ofs << "\tvoid set" << newName << "(" << getPropertyType(info.type) << " value) {" << endl;
		ofs << "\t\t" << info.name << " = value;" << endl;
		ofs << "\t\tfingerPrinter[" << i << "] = true;" << endl;
		ofs << "\t}" << endl;
	}
	ofs << endl;

	ofs << "protected:" << endl;
	ofs << "\tbool fingerPrinter[" << realContents.size() << "] = { false, };" << endl;
	for (auto info : contentsInfoList) {
		switch (info.fileType) {
		case XmlElementType::Comment:
			ofs << "\t// " << info.value << endl;
			break;
		case XmlElementType::Property:
			ofs << "\t" << getPropertyType(info.type) << " " << info.name << ";" << endl;
			break;
		}
	}
}

void CppExtractor::writeProtocolContents(ofstream& ofs, string base) {
	vector<FileInfo> realContents;
	for (auto contentsInfo : contentsInfoList) {
		if (contentsInfo.fileType != XmlElementType::Property) { continue; }
		realContents.push_back(contentsInfo);
	}

	ofs << "public:" << endl;
	ofs << "\tvirtual void serialize(DBBD::Buffer& buffer) {" << endl;
	ofs << "\t\tDBBD::" << base << "::writeHeader(buffer, getLength());" << endl;
	if (realContents.size() > 0) {
		ofs << "\t\tDBBD::Serialize::writeArray(buffer, fingerPrinter);" << endl;
		for (size_t i = 0; i < realContents.size(); i++) {
			auto info = realContents[i];
			ofs << "\t\tif (fingerPrinter[" << i << "]) { " << getDeSerialize(info.base, info.type, info.name, true) << " }" << endl;
		}
	}
	ofs << "\t}" << endl;;

	ofs << "\tvirtual void deserialize(DBBD::Buffer& buffer) {" << endl;
	ofs << "\t\tDBBD::" << base << "::readHeader(buffer);" << endl;
	if (realContents.size() > 0) {
		ofs << "\t\tDBBD::Serialize::readArray(buffer, fingerPrinter);" << endl;
		for (size_t i = 0; i < realContents.size(); i++) {
			auto info = realContents[i];
			ofs << "\t\tif (fingerPrinter[" << i << "]) { " << getDeSerialize(info.base, info.type, info.name, false) << " }" << endl;
		}
	}
	ofs << "\t}" << endl;;

	ofs << "\tvirtual unsigned int getLength() {" << endl;
	ofs << "\t\tunsigned int totalLength = DBBD::" << base << "::getLength();" << endl;
	if (realContents.size() > 0) {
		ofs << "\t\ttotalLength += sizeof(unsigned int) + sizeof(fingerPrinter);" << endl;
		for (size_t i = 0; i < realContents.size(); i++) {
			auto info = realContents[i];
			ofs << "\t\tif (fingerPrinter[" << i << "]) { totalLength += " << getLength(info.type, info.name) << "; }" << endl;
		}
	}
	ofs << "\t\treturn totalLength;" << endl;
	ofs << "\t}" << endl;

	if (realContents.size() <= 0) { return; }

	ofs << endl;

	ofs << "public:" << endl;
	for (size_t i = 0; i < realContents.size(); i++) {
		auto info = realContents[i];

		string newName = "";
		for (size_t j = 0; j < info.name.size(); j++) {
			char c = info.name[j];
			if (j == 0) {
				newName += toupper(c);
			}
			else {
				newName += c;
			}
		}

		ofs << "\t" << getPropertyType(info.type) << " get" << newName << "() { return " << info.name << "; }" << endl;
		ofs << "\tvoid set" << newName << "(" << getPropertyType(info.type) << " value) {" << endl;
		ofs << "\t\t" << info.name << " = value;" << endl;
		ofs << "\t\t" << "fingerPrinter[" << i << "] = true;" << endl;
		ofs << "\t}" << endl;
	}
	ofs << endl;

	ofs << "protected:" << endl;
	ofs << "\tbool fingerPrinter[" << realContents.size() << "] = { false, };" << endl;
	for (auto info : contentsInfoList) {
		switch (info.fileType) {
		case XmlElementType::Comment:
			ofs << "\t// " << info.value << endl;
			break;
		case XmlElementType::Property:
			ofs << "\t" << getPropertyType(info.type) << " " << info.name << ";" << endl;
			break;
		}
	}
}

string CppExtractor::getDeSerialize(string base, string type, string name, bool isSerialize) {
	string baseProcess = isSerialize ? "DBBD::Serialize::write" : "DBBD::Deserialize::read";
	switch (HashCode(type.c_str())) {
	case HashCode("string"):
	case HashCode("int64"):
	case HashCode("uint64"):
	case HashCode("int32"):
	case HashCode("uint32"):
	case HashCode("int16"):
	case HashCode("uint16"):
	case HashCode("bool"):
	case HashCode("char"):
	case HashCode("byte"):
	case HashCode("sbyte"):
		return baseProcess + "(buffer, " + name + ");";
	default:
		if (strcmp(base.c_str(), "cell") == 0
			|| strcmp(base.c_str(), "Cell") == 0) {
			return baseProcess + "(buffer, dynamic_cast<DBBD::Cell*>(&" + name + ");";
		}
		else {
			string msg = "illegal type and base, type: " + type + ", base: " + base;
			new exception(msg.c_str());
		}
	}
}

string CppExtractor::getLength(string type, string name) {
	switch (HashCode(type.c_str())) {
	case HashCode("int64"):
		return "sizeof(" + getPropertyType(type) + ")";
		break;
	case HashCode("string"):
		return "sizeof(unsigned int) + (" + name + ".size() * sizeof(wchar_t))";
	default:
		return name + ".getLength()";
	}
}