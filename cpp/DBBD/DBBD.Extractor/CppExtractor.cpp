#include "CppExtractor.h"

CppExtractor::CppExtractor(std::filesystem::path basePath, std::vector<std::string>& fileList)
	: BaseExtractor(ExtractorType::Cpp, basePath, fileList) {

}

void CppExtractor::writeHeader(std::ofstream& ofs) {
	ofs << "#include \"DBBD/Cell.h\"" << std::endl;
	ofs << "#include \"DBBD/Request.h\"" << std::endl;
	ofs << "#include \"DBBD/Response.h\"" << std::endl;
	ofs << std::endl;
}

void CppExtractor::writeCell(std::ofstream& ofs) {
	writeContentsHeader(ofs);
	writeCellContents(ofs);
	ofs << "}" << std::endl;
}

void CppExtractor::writeProtocol(std::ofstream& ofs) {
	writeContentsHeader(ofs);
	writeProtocolContents(ofs);
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
			ofs << "class " << info.name << " : public " << info.base << " {" << std::endl;
			ofs << "public:" << std::endl;
			ofs << "\t" << info.name << "() {}" << std::endl;
			ofs << "\t" << info.name << "(";

			std::vector<FileInfo> realContents;
			for (auto contentsInfo : contentsInfoList) {
				if (contentsInfo.fileType != XmlElementType::Property) { continue; }
				realContents.push_back(contentsInfo);
			}

			for (size_t i = 0; i < realContents.size(); i++) {
				if (i > 0) { ofs << ", "; }
				auto propertyInfo = realContents[i];
				ofs << getPropertyType(propertyInfo.type) << propertyInfo.name;
			}
			ofs << ")" << std::endl;
			ofs << "\t\t: ";
			for (size_t i = 0; i < realContents.size(); i++) {
				if (i > 0) { ofs << ", "; }
				auto propertyInfo = realContents[i];
				ofs << propertyInfo.name << "(" << propertyInfo.name << ")";
			}
			ofs << std::endl << "\t{}" << std::endl;
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
		ofs << "\t\t" << getDeSerialize(info.type, info.name, true) << std::endl;
	}
	ofs << "\t}" << std::endl;;

	ofs << "\tvirtual void deserialize(DBBD::Buffer& buffer) {" << std::endl;
	for (auto info : realContents) {
		ofs << "\t\t" << getDeSerialize(info.type, info.name, false) << std::endl;
	}
	ofs << "\t}" << std::endl;;

	ofs << "\tvirtual size_t getLength() {" << std::endl;
	ofs << "\t\treturn ";
	for (auto info : realContents) {
		switch (HashCode(info.type.c_str())) {
		case HashCode("string"):
			ofs << "sizeof(size_t) + " << info.name << ".length()";
			break;
		default:
			ofs << "sizeof(" << info.name << ")";
			break;
		}
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

	ofs << "private:" << std::endl;
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

void CppExtractor::writeProtocolContents(std::ofstream& ofs) {
	std::vector<FileInfo> realContents;
	for (auto contentsInfo : contentsInfoList) {
		if (contentsInfo.fileType != XmlElementType::Property) { continue; }
		realContents.push_back(contentsInfo);
	}

	ofs << "public:" << std::endl;
	ofs << "\tvirtual void serialize(DBBD::Buffer& buffer) {" << std::endl;
	ofs << "\t\tRequest::writeHeader(buffer, getLength());" << std::endl;
	for (auto info : realContents) {
		ofs << "\t\t" << getDeSerialize(info.type, info.name, true) << std::endl;
	}
	ofs << "\t}" << std::endl;;

	ofs << "\tvirtual void deserialize(DBBD::Buffer& buffer) {" << std::endl;
	ofs << "\t\tRequest::readHeader(buffer);" << std::endl;
	for (auto info : realContents) {
		ofs << "\t\t" << getDeSerialize(info.type, info.name, false) << std::endl;
	}
	ofs << "\t}" << std::endl;;

	ofs << "\tvirtual size_t getLength() {" << std::endl;
	ofs << "\t\treturn ";
	for (auto info : realContents) {
		switch (HashCode(info.type.c_str())) {
		case HashCode("string"):
			ofs << "sizeof(size_t) + " << info.name << ".length()";
			break;
		default:
			ofs << "sizeof(" << info.name << ")";
			break;
		}
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

	ofs << "private:" << std::endl;
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

std::string CppExtractor::getDeSerialize(std::string type, std::string name, bool isSerialize) {
	std::string base = isSerialize ? "Serialize::write" : "Deserialize::read";
	switch (HashCode(type.c_str())) {
	case HashCode("string"):
	case HashCode("int64"):
		return base + "(buffer, " + name + ");";
		//return ofs << "\t\t" << base << "(buffer, " << name << ");" << std::endl;
	default:
		return base + "(buffer, dynamic_cast<Cell*>(&" + name + ");";
	}
}