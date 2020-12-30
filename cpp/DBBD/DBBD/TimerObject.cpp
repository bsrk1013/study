#include "TimerObject.h"

namespace DBBD {
	TimerObject::TimerObject(IoContextSP context)
		: context(context)
	{
		std::cout << "TimerObject call..." << std::endl;
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

	void TimerObject::methodEvent(const boost::system::error_code& error,
		const size_t& eventType) {
		if (!existInfo(eventType)) {
			std::cout << "timer event not found, eventType: " << eventType << std::endl;
			return;
		}

		auto info = timerMap[eventType];

		info.method();
		if (info.isRepeat) {
			addTimerEvent(info.type, info.method, info.waitMs, info.isRepeat);
		}
		else {
			removeTimerEvent(info.type);
		}

		//method();
	}

	void TimerObject::addTimerEvent(const size_t& eventType, 
		const TimerParam& target, const size_t& waitMs, const bool& isRepeat) {
		if (isDisposed) {
			return;
		}

		auto waitTime = boost::posix_time::milliseconds(waitMs);
		
		if (!existInfo(eventType)) {
			std::cout << "addTimerEvent eventType[" << eventType << "] call..." << std::endl;
			auto timer = std::make_shared<boost::asio::deadline_timer>(*context, waitTime);
			
			TimerInfo newInfo;
			newInfo.type = eventType;
			newInfo.timer = timer;
			newInfo.method = target;
			newInfo.waitMs = waitMs;
			newInfo.isRepeat = isRepeat;
		}
		else {
			auto existInfo = timerMap[eventType];
			existInfo.timer->expires_from_now(waitTime);
		}

		auto info = timerMap[eventType];
		info.timer->async_wait(std::bind(&TimerObject::methodEvent, this,
			std::placeholders::_1, info.type));

		/*auto info = timerMap[eventType];
		if (!info.timer) {
			std::cout << "addTimerEvent eventType[" << eventType << "] call..." << std::endl;
			auto timer = std::make_shared<boost::asio::deadline_timer>(*context, waitTime);
			TimerInfo newInfo;
			newInfo.type = eventType;
			newInfo.timer = timer;
			newInfo.method = target;
			newInfo.
		}
		else {

		}*/


		/*auto waitTime = boost::posix_time::milliseconds(waitMs);

		auto eventTimer = timerMap[eventType];
		if (!eventTimer) {
			std::cout << "addTimerEvent eventType[" << eventType << "] call..." << std::endl;
			eventTimer = std::make_shared<boost::asio::deadline_timer>(*context, waitTime);
			timerMap[eventType] = eventTimer;
		}
		else {
			eventTimer->expires_from_now(waitTime);
		}

		eventTimer->async_wait(std::bind(&TimerObject::methodEvent, this,
			std::placeholders::_1, eventType, target));*/
	}

	void TimerObject::removeTimerEvent(const size_t& eventType) {
		auto it = timerMap.find(eventType);
		if (it == timerMap.end()) {
			return;
		}

		// 이미 예약된 이벤트는 취소 할 수 없음
		//eventTimer->cancel();

		std::cout << "removeTimerEvent eventType[" << eventType << "] call..." << std::endl;
		timerMap.erase(eventType);
	}

	bool TimerObject::existInfo(size_t eventType) {
		auto it = timerMap.find(eventType);
		return it != timerMap.end();
	}
}
