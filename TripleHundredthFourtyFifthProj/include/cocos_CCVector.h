/****************************************************************************
Copyright (c) 2010 ForzeField Studios S.L. http://forzefield.com
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2013-2014 Chukong Technologies

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/
#ifndef __CCVECTOR_H__
#define __CCVECTOR_H__

#include "cocos_ccMacros.h"
#include "cocos_CCRef.h"
#include <vector>
#include <functional>
#include <algorithm> // for std::find

NS_CC_BEGIN

template<class T>
class Vector
{
public:
    // ------------------------------------------
    // Iterators
    // ------------------------------------------
    typedef typename std::vector<T>::iterator iterator;
    typedef typename std::vector<T>::const_iterator const_iterator;
    
    typedef typename std::vector<T>::reverse_iterator reverse_iterator;
    typedef typename std::vector<T>::const_reverse_iterator const_reverse_iterator;
    
    iterator begin() { return _data.begin(); }
    const_iterator begin() const { return _data.begin(); }
    
    iterator end() { return _data.end(); }
    const_iterator end() const { return _data.end(); }
    
    const_iterator cbegin() const { return _data.cbegin(); }
    const_iterator cend() const { return _data.cend(); }
    
    reverse_iterator rbegin() { return _data.rbegin(); }
    const_reverse_iterator rbegin() const { return _data.rbegin(); }
    
    reverse_iterator rend() { return _data.rend(); }
    const_reverse_iterator rend() const { return _data.rend(); }
    
    const_reverse_iterator crbegin() const { return _data.crbegin(); }
    const_reverse_iterator crend() const { return _data.crend(); }
    
    /** Constructor */
    Vector<T>()
    : _data()
    {
        static_assert(std::is_convertible<T, Ref*>::value, "Invalid Type for cocos2d::Vector<T>!");
    }
    
    /** Constructor with a capacity */
    explicit Vector<T>(ssize_t capacity)
    : _data()
    {
        static_assert(std::is_convertible<T, Ref*>::value, "Invalid Type for cocos2d::Vector<T>!");
        CCLOGINFO("In the default constructor with capacity of Vector.");
        reserve(capacity);
    }

    /** Destructor */
    ~Vector<T>()
    {
        CCLOGINFO("In the destructor of Vector.");
        clear();
    }

    /** Copy constructor */
    Vector<T>(const Vector<T>& other)
    {
        static_assert(std::is_convertible<T, Ref*>::value, "Invalid Type for cocos2d::Vector<T>!");
        CCLOGINFO("In the copy constructor!");
        _data = other._data;
        addRefForAllObjects();
    }
    
    /** Move constructor */
    Vector<T>(Vector<T>&& other)
    {
        static_assert(std::is_convertible<T, Ref*>::value, "Invalid Type for cocos2d::Vector<T>!");
        CCLOGINFO("In the move constructor of Vector!");
        _data = std::move(other._data);
    }
    
    /** Copy assignment operator */
    Vector<T>& operator=(const Vector<T>& other)
    {
        if (this != &other) {
            CCLOGINFO("In the copy assignment operator!");
            clear();
            _data = other._data;
            addRefForAllObjects();
        }
        return *this;
    }
    
    /** Move assignment operator */
    Vector<T>& operator=(Vector<T>&& other)
    {
        if (this != &other) {
            CCLOGINFO("In the move assignment operator!");
            clear();
            _data = std::move(other._data);
        }
        return *this;
    }
    
