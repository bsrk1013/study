#include "Serialize.h"
#include "Common.hpp"

namespace DBBD {
	void Serialize::write(Buffer& buffer, const std::string& value) {
		unsigned int strSize = (unsigned int)value.length();
		write(buffer, strSize);
		for (unsigned int i = 0; i < strSize; i++) {
			char data = value[i];
			buffer.putByte(data);
		}
	}

	void Serialize::write(Buffer& buffer, const std::wstring& value) {
		std::string utf8 = uniToUtf8(value);
		write(buffer, utf8);
	}

	void Serialize::write(Buffer& buffer, char* const& value) {
		unsigned int strSize = (unsigned int)strlen(value);
		write(buffer, strSize);
		for (unsigned int i = 0; i < strSize; i++) {
			char data = value[i];
			buffer.putByte(data);
		}
	}

	void Serialize::write(Buffer& buffer, Cell* value) {
		value->serialize(buffer);
	}

	template<typename T1, typename T2>
	void Serialize::writeVector(Buffer& buffer, const T1& vec) {
		unsigned int vecSize = vec.size();
		write(buffer, vecSize);
		for (auto it = vec.begin(); it != vec.end(); it++) {
			T2 data = *it;
			write(buffer, data);
		}
	}
}