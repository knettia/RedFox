#include "RF/string.hpp" // RF::format_view
#include "RF/exception.hpp"

template<typename T>
inline T RF::lib::get_function_raw(const std::string_view symbol)
{
	#if defined (__linux__) || defined (__APPLE__)
	void *func = dlsym(handle_, symbol.data());
	if (!func) {
		throw std::runtime_error(RF::format_view("could not find/load library symbol '<0>'", symbol));
	}
	return reinterpret_cast<T>(func);
	#elif defined (_WIN32)
	FARPROC func = GetProcAddress(static_cast<HMODULE>(handle_), symbol.data());
	if (!func) {
		throw std::runtime_error(RF::format_view("could not find/load library symbol '<0>'", symbol));
	}
	return reinterpret_cast<T>(func);
	#endif
}

template<typename T>
inline std::function<T> RF::lib::get_function(const std::string_view symbol)
{
	#if defined (__linux__) || defined (__APPLE__)
	void *func = dlsym(handle_, symbol.data());
	if (!func)
	{ throw std::runtime_error(RF::format_view("could not find/load library symbol \'<0>\'", symbol)); }

	return reinterpret_cast<T *>(func);
	#elif defined (_WIN32)
	FARPROC func = GetProcAddress(static_cast<HMODULE>(handle_), symbol.data());
	if (!func)
	{ throw std::runtime_error(RF::format_view("could not find/load library symbol \'<0>\'", symbol)); }

	return reinterpret_cast<T *>(func);
	#endif
}

template <typename R, typename... Args>
template <typename F>
inline bool RF::abstract_function_t<R(Args...)>::define(const std::string& key, F&& f)
{
	function_type fn = std::forward<F>(f);
	std::lock_guard<std::mutex> lg(mutex_);
	auto it = map_.find(key);
	if (it == map_.end())
	{
		map_.emplace(key, std::move(fn));
		return false;
	}
	else
	{
		it->second = std::move(fn);
		return true;
	}
}

template <typename R, typename... Args>
inline bool RF::abstract_function_t<R(Args...)>::undefine(const std::string& key)
{
	std::lock_guard<std::mutex> lg(mutex_);
	auto it = map_.find(key);
	if (it == map_.end()) return false;
	map_.erase(it);
	return true;
}

template <typename R, typename... Args>
inline bool RF::abstract_function_t<R(Args...)>::contains(const std::string& key) const
{
	std::lock_guard<std::mutex> lg(mutex_);
	return map_.find(key) != map_.end();
}

template <typename R, typename... Args>
inline std::size_t RF::abstract_function_t<R(Args...)>::size() const
{
	std::lock_guard<std::mutex> lg(mutex_);
	return map_.size();
}

template <typename R, typename... Args>
inline void RF::abstract_function_t<R(Args...)>::clear()
{
	std::lock_guard<std::mutex> lg(mutex_);
	map_.clear();
}

// call for void return type
template <typename R, typename... Args>
template <typename Ret>
inline typename std::enable_if<std::is_void<Ret>::value, void>::type
RF::abstract_function_t<R(Args...)>::call(Args... args)
{
	std::vector<std::string> errors;
	std::vector<function_type> snapshot;
	{
		std::lock_guard<std::mutex> lg(mutex_);
		snapshot.reserve(map_.size());
		for (auto& kv : map_) snapshot.push_back(kv.second);
	}

	for (auto &fn : snapshot)
	{
		try
		{
			fn(args...);
		}
		catch (const std::exception& e)
		{
			errors.push_back(e.what());
		}
		catch (...)
		{
			errors.push_back("unknown exception");
		}
	}

	if (!errors.empty())
	{
		std::ostringstream os;
		os << RF::format_view("RF::abstract_function_t::call: <0> callable(s) threw:", errors.size());
		for (auto &msg : errors) os << RF::format_view("\n - <0>", msg);
		throw RF::runtime_error(os.str());
	}
}

// call for non-void return type
// has not been tested yet, and I don't expect it to work properly
// will probably have to fix in the future :(
template <typename R, typename... Args>
template <typename Ret>
inline typename std::enable_if<!std::is_void<Ret>::value, std::vector<R>>::type
RF::abstract_function_t<R(Args...)>::call(Args... args)
{
	std::vector<std::string> errors;
	std::vector<R> results;
	std::vector<function_type> snapshot;
	{
		std::lock_guard<std::mutex> lg(mutex_);
		snapshot.reserve(map_.size());
		for (auto& kv : map_) snapshot.push_back(kv.second);
	}

	results.reserve(snapshot.size());
	for (auto &fn : snapshot)
	{
		try
		{
			results.push_back(fn(args...));
		}
		catch (const std::exception& e)
		{
			errors.push_back(e.what());
		}
		catch (...)
		{
			errors.push_back("unknown exception");
		}
	}

	if (!errors.empty())
	{
		std::ostringstream os;
		os << RF::format_view("RF::abstract_function_t::call: <0> callable(s) threw:", errors.size());
		for (auto &msg : errors) os << RF::format_view("\n - <0>", msg);
		throw RF::runtime_error(os.str());
	}

	return results;
}
