#include <mohansella/kvstore/SimpleKVStore.hpp>

#include <shared_mutex>
#include <unordered_map>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <ctime>
#include <chrono>

#include <boost/interprocess/sync/file_lock.hpp>

#include <mohansella/serial/FileWriter.hpp>
#include <mohansella/serial/FileReader.hpp>

namespace mohansella::kvstore
{

    using namespace mohansella::serial;

    struct StoreValueContainer
    {
        StoreValue value;
        std::int32_t expiresOn = 0;
    };

    struct SimpleKVStore::Data
    {
        std::int64_t currSize = 0;
        std::int64_t limit;
        std::string filePath;
        std::string lockFilePath;
        std::shared_mutex sharedMutex;
        std::unordered_map<std::string, StoreValueContainer> kvMap;

        std::unique_ptr<boost::interprocess::file_lock> fileLock;

        void initFilePath(const std::string & filePath);
        std::int32_t getCurrEpochSeconds();
    };

    SimpleKVStore::SimpleKVStore(const std::string & filePath, std::int64_t limit)
    {
        this->data = std::make_unique<Data>();
        this->data->limit = limit;
        this->data->initFilePath(filePath);
        auto code = this->load();
        if(code < 0 && code != ErrorCode::CODE_FAILURE_FILE_NOT_FOUND)
        {
            std::cout << "unexpected error occured. code:" << code << ". throwing exception. check logs" << std::endl;
            throw std::exception("SimpleKVStore: unexpected error occured while deserializing"); 
        }
        this->purge();
    }

    SimpleKVStore::~SimpleKVStore()
    {
        this->store();
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
            container.expiresOn = ttlInSecs ? currEpochSeconds + ttlInSecs : 0;
            if(data->currSize > data->limit) //size_correction
            {
                return ErrorCode::CODE_FAILURE_SIZE_LIMIT_REACHED;
            }
            else
            {
                data->currSize += key.size() + value.size();
            }
            data->kvMap.insert({key, std::move(container)});
            return ErrorCode::CODE_ZERO;
        }
        else
        {
            if(pos->second.expiresOn != 0 && pos->second.expiresOn < currEpochSeconds) //already expired
            {
                data->currSize += value.size() - pos->second.value.size(); //size_correction
                pos->second.value = std::move(value);
                pos->second.expiresOn = ttlInSecs ? currEpochSeconds + ttlInSecs : 0;
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
            auto & expiresOn = pos->second.expiresOn;
            if(expiresOn == 0 || currEpochSeconds <= pos->second.expiresOn)
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
            auto isExpired = pos->second.expiresOn ? pos->second.expiresOn < currEpochSeconds : false;
            data->currSize -= key.size() + pos->second.value.size(); //size_correction
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
            auto & expiresOn = pos->second.expiresOn;
            if(expiresOn && expiresOn < currEpochSeconds)
            {
                data->currSize -= (pos->first.length() + pos->second.value.size()); //size_correction
                pos = data->kvMap.erase(pos);
            }
            else
            {
                pos++;
            }
        }
    }

    void SimpleKVStore::display()
    {
        std::scoped_lock lock(data->sharedMutex);
        auto pos = data->kvMap.begin();
        auto currEpochSeconds = data->getCurrEpochSeconds();
        std::cout << "showing " << data->kvMap.size() << " entires" << std::endl;
        for(auto & kvPair : data->kvMap)
        {
            std::cout << kvPair.first << ":";

            auto & expiresOn = kvPair.second.expiresOn;
            auto & storeValue = kvPair.second.value;
            if(storeValue.isString())
            {
                std::cout << *storeValue.asString();
            }
            else if(storeValue.isInteger())
            {
                std::cout << *storeValue.asInteger();
            }
            else
            {
                std::cout << "UNKNOWN_TYPE["  << storeValue.getType() << "]";
            }

            if(expiresOn)
            {
                auto delta = expiresOn - currEpochSeconds;
                std::cout << " expiresIn:" << delta << " seconds";
            }
            std::cout << std::endl;
        }
    }

