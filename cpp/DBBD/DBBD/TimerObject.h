#pragma once
#include <iostream>
#include <map>
#include "Define.h"
#include "boost/asio.hpp"

namespace DBBD {
	class TimerObject
	{
	public:
		TimerObject(IoContextSP context);

	protected:
		void addTimerEvent(int eventType, TimerParam target, size_t waitMs);
		void removeTimerEvent(int eventType);

	private:
		IoContextSP context;
		std::map<size_t, boost::asio::deadline_timer*> timerMap;
	};
}