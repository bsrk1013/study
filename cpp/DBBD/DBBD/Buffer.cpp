#include <iostream>
#include "Buffer.h"

namespace DBBD {
	Buffer::Buffer(const unsigned int& size) {
		bufferCapacity = size;
		buffer = new char[size];
		blockSize = size / 16;
		block = new char[blockSize];
	}

	Buffer::Buffer(const char* bufferBlock, const unsigned int& size) {
		bufferCapacity = size;
		buffer = new char[size];
		blockSize = size;
		block = new char[blockSize];
		memcpy(buffer, bufferBlock, size);
	}

	Buffer::~Buffer() {
		if (buffer != nullptr) {
			delete[] buffer;
			buffer = nullptr;
		}

		if (block != nullptr) {
			delete[] block;
			block = nullptr;
		}
	}

	void Buffer::putByte(const char& byteData) {
		buffer[bufferLastPos++] = byteData;
	}

	char* Buffer::viewByteBlock(const unsigned int& size) {
		return readByte(size);;
	}

	char* Buffer::readByteBlock(const unsigned int& size) {
		/*if ((bufferOffset + size) > bufferLastPos) {
			throw std::exception("Out of Range");
		}*/

		readByte(size);

		bufferOffset += size;

		return block;
	}

	char* Buffer::readByte(const unsigned int& size) {
		if (size > blockSize) {
			blockSize *= 2;
			delete[] block;
			block = new char[blockSize];
		}

		unsigned int dataIndex = 0;
		for (unsigned int i = bufferOffset; i < bufferOffset + size; i++) {
			block[dataIndex++] = buffer[i];
		}

		return block;
	}

	void Buffer::adjust() {
		unsigned int spare = bufferLastPos - bufferOffset;
		if (spare > bufferCapacity
			|| spare == 0) {
			bufferOffset = 0;
			bufferLastPos = 0;
		}
		else if (bufferOffset > 0
			&& spare > 0) {
			memmove(buffer, (buffer + bufferOffset), spare);
			bufferOffset = 0;
			bufferLastPos = spare;
		}
	}

	void Buffer::clearBuffer() {
		memset(buffer, 0, bufferLastPos);
		bufferOffset = 0;
		bufferLastPos = 0;
	}
}