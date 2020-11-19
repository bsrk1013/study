#pragma once
namespace DBBD {
	class Serialize;
	class Deserialize;
	class Cell
	{
	public:
		virtual ~Cell() = 0;

	public:
		virtual void serialize(Buffer* buffer) = 0;
		virtual void deserialize(Buffer* buffer) = 0;
		virtual size_t getLength() = 0;
	};
}