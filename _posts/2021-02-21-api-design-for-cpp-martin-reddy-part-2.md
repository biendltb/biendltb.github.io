---
layout: single
title: "A summary of API Design for C++ - Martin Reddy (part 2)"
date: 2021-02-21T01:14:00+07:00
categories: book
classes:
  - dark-theme
tags:
  - technology
  - api
  - c++
  - design patterns
---

The design patterns for C++ APIs

{% include toc icon="cog" title="Table of Contents" %}

# Patterns

This chapter concentrates on design patterns that are of particular importance to the design of high-quality APIs and discuss their practical implementation in C++.

## Pimpl idiom

"Pointer to implementation" or "pImpl" is a C++ programming technique that removes implementation details of a class from its object representation by placing them in a separate class, accessed through an opaque pointer.

This mechanism helps maintain a strong separation between API's interface and implementation. Pimpl is not strictly a design pattern, it can be considered a special case of Bridge design pattern.

### Design the pimpl class

Consider following example:

```cpp
// autotimer.h
#ifdef _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif
#include <string>

class AutoTimer
{
public:
    /// Create a new timer object with a human-readable name
    explicit AutoTimer(const std::string &name);
    /// On destruction, the timer reports how long it was alive
    ~AutoTimer();

private:
    // Return how long the object has been alive
    double GetElapsed() const;
    std::string mName;
#ifdef _WIN32
    DWORD mStartTime;
#else
    struct timeval mStartTime;
#endif
};
```
This API violates some important qualities presented in the previous part, including the exposing of platform-specific defines in the header file. However, C++ requires to declare private member in the header file so that we cannot avoid including the platform-specific `#if` directives.

Pimpl idiom could come to rescue here by **placing all the private members into a class (or struct)** that is forward declared in the header but defined in the `.cpp` file.

```cpp
// autotimer.h
#include <string>
class AutoTimer
{
public:
    explicit AutoTimer(const std::string &name);
    ~AutoTimer();
private:
    class Impl;
    Impl *mImpl;
};
```

Now, the API is much cleaner. There are no platform-specific preprocessor directives, and the user cannot see any of the class's private members by looking at the header file. The `.cpp` file now looks a little bit messy due to the platform-specific `#ifdef` but the point is that this messiness is completely hidden from users.

```cpp
// autotimer.cpp
#include "autotimer.h"
#include <iostream>
#if _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif
class AutoTimer::Impl
{
public:
    double GetElapsed() const
    {
#ifdef _WIN32
    return (GetTickCount() - mStartTime) / 1e3;
#else
    struct timeval end_time;
    gettimeofday(&end_time, NULL);
    double t1 = mStartTime.tv_usec / 1e6 + mStartTime.tv_sec;
    double t2 = end_time.tv_usec / 1e6 + end_time.tv_sec;
    return t2 - t1;
#endif
    }
    std::string mName;
#ifdef _WIN32
    DWORD mStartTime;
#else
    struct timeval mStartTime;
#endif
};
AutoTimer::AutoTimer(const std::string &name) :
    mImpl(new AutoTimer::Impl())
{
    mImpl->mName = name;
#ifdef _WIN32
    mImpl->mStartTime = GetTickCount();
#else
    gettimeofday(&mImpl->mStartTime, NULL);
#endif
}
AutoTimer::~AutoTimer()
{
    std::cout << mImpl->mName << ": took " << mImpl->GetElapsed()
        << " secs" << std::endl;
    delete mImpl;
    mImpl = NULL;
}
```

In the above example, the `Impl` class is a private nested class within the `AutoTimer` class. Declaring it as private helps avoid polluting the global namespace and the public API of the class. However, the limitation of this approach is that only methods of `AutoTimer` can access members of the `Impl`. We could make it public if other classes or free functions in the .cpp must access the `Impl` members. For example:

```cpp
// autotimer.h
class AutoTimer
{
public:
    explicit AutoTimer(const std::string &name);
    ~AutoTimer();
    // allow access from other classes/functions in autotimer.cpp
    class Impl;
private:
    Impl *mImpl;
};
```

Another design question worth considering is **how much logic to locate in** the `Impl` class. Some options include:
1. Only private member **variables**
2. Private member **variables and methods**
3. All methods of the public class, such that the public methods are simply **thin wrappers** on top of equivalent methods in the `Impl` class.

