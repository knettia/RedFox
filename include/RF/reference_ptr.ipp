template<typename T>
RF::reference_ptr<T>::reference_ptr(T *object)
:
	ptr_(object)
{
	if (!ptr_)
	{ throw std::invalid_argument("reference_ptr cannot be initialized with nullptr"); }
}

template<typename T>
T &RF::reference_ptr<T>::operator *() const noexcept
{
	assert(ptr_ && "dereferencing of a null reference_ptr is not allowed");
	return *ptr_;
}

template<typename T>
T *RF::reference_ptr<T>::operator->() const noexcept
{
	assert(ptr_ && "accessing of a null reference_ptr is not allowed");
	return ptr_;
}

template<typename T>
RF::reference_ptr<T>::operator T *() const noexcept
{
	return ptr_;
}

template<typename T>
T *RF::reference_ptr<T>::ptr_get() const noexcept
{
	return ptr_;
}

template<typename T>
bool RF::reference_ptr<T>::valid() const noexcept
{
	return ptr_ != nullptr;
}