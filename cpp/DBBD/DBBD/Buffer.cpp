#include <iostream>
#include "Buffer.h"

namespace DBBD {
	Buffer::Buffer(const size_t& size) {
		buffer = new char[size];
		blockSize = size / 16;
		block = new char[blockSize];
	}

	Buffer::Buffer(const char* bufferBlock) {

	}

	Buffer::~Buffer() {
		if (buffer != nullptr) {
			delete[] buffer;
		}

		if (block != nullptr) {
			delete[] block;
		}
	}

	void Buffer::putByte(const char& byteData) {
		buffer[bufferLastPos++] = byteData;
	}

	char* Buffer::readByteBlock(const size_t& size, const bool& incOffset) {
		/*if ((bufferOffset + size) > bufferLastPos) {
			throw std::exception("Out of Range");
		}*/

		if (size > blockSize) {
			blockSize *= 2;
			delete[] block;
			block = new char[blockSize];
		}

		size_t dataIndex = 0;
		for (size_t i = bufferOffset; i < bufferOffset + size; i++) {
			block[dataIndex++] = buffer[i];
		}

		if (incOffset) {
			bufferOffset += size;
		}

		return block;
	}

	void Buffer::clearBuffer() {
		memset(buffer, 0, bufferLastPos);
		bufferOffset = 0;
		bufferLastPos = 0;
	}
}