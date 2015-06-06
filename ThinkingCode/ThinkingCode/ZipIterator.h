#pragma once
#include "TupleHelper.h"
#include <tuple>

namespace Zip
{

#pragma region TemplateHelpers

   template<typename T>
   struct _GetIterator
   {
      using iterator = typename std::conditional< std::is_const<T>::value, 
                                                  typename std::remove_reference<T>::type::const_iterator, 
                                                  typename std::remove_reference<T>::type::iterator >::type;
   };

   //! \brief Pass-through function that can be used to expand a function call on all arguments of a parameter pack
   template<typename... T>
   inline void PassThrough( T&&... ) {}

#pragma endregion

#pragma region Zip

   template<typename... _Iters>
   struct _IterCollection
   {
      template<size_t Index>
      using value_type_t = typename std::tuple_element<Index, std::tuple<_Iters...>>::type::value_type;

      using value_ref_tuple_t = std::tuple<typename std::iterator_traits<_Iters>::reference...>;

      _IterCollection( _Iters&&... iterators ) :
         _iteratorPack( std::forward<_Iters>(iterators)... )
      {
      }

      inline bool MatchAny( const _IterCollection& other ) const
      {
         return AnyEqual( _iteratorPack, other._iteratorPack );
      }

      inline bool operator==( const _IterCollection& other ) const
      {
         return _iteratorPack == other._iteratorPack;
      }

      inline bool operator!=( const _IterCollection& other ) const
      {
         return !operator==( other );
      }

      inline value_ref_tuple_t Deref()
      {
         return DerefInternal( typename SequenceGenerator<sizeof...( _Iters )>::type() );
      }

      inline void Increment()
      {
         IncrementInternal( typename SequenceGenerator<sizeof...( _Iters )>::type() );
      }

   private:
      template<int... S>
      inline value_ref_tuple_t DerefInternal( Sequence<S...> )
      {
         return value_ref_tuple_t( *std::get<S>( _iteratorPack )... );
      }

      template<int... S>
      inline void IncrementInternal( Sequence<S...> )
      {
         PassThrough( std::get<S>( _iteratorPack ).operator++( )... );
      }

      std::tuple<_Iters...> _iteratorPack;
   };

   //! \brief An iterator that iterates over a range of collections simultaneously
   //!
   //! As such, it returns a tuple of elements at the current position when dereferenced. Since
   //! the collections might be of different lengths, this iterator stops when the first collection
   //! is exhausted
   template<typename... _Iters>
   class ZipIterator
   {
      using IterCollection_t = _IterCollection<_Iters...>;
      using value_ref_tuple_t = std::tuple<typename std::iterator_traits<_Iters>::reference...>;
   public:
      ZipIterator( IterCollection_t cur ) :
         _curIters( cur )
      {
      }

      inline value_ref_tuple_t operator*( )
      {
         return _curIters.Deref();
      }

      inline ZipIterator& operator++( )
      {
         _curIters.Increment();
         return *this;
      }

      inline bool operator==( const ZipIterator& other ) const
      {
         return _curIters.MatchAny( other._curIters ); //Again, for the comparison inside a range based for loop, one match is enough!
      }

      inline bool operator!=( const ZipIterator& other ) const
      {
         return !operator==( other );
      }

   private:
      IterCollection_t _curIters;
   };

   //! \brief 'Collection' that zips multiple iterators. This spawns the begin and end iterators
   template<typename... _Iters>
   class ZipCollection
   {
      using IterCollection_t = _IterCollection<_Iters...>;
   public:
      ZipCollection( IterCollection_t&& begins, IterCollection_t&& ends ) :
         _begins( std::forward<IterCollection_t>( begins ) ),
         _ends( std::forward<IterCollection_t>( ends ) )
      {
      }

      inline ZipIterator<_Iters...> begin( )
      {
         return ZipIterator<_Iters...>( _begins );
      }

      inline ZipIterator<_Iters...> end( )
      {
         return ZipIterator<_Iters...>(_ends );
      }
   private:
      IterCollection_t _begins;
      IterCollection_t _ends;
   };

   //! \brief Creates a zip iterator to iterator over a range of collections simultaneously
   //! \param args All the collections to iterator over
   //! \tparam Args Types of collections
   //! \returns A ZipIterator over all the collections
   template<typename... Args>
   ZipCollection<typename _GetIterator<Args>::iterator...> Zip( Args&&... args )
   {
      using IterCollection_t = _IterCollection<typename _GetIterator<Args>::iterator...>;
      return ZipCollection<typename _GetIterator<Args>::iterator...>( IterCollection_t( std::begin( args )... ),
                                                                      IterCollection_t( std::end(args)... ) );
   }

#pragma endregion

}
