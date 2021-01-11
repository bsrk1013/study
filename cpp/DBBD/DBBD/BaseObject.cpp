#include "BaseObject.h"

namespace DBBD {
	BaseObject::BaseObject()
		: isDisposed(false){
	}

	BaseObject::~BaseObject() {
		dispose();
	}

	void BaseObject::init(IoContextSP context) {
		if (!isDisposed) { return; }
		this->context = context;
	}

	void BaseObject::reset() {
		if (!isDisposed) { return; }
		isDisposed = false;
		context.reset();
	}

	void BaseObject::dispose() {
		if (isDisposed) { return; }

		isDisposed = true;
		context.reset();
	}
}