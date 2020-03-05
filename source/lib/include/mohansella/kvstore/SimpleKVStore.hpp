#pragma once

#include <string>
#include <memory>

namespace mohansella::kvstore 
{

    class SimpleKVStore 
    {

    public:
        SimpleKVStore();
        SimpleKVStore(const std::string & filePath);
        virtual ~SimpleKVStore();

    private:

        struct Data;
        std::unique_ptr<Data> data;

    };

}