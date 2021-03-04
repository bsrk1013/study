#pragma once
namespace DBBD {
	class Serialize;
	class Deserialize;
	class Buffer;
	class Cell
	{
	public:
		virtual ~Cell() {};

	public:
		virtual void serialize(Buffer& buffer) = 0;
		virtual void deserialize(Buffer& buffer) = 0;
		virtual unsigned int getLength() = 0;
		virtual std::string toString() = 0;
	};

	struct Header {
		Header(char* block) {
			unsigned int pos = 0;
			memcpy(&length, block + pos, sizeof(unsigned int));
			pos += sizeof(unsigned int);
			memcpy(&typeId, (block + pos), sizeof(unsigned int));
		}

		unsigned int length;
		unsigned int typeId;
	};
}