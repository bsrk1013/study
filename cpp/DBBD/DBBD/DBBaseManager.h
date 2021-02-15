#pragma once
#include <string>
#include <set>
#include <iostream>
#include <mutex>

namespace DBBD
{
	template <typename T>
	class DBBaseManager
	{
	protected:
		T getInfo()
		{
			if (!isInit) {
				std::string error(name + " is not inited");
				throw std::exception(error.c_str());
			}

			T info;
			{
				std::lock_guard<std::mutex> lock(lockObject);
				for (auto tempInfo : infoSet) {
					info = tempInfo;
					break;
				}

				if (info) {
					infoSet.erase(info);
					info->usedTime = std::chrono::system_clock::now();
				}
			}

			if (!info) {
				info = createInfo();
			}

			refreshInfo();

			return info;
		}

		void putInfo(T info)
		{
			std::scoped_lock<std::mutex> lock(lockObject);
			infoSet.insert(info);
			std::cout << "Try putInfo" << std::endl;
		}

		void refreshInfo()
		{
			// XXX
			auto now = std::chrono::system_clock::now();

			std::scoped_lock<std::mutex> lock(lockObject);
			if (infoSet.size() <= maxConnCount) { return; }
			for (auto iter = infoSet.begin(); iter != infoSet.end();) {
				auto info = *iter;
				std::chrono::duration elapsed = now - info->usedTime;
				auto elapsedSeconds = std::chrono::duration_cast<std::chrono::seconds>(elapsed);
				if (elapsedSeconds.count() >= 60 * 30) {
					closeInfoInternal(info);
					iter = infoSet.erase(iter);
				}
				else {
					iter++;
				}
			}
		}

		virtual T createInfo() = 0;
		virtual void closeInfoInternal(T info) = 0;

	private:
		struct ConnOrder {
			bool operator()(T r1, T r2) const {
				return r1->usedTime > r2->usedTime;
			}
		};

	public:
		std::set<T, ConnOrder> infoSet;

	protected:
		bool isInit = false;
		std::string name;
		short maxConnCount;
	
	private:
		std::mutex lockObject;
	};
}