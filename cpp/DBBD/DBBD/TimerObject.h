#pragma once
#include <iostream>
#include <map>
#include <boost/asio.hpp>
#include "Define.h"
#include "BaseObject.h"

namespace DBBD {
	class TimerObject;
	struct TimerInfo {
		TimerInfo() {
			memset(this, 0, sizeof(TimerInfo));
		}
		size_t type;
		TimerParam method;
		size_t waitMs;
		std::chrono::system_clock::time_point reservedTime;
		bool isRepeat;
		std::weak_ptr<TimerObject> baseObject;
	};

	using TimerInfoSP = std::shared_ptr<TimerInfo>;
	using TimerInfoWP = std::weak_ptr<TimerInfo>;
#define NEW_TIMER_INFO_SP() std::make_shared<TimerInfo>()

	class TimerObject : public std::enable_shared_from_this<TimerObject>
	{
	public:
		virtual ~TimerObject();
		void methodEvent(const size_t& eventType);

	protected:
		void addTimerEvent(const size_t& eventType, const TimerParam& method,
			const size_t& waitMs, const bool& isRepeat);
		void removeTimerEvent(const size_t& eventType);

	private:
		bool existInfo(size_t eventType);

	private:
		std::map<size_t, TimerInfoSP> timerMap;
	};
}