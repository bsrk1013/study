#include "pch.h"
#include "CppUnitTest.h"
#include <iostream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace DBBD
{
	class Refl {
	private:

	};
}

namespace DBBDTest
{
	class Point {
	private:
		int x;
		int y;
	};

	TEST_CLASS(ReflectionTest)
	{
	public:
		TEST_METHOD(MemberTest) {
		}
	};
}