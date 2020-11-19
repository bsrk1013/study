#pragma once
#include "Cell.h"

namespace DBBD {
	class Buffer;
	class Request : public Cell
	{
	public:
	public:
		virtual void serialize(Buffer* buffer) = 0;
		virtual void deserialize(Buffer* buffer) = 0;
		virtual size_t getLength() = 0;
	};
}