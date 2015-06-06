#pragma once

#include <tuple>

namespace
{

   template<typename _Action, size_t N, typename... _TupleArgs>
   struct _ForEachInTupleHelper
   {
      static void Call( std::tuple<_TupleArgs...>& tuple, _Action&& action )
      {
         action.operator()<std::tuple<_TupleArgs...>, N>( tuple );
         _ForEachInTupleHelper<_Action, N - 1, _TupleArgs...>::Call( tuple, std::forward<_Action>( action ) ); // Recursive call for the next tuple element
      }
   };

   template<typename _Action, typename... _TupleArgs>
   struct _ForEachInTupleHelper<_Action, 0, _TupleArgs...>
   {
      static void Call( std::tuple<_TupleArgs...>& tuple, _Action&& action )
      {
         action.operator()<std::tuple<_TupleArgs...>, 0>( tuple );
      }
   };

}

//! \brief Calls a function for each element in the given tuple
//! \param tuple The tuple on which to call the function
//! \tparam _Action Function template that has to define operator() in a way that it is able to take
//!                 a parameter of each of the tuple types
//! \tparam _FuncArgs Additional arguments for the _Action functor
//! \tparam _FirstArg First argument of the tuple
//! \tparam _Rest Optional other tuple arguments
template<typename _Action, typename... _TupleArgs>
void ForEachInTuple( std::tuple<_TupleArgs...>& tuple, _Action&& action )
{
   //TODO Explicitly forbid empty tuples
   static_assert( sizeof...( _TupleArgs ) > 0, "Empty tuple is not allowed!" );
   _ForEachInTupleHelper<_Action, sizeof...( _TupleArgs ) - 1, _TupleArgs...>::Call( tuple, std::forward<_Action>( action ) );
}

namespace
{

   template<size_t N, typename _Tuple>
   struct _AnyEqualHelper
   {
      static bool Compare( const _Tuple& l, const _Tuple& r )
      {
         if ( std::get<N>( l ) == std::get<N>( r ) ) return true;
         return _AnyEqualHelper<N - 1, _Tuple>::Compare( l, r );
      }
   };

   template<typename _Tuple>
   struct _AnyEqualHelper<0, _Tuple>
   {
      static bool Compare( const _Tuple& l, const _Tuple& r )
      {
         return std::get<0>( l ) == std::get<0>( r );
      }
   };

}

//! \brief Returns true if any elements of the two tuples are equal (compares pair-wise, not the whole permutation)
//! \param l First tuple
//! \param r Second tuple
//! \returns True if there is at least one pair of elements that are equal
template<typename _First, typename... _Rest>
bool AnyEqual( const std::tuple<_First, _Rest...>& l, const std::tuple<_First, _Rest...>& r )
{
   return _AnyEqualHelper<sizeof...( _Rest ), std::tuple<_First, _Rest...>>::Compare( l, r );
}

#pragma region Sequence

template<int... S>
struct Sequence{};

template<int N, int... S>
struct SequenceGenerator : SequenceGenerator<N - 1, N - 1, S...> {};

template<int... S>
struct SequenceGenerator<0, S...>
{
   using type = Sequence<S...>;
};

#pragma endregion