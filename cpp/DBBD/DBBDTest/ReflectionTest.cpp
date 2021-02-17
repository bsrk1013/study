#include "pch.h"
#include "CppUnitTest.h"
#include "DBBD/refl.hpp"
#include <iostream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace DBBDTest
{
	struct User
	{
		int id() const;
		void id(const int& value);

		const std::string& email() const;
		void email(const std::string& value);
	};

	TEST_CLASS(ReflectionTest)
	{
	public:
		TEST_METHOD(MemberTest) {
		}
	};
}