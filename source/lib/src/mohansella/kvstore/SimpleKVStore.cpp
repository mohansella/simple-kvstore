#include <mohansella/kvstore/SimpleKVStore.hpp>

#include <shared_mutex>
#include <fstream>
#include <unordered_map>

namespace mohansella::kvstore
{

    struct SimpleKVStore::Data
    {
        std::string filePath;
        std::shared_mutex sharedMutex;
    };

    SimpleKVStore::SimpleKVStore()
    {
        
    }

    SimpleKVStore::SimpleKVStore(const std::string & filePath)
    {

    }

    SimpleKVStore::~SimpleKVStore()
    {

    }

}