#pragma once
#include <queue>
#include <iostream>
#include <mutex>

namespace DBBD {
	template<typename T>
	class ObjectPoolManager {
	public:
		ObjectPoolManager() {}
		ObjectPoolManager(size_t initSize) {
			for (size_t i = 0; i < initSize; i++) {
				T object = new T;
				objectQueue.push(object);
			}
		}

		~ObjectPoolManager() {
			while (!objectQueue.empty()) {
				T object = objectQueue.front();
				objectQueue.pop();
				object->dispose();
				delete object;
			}
		}

	public:
		T pop() {
			if (objectQueue.empty()) {
				T object = new T;
				objectQueue.push(object);
			}

			T object = objectQueue.front();
			objectQueue.pop();
			return object;
		}

		void push(T object) {
			objectQueue.push(object);
		}

	private:
		ObjectPoolManager(ObjectPoolManager const&);
		ObjectPoolManager& operator=(ObjectPoolManager const&);

	private:
		std::queue<T> objectQueue;
	};

	template<typename T>
	class ObjectSPPoolManager {
	public:
		ObjectSPPoolManager() {
			maxObjectPoolCount = 16;
		}
		ObjectSPPoolManager(size_t initSize) {
			maxObjectPoolCount = 16;
			for (size_t i = 0; i < initSize; i++) {
				std::shared_ptr<T> object = std::make_shared<T>();
				objectQueue.push(object);
			}
		}

		~ObjectSPPoolManager() {
			while (!objectQueue.empty()) {
				std::shared_ptr<T> object = objectQueue.front();
				objectQueue.pop();
				object->dispose();
				object.reset();
			}
		}

	public:
		std::shared_ptr<T> pop() {
			if (objectQueue.empty()) {
				std::shared_ptr<T> object = std::make_shared<T>();
				objectQueue.push(object);
			}

			std::shared_ptr<T> object = objectQueue.front();
			objectQueue.pop();
			usedObjectCount++;
			return object;
		}

		void push(std::shared_ptr<T> object) {
			if (usedObjectCount >= maxObjectPoolCount) {
				usedObjectCount--;
				return;
			}
			objectQueue.push(object);
		}

	private:
		ObjectSPPoolManager(ObjectSPPoolManager const&) {}
		ObjectSPPoolManager& operator=(ObjectSPPoolManager const&) {}
		
	private:
		std::queue<std::shared_ptr<T>> objectQueue;
		size_t usedObjectCount;
		size_t maxObjectPoolCount;
	};
}