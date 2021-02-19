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
		virtual size_t getLength() = 0;
		virtual std::string toString() = 0;
	};

	struct Header {
		Header(char* block) {
			size_t pos = 0;
			memcpy(&length, block + pos, sizeof(size_t));
			pos += sizeof(size_t);
			memcpy(&typeId, (block + pos), sizeof(size_t));
		}

		size_t length;
		size_t typeId;
	};
}