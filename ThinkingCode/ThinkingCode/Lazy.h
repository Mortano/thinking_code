#pragma once

#include <functional>
#include <iterator>
#include <vector>

namespace Lazy
{

   template<typename _Val> using _Pred = std::function<bool( const _Val& )>;
   template<typename _Src, typename _Dst> using _Map = std::function<_Dst( const _Src& )>;

   template<typename T>
   struct Optional
   {
      static Optional False()
      {
         return Optional();
      }

      static Optional True( T&& val )
      {
         return Optional( std::move( val ) );
      }

      static Optional True( const T& val )
      {
         return Optional( val );
      }

      Optional( const Optional& ) = default;
      Optional& operator=( const Optional& ) = default;

      operator bool() const
      {
         return exists;
      }

      T val;
      bool exists;
   private:
      Optional() : exists( false ) {}
      Optional( const T& val ) : exists( true ), val( val ) {}
      Optional( T&& val ) : exists( true ), val( std::move(val) ) {}
   };

#pragma region LazyOperations

   //! \brief Lazy iterator that implements a filter operation
   //! \tparam _ValType Value type of the iterator
   //! \tparam _Iter Type of the nested iterator
   template<typename _ValType,
            typename _Iter>
   class LazyFilter : public std::iterator<std::forward_iterator_tag, _ValType>
   {
   public:
      using _IterType = LazyFilter;

      LazyFilter( _Iter start,
                  _Iter end, 
                  _Iter cur,
                  const _Pred<_ValType>& pred ) :
         _begin( start ),
         _end( end ),
         _cur( cur ),
         _pred( pred )
      {
      }

      LazyFilter( const LazyFilter& other ) :
         _begin( other._begin ),
         _end( other._end ),
         _cur( other._cur ),
         _pred( other._pred )
      {
      }

      LazyFilter& operator++(  )
      {
         //Filter skips elements, so we always increment to the next element that matches the predicate
         //or to the end
         if ( _cur == _end ) return *this;
         //Skip all elements that don't match the predicate
         do
         {
            ++_cur;
         }
         while ( _cur != _end && 
                  !_pred( *_cur ) );
         return *this;
      }

      _ValType operator*( ) const
      {
         if ( _cur == _end ) throw std::exception( "Dereferencing end iterator!" );
         return *_cur;
      }

      bool operator==( const LazyFilter& other ) const
      {
         return _cur == other._cur &&
                  _begin == other._begin &&
                  _end == other._end; //TODO Ideally we would also compare the predicate
      }
      
      bool operator!=( const LazyFilter& other ) const
      {
         return !operator==( other );
      }

      bool IsAtEnd() const
      {
         return _cur == _end;
      }

      LazyFilter begin( ) const
      {
         //Move to the first valid element
         auto begin = _begin;
         while ( begin != _end &&
                  !_pred( *begin ) )
         {
            ++begin;
         }
         return LazyFilter( _begin, _end, begin, _pred );
      }

      LazyFilter end( ) const
      {
         return LazyFilter( _begin, _end, _end, _pred );
      }
   private:
      const _Iter _begin;
      const _Iter _end;
      _Iter _cur;
      const _Pred<_ValType> _pred;
   };

   //! \brief Lazy iterator that implements a map operation
   //! \tparam _SrcType Source type of the map operation
   //! \tparam _DstType Destination type of the map operation
   //! \tparam _Iter Type of the nested iterator
   template<typename _SrcType,
            typename _DstType,
            typename _Iter>
   class LazyMap : public std::iterator<std::forward_iterator_tag, _DstType>
   {
   public:
      using _IterType = LazyMap;

      LazyMap( _Iter begin, 
               _Iter end, 
               _Iter cur, 
               const _Map<_SrcType, _DstType>& map ) :
         _begin( begin ),
         _end( end ),
         _cur( cur ),
         _map( map )
      {
      }

      LazyMap( const LazyMap& other ) :
         _begin( other._begin ),
         _end( other._end ),
         _cur( other._cur ),
         _map( other._map )
      {
      }

      LazyMap& operator++( )
      {
         //Map does not skip elements, so the increment operator is simple
         if ( _cur != _end ) ++_cur;
         return *this;
      }

      _DstType operator*( ) const
      {
         if ( _cur == _end ) throw std::exception( "Dereferencing end iterator!" );
         return _map( *_cur );
      }

      bool operator==( const LazyMap& other ) const
      {
         return _begin == other._begin && 
                _end == other._end && 
                _cur == other._cur;
      }

