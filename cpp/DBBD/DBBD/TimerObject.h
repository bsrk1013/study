#pragma once
#include <iostream>
#include <map>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <mutex>
#include "Define.h"

//#include "boost/asio.hpp"
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

	class TimerObject : public std::enable_shared_from_this<TimerObject>
	{
	public:
		TimerObject(IoContextSP context);
		virtual ~TimerObject();

	public:
		void start();
		void stop();

	protected:
		virtual void registTimerEvent() = 0;
		void addTimerEvent(const size_t& eventType, const TimerParam& target,
			const size_t& waitMs, const bool& isRepeat);
		void removeTimerEvent(const size_t& eventType);

	private:
		void methodEvent(const boost::system::error_code& error,
			std::weak_ptr<TimerObject> weakPtr, const size_t& eventType);
		void dispose();

	private:
		bool existInfo(size_t eventType);

	private:
		bool isDisposed = false;
		boost::mutex lockObject;
		//std::mutex lockObject;
		IoContextSP context;
		std::map<size_t, TimerInfoSP> timerMap;
	};
}