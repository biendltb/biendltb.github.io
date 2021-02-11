---
layout: single
title: "A summary of API Design for C++ - Morgan Kaufmann (part 1)"
date: 2021-02-09T21:24:00+07:00
categories: book
classes:
  - dark-theme
tags:
  - technology
  - api
  - c++
---

{% include toc icon="cog" title="Table of Contents" %}

# Introduction

In C++, API is simply a description of how to interact with a component. 

In is embodied as one or more header (.h) + supporting documentation files. An implementation for a given API is often represented as a library file that can be linked into end-user applications. This can be:
* static file: `.lib` in Windows, `.a` on Mac OS X + Linux
* dynamic file: `.dll` in Windows, `.dylib` on Mac OS X and Linux

API needs **good documentation** and **automated testing**.

### Why should you use APIs?
* Hides implementation: to gain the flexibility to change the implementation without causing upheaval for your users.
* Reduces dependency: systems that expose their implementation details tend to devolve into spaghetty code where every part of the system depends on the internal details of other parts of the system. Investing in good API design up front and paying the incremental cost to maintain a coherent design, it could save the maintenance cost in the long run.
* Promotes modularization: Developing an application on top of a collection of APIs promotes loossely coupled and modular architectures where the behavior of one module is not dependent on the internal details of another module.
* Reduces code duplication.
* Removes hardcoded assumptions.
* Easier to change the implementation without affecting other code that depends on the API.
* Easier to optimize.
* Parallel development.

### APIs and SDKs

The term of Software Development Kit (SDK) is closely related to the term API. Essentially, an SDK is a platform-specific package that you install on your computer in order to build applications against one or more APIs. However, an SDK may include other resources to help you use the APIs, such as documentation, example source code, and supporting tools.

### Cases where API is a must

Two of most common forms of communication "contract" used in computer applications are file formats and network protocols.

Given the format for a data file, any program can read and write files in that format (e.g. JPEG, PNG). Similarly, client/server applications, peer-to-peer applications and middleware services work by sending data back and forward using an established protocol, usually over a network socket.

Whenever you create a file format or client/server protocol, you should also create an API for it. This allows details of the specification, and any future changes to it, to be centralized and hidden.

# Qualities

There are no absolutes in API design: you cannot apply a fixed set of rules to every situation. However, while there may be individual cases where you decide that it is best for your project to deviate from certain advice in this chapter, you should do so only after reasoned and considered judgement.

This chapter concentrates on generic, language-neutral qualities of an API, such as information hiding, consistency, and loose coupling.

## Model the problem domain

* Provide a good abstraction: an API should provide a logical abstraction for the problem that it solves. It should be formuated in terms of high-level concepts that make sense in the chosen problem domain rather than exposing low-level implementation issues. You should be able to give your API documentation to a non-programmer and that person should be able to understand the concepts of the interface and how it is meant to work.

* Model the key objects: an API should also model the key objects for the problem domain. This process is often called object-oriented design or object modeling because it aims to describe the hierarchy of objects in the specific problem domain.

## Hide implementation details

The primary reason for creating an API is to hide any implementation details so that these can be changed at a later date without affecting any existing clients.

There are two main categories of techniques relating to this goal:

* Physical hiding: the private source code is simply not made available to users. For example, in C and C++, we could expose only the declaration and hide the definition by storing internal details in a separate file (.cpp) from the public interface (.h).

* Logical hiding: entails the use of language features to limit access to certain elements of the API. The object-oriented concept of encapsulation provides a mechanism for limiting access to members of an object. For example, in C++, we could use access control keywords: `public`, `protected`, `private`.

### Hide member variables

In term of good API design, you should **never make member variables public**. If data members form part of the logical interface of the API, then you should instead provide getter and/or setter methods that provide indirect access to the member variables.

Benefits of getter/setter routines:
* Validation: you can perform validation on the values to ensure the internal state of the class is always valid and consistent.

* Caching: we could store the value of frequently requested calculation after the first read and then directly return that value for future requests. This could be implemented with a getter method.

* Additional operation: if necessary, you can perform additional operations whenever the client tries to access a variable. (e.g. log all the time users changes the value of a preference setting).

* Debugging: when you want to track when variables are accessed or changed.

* Synchronization: make the thread safe for a member variable.

* Finer access control: implement some finer access controls beyond ones provided by languages, e.g. read-only with getter.

* Maintaining invariant relationships: some internal data values may depend on each other. This relationship could be implemented in setter function.

