#pragma once
#include <iostream>
#include "Define.h"

namespace DBBD {
	class BaseObject 
		: public std::enable_shared_from_this<BaseObject> {
	public:
		BaseObject();
		virtual ~BaseObject();

	public:
		std::shared_ptr<BaseObject> getPtr() {
			return shared_from_this();
		}

	public:
		virtual void init(IoContextSP context);
		virtual void dispose();
		virtual void reset();

	protected:
		bool isDisposed = true;
		IoContextSP context;
	};
}