#pragma once

#include <memory>

//Solving propositional logic problems using template metaprogramming

#ifdef SIMPLE

struct True
{
   static const bool Result = true;
};

struct False
{
   static const bool Result = false;
};

template<typename Exp>
struct Not
{
   static const bool Result = !Exp::Result;
};

template<typename Exp1, typename Exp2>
struct And
{
   static const bool Result = Exp1::Result && Exp2::Result;
};

template<typename Exp1, typename Exp2>
struct Or
{
   static const bool Result = Exp1::Result || Exp2::Result;
};

template<typename Exp1, typename Exp2>
struct Implies
{
   static const bool Result = !( Exp1::Result && ( !Exp2::Result ) );
};

template<typename Exp1, typename Exp2>
struct Equals
{
   static const bool Result = Exp1::Result == Exp2::Result;
};

#else

struct NoArg {};
struct OneArg {};
struct TwoArgs {};

template<typename Exp>
struct Not
{
   using NArgs = OneArg;
   using Arg1 = Exp;

   Not( Exp exp ) :
      exp( exp )
   {
   }

   Exp exp;

   bool operator()() const
   {
      return !exp();
   }
};

template<typename Exp1, typename Exp2>
struct And
{
   using NArgs = TwoArgs;
   using Arg1 = Exp1;
   using Arg2 = Exp2;

   And( Exp1 exp1, Exp2 exp2 ) :
      exp1( exp1 ),
      exp2( exp2 )
   {
   }

   Exp1 exp1;
   Exp2 exp2;

   bool operator()() const
   {
      return exp1() && exp2();
   }
};

template<typename Exp1, typename Exp2>
struct Or
{
   using NArgs = TwoArgs;
   using Arg1 = Exp1;
   using Arg2 = Exp2;

   Or( Exp1 exp1, Exp2 exp2 ) :
      exp1( exp1 ),
      exp2( exp2 )
   {
   }

   Exp1 exp1;
   Exp2 exp2;

   bool operator()( ) const
   {
      return exp1( ) || exp2( );
   }
};

template<typename Exp1, typename Exp2>
struct Implies
{
   using NArgs = TwoArgs;
   using Arg1 = Exp1;
   using Arg2 = Exp2;

   Implies( Exp1 exp1, Exp2 exp2 ) :
      exp1( exp1 ),
      exp2( exp2 )
   {
   }

   Exp1 exp1;
   Exp2 exp2;

   bool operator()( ) const
   {
      return !( exp1( ) && (!exp2( )) );
   }
};

template<typename Exp1, typename Exp2>
struct Equals
{
   using NArgs = TwoArgs;
   using Arg1 = Exp1;
   using Arg2 = Exp2;

   Equals( Exp1 exp1, Exp2 exp2 ) :
      exp1( exp1 ),
      exp2( exp2 )
   {
   }

   Exp1 exp1;
   Exp2 exp2;

   bool operator()( ) const
   {
      return exp1( ) == exp2( );
   }
};


struct Truth
{
   using NArgs = NoArg;

   Truth( bool val ) : val( val ) {}
   bool val;

   bool operator()() const
   {
      return val;
   }
};

template<typename Desired, typename... Args>
struct TypeArgMatching
{
   template<typename First,
            typename... Other,
            typename = typename std::enable_if<std::is_same<Desired, First>::value>::type>
   static Desired GetArg( First first, Other... args )
   {
      return first;
   }

   template<typename Ignore, 
            typename... CollapsedArgs>
   static typename std::enable_if<!std::is_same<Desired, Ignore>::value, Desired>::type GetArg( Ignore, CollapsedArgs... args )
   {
      //Recursively skip the first element and try GetArg again
      return GetArg( args... );
   }
};

template<typename Exp, typename... Truths>
struct BuildExpr
{
   template<typename A, typename... B> friend struct BuildExpr;
public:
   static Exp Build( Truths&&... args )
   {
      return DoBuild( std::forward<Truths>( args )..., Exp::NArgs( ) );
   }
private:
   static Exp DoBuild( Truths&&... args, NoArg )
   {
      return TypeArgMatching<Exp, Truths...>::GetArg( std::forward<Truths>( args )... );
   }

