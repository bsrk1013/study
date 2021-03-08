#pragma once
#include "BaseExtractor.h"

class CsharpExtractor : public BaseExtractor
{
public:
	CsharpExtractor(std::filesystem::path basePath, const std::vector<std::string>& fileList);
	virtual ~CsharpExtractor() {}

protected:
	// BaseExtractor을(를) 통해 상속됨
	virtual void writeHeader(std::ofstream& ofs) override;
	virtual void writeCell(std::ofstream& ofs) override;
	virtual void writeProtocol(std::ofstream& ofs) override;
	virtual void writeConst(std::ofstream& ofs, std::string fileName) override;
	virtual void writeContentsHeader(std::ofstream& ofs);
	virtual void writeCellContents(std::ofstream& ofs);
	virtual void writeProtocolContents(std::ofstream& ofs, std::string base);

private:
	std::string getLength(std::string type, std::string name);
};