#ifndef __GLOBAL_FUNC_HPP__
#define __GLOBAL_FUNC_HPP__

namespace global {
	template<typename T>
	T clamp(const T a, const T max, const T min)
	{
		return a > max ? max : (a < min ? min : a);
	}
	template<typename T>
	int sign(T x) {
		return x < 0 ? -1 : (x > 0 ? 1 : 0);
	}
	template<typename T>
	int sign_positive(T x) {
		return x < 0 ? -1 : 1;
	}
	template<typename T>
	int sign_negative(T x) {
		return x > 0 ? 1 : -1;
	}
}

#endif // !__GLOBAL_FUNC_HPP__
