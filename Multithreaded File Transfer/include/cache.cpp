#include "cache.h"
#include <cstdlib>

Cache::Cache() {
}

Cache::~Cache(){
}

void Cache::getCacheList(SOCKET client) {
    std::string text = {} ;

    if(cache_server.empty()){
        text+= "[LOG] - Server cache is empty";
        send(client , text.c_str() , text.length() , 0);
    }

    else{
        for (auto& [name, size]: cache_server) {
            text += "[CACHE] - "; text += name; text += " is in the cache. \n";
        }
        send(client , text.c_str() , text.length() , 0);
    }
}

void Cache::addFileOnCache(std::string name_, int size_) {
    cache_server.insert(cache_server.begin(),{name_,size_});
    sizeTotal += size_;
}

bool Cache::findFileOnCache(std::string name_) {
    if(cache_server.contains(name_)) return true;
    else return false;
}

void Cache::removeFileOnCache(){

    auto it = cache_server.begin();
    sizeTotal -= it->second;
    cache_server.erase(it);

}

int Cache::getCacheSize(){
    return sizeTotal;
}

bool Cache::verifySpaceAvalible(int size_) {
    if((sizeTotal + size_) > MAX_ESPACE) return true;
    else return false;
}
