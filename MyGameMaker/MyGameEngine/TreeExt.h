#pragma once

#include <list>
#include "readOnlyView.h"

template <class T>
class TreeExt {

private:
	T* _parent = nullptr;
	std::list<T> _children;

public:
	auto& parent() const { return _parent; }
	auto children() const { return readOnlyListView<T>(_children); }

	auto& root() const { return _parent ? _parent->root() : *this; }
	bool isRoot() const { return !_parent; }

	template <typename ...Args>
	auto& emplaceChild(Args&&... args) {
		_children.emplace_back(std::forward<Args>(args)...);
		_children.back()._parent = static_cast<T*>(this);
		return _children.back();
	}

	void removeChild(const T& child) { return _children.remove(std::forward(child)); }
};