#include <string>
#include "Deserialize.h"
#include "Common.hpp"

namespace DBBD {
	void Deserialize::read(Buffer& buffer, std::string& value) {
		unsigned int strSize;
		read(buffer, strSize);
		char* dataBuffer = buffer.readByteBlock(strSize);
		std::string str(&dataBuffer[0], &dataBuffer[0] + strSize);
		value = str;
	}

	void Deserialize::read(Buffer& buffer, std::wstring& value) {
		std::string utf8;
		read(buffer, utf8);

		value = utf8ToUni(utf8);
	}

	void Deserialize::read(Buffer& buffer, char* value) {
		unsigned int strSize;
		read(buffer, strSize);
		char* dataBuffer = buffer.readByteBlock(strSize);
		value = dataBuffer;
	}

	void Deserialize::read(Buffer& buffer, Cell* value) {
		value->deserialize(buffer);
	}

	template<typename T1, typename T2>
	void Deserialize::readVector(Buffer& buffer, T1& vec) {
		unsigned int vecSize;
		read(buffer, vecSize);

		for (unsigned int i = 0; i < vecSize; i++) {
			T2 value;
			read(buffer, value);
			vec.push_back(value);
		}
	}
}