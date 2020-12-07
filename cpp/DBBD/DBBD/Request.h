#pragma once
#include "Cell.h"

namespace DBBD {
	class Buffer;
	class Request : public Cell
	{
	public:
		virtual ~Request() {};

	public:
		virtual void serialize(Buffer* buffer);
		virtual void deserialize(Buffer* buffer);
		virtual void getLength();

	protected:
		size_t typeId;
	};
}