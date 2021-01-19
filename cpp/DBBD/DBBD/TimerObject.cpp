#include "TimerObject.h"
#include "TimerManager.h"

namespace DBBD {
	TimerObject::~TimerObject()
	{
		std::vector<size_t> keys;
		for (auto info : timerMap) {
			keys.push_back(info.first);
		}

		for (auto key : keys) {
			removeTimerEvent(key);
		}
	}

	void TimerObject::methodEvent(const size_t& eventType) {
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
	//void TimerObject::methodEvent(std::weak_ptr<BaseObject> weakPtr, const size_t& eventType) {
		/*auto ptr = weakPtr.lock();
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
		}*/
	}

	void TimerObject::addTimerEvent(const size_t& eventType,
		const TimerParam& method, const size_t& waitMs, const bool& isRepeat) {

		auto reservedTime = std::chrono::system_clock::now() + std::chrono::milliseconds(waitMs);

		TimerInfoSP info;
		if (!existInfo(eventType)) {
			info = NEW_TIMER_INFO_SP();
			info->type = eventType;
			info->method = method;
			info->waitMs = waitMs;
			info->reservedTime = reservedTime;
			info->isRepeat = isRepeat;
			info->baseObject = std::weak_ptr<TimerObject>(shared_from_this());
			timerMap[eventType] = info;
		}
		else {
			info = timerMap[eventType];
			info->waitMs = waitMs;
			info->reservedTime = reservedTime;
			info->isRepeat = isRepeat;
		}

		TimerManager::Instance()->addTimer(TimerInfoWP(info));
	}

	void TimerObject::removeTimerEvent(const size_t& eventType) {
		if (!existInfo(eventType)) { return; }

		std::cout << "removeTimerEvent, eventType: " << eventType << std::endl;
		auto info = timerMap[eventType];
		info.reset();
		timerMap.erase(eventType);
	}

	bool TimerObject::existInfo(size_t eventType) {
		return timerMap.find(eventType) != timerMap.end();
	}
}
