#pragma once
#include <string>

namespace DBBD
{
	class Buffer;
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

		template<typename T>
		void write(const T& value) {

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

		template<typename Iter>
		void writeIterator() {
		}

	private:
		Buffer* buffer;
	};
}