#include "TimerObject.h"

namespace DBBD {
	TimerObject::TimerObject(){
	}
	
	TimerObject::~TimerObject() {
		dispose();
	}

	void TimerObject::init(IoContextSP context) {
		BaseObject::init(context);
		registTimerEvent();
	}

	void TimerObject::dispose() {
		if (isDisposed) { return; }
		BaseObject::dispose();

		std::vector<int> keys;
		for (auto pair : timerMap) {
			keys.push_back(pair.first);
		}

		for (auto key : keys) {
			removeTimerEvent(key);
		}

		timerMap.clear();
	}

	void TimerObject::reset() {
		if (!isDisposed) { return; }
		BaseObject::reset();
	}

	void TimerObject::methodEvent(const boost::system::error_code& error,
		std::weak_ptr<BaseObject> weakPtr, const size_t& eventType) {
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

		auto info = timerMap[eventType];

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
			auto existInfo = timerMap[eventType];
			existInfo->timer->expires_from_now(waitTime);
		}

		auto info = timerMap[eventType];
		info->timer->async_wait(std::bind(&TimerObject::methodEvent, this,
			std::placeholders::_1, std::weak_ptr<BaseObject>(shared_from_this()), info->type));
	}

	void TimerObject::removeTimerEvent(const size_t& eventType) {
		auto it = timerMap.find(eventType);
		if (it == timerMap.end()) {
			return;
		}

		std::cout << "removeTimerEvent, eventType: " << eventType << std::endl;
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
