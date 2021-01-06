#include "TimerObject.h"

namespace DBBD {
	TimerObject::TimerObject(IoContextSP context)
		: context(context)
	{
	}
	
	TimerObject::~TimerObject() {
		dispose();
	}

	void TimerObject::start() {
		registTimerEvent();
	}

	void TimerObject::stop() {
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

	void TimerObject::methodEvent(const boost::system::error_code& error,
		std::weak_ptr<TimerObject> weakPtr, const size_t& eventType) {
		if (error) { return; }

		auto ptr = weakPtr.lock();
		if (!ptr) {
			return; 
		}

		if (!existInfo(eventType)) { return; }

		auto info = timerMap[eventType];

		info.method();
		if (info.isRepeat) {
			addTimerEvent(info.type, info.method, info.waitMs, info.isRepeat);
		}
		else {
			removeTimerEvent(info.type);
		}
	}

	void TimerObject::addTimerEvent(const size_t& eventType,
		const TimerParam& target, const size_t& waitMs, const bool& isRepeat) {
		if (isDisposed) {
			return;
		}

		auto waitTime = boost::posix_time::milliseconds(waitMs);
		
		if (!existInfo(eventType)) {
			auto timer = std::make_shared<boost::asio::deadline_timer>(*context, waitTime);
			
			TimerInfo newInfo;
			newInfo.type = eventType;
			newInfo.timer = timer;
			newInfo.method = target;
			newInfo.waitMs = waitMs;
			newInfo.isRepeat = isRepeat;
			timerMap[eventType] = newInfo;
		}
		else {
			auto existInfo = timerMap[eventType];
			existInfo.timer->expires_from_now(waitTime);
		}

		auto info = timerMap[eventType];
		info.timer->async_wait(std::bind(&TimerObject::methodEvent, shared_from_this(),
			std::placeholders::_1, std::weak_ptr<TimerObject>(shared_from_this()), info.type));
	}

	void TimerObject::removeTimerEvent(const size_t& eventType) {
		auto it = timerMap.find(eventType);
		if (it == timerMap.end()) {
			return;
		}

		it->second.timer->cancel();

		timerMap.erase(eventType);
	}

	bool TimerObject::existInfo(size_t eventType) {
		if (isDisposed) {
			return false;
		}

		auto it = timerMap.find(eventType);
		return it != timerMap.end();
	}
}
