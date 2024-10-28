#pragma once

#include <list>

template <class T>
class readOnlyListView {
	const std::list<T>& _list;
public:
	explicit readOnlyListView(const std::list<T>& list) : _list(list) {}

	auto begin() const { return const_cast<std::list<T>&>(_list).begin(); }
	auto end() const { return const_cast<std::list<T>&>(_list).end(); }

	auto cbegin() const { return _list.cbegin(); }
	auto cend() const { return _list.cend(); }

	auto& front() const { return const_cast<T&>(_list.front()); }
	auto& back() const { return const_cast<T&>(_list.back()); }

	auto size() const { return _list.size(); }
	auto empty() const { return _list.empty(); }

	auto& operator[](size_t index) const { return const_cast<T&>(_list[index]); }
};