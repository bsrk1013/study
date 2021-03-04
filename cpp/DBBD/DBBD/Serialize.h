#pragma once
#include <string>
#include "Buffer.h"
#include "Cell.h"

namespace DBBD
{
	class Serialize
	{
		// Value Write
	public:
		template<typename T>
		static void write(Buffer& buffer, const T& value) {
			unsigned int size = sizeof(T);
			for (unsigned int i = 0; i < size; i++) {
				char data = (char)(value >> i * DBBD::Buffer::byte);
				buffer.putByte(data);
			}
		}
		static void write(Buffer& buffer, const std::string& value);
		static void write(Buffer& buffer, const std::wstring& value);
		static void write(Buffer& buffer, char* const& value);
		static void write(Buffer& buffer, Cell* value);

		// Array Write
	public:
		template<typename T, size_t size>
		static void writeArray(Buffer& buffer, const T(&values)[size]) {
			unsigned int arraySize = size;
			write(buffer, arraySize);
			for (unsigned int i = 0; i < arraySize; i++) {
				T value = values[i];
				write(buffer, value);
			}
		}
		template<typename T1, typename T2>
		static void writeVector(Buffer& buffer, const T1& vec);
	};
}