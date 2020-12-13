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

	char* Buffer::viewByteBlock(const size_t& size) {
		return readByte(size);;
	}

	char* Buffer::readByteBlock(const size_t& size) {
		/*if ((bufferOffset + size) > bufferLastPos) {
			throw std::exception("Out of Range");
		}*/

		readByte(size);

		bufferOffset += size;

		return block;
	}

	char* Buffer::readByte(const size_t& size) {
		if (size > blockSize) {
			blockSize *= 2;
			delete[] block;
			block = new char[blockSize];
		}

		size_t dataIndex = 0;
		for (size_t i = bufferOffset; i < bufferOffset + size; i++) {
			block[dataIndex++] = buffer[i];
		}

		return block;
	}

	void Buffer::clearBuffer() {
		memset(buffer, 0, bufferLastPos);
		bufferOffset = 0;
		bufferLastPos = 0;
	}
}