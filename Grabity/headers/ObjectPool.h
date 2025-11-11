/*!****************************************************************
\file: ObjectPool.h
\author: Johny Yong Jun Siang, j.yong, 2301301
\brief: Defines the ObjectPool template class for managing a pool of reusable objects. Provides efficient object management by allowing
        reuse of objects from a free list, minimizing dynamic allocation overhead. It supports creating new objects, reusing existing ones, 
        and removing objects from active use.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/
#pragma once
#include <vector>
#include <cassert>

// ObjectPool class: Used to manage reusable objects
template <typename T>
class ObjectPool {
public:
    // Constructor: Reserve capacity for the object pool
    ObjectPool(size_t capacity = 10000);
    T* Create();    // Create a new object or reuse one from the pool if available
    void Remove(T* object);    // Remove an object from use and put it back in the free list for reuse
    void Clear();    // Clear all objects from the pool (useful for cleanup)

    const std::vector<T*>& GetActiveParticles() const { return inUse; }

private:
    // Vector to store all the objects in the pool
    std::vector<T> pool;
    std::vector<T*> inUse;    // Vector to store pointers to objects that are currently in use
    std::vector<T*> freeList;    // Vector to store pointers to objects available for reuse
};

//Template definition

// Constructor: Reserve capacity for the object pool
template <typename T>
ObjectPool<T>::ObjectPool(size_t capacity) {
    pool.reserve(capacity);  // Reserve memory to prevent reallocations
    inUse.reserve(capacity); // Reserve for objects currently in use
}

// Create a new object or reuse one from the pool if available
template <typename T>
T* ObjectPool<T>::Create() {
    if (freeList.size() > 0) {
        T* obj = freeList.back();
        freeList.pop_back();
        inUse.push_back(obj);  // Add it to the in-use list
        return obj;
    }
    // If no objects are available in the free list, create a new one
    pool.push_back(T());  // Adds a new object to the pool
    T* obj = &pool.back(); // Track the newly created object as in use
    inUse.push_back(obj);
    return obj;
}

// Remove an object from use and put it back in the free list for reuse
template <typename T>
void ObjectPool<T>::Remove(T* object) {
    typename std::vector<T*>::iterator it = std::find(inUse.begin(), inUse.end(), object);
    if (it != inUse.end()) {
        inUse.erase(it);    // Remove from the in-use list
        freeList.push_back(object); // Return to the free list for reuse
    }
}

// Clear all objects from the pool (useful for cleanup)
template <typename T>
void ObjectPool<T>::Clear() {
    freeList.clear();
    inUse.clear();
    pool.clear();
}

