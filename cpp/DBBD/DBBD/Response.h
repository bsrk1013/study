#pragma once
#include <memory>
#include "Cell.h"
#include "Serialize.h"
#include "Deserialize.h"

namespace DBBD {
	class Buffer;
	class Response : public Cell {
	public:
		virtual ~Response() {}

	public:
		void writeHeader(Buffer& buffer, const unsigned int& length) {
			Serialize::write(buffer, length);
			Serialize::write(buffer, typeId);
		}

		void readHeader(Buffer& buffer) {
			unsigned int temp = 0;
			Deserialize::read(buffer, temp);
			Deserialize::read(buffer, typeId);
		}

		virtual void serialize(Buffer& buffer) = 0;
		virtual void deserialize(Buffer& buffer) = 0;
		virtual unsigned int getLength() {
			return sizeof(Header);
		}
		virtual std::string toString() = 0;

		unsigned int getTypeId() { return typeId; }

	protected:
		unsigned int typeId;
	};
}