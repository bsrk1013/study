#pragma once
#include <iostream>
#include <map>
#include <boost/asio.hpp>
#include <mutex>
#include "Define.h"

//#include "boost/asio.hpp"
namespace DBBD {
	struct TimerInfo {
		TimerInfo() {
			memset(this, 0, sizeof(TimerInfo));
		}
		size_t type;
		TimerSP timer;
		TimerParam method;
		size_t waitMs;
		bool isRepeat;
	};

	class TimerObject
	{
	public:
		TimerObject(IoContextSP context);
		virtual ~TimerObject();

	protected:
		virtual void registTimerEvent() = 0;
		void addTimerEvent(const size_t& eventType, const TimerParam& target,
			const size_t& waitMs, const bool& isRepeat);
		void removeTimerEvent(const size_t& eventType);

	private:
		void init();
		void methodEvent(const boost::system::error_code& error,
			const size_t& eventType);
		void dispose();

	private:
		bool existInfo(size_t eventType);

	private:
		bool isDisposed = false;
		IoContextSP context;
		std::map<size_t, TimerInfo> timerMap;
	};
}