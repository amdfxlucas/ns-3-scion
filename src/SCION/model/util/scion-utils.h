#pragma once
#include <exception>
#include <iomanip>
#include <iostream>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <stdint.h>
#include <string>
#include <variant>

namespace ns3
{
template <typename T>
struct _NonConstReference
{
    T& value_;
    explicit _NonConstReference(T& value)
        : value_(value){};
};

template <typename T>
struct _ConstReference
{
    const T& value_;
    explicit _ConstReference(const T& value)
        : value_(value){};
};

template <typename T>
struct _Value
{
    T value_;

    explicit _Value(T&& value)
        : value_(std::move(value))
    {
    }
};

template <typename... Functions>
struct _overload : Functions...
{
    using Functions::operator()...;

    _overload(Functions... functions)
        : Functions(functions)...
    {
    }
};

template <typename T>
class Storage : public std::variant< _Value<T>, _ConstReference<T>, _NonConstReference<T>>
{
  public:
    using parent = typename std::variant< _Value<T>, _ConstReference<T>, _NonConstReference<T>>;
    using parent::parent;
    using parent::operator=;
    /*Storage(NonConstReference<T>t ) : parent(t) {};
    Storage(ConstReference<T> t) :parent(t){};
    Storage(Value<T> t):parent(t){};*/

    operator T&();
    operator const T&() const;
};

template <typename T>
const T&
getConstReference(const Storage<T>& storage)
{
    return std::visit(
        _overload([](const _Value<T>& value) -> const T& { return value.value_; },
                 [](const _NonConstReference<T>& value) -> const T& { return value.value_; },
                 [](const _ConstReference<T>& value) -> const T& { return value.value_; }),
        storage);
}

struct NonConstReferenceFromReference : public std::runtime_error
{
    explicit NonConstReferenceFromReference(const std::string& what)
        : std::runtime_error{what}
    {
    }
};

template <typename T>
T&
getReference(Storage<T>& storage)
{
    return std::visit(_overload([](_Value<T>& value) -> T& { return value.value_; },
                               [](_NonConstReference<T>& value) -> T& { return value.value_; },
                               [](_ConstReference<T>&) -> T& {
                                   throw NonConstReferenceFromReference{
                                       "Cannot get a non const reference from a const reference"};
                               }),
                      storage);
}

template <class T>
Storage<T>::operator const T&() const
{
    return getConstReference(*this);
}

template <class T>
Storage<T>::operator T&()
{
    return getReference(*this);
}

} // namespace ns3

// USAGE
/*class MyClass
{
public:
    explicit MyClass(std::string& value) :       storage_(NonConstReference(value)){}
    explicit MyClass(std::string const& value) : storage_(ConstReference(value)){}
    explicit MyClass(std::string&& value) :      storage_(Value(std::move(value))){}

    void print() const
    {
        std::cout << getConstReference(storage_) << '\n';
    }

    operator std::string& ()
    {   std::cout << "operator T&() called "<< std::endl;
        return storage_;
    }

    friend std::ostream& operator<< ( std::ostream & os, MyClass&);
private:
    Storage<std::string> storage_;
};

std::ostream& operator<< ( std::ostream & os, MyClass& my){ return os << std::string(my);};

int main()
{
// Consider the first call site, with an lvalue:

std::string s = "hello";
MyClass myObject{s};
myObject.print();
//It matches the first constructor, and creates a NonConstReference inside of the storage member.
// The non-const reference is converted into a const reference when the print function calls
getConstReference.

//Now consider the second call site, with the temporary value:

MyClass myObject1{std::string{"hello"}};
myObject1.print();
//This one matches the third constructor, and moves the value inside of the storage.
//getConstReference then returns a const reference to that value to the print function.


std::cout << myObject << std::endl;

std::cout << static_cast<std::string&>(myObject) << std::endl;

}
*/