#include <iostream>
#include "Buffer.h"

namespace DBBD {
	Buffer::Buffer(const size_t& size) {
		buffer = new char[size];
	}

	Buffer::Buffer(const char* bufferBlock) {

	}

	Buffer::~Buffer() {
		if (buffer != nullptr) {
			delete[] buffer;
		}
	}

	void Buffer::putByte(const char byteData) {
		buffer[bufferOffset++] = byteData;
		bufferLastPos++;
	}

	char* Buffer::readByteBlock(const size_t& size) {
		if ((bufferOffset + size) > bufferLastPos) {
		}

		char* block = new char[size];
		size_t dataIndex = 0;
		for (size_t i = bufferOffset; i < bufferOffset + size; i++) {
			block[dataIndex++] = buffer[i];
		}

		bufferOffset += size;

		return block;
	}

	void Buffer::clearBuffer() {
		memset(buffer, 0, bufferLastPos);
		bufferOffset = 0;
		bufferLastPos = 0;
	}
}