#include "RF/library.hpp" // header

// library manager

std::shared_ptr<RF::library_m::lib> RF::library_m::load_library(const std::string_view libname)
{
	auto it = RF::library_m::libraries_.find(libname.data());
	if (it != RF::library_m::libraries_.end())
	{
		return it->second;
	}

	auto lib_ptr = std::make_shared<RF::library_m::lib>(libname);

	if (lib_ptr->is_valid())
	{RF::library_m::libraries_[libname.data()] = lib_ptr; }

	return lib_ptr;
}

void RF::library_m::unload_library(const std::string_view libname)
{
	auto it = RF::library_m::libraries_.find(libname.data());
	if (it != RF::library_m::libraries_.end())
	{ RF::library_m::libraries_.erase(it); }
}

// lib
RF::library_m::lib::lib(const std::string_view libname)
{
	#if defined (__linux__) || defined (__APPLE__)
	handle_ = dlopen(libname.data(), RTLD_LAZY);
	#elif defined (_WIN32)
	handle_ = LoadLibrary(libname.data());
	#endif
}

RF::library_m::lib::~lib()
{
	#if defined (__linux__) || defined (__APPLE__)
	if (handle_)
	{ dlclose(handle_);}
	#elif defined (_WIN32)
	if (handle_)
	{ FreeLibrary(static_cast<HMODULE>(handle_)); }
	#endif
}

bool RF::library_m::lib::is_valid() const
{ return this->handle_ != nullptr; }