      bool operator!=( const LazyMap& other ) const
      {
         return !operator==( other );
      }

      bool IsAtEnd() const
      {
         return _cur == _end;
      }

      LazyMap begin() const
      {
         return LazyMap( _begin, _end, _begin, _map );
      }

      LazyMap end() const
      {
         return LazyMap( _begin, _end, _end, _map );
      }
   private:
      const _Iter _begin;
      const _Iter _end;
      _Iter _cur;
      const _Map<_SrcType, _DstType> _map;
   };

   //! \brief Lazy iterator that is limited to a specific number of elements
   //! \tparam _ValType Value type of the iterator
   //! \tparam _Iter Type of the nested iterator
   template<typename _ValType,
            typename _Iter>
   class LazyLimit : public std::iterator<std::forward_iterator_tag, _ValType>
   {
   public:
      using _IterType = LazyLimit;

      LazyLimit( _Iter begin, _Iter end, _Iter cur, size_t idx, size_t limit ) :
         _begin( begin ),
         _end( end ),
         _cur( cur ),
         _index( idx ),
         _limit( limit )
      {
      }

      LazyLimit(const LazyLimit& other) = default;
      LazyLimit& operator=( const LazyLimit& ) = default;

      _ValType operator*( ) const
      {
         if ( _index == _limit ||
              _cur == _end )
         {
            throw std::exception("Dereferencing end iterator!");
         }
         return *_cur;
      }

      LazyLimit& operator++( )
      {
         if ( _index != _limit &&
              _cur != _end )
         {
            ++_cur;
            ++_index;
            if ( _cur == _end )
            {
               _index = _limit;
            }
            else if ( _index == _limit )
            {
               while ( _cur != _end ) ++_cur; //So that comparison with end() works
            }
         }
         return *this;
      }

      bool operator==( const LazyLimit& other ) const
      {
         return _begin == other._begin && 
                _end == other._end &&
                _cur == other._cur &&
                _index == other._index &&
                _limit == other._limit;
      }

      bool operator!=( const LazyLimit& other ) const
      {
         return !operator==( other );
      }

      LazyLimit begin() const
      {
         size_t startIdx = 0;
         if ( _begin == _end ) startIdx = _limit; //To prevent a crash with empty ranges
         return LazyLimit(_begin, _end, _begin, startIdx, _limit);
      }

      LazyLimit end() const
      {
         //This is the end of the unlimited range, however once the limit is reached on a range, the _cur
         //iterator is set to _end, thus making this work
         return LazyLimit(_begin, _end, _end, _limit, _limit);
      }
   private:
      _Iter _begin;
      _Iter _end;
      _Iter _cur;
      size_t _index;
      size_t _limit;
   };

#pragma endregion

#pragma region LazyRanges

   //! \brief Range for a common container
   template<typename _Cont, typename _ValType = typename _Cont::value_type, typename _Iter = typename _Cont::const_iterator>
   class ContainerRange : public std::iterator<std::forward_iterator_tag, _ValType>
   {
   public:
      using _IterType = _Iter;

      ContainerRange( const _Cont& cont ) :
         _begin( std::begin( cont ) ),
         _end( std::end( cont ) ),
         _cur( std::begin( cont ) )
      {
      }

      ContainerRange( const ContainerRange& other ) = default;
      ContainerRange& operator=( const ContainerRange& other ) = default;

      ContainerRange& operator++( )
      {
         if ( _cur != _end ) ++_cur;
         return *this;
      }

      const _ValType& operator*( ) const
      {
         if ( _cur == _end ) throw std::exception( "Dereferencing end iterator!" );
         return *_cur;
      }

      bool operator==( const ContainerRange& other ) const
      {
         return _begin == other._begin && _end == other._end && _cur == other._cur;
      }

      bool operator!=( const ContainerRange& other ) const
      {
         return operator==( other );
      }

      _Iter begin() const
      {
         return _begin;
      }

      _Iter end() const
      {
         return _end;
      }

      bool IsAtEnd() const
      {
         return _cur == _end;
      }
   private:
      _Iter _begin;
      _Iter _end;
      _Iter _cur;
   };

   template<typename _ValType,
            typename _Range,
            typename _Iter = typename _Range::_IterType>
   class LazyRange : public std::iterator<std::forward_iterator_tag, _ValType>
   {
   public:
      using _ThisType = LazyRange;

      LazyRange( _Range range ) :
         _range( range )
      {
      }

