/*
 * callbackmanager.h
 *
 *  Created on: 16 jun. 2024
 *      Author: jancu
 */

#ifndef CALLBACKMANAGER_H_
#define CALLBACKMANAGER_H_

#include <functional>

template <typename R, typename... Args>
// restrict to arithmetic data types for return value, or void
#ifdef __GNUC__ // this requires a recent version of GCC.
#if __GNUC_PREREQ(10,0)
  requires std::is_void<R>::value || std::is_arithmetic_v<R>
#endif
#endif

class Callback {
public:
	Callback() : callback_(nullptr){}

	inline void set(std::function<R(Args... args)> callback) {
	    callback_ = & callback;
	}

	inline void unset() {
	    callback_ = nullptr;
	}

	/*
	 * R can either be an arithmetic type, or void
	 */
	inline R call(Args... args) {
		if constexpr (std::is_void<R>::value) {
			if (callback_ == nullptr) {
				return;
			}
			(*callback_)(args...);
		}

		if constexpr (! std::is_void<R>::value) {
			if (callback_ == nullptr) {
				return 0; // R can only be a arithmetic type. 0 should work as default.
			}
			return (*callback_)(args...);
		}
	}

	inline bool is_set() {
		return (callback_ != nullptr);		
	}

private:
	std::function<R(Args... args)> *callback_;
};


#endif /* CALLBACKMANAGER_H_ */
