#pragma once
#include "Cell.h"

namespace DBBD {
	class Buffer;
	class Request : public Cell
	{
	public:
		virtual ~Request() {};

	protected:
		size_t typeId;
	};
}