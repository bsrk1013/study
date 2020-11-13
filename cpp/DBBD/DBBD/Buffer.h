#pragma once

namespace DBBD {
	class Buffer
	{
	public:
		Buffer(const size_t& size);
		Buffer(const char* bufferBlock);
		~Buffer();

	public:
		void putByte(const char byteData);
		char* readByteBlock(const size_t& size);
		void clearBuffer();

	public:
		char* getBuffer() { return buffer; }
		size_t getBufferLastPos() { return bufferLastPos; }
		void setBufferLastPos(size_t value) { bufferLastPos = value; }
		size_t getBufferOffset() { return bufferOffset; }
		void setBufferOffset(size_t value) { bufferOffset = value; }

	private:
		static const size_t bit = 1;
		static const size_t byte = 8 * bit;

	private:
		char* buffer = nullptr;
		size_t bufferOffset = 0;
		size_t bufferLastPos = 0;
	};
}