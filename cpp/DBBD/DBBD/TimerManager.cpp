#include "TimerManager.h"

void DBBD::TimerManager::init(size_t threadCount)
{
	this->threadCount = threadCount;

	for (size_t i = 0; i < threadCount; i++) {
		ThreadSP thread = NEW_THREAD_SP([this]() { update(); });
		threadIdTable[thread->get_id()] = i;
		threadList.push_back(thread);
	}

	isInit = true;
}

void DBBD::TimerManager::addTimer(TimerInfoWP infoPtr) {
	if (!isInit) {
		throw std::exception("TimerManager is not inited...");
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

void DBBD::TimerManager::update()
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
			if (!info) {
				it = deque->erase(it);
				continue;
			}

			if (!info->baseObject.lock()) {
				it = deque->erase(it);
				continue;
			}


			if (now < info->reservedTime) {
				it++;
				continue;
			}

			info->baseObject.lock()->methodEvent(info->type);
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

bool DBBD::TimerManager::waitCondition(std::thread::id threadId) {
	auto targetIndex = threadIdTable[threadId];
	return !processMap[targetIndex].empty();
}

size_t DBBD::TimerManager::getProcessNum() {
	size_t value = processNum++;
	if (processNum >= threadCount) {
		processNum = 0;
	}
	return value;
}