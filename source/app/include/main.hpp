#pragma once

#include <cstdint>
#include <vector>
#include <string>

#ifdef WIN32
	int wmain(int argc, const wchar_t *argv[]);
#else // WIN32
	int main(int argc, const char* argv[]);
#endif // WIN32

class SimpleKVStoreMain
{

public:
    static std::int32_t main(int argc, const char* argv[]);
    static std::int32_t main(int argc, const wchar_t* argv[]);
    static std::int32_t main(std::vector<std::string> & args);

private:
    struct Data;

};