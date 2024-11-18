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

template <class T>
class readOnlyVector {

	const std::vector<T> _vec;
public:
	// explicit makes it so that it can't be initialized with a compatible type

	explicit readOnlyVector(const std::vector<T>& vec) : _vec(vec) {};
	// universal vec reference,     initializes vec calling copy constructor

	auto begin() const { return _vec.begin(); }
	auto end() const { return _vec.end(); }

	auto& front() const { return const_cast<T&>(_vec.front()); }
	auto& back() const { return const_cast<T&>(_vec.back()); }

	auto size() const { return _vec.size(); }
	bool empty() const { return _vec.empty(); }

	auto& operator[](size_t index) const { return const_cast<T&>(_vec[index]); }

};