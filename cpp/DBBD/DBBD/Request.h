#pragma once
#include "Cell.h"

namespace DBBD {
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

	class Buffer;
	class Request : public Cell
	{
	public:
		virtual ~Request() {};

	public:
		void writeHeader(Buffer& buffer, size_t length) {
			Serialize::write(buffer, length);
			Serialize::write(buffer, typeId);
		}

		void readHeader(Buffer& buffer) {
			size_t temp = 0;
			Deserialize::read(buffer, temp);
			Deserialize::read(buffer, typeId);
		}

		virtual void serialize(Buffer& buffer) = 0;
		virtual void deserialize(Buffer& buffer) = 0;
		virtual size_t getLength() {
			return sizeof(Header);
		}

		size_t getTypeId() { return typeId; }

	protected:
		size_t typeId;
	};
}