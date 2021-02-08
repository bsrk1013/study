#include "Serialize.h"

namespace DBBD {
	void Serialize::write(Buffer& buffer, const std::string& value) {
		size_t strSize = value.length();
		write(buffer, strSize);
		for (size_t i = 0; i < strSize; i++) {
			char data = value[i];
			buffer.putByte(data);
		}
	}

	void Serialize::write(Buffer& buffer, char* const& value) {
		size_t strSize = strlen(value);
		write(buffer, strSize);
		for (size_t i = 0; i < strSize; i++) {
			char data = value[i];
			buffer.putByte(data);
		}
	}

	void Serialize::write(Buffer& buffer, Cell* value) {
		value->serialize(buffer);
	}

	template<typename T1, typename T2>
	void Serialize::writeVector(Buffer& buffer, const T1& vec) {
		size_t vecSize = vec.size();
		write(buffer, vecSize);
		for (auto it = vec.begin(); it != vec.end(); it++) {
			T2 data = *it;
			write(buffer, data);
		}
	}
}