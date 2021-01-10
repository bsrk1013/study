#pragma once
#include <iostream>
#include <map>
#include <boost/asio.hpp>
#include "Define.h"
#include "BaseObject.h"

namespace DBBD {
	struct TimerInfo {
		TimerInfo() {
			memset(this, 0, sizeof(TimerInfo));
		}
		~TimerInfo() {
			if (timer) {
				std::cout << "timer reset" << std::endl;
				timer.reset();
			}
		}
		size_t type;
		TimerSP timer;
		TimerParam method;
		size_t waitMs;
		bool isRepeat;
	};

	using TimerInfoSP = std::shared_ptr<TimerInfo>;

	class TimerObject
		: public BaseObject// : public std::enable_shared_from_this<TimerObject>
	{
	public:
		TimerObject();
		virtual ~TimerObject();

	public:
		virtual void init(IoContextSP context);
		virtual void dispose();
		virtual void reset();

	protected:
		virtual void registTimerEvent() = 0;
		void addTimerEvent(const size_t& eventType, const TimerParam& target,
			const size_t& waitMs, const bool& isRepeat);
		void removeTimerEvent(const size_t& eventType);

	private:
		void methodEvent(const boost::system::error_code& error,
			std::weak_ptr<BaseObject> weakPtr, const size_t& eventType);

	private:
		bool existInfo(size_t eventType);

	private:
		std::map<size_t, TimerInfoSP> timerMap;
	};
}