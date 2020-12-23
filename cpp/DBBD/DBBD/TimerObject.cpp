#include "TimerObject.h"

namespace DBBD {
	TimerObject::TimerObject(IoContextSP context)
		: context(context)
	{
	}
	
	TimerObject::~TimerObject() {
		for (auto pair: timerMap) {
			delete pair.second;
		}

		timerMap.clear();
	}

	void TimerObject::addTimerEvent(int eventType, TimerParam target, size_t waitMs) {
		auto waitTime = boost::posix_time::milliseconds(waitMs);

		auto eventTimer = timerMap[eventType];
		if (!eventTimer) {
			eventTimer = new boost::asio::deadline_timer(*context, waitTime);
			timerMap[eventType] = eventTimer;
		}
		else {
			eventTimer->expires_at(eventTimer->expires_at() + waitTime);
		}

		eventTimer->async_wait(target);
	}

	void TimerObject::removeTimerEvent(int eventType) {
		auto eventTimer = timerMap[eventType];
		if (!eventTimer) {
			return;
		}

		eventTimer->cancel();

		delete eventTimer;
		timerMap.erase(eventType);
	}
}
