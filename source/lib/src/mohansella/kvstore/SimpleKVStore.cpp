#include <mohansella/kvstore/SimpleKVStore.hpp>

#include <shared_mutex>
#include <fstream>
#include <unordered_map>

namespace mohansella::kvstore
{

    struct StoreValueContainer
    {
        StoreValue value;
        std::int32_t expiresOn = 0;
    };

    struct SimpleKVStore::Data
    {
        std::string filePath;
        std::shared_mutex sharedMutex;
        std::unordered_map<std::string, StoreValueContainer> kvMap;

        void initFilePath(const std::string & filePath);
        std::int32_t getCurrEpochSeconds();
    };

    SimpleKVStore::SimpleKVStore(const std::string & filePath)
    {
        this->data = std::make_unique<Data>();
        this->data->initFilePath(filePath);
    }

    SimpleKVStore::~SimpleKVStore()
    {
        this->flush();
    }

    std::string SimpleKVStore::getFilePath() const
    {
        return data->filePath;
    }
    
    ErrorCode SimpleKVStore::create(const std::string & key, StoreValue & value, std::int32_t ttlInSecs)
    {
        std::scoped_lock lock(data->sharedMutex);
        auto pos = data->kvMap.find(key);
        auto currEpochSeconds = data->getCurrEpochSeconds();
        if(pos == data->kvMap.end()) //new kv pair
        {
            StoreValueContainer container;
            container.value = std::move(value);
            container.expiresOn = currEpochSeconds+ ttlInSecs;
            data->kvMap.insert({key, std::move(container)});
            return ErrorCode::CODE_ZERO;
        }
        else
        {
            if(pos->second.expiresOn != 0 && pos->second.expiresOn < currEpochSeconds) //already expired
            {
                pos->second.value = std::move(value);
                pos->second.expiresOn = currEpochSeconds + ttlInSecs;
                return ErrorCode::CODE_ZERO;
            }
            else //return error
            {
                return ErrorCode::CODE_FAILURE_KEY_ALREADY_EXISTS;
            }
        }
    }

    ErrorCode SimpleKVStore::read(const std::string & key, StoreValue & value)
    {
        std::shared_lock lock(data->sharedMutex);
        auto pos = data->kvMap.find(key);
        if(pos == data->kvMap.end())
        {
            return ErrorCode::CODE_FAILURE_KEY_NOT_EXISTS;
        }
        else
        {
            auto currEpochSeconds = data->getCurrEpochSeconds();
            if(currEpochSeconds <= pos->second.expiresOn)
            {
                pos->second.value.copyTo(value);
                return ErrorCode::CODE_ZERO;
            }
            else
            {
                return ErrorCode::CODE_FAILURE_KEY_NOT_EXISTS;
            }
        }
    }

    ErrorCode SimpleKVStore::remove(const std::string & key)
    {
        std::scoped_lock lock(data->sharedMutex);
        auto pos = data->kvMap.find(key);
        if(pos != data->kvMap.end())
        {
            auto currEpochSeconds = data->getCurrEpochSeconds();
            auto isExpired = pos->second.expiresOn < currEpochSeconds;
            data->kvMap.erase(pos);
            if(isExpired)
            {
                return ErrorCode::CODE_FAILURE_KEY_NOT_EXISTS;
            }
            else
            {
                return ErrorCode::CODE_ZERO;
            }
        }
        else
        {
            return ErrorCode::CODE_FAILURE_KEY_NOT_EXISTS;
        }
    }

    void SimpleKVStore::purge()
    {
        std::scoped_lock lock(data->sharedMutex);
        auto pos = data->kvMap.begin();
        auto currEpochSeconds = data->getCurrEpochSeconds();
        while(pos != data->kvMap.end())
        {
            if(pos->second.expiresOn < currEpochSeconds)
            {
                pos = data->kvMap.erase(pos);
            }
            else
            {
                pos++;
            }
        }
    }

    ErrorCode SimpleKVStore::flush()
    {
        this->purge();
        return ErrorCode::CODE_ZERO;
    }

    void SimpleKVStore::Data::initFilePath(const std::string & filePath)
    {

    }

}