### Hide implementation methods

In addition to hiding all member variables, you should also hide all methods that do not need to be public.

In C++, never return non-const pointers or references to private data members.

### Hide implementation classes

Class could be considered to hide from public if it's not necessary visible to users. Its member variables, thus, can ignore the use of getter/setter mechanism.

### Minimally complete

A good API should be minimally complete. That is, it should be as small as possible, but no smaller.

* _Don't overpromise_: every public element in your API is a promise - a promise that you will support that functionality for the lifetime of the API. The key point is that once you release an API and have clients using it, adding new functionality is easy but removing functionality is really difficult. The best advice then is: when in doubt, leave it out.

* _Add virtual functions judiciously_:
One subtle way that you can expose more functionality than you intended is through inheritance, that is, by making certain member functions virtual. Doing so allows your clients to subclass your class and reimplement any virtual methods. While this can be very powerful, you should be aware of the potential pitfalls:
    * Changes you makes in the future base class could have a detrimental impact on your clients's virtual functions that they implemented. This is often referred as the "fragile base class problem".
    * You client may use your API in ways that you never intended or imagined.
    * Virtual function calls are resolved at the run time while non-virtual ones are resolved at the compile time. This makes virtual function calls slower and increase the size of an object.

> Avoid declaring functions as overridable (virtual) until you have a valid and compelling need to do so.

* _Convenience APIs_: There is a natural tension between reducing the number of functions in your API while also making the API easy to use for a range of clients. We could create high-level convenience APIs and keep it separated from the core API which perform the basic functionality.

## Easy to use

A well-designed API should make simple task easy and obvious. This API quality follows closely from the quality of minimalism: if your API is simple, it should be easy to understand.

Techniques to make API easier to understand and ultimately easier to use:
* _Discoverable_: first-time users should be able to figure out how to use the API.
* _Difficult to misuse_: One of the most common ways to misuse an PPI is passing the wrong arguments or passing illegal values to a method. For example, using enum instead of boolean in the function arguments will improve the code readability, and thus, reduce the chance of misusing the function.
* _Consistent_: A good API should apply a consistent design approach so that tis conventions are easy to remember, and therefore easy to adopt.

> Use consistent function naming and parameter ordering.

* _Platform independent_: A well-designed C ++ API should always **avoid** platform-specific `#if` / `#ifdef` lines in its public headers. This poor design creates a different API on different platforms. Doing so forces the clients of your API to introduce the same platform specificity into their own application. Those `#if` statement should be hidden in the `.cpp` file instead of being exposed in the header file.


## Loosely coupled
First, understand the concepts:
* **Coupling**: A measure of the strength of interconnection between software components, that is, the degree to which each component depends on other components in the system.
* **Cohesion**: A measure of how coherent or strongly related the various functions of a single software component are.

> Good APIs exhibit loose coupling and high cohesion.

The coupling between two components could be measured based on:

* Size: relates to number of connections between components, including the number of classes, methods, arguments per method, etc. For example, a method with fewer parameters is more loosely coupled to components that call it.
* Visibility:
* Intimacy: refers to the directness of the connection between components. For example, if A is coupled to B, B coupled to C then A coupled to C. Another example is, inheriting from a class is a tigher coupling than including that class as a member variable (composition) because inheritance also provides access to all protected members of the class.
* Flexibility: Relates to the ease of changing the connections between components.

Following techniques could help reduce the coupling within an API:

### Coupling by name only

In some cases, when class A only needs the name of class B without the need of accessing data in class B, forward declaration could be use for class B instead of including the entire interface, and so reduce the coupling between the two classes. For example,

```cpp
class MyObject; // forward declaration, only need to know the name of My object

class MyObjectHolder
{
public:
    MyObjectHolder();

    void SetObject(MyObject *obj);
    MyObject *GetObject() const;

private:
    MyObject *mObj;
};
```

> Use a forward declaration for a class unless you actually need to `#include` its full definition.

### Reducing class coupling

Scott Meyers recommends that whenever you have a choice, you should prefer declaring a function as a **non-member non-friend function** rather than as a member function. Doing so improves encapsulation and also reduces the degree of coupling of those functions to the class.

For example, consider the following class that provide a `PrintName()` function:
```cpp
// myobject.h
class MyObject
{
public:
    void PrintName() const;
    std::string GetName() const;
    ...

protected:
    ...

private:
    std::string mName;
    ...
}
```

We could convert the `PrintName()` member function to a non-member function:

