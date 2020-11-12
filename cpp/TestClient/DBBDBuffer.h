#pragma once
#include <iostream>

namespace DBBD {
	static const size_t bit = 1;
	static const size_t byte = 8 * bit;

	class Buffer {
	public:
		Buffer(size_t size) {
			buffer = new char[size];
		}
		Buffer(const char* bufferBlock) {

		}
		~Buffer() {
			if (buffer != nullptr) {
				delete[] buffer;
			}
		}

	public:
		void putByte(const char byteData) {
			buffer[bufferOffset++] = byteData;
			bufferLastPos++;
		}

		char* readByteBlock(size_t size) {
			if (bufferOffset + size > bufferLastPos) {
				throw std::invalid_argument("buffer write size over");
			}

			char* block = new char[size];
			size_t dataIndex = 0;
			for (size_t i = bufferOffset; i < bufferOffset + size; i++) {
				block[dataIndex++] = buffer[i];
			}

			bufferOffset += size;

			return block;
		}

	public:
		char* getBuffer() { return buffer; }
		size_t getBufferOffset() { return bufferOffset; }
		void setBufferOffset(size_t value) { bufferOffset = value; }

	private:
		char* buffer = nullptr;
		size_t bufferOffset = 0;
		size_t bufferLastPos = 0;
	};
}