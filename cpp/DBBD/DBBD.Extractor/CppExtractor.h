#pragma once
#include "BaseExtractor.h"

class CppExtractor : public BaseExtractor
{
public:
	CppExtractor(std::filesystem::path basePath, std::vector<std::string>& fileList);
	virtual ~CppExtractor() {}
protected:
	virtual void writeHeader(std::ofstream& ofs) override;
	virtual void writeCell(std::ofstream& ofs) override;
	virtual void writeProtocol(std::ofstream& ofs) override;
	virtual void writeConst(std::ofstream& ofs, std::string fileName) override;
	virtual void writeContentsHeader(std::ofstream& ofs) override;
	virtual void writeCellContents(std::ofstream& ofs) override;
	virtual void writeProtocolContents(std::ofstream& ofs, std::string base) override;

private:
	std::string getDeSerialize(std::string type, std::string name, bool isSerialize);
	std::string getLength(std::string type, std::string name);
};