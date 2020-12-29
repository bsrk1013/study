#include "TimerObject.h"

namespace DBBD {
	TimerObject::TimerObject(IoContextSP context)
		: context(context)
	{
	}
	
	TimerObject::~TimerObject() {
		std::cout << "~TimerObject call..." << std::endl;
		dispose();
	}

	void TimerObject::dispose() {
		if (isDisposed) {
			return;
		}

		std::vector<int> keys;
		for (auto pair : timerMap) {
			keys.push_back(pair.first);
		}

		for (auto key : keys) {
			removeTimerEvent(key);
		}

		timerMap.clear();

		isDisposed = true;
	}

	void TimerObject::addTimerEvent(int eventType, TimerParam target, size_t waitMs) {
		if (isDisposed) {
			return;
		}

		auto waitTime = boost::posix_time::milliseconds(waitMs);

		auto eventTimer = timerMap[eventType];
		if (!eventTimer) {
			eventTimer = new boost::asio::deadline_timer(*context, waitTime);
			timerMap[eventType] = eventTimer;
		}
		else {
			eventTimer->expires_at(eventTimer->expires_at() + waitTime);
		}

		if (!isDisposed) {
			eventTimer->async_wait(target);
		}
	}

	void TimerObject::removeTimerEvent(int eventType) {
		auto eventTimer = timerMap[eventType];
		if (!eventTimer) {
			return;
		}

		eventTimer->cancel_one();

		std::cout << "~event timer type[" << eventType << "] release..." << std::endl;
		delete eventTimer;
		timerMap.erase(eventType);
	}
}
