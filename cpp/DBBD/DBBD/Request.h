#pragma once
#include "Cell.h"

namespace DBBD {
	static const size_t HeaderSize = sizeof(size_t) + sizeof(size_t);
	struct Header {
		Header(char* block) {

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
			serialize(buffer);
		}

		void readHeader(Buffer& buffer) {
			size_t temp = 0;
			Deserialize::read(buffer, temp);
			deserialize(buffer);
		}

		virtual void serialize(Buffer& buffer) {
			Serialize::write(buffer, typeId);
		}

		virtual void deserialize(Buffer& buffer) {
			Deserialize::read(buffer, typeId);
		}
		virtual size_t getLength() {
			return sizeof(typeId);
		}

	protected:
		size_t typeId;
	};
}