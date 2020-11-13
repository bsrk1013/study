#pragma once
#include <iostream>

namespace DBBD {
	class Buffer;
	class Deserialize
	{
	public:
		Deserialize(DBBD::Buffer* buffer) {
			this->buffer = buffer;
		}

		// Value Read
	public:
		template<typename T>
		void read(T& value) {
			char* dataBuffer = buffer->readByteBlock(sizeof(T));
			memcpy(&value, dataBuffer, sizeof(T));
			delete[] dataBuffer;
		}

		void read(std::string& value) {
			size_t strSize;
			read(strSize);
			char* dataBuffer = buffer->readByteBlock(strSize);
			std::string str(&dataBuffer[0], &dataBuffer[0] + strSize);
			value = str;
		}

		void read(char* value) {
			size_t strSize;
			read(strSize);
			char* dataBuffer = buffer->readByteBlock(strSize);
			value = dataBuffer;
		}

		// Array Read
	public:
		template<typename T>
		void readArray(T*& values) {
			size_t arraySize;
			read(arraySize);

			values = new T[arraySize];
			for (size_t i = 0; i < arraySize; i++) {
				T value;
				read(value);
				values[i] = value;
			}
		}

		template<typename T1, typename T2>
		void readVector(T1& vec) {
			size_t vecSize;
			read(vecSize);

			for (size_t i = 0; i < vecSize; i++) {
				T2 value;
				read(value);
				vec.push_back(value);
			}
		}

	private:
		DBBD::Buffer* buffer;
	};
}