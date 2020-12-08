#pragma once
#include <string>

namespace DBBD
{
	class Buffer;
	class Cell;
	class Serialize
	{
		// Value Write
	public:
		static void write(Buffer& buffer, const short& value);
		static void write(Buffer& buffer, const size_t& value);
		static void write(Buffer& buffer, const std::string& value);
		static void write(Buffer& buffer, char* const& value);

		static void write(Buffer& buffer, Cell* value);

		// Array Write
	public:
		template<typename T, size_t size>
		static void writeArray(Buffer& buffer, const T(&values)[size]);
		template<typename T1, typename T2>
		static void writeVector(Buffer& buffer, const T1& vec);

	private:
		template<typename T>
		static void write(Buffer& buffer, const T& value);
	};
}