   static Exp DoBuild( Truths&&... args, OneArg )
   {
      return Exp( BuildExpr<Exp::Arg1, Truths...>::DoBuild( std::forward<Truths>( args )..., Exp::Arg1::NArgs( ) ) );
   }

   static Exp DoBuild( Truths&&... args, TwoArgs )
   {
      return Exp( 
         BuildExpr<Exp::Arg1, Truths...>::DoBuild( std::forward<Truths>( args )..., Exp::Arg1::NArgs( ) ),
         BuildExpr<Exp::Arg2, Truths...>::DoBuild( std::forward<Truths>( args )..., Exp::Arg2::NArgs( ) ) );
   }
};


struct A : public Truth
{
   A( bool val ) : Truth( val ) {}
};

struct B : public Truth
{
   B( bool val ) : Truth( val ) {}
};

struct C : public Truth
{
   C( bool val ) : Truth( val ) {}
};

enum class Validity
{
   Always,
   Never,
   Unknown
};

//TODO Use loops to create the 2^N different states required for checking validity!

template<typename Exp, typename Truth1>
Validity CheckValidity()
{
   bool result1 = BuildExpr<Exp, Truth1>::Build( true )( );
   bool result2 = BuildExpr<Exp, Truth1>::Build( false )( );

   if ( !result1 && !result2 ) return Validity::Never;
   if ( result1 && result2 ) return Validity::Always;
   return Validity::Unknown;
}

template<typename Exp, typename Truth1, typename Truth2>
Validity CheckValidity()
{
   bool r1 = BuildExpr<Exp, Truth1, Truth2>::Build( true, true )();

   bool r2 = BuildExpr<Exp, Truth1, Truth2>::Build( true, false )();
   if ( r2 != r1 ) return Validity::Unknown;

   bool r3 = BuildExpr<Exp, Truth1, Truth2>::Build( false, true )();
   if ( r3 != r1 ) return Validity::Unknown;

   bool r4 = BuildExpr<Exp, Truth1, Truth2>::Build( false, false )();
   if ( r4 != r1 ) return Validity::Unknown;

   return r1 ? Validity::Always : Validity::Never;
}

template<typename Exp, typename T1, typename T2, typename T3>
Validity CheckValidity()
{
   bool first = BuildExpr<Exp, T1, T2, T3>::Build( false, false, false )();
   for ( int i = 1; i < 8; i++ )
   {
      bool next = BuildExpr<Exp, T1, T2, T3>::Build( i / 2 != 0, i % 2 != 0, i % 4 != 0 )();
      if ( next != first ) return Validity::Unknown;
   }
   return first ? Validity::Always : Validity::Never;
}

void Test( )
{
   //This is the raw version
   auto result = BuildExpr<And<Not<A>, Or<A, Not<B>>>, A, B>::Build( false, true )();

   //But we can also check validity like this
   auto v1 = CheckValidity<Equals<Implies<A, B>, Or<A, Not<B>>>, A, B>();
   auto v2 = CheckValidity<Equals<Implies<A, B>, Implies<Not<A>, Not<B>>>, A, B>( );
   auto v3 = CheckValidity<Equals<Implies<A, B>, Implies<Not<B>, Not<A>>>, A, B>( );
   auto v4 = CheckValidity<Or<A, Or<B, Implies<A, B>>>, A, B>();
   auto v5 = CheckValidity<Equals<And<A, B>, Not<Or<Not<A>, Not<B>>>>, A, B>();

   auto v6 = CheckValidity<And<Implies<A, B>, And<Implies<B, C>, Implies<C, A>>>, A, B, C>();
   auto v7 = CheckValidity<And<Implies<A, B>, Not<Or<Not<A>, B>>>, A, B>();
   auto v8 = CheckValidity<Equals<And<Implies<A, B>, Implies<B, C>>, Implies<A, C>>, A, B, C>();

   int bla = 5;
}

#endif