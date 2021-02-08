#include <string>
#include "Deserialize.h"

namespace DBBD {
	void Deserialize::read(Buffer& buffer, std::string& value) {
		size_t strSize;
		read(buffer, strSize);
		char* dataBuffer = buffer.readByteBlock(strSize);
		std::string str(&dataBuffer[0], &dataBuffer[0] + strSize);
		value = str;
	}

	void Deserialize::read(Buffer& buffer, char* value) {
		size_t strSize;
		read(buffer, strSize);
		char* dataBuffer = buffer.readByteBlock(strSize);
		value = dataBuffer;
	}

	void Deserialize::read(Buffer& buffer, Cell* value) {
		value->deserialize(buffer);
	}

	template<typename T1, typename T2>
	void Deserialize::readVector(Buffer& buffer, T1& vec) {
		size_t vecSize;
		read(buffer, vecSize);

		for (size_t i = 0; i < vecSize; i++) {
			T2 value;
			read(buffer, value);
			vec.push_back(value);
		}
	}
}