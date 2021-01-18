#include <string>
#include "Deserialize.h"
#include "Buffer.h"
#include "Cell.h"

namespace DBBD {
	template<typename T>
	void Deserialize::read(Buffer& buffer, T& value) {
		char* dataBuffer = buffer.readByteBlock(sizeof(T));
		memcpy(&value, dataBuffer, sizeof(T));
	}

	void Deserialize::read(Buffer& buffer, unsigned char& value) {
		read<unsigned char>(buffer, value);
	}

	void Deserialize::read(Buffer& buffer, short& value) {
		read<short>(buffer, value);
	}

	void Deserialize::read(Buffer& buffer, int& value) {
		read<int>(buffer, value);
	}

	void Deserialize::read(Buffer& buffer, long& value) {
		read<long>(buffer, value);
	}

	void Deserialize::read(Buffer& buffer, size_t& value) {
		read<size_t>(buffer, value);
	}

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

	template<typename T>
	void Deserialize::readArray(Buffer& buffer, T*& values) {
		size_t arraySize;
		read(buffer, arraySize);

		values = new T[arraySize];
		for (size_t i = 0; i < arraySize; i++) {
			T value;
			read(buffer, value);
			values[i] = value;
		}
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