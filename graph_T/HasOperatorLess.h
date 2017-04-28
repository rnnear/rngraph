// judge if a class has reloaded operator< or not
#ifndef HASOPERATORLESS_H
#define HASOPERATORLESS_H
template<typename T>
struct HasOperatorLess
{
	//template<typename T0, bool (T0::*)(const T0&)> struct temp1;
	//template<typename T1> static char check(temp1<T1, &T1::operator<> *);
	template<typename T1> static char check(decltype(T1() < T1())*);
	template<typename T2> static int check(...);
	static constexpr bool value = (sizeof check<T>(0) == sizeof(char));
};
#endif
