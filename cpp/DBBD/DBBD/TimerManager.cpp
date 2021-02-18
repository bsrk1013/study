#include "TimerManager.h"
#include "Log.h"

namespace DBBD
{
	void TimerManager::init(size_t threadCount)
	{
		this->threadCount = threadCount;

		for (size_t i = 0; i < threadCount; i++) {
			ThreadSP thread = NEW_THREAD_SP([this]() { update(); });
			threadIdTable[thread->get_id()] = i;
			threadList.push_back(thread);
		}

		isInit = true;
		LOG_INFO("TimerManager init, threadCount: {}", threadCount);
	}

	void TimerManager::release()
	{
	}

	void TimerManager::addTimer(TimerInfoWP infoPtr) {
		if (!isInit) {
			throw std::exception("TimerManager is not inited");
		}

		{
			std::lock_guard<std::mutex> lock(lockObject);

			auto info = infoPtr.lock();
			if (!info) { return; }


			auto deque = &processMap[getProcessNum()];
			deque->push_back(infoPtr);
		}

		conditionVariable.notify_one();
	}

	void TimerManager::update()
	{
		while (true) {
			auto now = std::chrono::system_clock::now();

			std::unique_lock<std::mutex> lock(lockObject);
			conditionVariable.wait(lock, [this]() {return this->waitCondition(std::this_thread::get_id()); });

			auto threadId = std::this_thread::get_id();
			auto dequeIndex = threadIdTable[threadId];
			auto deque = &processMap[dequeIndex];
			lock.unlock();

			auto it = deque->begin();
			while (it != deque->end()) {
				auto info = it->lock();
				// info가 소멸했으므로 deque에서 없에주자
				if (!info) {
					it = deque->erase(it);
					continue;
				}

				if (now < info->reservedTime) {
					it++;
					continue;
				}

				auto timer = info->baseObject.lock();
				if (!timer) {
					it = deque->erase(it);
					continue;
				}

				timer->methodEvent(info->type);
				//info->method();

				it = deque->erase(it);
				// FIXME 로직 구현
				/*if (now < it->reservedTime) {
					it++;
					continue;
				}*/

				//it = deque->erase(it);
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}

	bool TimerManager::waitCondition(std::thread::id threadId) {
		auto targetIndex = threadIdTable[threadId];
		return !processMap[targetIndex].empty();
	}

	size_t TimerManager::getProcessNum() {
		size_t value = processNum++;
		if (processNum >= threadCount) {
			processNum = 0;
		}
		return value;
	}
}