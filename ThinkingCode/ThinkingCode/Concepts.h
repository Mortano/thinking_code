#pragma once

//void_t as taken from Walter E. Brown, great stuff! Have to use a workaround because of the VS 2013 compiler

template<typename...>
struct _Voidifyer
{
   using type = void;
};

template<typename... Args>
using void_t = _Voidifyer<Args...>::type;