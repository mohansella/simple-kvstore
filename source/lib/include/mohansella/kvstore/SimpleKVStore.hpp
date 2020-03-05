#pragma once

#include <string>
#include <memory>

#include <mohansella/kvstore/StoreValue.hpp>
#include <mohansella/kvstore/ErrorCode.hpp>

namespace mohansella::kvstore 
{

    class SimpleKVStore 
    {

    public:
        SimpleKVStore();
        SimpleKVStore(const std::string & filePath);
        virtual ~SimpleKVStore();

        std::string getFilePath() const;
        
        ErrorCode create(const std::string & key, StoreValue & value, std::int32_t ttlInSecs = 0);
        ErrorCode read(const std::string & key, StoreValue & value);
        ErrorCode remove(const std::string & key);

        std::int32_t getCount(); //total key value pairs
        std::int64_t getSize();  //total size in bytes

    private:

        struct Data;
        std::unique_ptr<Data> data;

    };

}