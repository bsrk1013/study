#pragma once

namespace DBBD {
	static const size_t HeaderSize = sizeof(size_t) + sizeof(size_t);

	class Buffer
	{
	public:
		Buffer(const size_t& size);
		Buffer(const char* bufferBlock);
		~Buffer();

	public:
		void putByte(const char& byteData);
		char* readByteBlock(const size_t& size);
		char* viewByteBlock(const size_t& size);
		void adjust();
		void clearBuffer();

	public:
		inline void increaseLastPos(size_t size) { bufferLastPos += size; }

	private:
		char* readByte(const size_t& size);

	public:
		char* getBuffer() { return buffer; }
		size_t getBufferLastPos() { return bufferLastPos; }
		void setBufferLastPos(size_t value) { bufferLastPos = value; }
		size_t getBufferOffset() { return bufferOffset; }
		void setBufferOffset(size_t value) { bufferOffset = value; }

	public:
		static const size_t bit = 1;
		static const size_t byte = 8 * bit;

	private:
		char* buffer = nullptr;
		char* block = nullptr;
		size_t blockSize = 0;
		size_t bufferOffset = 0;
		size_t bufferLastPos = 0;
	};
}