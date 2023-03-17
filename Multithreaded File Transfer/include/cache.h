#include <vector>
#include <string>
#include <winsock2.h>
#include <unordered_map>

#define MAX_ESPACE 64000000

class Cache {

private:

    std::unordered_map<std::string,int> cache_server;
    int sizeTotal = 0;

public:
    Cache();
    ~Cache();

    void getCacheList(SOCKET client);
    void addFileOnCache(std::string name_, int size_);
    bool findFileOnCache(std::string name_);
    void removeFileOnCache();
    int getCacheSize();
    bool verifySpaceAvalible(int size_);


};