```cpp
class MyObject
{
public:
    std::string GetName() const;
    ...

protected:
    ...

private:
    std::string mName;
    ...
}

void PrintName(const MyObject &obj);
```

The `PrintName()` function in the later form is less coupling since it can only access the public method of `MyObject`.

> Prefer using non-member non-friend functions instead of member functions to reduce coupling.

### Manager classes

A manager class is one that owns and coordinates several lower-level classes. This can be used to break the dependency of one or more classes upon a collection of low-level classes.

This manager class contains characteristics of low-level classes and act as a hub to remove direct connections from higher level classes down to each low-level class.


### Callbacks, Observers and Notifications

Consider this type of coupling in a client-server application: a GUI needs to get the name of a user to diplay given the user ID. The normal process would be:

1. GUI calls the Client::GetName().
2. Client sends request to server.
3. Client receives the information from the server's response.
4. Client calls GUI::DisplayName().

This is a two-way dependency between GUI and Client and a tightly coupled design. A solution for it could be to have the GUI register interest in updates from the Client. There are several ways this can be done, such as callbacks, observers, and notifications.

However, there are some general issues that should be aware of when using any of these schemes:

* Reentrancy: when writing an API that calls out to unknown user code, you have to consider that this code may call back into your API. In fact, the client may not even realise that this is happening. This callback might affect the state consistency of your API (e.g. modify the data in an unexpected way).

* Lifetime management: Clients should have a clean way to disconnect from your API, that is, to declare that they are no longer interested in receiving updates. This is particularly important when the client object is deleted because further attempts to send messages to it could cause a crash. Similarly, your API may wish to guard against **duplicate registrations** to avoid calling the same client code multiple times for the same event.

* Event ordering: The sequence of callbacks or notifications should be clear to the user of your API.

<ins>**Callbacks**</ins>

In C/C++, a callback is a pointer to a function within module A that is passed to module B so that B can invoke the function in A at an appropriate time. Module B knows nothing about module A and has no include or link dependencies upon A. This makes callbacks particularly useful to allow low-level code to execute high-level code that it cannot have a link dependency on. As such, callbacks are a popular technique to break cyclic dependencies in large projects.

A simple callback API in C++:
```cpp
#include <string>

class ModuleB
{
public:
    // define a function pointer called CallbackType that takes a string and 
    // a data and return void
    typedef void (*CallbackType)(const std::string &name, void *data);
    void SetCallback(CallbackType cb, void *data);
    ...
private:
    CallbackType mCallback;
    void *mClosure;
}
```

This class could then invoke the callback function, for example:
```cpp
if (mCallback)
{
    (*mCallback)("HelloWorld", mClosure);
}
```

A clearer example of callback function from [SOF](https://stackoverflow.com/a/2298865/6088342):
```cpp
//Define a type for the callback signature,
//it is not necessary, but makes life easier

//Function pointer called CallbackType that takes a float
//and returns an int
typedef int (*CallbackType)(float);  


void DoWork(CallbackType callback)
{
  float variable = 0.0f;

  //Do calculations

  //Call the callback with the variable, and retrieve the
  //result
  int result = callback(variable);

  //Do something with the result
}

int SomeCallback(float variable)
{
  int result;

  //Interpret variable

  return result;
}

int main(int argc, char ** argv)
{
  //Pass in SomeCallback to the DoWork
  DoWork(&SomeCallback);
}
```

<ins>**Observers**</ins>

This is a software design pattern where an object maintains a list of its dependent objects (observers) and notifies them by calling one of their methods. This will be presented in the next part of patterns.

<ins>**Notification**</ins>

Callbacks and observers tend to be created for a particular task, and the mechanism to use them is normally defined within the objects that need to perform the actual callback. An alternative solution is to build a centralized mechanism to send notifications, or events, between unconnected parts of the system. The sender does not need to know about the receiver beforehand, which lets us reduce coupling between the sender and the receiver.

## Stable, documented, and tested

A well-designed API should be stable and future proof. In this context, stable does not necessarily mean that the API never changes, but rather that the interface should be versioned and should not change incompatibly from one version to the next. Related to this, the term future proof means that an API should be designed to be extensible so that it can be evolved elegantly rather than changed chaotically.

A good API should also be well documented so that users have clear information about the capabilities, behavior, best practices, and error conditions for the API. Finally, there should be extensive automated tests written for the implementation of the API so that new changes can be made with the confidence that existing use cases have not been broken.


(TO BE CONTINUED)