// Don't uses operator since we could not decide whether it needs 'retain'/'release'.
//    T& operator[](int index)
//    {
//        return _data[index];
//    }
//    
//    const T& operator[](int index) const
//    {
//        return _data[index];
//    }
    
    /** @brief Request a change in capacity 
     *  @param capacity Minimum capacity for the vector.
     *         If n is greater than the current vector capacity, 
     *         the function causes the container to reallocate its storage increasing its capacity to n (or greater).
     */
    void reserve(ssize_t n)
    {
        _data.reserve(n);
    }
    
    /** @brief Returns the size of the storage space currently allocated for the vector, expressed in terms of elements.
     *  @note This capacity is not necessarily equal to the vector size. 
     *        It can be equal or greater, with the extra space allowing to accommodate for growth without the need to reallocate on each insertion.
     *  @return The size of the currently allocated storage capacity in the vector, measured in terms of the number elements it can hold.
     */
    ssize_t capacity() const
    {
        return _data.capacity();
    }
    
    /** @brief Returns the number of elements in the vector.
     *  @note This is the number of actual objects held in the vector, which is not necessarily equal to its storage capacity.
     *  @return The number of elements in the container.
     */
    ssize_t size() const
    {
        return  _data.size();
    }
    
    /** @brief Returns whether the vector is empty (i.e. whether its size is 0).
     *  @note This function does not modify the container in any way. To clear the content of a vector, see Vector<T>::clear.
     */
    bool empty() const
    {
        return _data.empty();
    }
    
    /** Returns the maximum number of elements that the vector can hold. */
    ssize_t max_size() const
    {
        return _data.max_size();
    }
    
    /** Returns index of a certain object, return UINT_MAX if doesn't contain the object */
    ssize_t getIndex(T object) const
    {
        auto iter = std::find(_data.begin(), _data.end(), object);
        if (iter != _data.end())
            return iter - _data.begin();

        return -1;
    }

    /** @brief Find the object in the vector.
     *  @return Returns an iterator to the first element in the range [first,last) that compares equal to val. 
     *          If no such element is found, the function returns last.
     */
    const_iterator find(T object) const
    {
        return std::find(_data.begin(), _data.end(), object);
    }
    
    iterator find(T object)
    {
        return std::find(_data.begin(), _data.end(), object);
    }
    
    /** Returns the element at position 'index' in the vector. */
    T at(ssize_t index) const
    {
        CCASSERT( index >= 0 && index < size(), "index out of range in getObjectAtIndex()");
        return _data[index];
    }

    /** Returns the first element in the vector. */
    T front() const
    {
        return _data.front();
    }
    
    /** Returns the last element of the vector. */
    T back() const
    {
        return _data.back();
    }

    /** Returns a random element of the vector. */
    T getRandomObject() const
    {
        if (!_data.empty())
        {
            ssize_t randIdx = rand() % _data.size();
            return *(_data.begin() + randIdx);
        }
        return nullptr;
    }

    /** Returns a Boolean value that indicates whether object is present in vector. */
    bool contains(T object) const
    {
        return( std::find(_data.begin(), _data.end(), object) != _data.end() );
    }

    /** Returns true if the two vectors are equal */
    bool equals(const Vector<T> &other)
    {
        ssize_t s = this->size();
        if (s != other.size())
            return false;
        
        for (ssize_t i = 0; i < s; i++)
        {
            if (this->at(i) != other.at(i))
            {
                return false;
            }
        }
        return true;
    }

    // Adds objects
    
    /** @brief Adds a new element at the end of the vector, after its current last element.
     *  @note This effectively increases the container size by one,
     *        which causes an automatic reallocation of the allocated storage space 
     *        if -and only if- the new vector size surpasses the current vector capacity.
     */
    void pushBack(T object)
    {
        CCASSERT(object != nullptr, "The object should not be nullptr");
        _data.push_back( object );
        object->retain();
    }
    
    /** Push all elements of an existing vector to the end of current vector. */
    void pushBack(const Vector<T>& other)
    {
        for(const auto &obj : other) {
            _data.push_back(obj);
            obj->retain();
        }
    }

    /** @brief Insert a certain object at a certain index 
     *  @note The vector is extended by inserting new elements before the element at the specified 'index',
     *        effectively increasing the container size by the number of elements inserted.
     *        This causes an automatic reallocation of the allocated storage space 
     *        if -and only if- the new vector size surpasses the current vector capacity.
     */
    void insert(ssize_t index, T object)
    {
        CCASSERT(index >= 0 && index <= size(), "Invalid index!");
        CCASSERT(object != nullptr, "The object should not be nullptr");
        _data.insert((std::begin(_data) + index), object);
        object->retain();
    }
    
    // Removes Objects

    /** Removes the last element in the vector, 
     *  effectively reducing the container size by one, decrease the referece count of the deleted object.
     */
    void popBack()
    {
        CCASSERT(!_data.empty(), "no objects added");
        auto last = _data.back();
        _data.pop_back();
        last->release();
    }
    
    /** @brief Remove a certain object in Vector.
     *  @param object The object to be removed.
     *  @param removeAll Whether to remove all elements with the same value.
     *                   If its value is 'false', it will just erase the first occurrence.
     */
    void eraseObject(T object, bool removeAll = false)
    {
        CCASSERT(object != nullptr, "The object should not be nullptr");
        
        if (removeAll)
        {
            for (auto iter = _data.begin(); iter != _data.end();)
            {
                if ((*iter) == object)
                {
                    iter = _data.erase(iter);
                    object->release();
                }
                else
                {
                    ++iter;
                }
            }
        }
        else
        {
            auto iter = std::find(_data.begin(), _data.end(), object);
            if (iter != _data.end())
            {
                _data.erase(iter);
                object->release();
            }
        }
    }

    /** @brief Removes from the vector with an iterator. 
     *  @param position Iterator pointing to a single element to be removed from the vector.
     *  @return An iterator pointing to the new location of the element that followed the last element erased by the function call.
     *          This is the container end if the operation erased the last element in the sequence.
     */
    iterator erase(iterator position)
    {
        CCASSERT(position >= _data.begin() && position < _data.end(), "Invalid position!");
        (*position)->release();
        return _data.erase(position);
    }
    
    /** @brief Removes from the vector with a range of elements (  [first, last)  ).
     *  @param first The beginning of the range
     *  @param last The end of the range, the 'last' will not used, it's only for indicating the end of range.
     *  @return An iterator pointing to the new location of the element that followed the last element erased by the function call.
     *          This is the container end if the operation erased the last element in the sequence.
     */
    iterator erase(iterator first, iterator last)
    {
        for (auto iter = first; iter != last; ++iter)
        {
            (*iter)->release();
        }
        
        return _data.erase(first, last);
    }
    
    /** @brief Removes from the vector with an index.
     *  @param index The index of the element to be removed from the vector.
     *  @return An iterator pointing to the new location of the element that followed the last element erased by the function call.
     *          This is the container end if the operation erased the last element in the sequence.
     */
    iterator erase(ssize_t index)
    {
        CCASSERT(!_data.empty() && index >=0 && index < size(), "Invalid index!");
        auto it = std::next( begin(), index );
        (*it)->release();
        return _data.erase(it);
    }

    /** @brief Removes all elements from the vector (which are destroyed), leaving the container with a size of 0.
     *  @note All the elements in the vector will be released (referece count will be decreased).
     */
    void clear()
    {
        for( auto it = std::begin(_data); it != std::end(_data); ++it ) {
            (*it)->release();
        }
        _data.clear();
    }

    // Rearranging Content

    /** Swap two elements */
    void swap(T object1, T object2)
    {
        ssize_t idx1 = getIndex(object1);
        ssize_t idx2 = getIndex(object2);

        CCASSERT(idx1>=0 && idx2>=0, "invalid object index");

        std::swap( _data[idx1], _data[idx2] );
    }
    
    /** Swap two elements with certain indexes */
    void swap(ssize_t index1, ssize_t index2)
    {
        CCASSERT(index1 >=0 && index1 < size() && index2 >= 0 && index2 < size(), "Invalid indices");

        std::swap( _data[index1], _data[index2] );
    }

    /** Replace object at index with another object. */
    void replace(ssize_t index, T object)
    {
        CCASSERT(index >= 0 && index < size(), "Invalid index!");
        CCASSERT(object != nullptr, "The object should not be nullptr");
        
        _data[index]->release();
        _data[index] = object;
        object->retain();
    }

    /** reverses the vector */
    void reverse()
    {
        std::reverse( std::begin(_data), std::end(_data) );
    }
    
    /** Shrinks the vector so the memory footprint corresponds with the number of items */
    void shrinkToFit()
    {
        _data.shrink_to_fit();
    }
    
protected:
    
    /** Retains all the objects in the vector */
    void addRefForAllObjects()
    {
        for(const auto &obj : _data) {
            obj->retain();
        }
    }
    
    std::vector<T> _data;
};

// end of data_structure group
/// @}

NS_CC_END

#endif // __CCVECTOR_H__
