template<typename T1, typename T2>
RF::map<T1, T2>::map(std::initializer_list<std::pair<T1, T2>> list)
{
	for (const auto &[key, value] : list)
	{
		this->forward_map_.emplace(key, value);
		this->reverse_map_.emplace(value, key);
	}
}

template<typename T1, typename T2>
std::optional<T2> RF::map<T1, T2>::operator[](T1 key) const
{
	auto it = this->forward_map_.find(key);
	if (it != this->forward_map_.end())
	{ return it->second; }

	return std::nullopt;
}

template<typename T1, typename T2>
std::optional<T1> RF::map<T1, T2>::operator[](T2 key) const
{
	auto it = this->reverse_map_.find(key);
	if (it != this->reverse_map_.end())
	{ return it->second; }

	return std::nullopt;
}

template<typename T1, typename T2>
bool RF::map<T1, T2>::insert(T1 k, T2 v)
{
	if (
		this->forward_map_.find(k) == this->forward_map_.end() &&
		this->reverse_map_.find(v) == this->reverse_map_.end()
	)
	{
		this->forward_map_.emplace(k, v);
		this->reverse_map_.emplace(v, k);
		return true;
	}

	return false;
}

template<typename T1, typename T2>
bool RF::map<T1, T2>::exsert(T1 k, T2 v)
{
	if (
		this->forward_map_.find(k) != this->forward_map_.end() &&
		this->reverse_map_.find(v) != this->reverse_map_.end()
	)
	{
		this->forward_map_.erase(k);
		this->reverse_map_.erase(v);
		return true;
	}
	
	return false;
}