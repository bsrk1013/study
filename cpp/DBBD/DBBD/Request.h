#pragma once
#include "Cell.h"

namespace DBBD {
	class Buffer;
	class Request : public Cell
	{
	public:
		Request(size_t typeId) :typeId(typeId) {}
		virtual ~Request() {};

	public:
		virtual void serialize(Buffer* buffer) {
			Serialize::write(buffer, typeId);
		}

		virtual void deserialize(Buffer* buffer) {
			Deserialize::read(buffer, typeId);
		}
		virtual size_t getLength() {
			return sizeof(typeId);
		}

	protected:
		size_t typeId;
	};
}