Each of the above options may be appropriate under different circumstances. However, option 2 is preferred in general.

A couple of caveats with this approach (Herb Sutter, 1999):
* Virtual methods can't be hidden in the implementation class. These must appear in the public class so that any derived classes are able to override them.

* You may need to add a pointer in the implementation class back to the public class so that the Impl class can call public methods. Although you could also pass the public class into the implementation class methods that need it.

### Copy semantics
A C ++ compiler will create a copy constructor and assignment operator for your class if you don’t explicitly define them. However, these default constructors will only perform a shallow copy of your object. This is bad for pimpled classes because it means that if a client copies your object then both objects will point to the same implementation object, `Impl` . However, both objects will attempt to delete this same `Impl` object in their destructors, which will most likely lead to a crash. Two options for dealing with this are as follow.

* **Make your class uncopyable**: You can do it by explicitly declare a **copy constructor** and **assignment operator** in the header file. You don't have to provide implementations for these. This will give a link error if any code try to copy them. Alternatively, you could declare them as private, which will give a compile error instead.
```cpp
#include <string>
class AutoTimer
{
public:
    explicit AutoTimer(const std::string &name);
    ~AutoTimer();
private:
    // Make this object be non-copyable
    AutoTimer(const AutoTimer &);
    const AutoTimer &operator =(const AutoTimer &);
    class Impl;
    Impl *mImpl;
};
```
In C++11, you could explicitly forbid it:
```cpp
AutoTimer(const AutoTimer &) = delete;
```

* **Explicitly define the copy semantics**: If you do want your users to be able to copy your pimpled objects, then you should declare and define your own copy constructor and assignment operator. These can then perform a deep copy of your object, that is, create a copy of the Impl object instead of just copying the pointer.

### Smart pointers

One of the inconvenient and error-prone aspects of pimpl is the need to allocate and deallocate the implementation object. Every now and then you may forget to delete the object in your destructor or you may introduce bugs by accessing the Impl object before you’ve allocated it or after you’ve destroyed it. As a convention, you should therefore ensure that the very first thing your constructor does is to allocate the Impl object (preferably via its initialization list), and the very last thing your destructor does is to delete it.

> Think about the copy semantics of your pimpl classes and consider using a smart pointer to manage the initialization and destruction of the implementation pointer.

### Advantages of Pimpl

* **Information hiding**: private members are now completely hidden from your public interface. This also makes your public header files are cleaners and more clear to users.

* **Reduce coupling**: as shown in the example, the dependencies of the header on those files `windows.h` and `sys/time.h` could increase the compile-time coupling of your API on other parts of the system. Using pimpl, you can move those dependencies into the `.cpp` file and remove those elements of coupling.

* **Faster compiles**: Another implication of moving implementation-specific includes to the `.cpp` file is that the include hierarchy of your API is reduced. This can have a very direct effect on compile times.

* **Greater binary compatibility**: The size of a pimpled object never changes because your object is always the size of a single pointer. Any changes you make to private member variables will only affect the size of the implementation class that is hidden inside of the `.cpp` file. This makes it possible to make major implementation changes without changing the binary representation of your object.

* **Lazy allocation**: The `mImpl` class can be constructed on demand. This may be useful if the class allocates limited or costly resources such as a network connection.

### Disadvantages of pimpl
* Allocate and free an additional implementation object.
* Extra developer inconvenience to prefix all private member accesses with something like `mImpl->`. This can make the implementation code harder to read and debug due to the
additional layer of abstraction. This becomes even more complicated when the Impl class has a pointer back to the public class. You must also remember to define a copy constructor or disable copying of the class. However, these inconveniences are not exposed to users of your API and are therefore not a concern from the point of view of your API’s design. They are a burden that you the developer must shoulder in order that all of your users receive a cleaner and more efficient API. To quote a certain science officer and his captain: “The needs of the many outweigh the needs of the few. Or the one.”
* The compiler will no longer catch changes to member variables
within const methods. This is because member variables now live in a separate object. Your compiler will only check that you don’t change the value of the `mImpl` pointer in a const method, but not whether you change any members pointed to by `mImpl`.

