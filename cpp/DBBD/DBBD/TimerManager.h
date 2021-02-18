#pragma once
#include <thread>
#include <vector>
#include <deque>
#include <map>
#include "Singleton.h"
#include "TimerObject.h"
#include "Define.h"

namespace DBBD
{
	class TimerManager : public Singleton<TimerManager>
	{
	public:
		void init(size_t threadCount);
		virtual void release() override;
		void addTimer(TimerInfoWP);

	private:
		void update();
		bool waitCondition(std::thread::id);
		size_t getProcessNum();

	private:
		bool isInit = false;
		size_t threadCount;
		std::atomic<size_t> processNum;
		std::vector<ThreadSP> threadList;
		std::map<std::thread::id, size_t> threadIdTable;
		std::map<size_t, std::deque<TimerInfoWP>> processMap;
		std::condition_variable conditionVariable;
		std::mutex lockObject;
	};
}