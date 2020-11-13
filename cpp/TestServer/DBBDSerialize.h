#pragma once
#include <string>

namespace DBBD
{
	class Buffer;
	class Cell;
	class Serialize
	{
	public:
		Serialize(Buffer* buffer) {
			this->buffer = buffer;
		}

		// Value Write
	public:
		template<typename T>
		void write(const T& value) {
			size_t size = sizeof(T);
			for (size_t i = 0; i < size; i++) {
				char data = value >> i * byte;
				buffer->putByte(data);
			}
		}

		template<>
		void write(const std::string& value) {
			size_t strSize = value.length();
			write(strSize);
			for (size_t i = 0; i < strSize; i++) {
				char data = value[i];
				buffer->putByte(data);
			}
		}

		template<>
		void write(char* const& value) {
			size_t strSize = strlen(value);
			write(strSize);
			for (size_t i = 0; i < strSize; i++) {
				char data = value[i];
				buffer->putByte(data);
			}
		}

		// Array Write
	public:
		template<typename T, size_t size>
		void writeArray(const T(&values)[size]) {
			size_t arraySize = size;
			write(arraySize);
			for (size_t i = 0; i < arraySize; i++) {
				T value = values[i];
				write(value);
			}
		}

		template<typename T1, typename T2>
		void writeVector(const T1& vec) {
			size_t vecSize = vec.size();
			write(vecSize);
			for (auto it = vec.begin(); it != vec.end(); it++) {
				T2 data = *it;
				write(data);
			}
		}

	private:
		Buffer* buffer;
	};
}