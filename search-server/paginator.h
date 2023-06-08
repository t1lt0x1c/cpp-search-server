#pragma once

#include <iostream>
#include <vector>

template <typename Iterator>
class IteratorRange {
public:
        IteratorRange(Iterator begin, Iterator end, size_t size) 
        :begin_(begin), 
         end_(end), 
         size_(size)
        {    
        }
        auto begin() {
            return begin_;
            }
        auto end() {
            return end_;
        }
        auto size() {
            return size_;
        }
private:
    Iterator begin_;
    Iterator end_;
    size_t size_;
};

template <typename Iterator>
class Paginator {
public:
    Paginator(Iterator begin, Iterator end, size_t size) {
        bool i = true;
        while (i){
            if (distance(begin, end) > size){
                auto it = begin;
                advance(it, size);
                data.push_back(IteratorRange(begin, it, size));
                begin = it;
            } else {
                data.push_back(IteratorRange(begin, end, size));
                i = false;
            }        
        }
    }     
    auto begin() const {
        return data.begin();
    }
    auto end() const {
        return data.end();
    }
    private:
        std::vector<IteratorRange<Iterator>> data;
}; 

template <typename Container>
auto Paginate(const Container& c, size_t page_size) {
    return Paginator(begin(c), end(c), page_size);
}