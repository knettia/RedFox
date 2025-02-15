#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <functional>

#if defined (__linux__) || defined (__APPLE__)
#include <dlfcn.h>
#endif

#include "RedFox/core/utils/string_utilities.hpp" // RF::format_view

namespace RF
{
	class library_m // static
	{
	private:
		class lib
		{
		private:
			void* handle_ = nullptr;
		public:
			explicit lib(const std::string_view libname);
			~lib();

			// inline
			template<typename T>
			std::function<T> get_function(const std::string_view symbol)
			{
				void *func = dlsym(handle_, symbol.data());
				if (!func)
				{ throw std::runtime_error(RF::format_view("<1>: could not find/load library symbol \'<0>\'", symbol, handle_)); }

				return reinterpret_cast<T *>(func);
			}

			bool is_valid() const;
		};

		library_m() = default;
		~library_m() = default;
		std::unordered_map<std::string, std::shared_ptr<lib>> libraries_;
	public:
		static library_m &self();

		std::shared_ptr<lib> load_library(const std::string_view libname);
		void unload_library(const std::string_view libname);
	};
} // namespace RF