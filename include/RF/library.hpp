#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <functional>

#if defined (__linux__) || defined (__APPLE__)
#include <dlfcn.h>
#elif defined(_WIN32)
#include <windows.h>
#endif

namespace RF
{
	class lib
	{
	private:
		void *handle_ = nullptr;
	public:
		explicit lib(const std::string_view libname);
		~lib();

		// inline
		template<typename T>
		std::function<T> get_function(const std::string_view symbol);

		template<typename T>
		T get_function_raw(const std::string_view symbol);

		bool is_valid() const;
	};

	class library_m // static
	{
	private:
		static std::unordered_map<std::string, std::shared_ptr<RF::lib>> libraries_;
	public:
		library_m() = delete;
		~library_m() = delete;

		static std::shared_ptr<lib> load_library(const std::string_view libname);
		static void unload_library(const std::string_view libname);
	};

	template <typename Signature>
	class abstract_function_t;

	template <typename R, typename... Args>
	class abstract_function_t<R(Args...)>
	{
	public:
		using function_type = std::function<R(Args...)>;

		abstract_function_t() = default;
		~abstract_function_t() = default;

		bool define(const std::string& key, function_type);
		bool undefine(const std::string& key);

		bool contains(const std::string& key) const;
		std::size_t size() const;
		void clear();

		template <typename Ret = R>
		typename std::enable_if<std::is_void<Ret>::value, void>::type call(Args... args);

		template <typename Ret = R>
		typename std::enable_if<!std::is_void<Ret>::value, std::vector<R>>::type call(Args... args);
	private:
		mutable std::mutex mutex_;
		std::unordered_map<std::string, function_type> map_;
	};
} // namespace RF

#include "library.ipp"
