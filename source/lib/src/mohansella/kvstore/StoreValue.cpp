#include <mohansella/kvstore/StoreValue.hpp>


#include <memory>

namespace mohansella::kvstore
{

    StoreValue::StoreValue()
    {
        this->valueType = StoreValue::Type::Unknown;
    }

    StoreValue::StoreValue(std::int64_t value)
    {
        this->valueType = StoreValue::Type::Integer;
        this->sizeVal = sizeof(value);
        this->pointer.integer = value;
    }

    StoreValue::StoreValue(std::string & value)
    {
        this->valueType = StoreValue::Type::String;
        this->sizeVal = value.length();
        auto ptr = std::make_unique<std::string>();
        *ptr = std::move(value);
        this->pointer.string = ptr.release();
    }

    StoreValue::StoreValue(StoreValue && value)
    {
        this->valueType = StoreValue::Type::Unknown;
        this->operator=(std::move(value));
    }
    
    StoreValue & StoreValue::operator=(StoreValue && other)
    {
        this->~StoreValue();
        
        //copy from other
        this->pointer = other.pointer;
        this->valueType = other.valueType;
        this->sizeVal = other.sizeVal;

        //reset other
        other.pointer = {0};
        other.valueType = StoreValue::Type::Unknown;
        other.sizeVal = 0;

        return *this;
    }

    StoreValue::~StoreValue()
    {
        switch (this->valueType)
		{
            case StoreValue::Type::Unknown:
                {
                    break; //nop
                }
            case StoreValue::Type::Integer:
                {
                    break; //nop
                }
            case StoreValue::Type::String:
                {
                    std::unique_ptr<std::string>(this->pointer.string);
                    break;
                }
            default:
                break;
        }
    }

    StoreValue::Type StoreValue::getType() const
    {
        return this->valueType;
    }

    std::int32_t StoreValue::size() const
    {
        return this->sizeVal;
    }
    
    bool StoreValue::isInteger() const
    {
        return this->getType() == StoreValue::Type::Integer;
    }

    bool StoreValue::isString() const
    {
        return this->getType() == StoreValue::Type::String;
    }

    const std::int64_t * StoreValue::asInteger() const
    {
        return this->isInteger() ? &this->pointer.integer : nullptr;
    }

    const std::string * StoreValue::asString() const
    {
        return this->isString() ? this->pointer.string : nullptr;
    }

    void StoreValue::copyTo(StoreValue & other) const
    {
        other.~StoreValue();
        other.valueType = this->valueType;
        other.sizeVal = this->sizeVal;
        other.pointer = {0};
        switch (this->valueType)
        {
            case StoreValue::Type::Integer:
                {
                    other.pointer.integer = this->pointer.integer;
                    break; //nop
                }
            case StoreValue::Type::String:
                {
                    auto ptr = std::unique_ptr<std::string>();
                    *ptr = *this->pointer.string;
                    other.pointer.string = ptr.release();
                    break;
                }
            default:
                break;
        }
    }

}