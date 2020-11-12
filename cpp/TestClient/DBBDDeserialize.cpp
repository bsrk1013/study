#include "DBBDDeserialize.h"
#include "DBBDBuffer.h"

namespace DBBD {
	Deserialize::Deserialize(Buffer* buffer) {
		this->buffer = buffer;
	}

	template<typename T>
	void Deserialize::read(T& value) {
		char* dataBuffer = buffer->readByteBlock(sizeof(T));
		memcpy(&value, dataBuffer, sizeof(T));
		delete[] dataBuffer;
	}

	void Deserialize::read(std::string& value) {
		size_t strSize;
		read(strSize);
		char* dataBuffer = buffer->readByteBlock(strSize);
		std::string str(&dataBuffer[0], &dataBuffer[0] + strSize);
		value = str;
	}

	void Deserialize::read(char* value) {
		size_t strSize;
		read(strSize);
		char* dataBuffer = buffer->readByteBlock(strSize);
		value = dataBuffer;
	}
}