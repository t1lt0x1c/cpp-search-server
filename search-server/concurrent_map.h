#pragma once

#include <algorithm>
#include <cstdlib>
#include <future>
#include <map>
#include <numeric>
#include <random>
#include <string>
#include <vector>

#include "log_duration.h"
//#include "test_framework.h"

using namespace std::string_literals;

template <typename Key, typename Value>
class ConcurrentMap {
public:
    static_assert(std::is_integral_v<Key>, "ConcurrentMap supports only integer keys");
    
    struct Bucket {
        //Bucket(size_t count) : map_(count), mutex_(count) {}
        std::map<Key, Value> map_;
        std::mutex mutex_;
    };
    struct Access {  
        Access(Key key, Bucket& bucket) : guard(bucket.mutex_), ref_to_value(bucket.map_[key]) {}  
        std::lock_guard<std::mutex> guard;
        Value& ref_to_value;     
    };

    explicit ConcurrentMap(size_t bucket_count) : buckets(bucket_count) {}
    
    Access operator[](const Key& key){
        uint64_t key_64 = static_cast<uint64_t>(key);
        auto index = key_64 % buckets.size();
        return {key, buckets[index]};
    };
    
    void Erase(const Key& key) {
        uint64_t key_64 = static_cast<uint64_t>(key);
        auto index = key_64 % buckets.size();
        std::lock_guard lock(buckets[index].mutex_);
        buckets[index].map_.erase(key);
    }
    
    std::size_t Size() {
        std::size_t result = 0;
        for (auto& [map, mutex] : buckets) {
            std::lock_guard lock(mutex);
            result += map.size();
        }
        return result;
    }
    
    std::map<Key, Value> BuildOrdinaryMap(){
        std::map<Key, Value> result;
        for (auto& [map, mutex] : buckets) {
            std::lock_guard lock(mutex);
            //std::cout << map.size() << std::endl;
            result.insert(map.begin(), map.end());
        }
        return result;
    };

private:
    // ...
    std::vector<Bucket> buckets;
};
