/*
 * callbackmanager.cpp
 *
 *  Created on: 16 jun. 2024
 *      Author: jancu
 */

 module;

 #include <functional>
 
 export module callbackmanager;
 
 namespace callbackmanager {
 
 export template <typename R, typename... Args>
  requires std::is_void<R>::value || std::is_arithmetic_v<R>
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

} // namespace callbackmanager