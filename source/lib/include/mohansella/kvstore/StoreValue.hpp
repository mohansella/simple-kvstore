#pragma once

#include <cstdint>
#include <string>

namespace mohansella::kvstore
{

    struct StoreValue
    {

        enum Type
        {
            Unknown = 0,
            Integer,
            String
        };

        StoreValue();
        StoreValue(std::int64_t value);
        StoreValue(std::string & value); //this constructor will move the value
        StoreValue(StoreValue && value); //move constructor
        StoreValue & operator=(StoreValue && value); //move assignment
        ~StoreValue();

        StoreValue::Type getType() const;
        std::int32_t size() const; //in bytes
        
        bool isInteger() const;
        bool isString() const;

        const std::int64_t * asInteger() const;
        const std::string * asString() const;

    private:

        union Pointer
		{
			std::int64_t integer;
			std::string * string;
		} pointer { 0 };
        StoreValue::Type valueType;
        std::int32_t sizeVal = 0;

        StoreValue(const StoreValue& from) = delete;            //delete copy constructor
		StoreValue& operator=(const StoreValue& from) = delete; //delete copy operator
    };

}