      LazyRange( const _ThisType& other ) :
         _range( other._range )
      {
      }

      _ThisType& operator=( const _ThisType& other )
      {
         _range = other._range;
      }

      _ThisType& operator++( )
      {
         ++_range;
         return *this;
      }

      _ValType operator*( ) const
      {
         return *_range;
      }

      bool operator==( const _ThisType& other ) const
      {
         return _range == other._range;
      }

      bool operator!=( const _ThisType& other ) const
      {
         return !operator==( other );
      }

      _Iter begin( ) const
      {
         return std::begin( _range ); 
      }
      
      _Iter end( ) const
      {
         return std::end( _range );
      }

      //The transformation methods

      //! \brief Apply a map operation to this range
      //! \param map The map function
      //! \returns A LazyRange with the mapping operation applied
      //! \tparam _Dst The destination type of the map operation
      template<typename _Dst>
      LazyRange<_Dst, LazyMap<_ValType, _Dst, _Iter>> Map( const _Map<_ValType, _Dst>& map ) const
      {
         using _MapType = LazyMap<_ValType, _Dst, _Iter>;
         using _Ret = LazyRange<_Dst, _MapType>;

         auto iter = _MapType( std::begin( _range ), std::end( _range ), std::begin( _range ), map );
         return _Ret( iter );
      }

      //! \brief Apply a map operation to this range
      //! \param map The map function
      //! \returns A LazyRange with the mapping operation applied
      LazyRange<_ValType, LazyMap<_ValType, _ValType, _Iter>> Map( const _Map<_ValType, _ValType>& map ) const
      {
         using _MapType = LazyMap<_ValType, _ValType, _Iter>;
         using _Ret = LazyRange<_ValType, _MapType>;

         auto iter = _MapType( std::begin( _range ), std::end( _range ), std::begin( _range ), map );
         return _Ret( iter );
      }

      //! \brief Apply a filter operation to this range
      //! \param pred Predicate for the filter operation
      //! \returns A LazyRange with the filter operation applied
      LazyRange<_ValType, LazyFilter<_ValType, _Iter>> Filter( const _Pred<_ValType>& pred ) const
      {
         using _FilterType = LazyFilter<_ValType, _Iter>;
         using _Ret = LazyRange<_ValType, _FilterType>;

         auto iter = _FilterType( std::begin( _range ), std::end( _range ), std::begin( _range ), pred );
         return _Ret( iter );
      }

      //! \brief Returns the first element in this range if it exists
      //! \returns Optional that contains the first element in this range if it exists
      Optional<_ValType> First() const
      {
         for ( auto val : _range )
         {
            return Optional<_ValType>::True( val );
         }
         return Optional<_ValType>::False();
      }

      //! \brief Apply a limit to this range
      //!
      //! A limit, as the name suggests, limits the maximum number of elements in a range to 
      //! a specific number. The actual number of elements in the range may be lower than the
      //! limit however.
      //! \param limit The maximum number of elements 
      //! \returns A LazyRange with the limit applied
      LazyRange<_ValType, LazyLimit<_ValType, _Iter>> Limit( size_t limit ) const
      {
         using _LimitType = LazyLimit<_ValType, _Iter>;
         using _Ret = LazyRange<_ValType, _LimitType>;
         
         auto iter = _LimitType(std::begin(_range), std::end(_range), std::begin(_range), 0, limit);
         return _Ret( iter );
      }

      //Container conversion methods

      //! \brief Converts this range to a vector, thus evaluating all elements in the range
      //! \returns The elements of this range after evaluation, stored in a vector
      std::vector<_ValType> ToVector() const
      {
         std::vector<_ValType> ret;
         for ( auto elem : _range )
         {
            ret.push_back( elem );
         }
         return std::move( ret );
      }
   private:
      _Range _range;
   };

#pragma endregion

#pragma region MakeFunction

   //! \brief Returns a LazyRange based on the given container
   //! \param container Any container that supports iterators
   //! \returns A LazyRange of the given container
   //! \tparam _Cont The container type
   template<typename _Cont,
            typename _ValType = typename _Cont::value_type,
            typename _IterType = typename _Cont::const_iterator>
   LazyRange<_ValType, ContainerRange<_Cont>> MakeLazy( const _Cont& container )
   {
      using _Range = ContainerRange<_Cont>;
      using _Ret = LazyRange<_ValType, _Range>;
      return _Ret( _Range( container ) );
   }

#pragma endregion

}