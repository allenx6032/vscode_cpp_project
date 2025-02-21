// pch.h: 这是预编译标头文件。
// 下方列出的文件仅编译一次，提高了将来生成的生成性能。
// 这还将影响 IntelliSense 性能，包括代码完成和许多代码浏览功能。
// 但是，如果此处列出的文件中的任何一个在生成之间有更新，它们全部都将被重新编译。
// 请勿在此处添加要频繁更新的文件，这将使得性能优势无效。

#ifndef PCH_H
#define PCH_H

// 添加要在此处预编译的标头
#include "framework.h"

#include <string>
#include <sstream> // Add this include at the top of the file
#include <vector>
#include <functional>
#include <algorithm>
#include <cstring>
//#include <any.hpp>
#include <map> 
#include <ctime>
#include <cstdint>
#include <typeinfo>
#include <memory>
#include <stdexcept>
#include <utility>


class Any {
public:
	// 默认构造函数（空对象）
	Any() = default;

	// 构造含值的对象
	template <typename T>
	Any(const T& value) : holder_(new Holder<T>(value)) {}

	// 移动构造函数
	Any(Any&& other) noexcept : holder_(std::move(other.holder_)) {}

	// 拷贝构造函数
	Any(const Any& other) :
		holder_(other.holder_ ? other.holder_->clone() : nullptr) {}

	// 赋值运算符
	Any& operator=(Any other) {
		swap(other);
		return *this;
	}

	// 类型查询
	const std::type_info& type() const noexcept {
		return holder_ ? holder_->type() : typeid(void);
	}

	// 是否含有值
	bool has_value() const noexcept {
		return static_cast<bool>(holder_);
	}

	// 交换操作
	void swap(Any& other) noexcept {
		holder_.swap(other.holder_);
	}

	// 清空内容
	void reset() noexcept {
		holder_.reset();
	}

private:
	// 类型擦除基类
	struct BaseHolder {
		virtual ~BaseHolder() = default;
		virtual const std::type_info& type() const = 0;
		virtual BaseHolder* clone() const = 0;
	};

	// 具体类型存储模板
	template <typename T>
	struct Holder : BaseHolder {
		T value;

		explicit Holder(const T& v) : value(v) {}
		explicit Holder(T&& v) : value(std::move(v)) {}

		const std::type_info& type() const override {
			return typeid(T);
		}

		BaseHolder* clone() const override {
			return new Holder(value);
		}
	};

	std::unique_ptr<BaseHolder> holder_;

	// 类型转换实现
	template <typename T>
	friend T* any_cast(Any* any) noexcept;
};

// 类型转换函数（指针版本）
template <typename T>
T* any_cast(Any* any) noexcept {
	return (any && any->type() == typeid(T)) ?
		&static_cast<Any::Holder<T>*>(any->holder_.get())->value :
		nullptr;
}

// 类型转换函数（引用版本）
template <typename T>
T& any_cast(Any& any) {
	auto p = any_cast<T>(&any);
	if (!p) throw std::bad_cast();
	return *p;
}

// 常量引用版本
template <typename T>
const T& any_cast(const Any& any) {
	return any_cast<T>(const_cast<Any&>(any));
}

// 移动版本
template <typename T>
T any_cast(Any&& any) {
	auto p = any_cast<typename std::remove_reference<T>::type>(&any);
	if (!p) throw std::bad_cast();
	return std::move(*p);
}



#endif //PCH_H
