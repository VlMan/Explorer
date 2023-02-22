#pragma once

#include <algorithm>
#include <vector>
#include <QObject>

template <typename T, typename T2>
class AlgorithmWrapper {
public:
	static bool any_of(const std::vector<T>& vec, const T& value) {
		return std::any_of(vec.begin(), vec.end(), [&](const T& v) { return v == value; });
	}

	static bool any_of(const QVector<T>& vec, const T& value) {
		return any_of(vec.toStdVector(), value);
	}

	static bool any_of(const QVector<T>& vec, const T2& value) {
		return any_of(vec.toStdVector(), qobject_cast<T>(value));
	}

	static bool none_of(const std::vector<T>& vec, const T& value) {
		return std::none_of(vec.begin(), vec.end(), [&](const T& v) { return v == value; });
	}

	static typename std::vector<T>::iterator find_if(std::vector<T>& vec, const std::function<bool(const T&)>& predicate) {
		return std::find_if(vec.begin(), vec.end(), predicate);
	}

	static bool all_of(const std::vector<T>& vec, const std::function<bool(const T&)>& predicate) {
		return std::all_of(vec.begin(), vec.end(), predicate);
	}
};