    ErrorCode SimpleKVStore::store()
    {
        std::scoped_lock lock(data->sharedMutex);
        try
        {
            FileWriter writer(data->filePath);
            writer.startArray();
            for(const auto & kvPair : data->kvMap)
            {
                writer.putString(kvPair.first);
                writer.putInteger(kvPair.second.expiresOn);
                auto & value = kvPair.second.value;
                switch (value.getType())
                {
                case StoreValue::Type::Integer:
                    {
                        writer.putLong(*value.asInteger());
                        break;                        
                    }
                case StoreValue::Type::String:
                    {
                        writer.putString(*value.asString());
                        break;
                    }
                default:
                    {
                        writer.putNull();
                        break;
                    }
                }
            }
            writer.endArray();
            return ErrorCode::CODE_ZERO;
        }
        catch(std::exception & e)
        {
            std::cout << "exception occured while writing to file:" << data->filePath << ". reason:" << e.what() << std::endl;
            return ErrorCode::CODE_FAILURE_FILE_WRITE_FAILED;
        }
    }

    ErrorCode SimpleKVStore::load()
    {
        if(std::filesystem::exists(data->filePath))
        {
            try
            {
                FileReader reader(data->filePath);
                reader.startArray();
                auto syntax = reader.peekSyntax();
                while(syntax != ByteSerialType::BSTYPE_ARRAY_END)
                {
                    std::string key;
                    reader.readString(key);
                    auto expiresOn = reader.readInteger();
                    auto valType = reader.peekSyntax();
                    StoreValueContainer container;
                    container.expiresOn = expiresOn;
                    switch (valType)
                    {
                        case ByteSerialType::BSTYPE_INTEGER:
                            {
                                StoreValue value(reader.readLong());
                                container.value = std::move(value);
                                data->currSize += key.size() + container.value.size(); //size_correction
                                data->kvMap.insert({key, std::move(container)});
                                break;
                            }
                        case ByteSerialType::BSTYPE_STRING:
                            {
                                std::string strValue;
                                reader.readString(strValue);
                                StoreValue value(strValue);
                                container.value = std::move(value);
                                data->currSize += key.size() + container.value.size(); //size_correction
                                data->kvMap.insert({key, std::move(container)});
                                break;
                            }
                        default:
                            {
                                reader.readNull();
                                break;
                            }
                    }
                    syntax = reader.peekSyntax();
                }
                reader.endArray();
                return ErrorCode::CODE_ZERO;
            }
            catch(std::exception & e)
            {
                std::cout << "exception occured while reading from file:" << data->filePath << ". reason:" << e.what() << std::endl;
                return ErrorCode::CODE_FAILURE_FILE_READ_FAILED;
            }
        }
        else
        {
            return ErrorCode::CODE_FAILURE_FILE_NOT_FOUND;
        }
    }

    void SimpleKVStore::Data::initFilePath(const std::string & filePath)
    {
        if(filePath.empty())
        {
            this->filePath = "kvstore.data"; //default file name
        }
        else
        {
            this->filePath = filePath;
        }

        //create lockfile if not exists
        this->lockFilePath = this->filePath + ".lock";
        if(!std::filesystem::exists(this->lockFilePath))
        {
            std::ofstream ofStream(this->lockFilePath);
        }

        this->fileLock = std::make_unique<boost::interprocess::file_lock>(this->lockFilePath.c_str());
        if(!this->fileLock->try_lock())
        {
            std::cout << "unable to lock filePath:" << this->lockFilePath << std::endl;
            throw std::exception("SimpleKVStore: unable to lock filepath");
        }
    }

    std::int32_t SimpleKVStore::Data::getCurrEpochSeconds()
    {
        return (std::int32_t) std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    }

}