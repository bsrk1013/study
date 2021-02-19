#pragma once
#include <memory>
#include "Cell.h"
#include "Serialize.h"
#include "Deserialize.h"

namespace DBBD {
	class Buffer;
	class Request : public Cell {
	public:
		virtual ~Request() {}

	public:
		void writeHeader(Buffer& buffer, const size_t& length) {
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
		virtual std::string toString() = 0;

		size_t getTypeId() { return typeId; }

	protected:
		size_t typeId;
	};
}