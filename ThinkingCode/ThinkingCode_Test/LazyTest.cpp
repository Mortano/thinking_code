#include "stdafx.h"
#include "CppUnitTest.h"
#include "Lazy.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ThinkingCode_Test
{		

	TEST_CLASS(LazyTest)
	{
	public:
		
		TEST_METHOD(TestFilter)
		{
         //With empty range
         {
            std::vector<int> vec;

            auto filtered = Lazy::MakeLazy(vec).Filter([] (const int&){return true;});

            for ( auto elem : filtered ) Assert::Fail(L"Filter with empty range should not have elements!");
         }

         //With some elements
         {
            std::vector<int> vec = {1,2,3,4,5,6,7,8,9};

            auto passAlways = Lazy::MakeLazy( vec ).Filter( [] ( const int& ){return true; } );
            auto passAlwaysVec = passAlways.ToVector(); //If the tests pass with ToVector, the also pass with range-based for, this is the same

            Assert::AreEqual(vec.size(), passAlwaysVec.size(), L"Pass-always filter does not return the same elements!");
            for ( size_t i = 0; i < vec.size(); i++ )
            {
               Assert::AreEqual(vec[i], passAlwaysVec[i], L"Pass-always filter does not return the same elements!");
            }

            auto passOdd = Lazy::MakeLazy(vec).Filter([] (const int& val) {return ( val & 1 ) != 0;});
            auto passOddVec = passOdd.ToVector();

            Assert::IsTrue( passOddVec.size() == 5, L"Filter not working!" );
            Assert::IsTrue( passOddVec[0] == 1, L"Filter returning wrong element!" );
            Assert::IsTrue( passOddVec[1] == 3, L"Filter returning wrong element!" );
            Assert::IsTrue( passOddVec[2] == 5, L"Filter returning wrong element!" );
            Assert::IsTrue( passOddVec[3] == 7, L"Filter returning wrong element!" );
            Assert::IsTrue( passOddVec[4] == 9, L"Filter returning wrong element!" );
         }

         //Never pass
         {
            std::vector<int> vec = {1,2,3,4,5,6,7,8,9};

            auto passNever = Lazy::MakeLazy(vec).Filter([] (const int&){return false;});
            auto passNeverVec = passNever.ToVector();

            Assert::IsTrue(passNeverVec.size() == 0, L"Pass-never filter not working!");
         }
		}

      TEST_METHOD( TestMap )
      {
         //With empty range
         {
            std::vector<int> vec;

            auto map = Lazy::MakeLazy(vec).Map([] (const int&){ return 0; });
            auto mapVec = map.ToVector();

            Assert::IsTrue(mapVec.size() == 0, L"Map with empty range not working!");
         }

         //With some elements
         {
            std::vector<int> vec = {1,2,3,4};

            auto mapDouble = Lazy::MakeLazy(vec).Map([] (const int& val){ return val * 2;});
            auto mapDoubleVec = mapDouble.ToVector();

            Assert::IsTrue(mapDoubleVec.size() == vec.size(), L"Map does not preserve the size of the range!");
            Assert::IsTrue( mapDoubleVec[0] == 2, L"Map not working!" );
            Assert::IsTrue( mapDoubleVec[1] == 4, L"Map not working!" );
            Assert::IsTrue( mapDoubleVec[2] == 6, L"Map not working!" );
            Assert::IsTrue( mapDoubleVec[3] == 8, L"Map not working!" );

            auto mapCrossType = Lazy::MakeLazy(vec).Map<double>([] (const int& val) { return val * 1.0; });
            auto mapCrossTypeVec = mapCrossType.ToVector();

            Assert::IsTrue(mapCrossTypeVec.size() == vec.size(), L"Cross-type map does not preserve the size of the range!");
         }
      }

      TEST_METHOD( TestLimit )
      {
         //With empty range
         {
            std::vector<int> vec;

            auto limit = Lazy::MakeLazy(vec).Limit(5);
            auto limitVec = limit.ToVector();

            Assert::IsTrue(limitVec.size() == 0, L"Limit with empty range not working!");
         }

         //With some elements
         {
            std::vector<int> vec = {1,2,3,4,5,6,7,8,9};

            auto limitLow = Lazy::MakeLazy(vec).Limit(2);
            auto limitLowVec = limitLow.ToVector();

            Assert::IsTrue(limitLowVec.size() == 2, L"Limit not working!");
            Assert::IsTrue(limitLowVec[0] == vec[0], L"Limit is not preserving the order!");
            Assert::IsTrue(limitLowVec[1] == vec[1], L"Limit is not preserving the order!");

            //Also test a high limit!
            auto limitHigh = Lazy::MakeLazy(vec).Limit(100);
            auto limitHighVec = limitHigh.ToVector();

            Assert::IsTrue(limitHighVec.size() == vec.size(), L"Limit with size > range size not working!");
         }
      }

	};
}