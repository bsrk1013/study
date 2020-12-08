#pragma once
#include <string>

namespace DBBD {
	class Buffer;
	class Cell;
	class Deserialize
	{
		// Value Read
	public:
		static void read(Buffer& buffer, short& value);
		static void read(Buffer& buffer, size_t& value);
		static void read(Buffer& buffer, std::string& value);
		static void read(Buffer& buffer, char* value);

		static void read(Buffer& buffer, Cell* value);

		// Array Read
	public:
		template<typename T>
		static void readArray(Buffer& buffer, T*& values);
		template<typename T1, typename T2>
		static void readVector(Buffer& buffer, T1& vec);

	private:
		template<typename T>
		static void read(Buffer& buffer, T& value);
	};
}