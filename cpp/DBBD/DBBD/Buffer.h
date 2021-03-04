#pragma once

namespace DBBD {
	static const unsigned int HeaderSize = sizeof(unsigned int) + sizeof(unsigned int);

	class Buffer
	{
	public:
		Buffer(const unsigned int& size);
		Buffer(const char* bufferBlock);
		~Buffer();

	public:
		void putByte(const char& byteData);
		char* readByteBlock(const unsigned int& size);
		char* viewByteBlock(const unsigned int& size);
		void adjust();
		void clearBuffer();

	public:
		inline void increaseLastPos(const unsigned int& size) { bufferLastPos += size; }

	private:
		char* readByte(const unsigned int& size);

	public:
		char* getBuffer() { return buffer; }
		unsigned int getBufferLastPos() { return bufferLastPos; }
		void setBufferLastPos(const unsigned int& value) { bufferLastPos = value; }
		unsigned int getBufferOffset() { return bufferOffset; }
		void setBufferOffset(const unsigned int& value) { bufferOffset = value; }

	public:
		static const unsigned int bit = 1;
		static const unsigned int byte = 8 * bit;

	private:
		char* buffer = nullptr;
		char* block = nullptr;
		unsigned int blockSize = 0;
		unsigned int bufferOffset = 0;
		unsigned int bufferLastPos = 0;
		unsigned int bufferCapacity = 0;
	};
}