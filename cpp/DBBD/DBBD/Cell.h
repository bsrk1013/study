#pragma once
namespace DBBD {
	class Serialize;
	class Deserialize;
	class Cell
	{
	public:
		virtual ~Cell() {};

	public:
		virtual void serialize(Buffer* buffer/*, Serialize* serialize*/) = 0;
		virtual void deserialize(Buffer* buffer/*, Deserialize* deserialize*/) = 0;
		//virtual size_t getSize() = 0;
	};
}