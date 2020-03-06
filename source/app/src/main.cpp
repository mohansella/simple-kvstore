#include <main.hpp>

#include <locale>
#include <codecvt>
#include <iostream>

#include <boost/program_options.hpp>

#include <mohansella/kvstore/SimpleKVStore.hpp>

using namespace mohansella::kvstore;

namespace po = boost::program_options;

#ifdef WIN32
	int wmain(int argc, const wchar_t *argv[])
    {
        return SimpleKVStoreMain::main(argc, argv);
    }
#else // WIN32
	int main(int argc, const char* argv[])
    {
        return SimpleKVStoreMain::main(argc, argv);
    }
#endif // WIN32

struct SimpleKVStoreMain::Data
{
    static std::int32_t unsafeMain(po::options_description & desc, po::variables_map & vm, bool showHelp, bool entryHandled);
    static void showCodes();
};

std::int32_t SimpleKVStoreMain::main(int argc, const char* argv[])
{
    std::vector<std::string> args;
    for(auto i = 0; i < argc; i++)
    {
        args.push_back(argv[i]);
    }
    return SimpleKVStoreMain::main(args);
}

std::int32_t SimpleKVStoreMain::main(int argc, const wchar_t* argv[])
{
    std::vector<std::string> args;
    
    //setup converter
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;

    for(auto i = 0; i < argc; i++)
    {
        std::wstring curArg = argv[i];
        args.push_back(converter.to_bytes(curArg));
    }
    return SimpleKVStoreMain::main(args);
}

std::int32_t SimpleKVStoreMain::main(std::vector<std::string> & args)
{

    po::options_description desc("SimpleKVStore: a command line argument parser to experiment the library SimpleKVStore(a simple key-value store library)");

    desc.add_options()
			("help,h", "show help")
			("limit,l", po::value<std::int64_t>()->default_value(1024 * 1024 * 1024), "file max size. default is 1GB")
			("filepath,p", po::value<std::string>()->default_value(""), "path to key-value store file")
			("operation,o", po::value<std::string>(), "operations can be show, create, read, delete, codes")
			("key,k", po::value<std::string>(), "key is required from most of the CRD operations")
			("value,v", po::value<std::string>(), "flow name to parse message")
			("timetolive,t", po::value<std::int32_t>()->default_value(0), "optional time to live in seconds. required for create operation");

    auto showHelp = false;
    auto entryHandled = false;

    auto argc = (int) args.size();
    auto argv = std::make_unique<const char*[]>(argc);
    for (auto i = 0; i < argc; i++)
    {
        argv[i] = args[i].c_str();
    }

    //parse commandline arguments
    po::variables_map vm;
    try
    {
        po::store(po::parse_command_line(argc, argv.get(), desc), vm);
    }
    catch (std::exception& e)
    {
        std::cout << "[error] invalid arguments. errmsg:" << e.what() << std::endl;
        showHelp = true;
        entryHandled = true;
    }
    po::notify(vm);

    //handled parsed arguments
    try
    {
        return Data::unsafeMain(desc, vm, showHelp, entryHandled);
    }
    catch(const std::exception& e)
    {
        std::cout << "[error] exception while handling commandline operation. errmsg:" << e.what() << std::endl;
        return -1;
    }
    
}

std::int32_t SimpleKVStoreMain::Data::unsafeMain(po::options_description & desc, po::variables_map & vm, bool showHelp, bool entryHandled)
{
    std::int32_t toReturn = 0;

    if(vm.count("help"))
    {
        entryHandled = true;
    }

    if(!entryHandled && vm.count("operation"))
    {
        auto filePath = vm["filepath"].as<std::string>();
        auto limit = vm["limit"].as<std::int64_t>();
        auto operation = vm["operation"].as<std::string>();
        
        std::unique_ptr<SimpleKVStore> _kvStore;
        auto getKvStore = [&]()
        {
            if(!_kvStore)
            {
                _kvStore = std::make_unique<SimpleKVStore>(filePath, limit);
            }
            return _kvStore.get();
        };

        ErrorCode code = ErrorCode::CODE_ZERO;
        entryHandled = true;
        if(operation == "show")
        {
            auto kvStore = getKvStore();
            kvStore->display();
        }
        else if(operation == "codes")
        {
            Data::showCodes();
        }
        else if(operation == "create" && vm.count("key") & vm.count("value"))
        {
            auto key = vm["key"].as<std::string>();
            auto value = vm["value"].as<std::string>();
            auto ttl = vm["timetolive"].as<std::int32_t>();
            auto kvStore = getKvStore();
            StoreValue storeValue(value);
            code = kvStore->create(key, storeValue, ttl);
        }
        else if(operation == "read" && vm.count("key"))
        {
            auto key = vm["key"].as<std::string>();
            auto kvStore = getKvStore();
            StoreValue storeValue;
            code = kvStore->read(key, storeValue);
            if(code >= ErrorCode::CODE_ZERO)
            {
                if(storeValue.isString())
                {
                    std::cout << *storeValue.asString() << std::endl;
                }
                else if(storeValue.isInteger())
                {
                    std::cout << *storeValue.asInteger() << std::endl;
                }
                else
                {
                    std::cout << "UNKNOWN_TYPE["  << storeValue.getType() << "]" << std::endl;
                }
            }
        }
        else if(operation == "delete" && vm.count("key"))
        {
            auto key = vm["key"].as<std::string>();
            auto kvStore = getKvStore();
            code = kvStore->remove(key);
        }
        else
        {
            entryHandled = false;
        }

        if(code < ErrorCode::CODE_ZERO)
        {
            std::cout << "[error] operation:" << operation << " failed with code:" << code << std::endl;
        }

    }

    if (!entryHandled || showHelp || vm.count("help"))
    {
        std::cout << desc << std::endl;
        entryHandled = true;
        toReturn = 1;
    }

    return toReturn;
}

void SimpleKVStoreMain::Data::showCodes()
{
    std::cout << "-1 : LARGE_KEY" << std::endl;
    std::cout << "-2 : LARGE_VALUE" << std::endl;
    std::cout << "-3 : KEY_ALREADY_EXISTS" << std::endl;
    std::cout << "-4 : KEY_NOT_EXISTS" << std::endl;
    std::cout << "-5 : SIZE_LIMIT_REACHED" << std::endl;
    std::cout << "-6 : FILE_NOT_FOUND" << std::endl;
    std::cout << "-7 : FILE_READ_FAILED" << std::endl;
    std::cout << "-8 : FILE_WRITE_FAILED" << std::endl;
}
 