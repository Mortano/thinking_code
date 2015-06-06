#include "stdafx.h"
#include "CppUnitTest.h"
#include "ZipIterator.h"

#include <vector>
#include <list>
#include <deque>
#include <array>
#include <map>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ThinkingCode_Test
{

	TEST_CLASS(ZipIteratorTest)
	{
	public:
		
      TEST_METHOD( TestWithOneCollection )
      {
         // Vector
         {
            std::vector<int> v1;

            for ( auto elem : Zip::Zip( v1 ) )
            {
               Assert::Fail( L"Zip over empty vector must not iterate!" );
            }

            v1.push_back( 42 );

            size_t index = 0;
            for ( auto elem : Zip::Zip( v1 ) )
            {
               Assert::AreEqual( v1[index], std::get<0>( elem ), L"Zip iterator returns wrong element when iterating over one vector!" );
               index++;
            }

            Assert::AreEqual( v1.size(), index, L"Wrong number of iterations with vector!" );

            index = 0;
            v1.clear();

            static const size_t ElementCount = 2048;
            v1.reserve( ElementCount );
            for ( size_t i = 0; i < ElementCount; i++ ) v1.push_back( i );

            for ( auto elem : Zip::Zip( v1 ) )
            {
               Assert::AreEqual( v1[index], std::get<0>( elem ), L"Zip iterator returns wrong element when iterating over one vector!" );
               index++;
            }

            Assert::AreEqual( v1.size(), index, L"Wrong number of iterations with vector!" );
         }

         // List
         {
            std::list<int> l1;

            for ( auto elem : Zip::Zip( l1 ) )
            {
               Assert::Fail( L"Zip over empty list must not iterate!" );
            }

            l1.push_back( 42 );

            size_t index = 0;
            auto listIter = l1.begin();
            for ( auto elem : Zip::Zip( l1 ) )
            {
               Assert::AreEqual( *listIter, std::get<0>( elem ), L"Zip iterator returns wrong element when iterating over one list!" );
               index++;
               listIter++;
            }

            Assert::AreEqual( l1.size( ), index, L"Wrong number of iterations with list!" );

            index = 0;
            l1.clear( );

            static const size_t ElementCount = 2048;
            for ( size_t i = 0; i < ElementCount; i++ ) l1.push_back( i );

            listIter = l1.begin();

            for ( auto elem : Zip::Zip( l1 ) )
            {
               Assert::AreEqual( *listIter, std::get<0>( elem ), L"Zip iterator returns wrong element when iterating over one list!" );
               index++;
               listIter++;
            }

            Assert::AreEqual( l1.size( ), index, L"Wrong number of iterations with vector!" );
         }
		}

      TEST_METHOD( TestMultipleCollectionsSameLength )
      {
         // Two collections of the same base type
         {
            std::vector<int> v1 = { 1, 2, 3 };
            std::vector<std::string> v2 = { "one", "two", "three" };

            size_t index = 0;
            for ( auto tuple : Zip::Zip( v1, v2 ) )
            {
               Assert::AreEqual( v1[index], std::get<0>( tuple ), L"Element of first vector not equal!" );
               Assert::AreEqual( v2[index], std::get<1>( tuple ), L"Element of second vector not equal!" );

               index++;
            }

            Assert::AreEqual( v1.size(), index, L"Wrong iteration count!" );
         }

         // Three collections of different types
         {
            std::vector<int> v1 = { 1, 2, 3, 4 };
            std::list<std::string> l1 = { "one", "two", "three", "four" };
            std::array<float, 4> a1 = { 1.f, 2.f, 3.f, 4.f };

            size_t index = 0;
            for ( auto tuple : Zip::Zip( v1, l1, a1 ) )
            {
               Assert::AreEqual( v1[index], std::get<0>( tuple ), L"Element of vector is wrong!" );
               Assert::AreEqual( a1[index], std::get<2>( tuple ), L"Element of array is wrong!" );

               switch ( index )
               {
               case 0:
                  Assert::AreEqual( std::string( "one" ), std::get<1>( tuple ), L"Element of list is wrong!" );
                  break;
               case 1:
                  Assert::AreEqual( std::string( "two" ), std::get<1>( tuple ), L"Element of list is wrong!" );
                  break;
               case 2:
                  Assert::AreEqual( std::string( "three" ), std::get<1>( tuple ), L"Element of list is wrong!" );
                  break;
               case 3:
                  Assert::AreEqual( std::string( "four" ), std::get<1>( tuple ), L"Element of list is wrong!" );
                  break;
               default:
                  Assert::Fail( L"Wrong index!" );
                  break;
               }

               index++;
            }

            Assert::AreEqual( v1.size(), index, L"Wrong iteration count!" );
         }

         // Empty collections
         {
            std::list<std::string> l1;
            std::deque<float> d1;

            for ( auto tuple : Zip::Zip( l1, d1 ) )
            {
               Assert::Fail( L"Iteration step over empty collections!" );
            }
         }
      }

      TEST_METHOD( TestMultipleCollectionsDifferentLength )
      {
         // Two vectors, different length
         {
            std::vector<int> v1 = { 1, 2, 3, 4, 5 };
            std::vector<std::string> v2 = { "one", "two" };

            size_t index = 0;
            for ( auto tuple : Zip::Zip( v1, v2 ) )
            {
               Assert::AreEqual( v1[index], std::get<0>( tuple ), L"Element of first vector is wrong!" );
               Assert::AreEqual( v2[index], std::get<1>( tuple ), L"Element of second vector is wrong!" );

               index++;
            }

            Assert::AreEqual( std::min( v1.size(), v2.size() ), index, L"Index should be equal to the size of the smallest collection!" );
         }

         // Different collections of different lengths
         {
            std::vector<int> v1 = { 1, 2, 3, 4, 5 };
            std::array<std::string, 4> a1 = { "one", "two", "three", "four" };
            std::map<int, float> m1 = { { 0, 0.f }, { 1, 1.f }, { 2, 2.f }, { 3, 3.f }, { 4, 4.f } };

            size_t index = 0;
            for ( auto tuple : Zip::Zip( v1, a1, m1 ) )
            {
               Assert::AreEqual( v1[index], std::get<0>( tuple ), L"Element of vector is wrong!" );
               Assert::AreEqual( a1[index], std::get<1>( tuple ), L"Element of array is wrong!" );
               Assert::AreEqual( static_cast<int>( index ), std::get<2>( tuple ).first, L"Element of map is wrong!" );

               index++;
            }

            Assert::AreEqual( std::min( v1.size(), std::min( a1.size(), m1.size() ) ), index, L"Index should be equal to the size of the smallest collection!" );
         }

         //Empty and non-empty collection
         {
            std::vector<int> nonEmpty = { 1, 2, 3 };
            std::list<std::string> empty;

            for ( auto tuple : Zip::Zip( nonEmpty, empty ) )
            {
               Assert::Fail( L"Iteration step over empty collection!" );
            }
         }
      }

	};
}