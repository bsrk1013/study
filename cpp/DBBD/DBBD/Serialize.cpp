#include "Serialize.h"
#include "Buffer.h"
#include "Cell.h"

namespace DBBD {
	template<typename T>
	void Serialize::write(Buffer& buffer, const T& value) {
		size_t size = sizeof(T);
		for (size_t i = 0; i < size; i++) {
			char data = value >> i * DBBD::Buffer::byte;
			buffer.putByte(data);
		}
	}

	void Serialize::write(Buffer& buffer, const unsigned char& value) {
		write<unsigned char>(buffer, value);
	}

	void Serialize::write(Buffer& buffer, const short& value) {
		write<short>(buffer, value);
	}

	void Serialize::write(Buffer& buffer, const int& value) {
		write<int>(buffer, value);
	}

	void Serialize::write(Buffer& buffer, const long& value) {
		write<long>(buffer, value);
	}

	void Serialize::write(Buffer& buffer, const size_t& value) {
		write<size_t>(buffer, value);
	}

	void Serialize::write(Buffer& buffer, const std::string& value) {
		size_t strSize = value.length();
		write(buffer, strSize);
		for (size_t i = 0; i < strSize; i++) {
			char data = value[i];
			buffer.putByte(data);
		}
	}

	void Serialize::write(Buffer& buffer, char* const& value) {
		size_t strSize = strlen(value);
		write(buffer, strSize);
		for (size_t i = 0; i < strSize; i++) {
			char data = value[i];
			buffer.putByte(data);
		}
	}

	void Serialize::write(Buffer& buffer, Cell* value) {
		value->serialize(buffer);
	}

	/*void Serialize::write(Buffer* buffer, Request* value) {
		value->
	}*/

	template<typename T, size_t size>
	void Serialize::writeArray(Buffer& buffer, const T(&values)[size]) {
		size_t arraySize = size;
		write(buffer, arraySize);
		for (size_t i = 0; i < arraySize; i++) {
			T value = values[i];
			write(buffer, value);
		}
	}

	template<typename T1, typename T2>
	void Serialize::writeVector(Buffer& buffer, const T1& vec) {
		size_t vecSize = vec.size();
		write(buffer, vecSize);
		for (auto it = vec.begin(); it != vec.end(); it++) {
			T2 data = *it;
			write(buffer, data);
		}
	}
}