#pragma once
#include <iostream>
#include <map>
#include <boost/asio.hpp>
#include "Define.h"

//#include "boost/asio.hpp"

namespace DBBD {
	class TimerObject
	{
	public:
		TimerObject(IoContextSP context);
		virtual ~TimerObject();

	protected:
		void addTimerEvent(int eventType, TimerParam target, size_t waitMs);
		void removeTimerEvent(int eventType);

	private:
		void dispose();

	private:
		bool isDisposed = false;
		IoContextSP context;
		std::map<size_t, boost::asio::deadline_timer*> timerMap;
	};
}