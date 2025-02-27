#include "RedFox/core/types/library.hpp" // header
#include "RedFox/core/utils/ignoramus.hpp" // RF::ignoramus::logf

// library manager
RF::library_m& RF::library_m::self()
{
	static RF::library_m inst;
	return inst;
}

std::shared_ptr<RF::library_m::lib> RF::library_m::load_library(const std::string_view libname)
{
	auto it = this->libraries_.find(libname.data());
	if (it != this->libraries_.end())
	{
		RF::ignoramus::logf(RF::ignoramus_t::info, "Using cached library: <0>", libname);
		return it->second;
	}

	auto lib_ptr = std::make_shared<RF::library_m::lib>(libname);

	if (lib_ptr->is_valid())
	{ this->libraries_[libname.data()] = lib_ptr; }

	return lib_ptr;
}

void RF::library_m::unload_library(const std::string_view libname)
{
	auto it = this->libraries_.find(libname.data());
	if (it != this->libraries_.end())
	{ this->libraries_.erase(it); }
}

// lib
RF::library_m::lib::lib(const std::string_view libname)
{ }

RF::library_m::lib::~lib()
{ }

bool RF::library_m::lib::is_valid() const
{ return this->handle_ != nullptr; }