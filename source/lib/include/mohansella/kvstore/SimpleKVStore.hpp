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
        SimpleKVStore(const std::string & filePath = {}, std::int64_t limit = 1024 * 1024 * 1024); //1GB
        virtual ~SimpleKVStore();

        std::string getFilePath() const;
        
        ErrorCode create(const std::string & key, StoreValue & value, std::int32_t ttlInSecs = 0);
        ErrorCode read(const std::string & key, StoreValue & value);
        ErrorCode remove(const std::string & key);

        void purge();
        void display();

        ErrorCode store();//stores the current state to its file

    private:
        ErrorCode load();

        struct Data;
        std::unique_ptr<Data> data;

    };

}