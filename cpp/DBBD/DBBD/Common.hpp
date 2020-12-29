#pragma once
#include <iostream>
#include <Windows.h>

namespace DBBD {
	template <typename T>
	size_t GetPacketLength(T value) {
		return sizeof(T);
	}

	template <typename T>
	size_t GetPacketLength() {
		return sizeof(T);
	}

	size_t GetPacketLength(std::string value) {
		return sizeof(size_t) + value.size();
	}

	size_t GetPacketLength(char* value) {
		return sizeof(size_t) + strlen(value);
	}
}