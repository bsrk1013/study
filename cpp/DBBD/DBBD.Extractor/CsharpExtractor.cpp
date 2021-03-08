#include "CsharpExtractor.h"
#include <string>

using namespace std;

CsharpExtractor::CsharpExtractor(std::filesystem::path basePath, const vector<string>& fileList)
	: BaseExtractor(ExtractorType::Csharp, basePath, fileList) {

}

void CsharpExtractor::writeHeader(std::ofstream& ofs)
{
	ofs << "using DBBD;" << endl;
	ofs << "using System;" << endl;
	ofs << "using System.Linq;" << endl;
	ofs << "using System.Text;" << endl;
	ofs << "using System.Collections.Generic;" << endl;
	ofs << endl;
}

void CsharpExtractor::writeCell(std::ofstream& ofs)
{
	writeContentsHeader(ofs);
	writeCellContents(ofs);
	ofs << "}" << endl;
}

void CsharpExtractor::writeProtocol(std::ofstream& ofs)
{
	writeContentsHeader(ofs);

	for (auto info : headerInfoList) {
		if (info.fileType == XmlElementType::Protocol) {
			writeProtocolContents(ofs, info.base);
			break;
		}
	}

	ofs << "}" << endl;
}

void CsharpExtractor::writeConst(std::ofstream& ofs, std::string fileName)
{
	size_t pos = fileName.find(".");
	auto namespaceName = fileName.substr(0, pos);
	ofs << " static class " << namespaceName << endl;
	ofs << "{" << endl;

	ofs << "\tpublic enum Value" << endl;
	ofs << "\t{" << endl;
	for (auto info : headerInfoList) {
		ofs << "\t\t" << info.name << " = " << info.value << "," << endl;
	}
	ofs << "\t}" << endl;

	ofs << "\tprivate static Dictionary<Value, string> stringMap = new Dictionary<Value, string>" << endl;
	ofs << "\t{" << endl;
	for (auto info : headerInfoList) {
		ofs << "\t\t{ Value." << info.name << ", \"" << info.name << "\" }," << endl;
	}
	ofs << "\t}" << endl;

	ofs << "\tpublic static string Get(Value value)" << endl;
	ofs << "\t{" << endl;
	ofs << "\t\tif (!stringMap.ContainsKey(value))" << endl;
	ofs << "\t\t{" << endl;
	ofs << "\t\t\treturn \"\";" << endl;
	ofs << "\t\t}" << endl;
	ofs << "\t\treturn stringMap[value];" << endl;
	ofs << "\t}" << endl;

	ofs << "}" << endl;
}

void CsharpExtractor::writeContentsHeader(std::ofstream& ofs)
{
	vector<FileInfo> realContents;
	for (auto contentsInfo : contentsInfoList) {
		if (contentsInfo.fileType != XmlElementType::Property) { continue; }
		realContents.push_back(contentsInfo);
	}

	for (auto info : headerInfoList) {
		switch (info.fileType) {
		case XmlElementType::Comment: {
			ofs << "// " << info.value << endl;
			break;
		}
		case XmlElementType::Protocol:
		case XmlElementType::Cell: {
			if (info.fileType == XmlElementType::Cell) {
				ofs << "class " << info.name << " : DBBD.I" << info.base << endl;
			}
			else if (info.fileType == XmlElementType::Protocol) {
				ofs << "class " << info.name << " : DBBD." << info.base << endl;
			}
			ofs << "{" << endl;
			ofs << "\tpublic " << info.name << "()" << endl;
			ofs << "\t{" << endl;
			if (info.fileType == XmlElementType::Cell) {
				if (realContents.size() > 0) {
					ofs << "\t\tfingerPrinter.AddRange(Enumerable.Repeat(false, " << realContents.size() << "));" << endl;
				}
			}
			else if (info.fileType == XmlElementType::Protocol) {
				ofs << "\t\ttypeId = (uint)ProtocolType.Value." << info.type << ";" << endl;
				if (realContents.size() > 0) {
					ofs << "\t\tfingerPrinter.AddRange(Enumerable.Repeat(false, " << realContents.size() << "));" << endl;
				}
			}
			ofs << "\t}" << endl << endl;
			break;
		}
		}
	}
}

