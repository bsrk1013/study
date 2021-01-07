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
		auto ptr = weakPtr.lock();
		if (!ptr) {
			std::cout << "methodEvent, object release, eventType: " << eventType << std::endl;;
			return;
		}

		if (error) { 
			std::cout << "methodEvent, error, eventType: " << eventType << ", error: " << error << std::endl;;
			return; 
		}

		if (!existInfo(eventType)) { 
			std::cout << "methodEvent, info is null, eventType: " << eventType << std::endl;;
			return; 
		}

		std::cout << "methodEvent, eventType: " << eventType << std::endl;;
		lockObject.lock();
		auto info = timerMap[eventType];
		lockObject.unlock();

		info->method();
		if (info->isRepeat) {
			addTimerEvent(info->type, info->method, info->waitMs, info->isRepeat);
		}
		else {
			removeTimerEvent(info->type);
		}
	}

	void TimerObject::addTimerEvent(const size_t& eventType,
		const TimerParam& target, const size_t& waitMs, const bool& isRepeat) {
		if (isDisposed) {
			return;
		}

		auto waitTime = boost::posix_time::milliseconds(waitMs);
		
		if (!existInfo(eventType)) {
			lockObject.lock();
			auto timer = NEW_TIMER_SP(*context, waitTime);
			
			TimerInfoSP newInfo = std::make_shared<TimerInfo>();
			newInfo->type = eventType;
			newInfo->timer = timer;
			newInfo->method = target;
			newInfo->waitMs = waitMs;
			newInfo->isRepeat = isRepeat;
			timerMap[eventType] = newInfo;
		}
		else {
			lockObject.lock();
			auto existInfo = timerMap[eventType];
			existInfo->timer->expires_from_now(waitTime);
		}

		auto info = timerMap[eventType];
		info->timer->async_wait(std::bind(&TimerObject::methodEvent, this,
			std::placeholders::_1, std::weak_ptr<TimerObject>(shared_from_this()), info->type));
		lockObject.unlock();
	}

	void TimerObject::removeTimerEvent(const size_t& eventType) {
		lockObject.lock();
		auto it = timerMap.find(eventType);
		if (it == timerMap.end()) {
			lockObject.unlock();
			return;
		}

		std::cout << "removeTimerEvent, eventType: " << eventType << std::endl;
		timerMap.erase(eventType);
		lockObject.unlock();
	}

	bool TimerObject::existInfo(size_t eventType) {
		if (isDisposed) {
			return false;
		}

		lockObject.lock();
		auto it = timerMap.find(eventType);
		lockObject.unlock();
		return it != timerMap.end();
	}
}
