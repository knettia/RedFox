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
	class library_m // static
	{
	private:
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

			bool is_valid() const;
		};

		static std::unordered_map<std::string, std::shared_ptr<lib>> libraries_;
	public:
		library_m() = delete;
		~library_m() = delete;

		static std::shared_ptr<lib> load_library(const std::string_view libname);
		static void unload_library(const std::string_view libname);
	};
} // namespace RF

#include "library.ipp"