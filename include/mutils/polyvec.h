/// Copyright (c) 2023 Samir Bioud
/// 
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
/// 
/// The above copyright notice and this permission notice shall be included in all
/// copies or substantial portions of the Software.
/// 
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
/// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
/// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
/// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
/// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
/// OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
/// OR OTHER DEALINGS IN THE SOFTWARE.
/// 

#pragma once

#include<memory>
#include<vector>
namespace mutils{


/**
 * A Vector-Like class which supports polymorphic types
 * A PolyVec takes ownership of its contained items
*/
template<class _Items>
class PolyVec{
    std::vector<std::unique_ptr<_Items>> m_data;
public:

    struct Iterator{
        using iterator_category = std::random_access_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type        = _Items&;
        using pointer           = std::unique_ptr<_Items>*;
        using refefence         = std::unique_ptr<_Items>&;

        value_type operator*() const { return **m_ptr; }
        pointer operator->() { return m_ptr; }

        Iterator(pointer ptr) : m_ptr(ptr){};

        Iterator& operator++(){ m_ptr++; return *this; } 
        Iterator operator++(int) { 
            Iterator tmp = *this;
            ++(*this);
            return tmp; 
        }


        friend bool operator== (const Iterator& a, const Iterator& b) { return a.m_ptr == b.m_ptr; };
        friend bool operator!= (const Iterator& a, const Iterator& b) { return a.m_ptr != b.m_ptr; }; 

    private:
        pointer m_ptr;
    };

    struct ConstIterator{
        using iterator_category = std::random_access_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type        = _Items&;
        using pointer           = std::unique_ptr<_Items> const*;
        using refefence         = std::unique_ptr<_Items>&;

        value_type operator*() const { return **m_ptr; }
        const pointer operator->() const { return m_ptr; }

        ConstIterator(pointer ptr) : m_ptr(ptr){};

        ConstIterator& operator++(){ m_ptr++; return *this; } 
        ConstIterator operator++(int) { 
            ConstIterator tmp = *this;
            ++(*this);
            return tmp; 
        }


        friend bool operator== (const ConstIterator& a, const ConstIterator& b) { return a.m_ptr == b.m_ptr; };
        friend bool operator!= (const ConstIterator& a, const ConstIterator& b) { return a.m_ptr != b.m_ptr; }; 

    private:
        pointer m_ptr;
    };

    PolyVec() = default;
    PolyVec(size_t reserve_count) : m_data(reserve_count){}


    /**
     * Push a unique_ptr to the vector, simply transfers ownership
    */
    template<typename T>
    void push(std::unique_ptr<T> item){
        static_assert(std::is_base_of<_Items, T>::value, "You cannot push an item to a PolyVec which does not inherit from the Base Class");
        m_data.push_back(std::move(item));
    }

    /**
     * Transfer ownership of a class instance to the polyvec
    */
    template<typename T>
    void push(T&& item){
        static_assert(std::is_base_of<_Items, T>::value, "You cannot push an item to a PolyVec which does not inherit from the Base Class");
        m_data.push_back(std::make_unique<T>(std::move(item)));
    }

    std::unique_ptr<_Items> pop(){
        return m_data.pop_back();
    }

    std::unique_ptr<_Items> pop_at(size_t idx){
        auto item = std::move(m_data[idx]);
        m_data.erase(m_data.begin()+idx);
        return item;
    }

    _Items& operator[](size_t idx){
        auto& item = m_data[idx];
        return *item;
    }

    size_t size() const{
        return m_data.size();
    }

    size_t capacity() const{
        return m_data.capacity();
    }

    void reserve(size_t n){
        m_data.reserve(n);
    }

    void clear(size_t n){
        m_data.clear();
    }

    bool empty() const{
        return m_data.empty();
    }


    Iterator begin(){
        return &*m_data.begin();
    }
    Iterator end(){
        return &*m_data.end();
    }

    Iterator rbegin(){
        return &*m_data.rbegin();
    }
    Iterator rend(){
        return &*m_data.rend();
    }

    Iterator back(){
        return &*m_data.back();
    }

    ConstIterator begin() const{
        return &*m_data.cbegin();
    }
    ConstIterator end() const{
        return &*m_data.cend();
    }

};
};