void CsharpExtractor::writeCellContents(std::ofstream& ofs)
{
	vector<FileInfo> realContents;
	for (auto contentsInfo : contentsInfoList) {
		if (contentsInfo.fileType != XmlElementType::Property) { continue; }
		realContents.push_back(contentsInfo);
	}
	vector<FileInfo> header;
	for (auto headerInfo : headerInfoList) {
		if (headerInfo.fileType == XmlElementType::Cell) { header.push_back(headerInfo); break; }
	}

	ofs << "\tpublic void Serialize(DBBD.Buffer buffer)" << endl;
	ofs << "\t{" << endl;
	if (realContents.size() > 0) {
		ofs << "\t\tDBBD.Serizlie.Write(buffer, fingerPrinter);" << endl;
		for (size_t i = 0; i < realContents.size(); i++) {
			auto info = realContents[i];
			ofs << "\t\tif (fingerPrinter[" << i << "]) { " << "DBBD.Serialize.Write(buffer, " << info.name << "); }" << endl;
		}
	}
	ofs << "\t}" << endl << endl;

	ofs << "\tpublic void Deserialize(DBBD.Buffer buffer)" << endl;
	ofs << "\t{" << endl;
	if (realContents.size() > 0) {
		ofs << "\t\tDBBD.Deserialize.Read(buffer, out fingerPrinter);" << endl;
		for (size_t i = 0; i < realContents.size(); i++) {
			auto info = realContents[i];
			ofs << "\t\tif (fingerPrinter[" << i << "]) { " << "DBBD.Deserialize.Read(buffer, out " << info.name << "); }" << endl;
		}
	}
	ofs << "\t}" << endl << endl;

	ofs << "\tpublic uint GetLength()" << endl;
	ofs << "\t{" << endl;
	ofs << "\t\tuint totalLength = 0;" << endl;
	if (realContents.size() > 0) {
		ofs << "\t\ttotalLength += (uint)(sizeof(uint) + fingerPrinter.Count);" << endl;
		for (size_t i = 0; i < realContents.size(); i++) {
			auto info = realContents[i];
			ofs << "\t\tif (fingerPrinter[" << i << "]) { totalLength += (uint)(" << getLength(info.type, info.name) << "); }" << endl;
		}
	}
	ofs << "\t\treturn totalLength;" << endl;
	ofs << "\t}" << endl << endl;

	if (realContents.size() <= 0) { return; }

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

		ofs << "\tpublic " << getPropertyType(info.type) << " " << newName << " { get { return " << info.name << "; } set { " << info.name << " = value; fingerPrinter[" << i << "] = true; } }" << endl;
	}

	ofs << endl;
	ofs << "\tprivate List<bool> fingerPrinter = new List<bool>();" << endl;

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

		ofs << "\tprivate " << getPropertyType(info.type) << " " << info.name << ";" << endl;
	}
}

void CsharpExtractor::writeProtocolContents(std::ofstream& ofs, std::string base)
{
	vector<FileInfo> realContents;
	for (auto contentsInfo : contentsInfoList) {
		if (contentsInfo.fileType != XmlElementType::Property) { continue; }
		realContents.push_back(contentsInfo);
	}

	ofs << "\tpublic override void Serialize(DBBD.Buffer buffer)" << endl;
	ofs << "\t{" << endl;
	ofs << "\t\tbase.WriteHeader(buffer, GetLength());" << endl;
	if (realContents.size() > 0) {
		ofs << "\t\tDBBD.Serialize.Write(buffer, fingerPrinter);" << endl;
		for (size_t i = 0; i < realContents.size(); i++) {
			auto info = realContents[i];
			ofs << "\t\tif (fingerPrinter[" << i << "]) { " << "DBBD.Serialize.Write(buffer, " << info.name << "); }" << endl;
		}
	}
	ofs << "\t}" << endl << endl;

	ofs << "\tpublic void Deserialize(DBBD.Buffer buffer)" << endl;
	ofs << "\t{" << endl;
	ofs << "\t\tbase.ReadHeader(buffer);" << endl;
	if (realContents.size() > 0) {
		ofs << "\t\tDBBD.Deserialize.Read(buffer, out fingerPrinter);" << endl;
		for (size_t i = 0; i < realContents.size(); i++) {
			auto info = realContents[i];
			ofs << "\t\tif (fingerPrinter[" << i << "]) { " << "DBBD.Deserialize.Read(buffer, out " << info.name << "); }" << endl;
		}
	}
	ofs << "\t}" << endl << endl;

	ofs << "\tpublic uint GetLength()" << endl;
	ofs << "\t{" << endl;
	ofs << "\t\tuint totalLength = 0;" << endl;
	if (realContents.size() > 0) {
		ofs << "\t\ttotalLength += (uint)(base.GetLength() + sizeof(uint) + fingerPrinter.Count);" << endl;
		for (size_t i = 0; i < realContents.size(); i++) {
			auto info = realContents[i];
			ofs << "\t\tif (fingerPrinter[" << i << "]) { totalLength += (uint)(" << getLength(info.type, info.name) << "); }" << endl;
		}
	}
	ofs << "\t\treturn totalLength;" << endl;
	ofs << "\t}" << endl;

	if (realContents.size() <= 0) { return; }

	ofs << endl;

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

		ofs << "\tpublic " << getPropertyType(info.type) << " " << newName << " { get { return " << info.name << "; } set { " << info.name << " = value; fingerPrinter[" << i << "] = true; } }" << endl;
	}

	ofs << endl;
	ofs << "\tprivate List<bool> fingerPrinter = new List<bool>();" << endl;

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

		ofs << "\tprivate " << getPropertyType(info.type) << " " << info.name << ";" << endl;
	}
}

string CsharpExtractor::getLength(string type, string name) {
	switch (HashCode(type.c_str())) {
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
		return "sizeof(" + getPropertyType(type) + ")";
		break;
	case HashCode("string"):
		return "sizeof(" + getPropertyType("uint32") + ") + Encoding.UTF8.GetByteCount(" + name + ")";
	default:
		return name + ".GetLength()";
	}
}