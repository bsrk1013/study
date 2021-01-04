#pragma once
#include <filesystem>
#include <fstream>
#include <thread>
#include <string>
#include <vector>
#include <map>
#include "tinyxml2.h"

enum ExtractorType {
	Cpp = 0,
	Csharp = 1,
};

enum XmlElementType {
	None = 0,
	Comment,
	Cell,
	Protocol,
	Const,
	Property,
};

static inline constexpr unsigned int HashCode(const char* str)
{
	return str[0] ? static_cast<unsigned int>(str[0]) + 0xEDB8832Full * HashCode(str + 1) : 8603;
}

static inline std::string ltrim(std::string& s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
		return !std::isspace(ch);
		}));
	return s;
}

// trim from end (in place)
static inline std::string rtrim(std::string& s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
		return !std::isspace(ch);
		}).base(), s.end());
	return s;
}

// trim from both ends (in place)
static inline std::string trim(std::string& s) {
	ltrim(s);
	rtrim(s);
	return s;
}

struct FileInfo {
	XmlElementType fileType;
	std::string name;
	std::string type;
	std::string base;
	std::string value;
};

class BaseExtractor
{
public:
	BaseExtractor(ExtractorType type, 
		std::filesystem::path basePath, std::vector<std::string>& fileList);
	virtual ~BaseExtractor() {}

public:
	void start();

protected:
	virtual void writeHeader(std::ofstream& ofs) = 0;
	virtual void writeCell(std::ofstream& ofs) = 0;
	virtual void writeProtocol(std::ofstream& ofs) = 0;
	virtual void writeConst(std::ofstream& ofs, std::string fileName) = 0;
	virtual void writeContentsHeader(std::ofstream& ofs) = 0;
	virtual void writeCellContents(std::ofstream& ofs) = 0;
	virtual void writeProtocolContents(std::ofstream& ofs, std::string base) = 0;

protected:
	std::filesystem::path getOutputPath();
	std::filesystem::path getOutputFileName(std::string fileName);
	std::string getPropertyType(std::string type);

private:
	void parseXml(std::string fileName);
	void parseRoot(std::ofstream& ofs, tinyxml2::XMLElement* node, std::string fileName);
	void parseContents(std::ofstream& ofs, tinyxml2::XMLElement* node, std::string fileName);

protected:
	std::filesystem::path basePath;
	std::filesystem::path baseOutputPath;
	std::vector<std::thread*> threadList;
	std::vector<std::string> fileList;
	std::vector<FileInfo> headerInfoList;
	std::vector<FileInfo> contentsInfoList;

private:
	ExtractorType type;
};