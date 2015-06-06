#include "stdafx.h"
#include "CppUnitTest.h"
#include "TupleHelper.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ThinkingCode_Test
{
	TEST_CLASS(TupleHelperTest)
	{
	public:
		
      TEST_METHOD( TestAnyEqual )
      {
         //With one param
         {
            std::tuple<int> l{ 1 };
            std::tuple<int> r{ 1 };

            Assert::IsTrue( AnyEqual( l, r ), L"Expected true for two tuples with one equal parameter!" );

            std::get<0>( l ) = 2;

            Assert::IsFalse( AnyEqual( l, r ), L"Expected false for two tuples with one different parameter!" );
         }

         //With multiple params
         {
            std::tuple<int, std::string, float> l{ 12, "Arkhangelsk", 42.f };
            std::tuple<int, std::string, float> r{ 12, "Arkhangelsk", 42.f };

            //All equal
            Assert::IsTrue( AnyEqual( l, r ), L"Expected true for two tuples with three parameters that are equal!" );

            std::get<2>( l ) = 43.f;

            Assert::IsTrue( AnyEqual( l, r ), L"Expected true for two tuples with three parameters and two equal ones!" );

            std::get<1>( l ) = "Blablabla";

            Assert::IsTrue( AnyEqual( l, r ), L"Expected true for two tuples with three parameters and one equal parameter!" );

            std::get<0>( l ) = 13;

            Assert::IsFalse( AnyEqual( l, r ), L"Expected false for two tuples with three different parameters!" );
         }
		}

	};
}