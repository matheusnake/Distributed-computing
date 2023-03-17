# A Multithreaded File Transfer with Cache and Locking
![C](https://img.shields.io/badge/C++-00599C?style=flat-square&logo=C%2B%2B&logoColor=white)


- [Getting Started](#Getting_Started)
  - [Tools](#tools)
  - [Running the application](#Running_the_application) 
- [The Project](#project)
  - [Design Decisions](#design)
  - [Libraries](#lib)
- [Overview](#overview)
  - [Client](#client)
  - [Server](#server)
    - [Thread](#thread)
    - [lock](#lock)
    - [Cache](#cache)
***
## <a id="Getting_Started" />Getting Started
### <a id="Tools" />tools

- Windows 11
- Compiler Version : gcc-12.2.0-llvm-15.0.7-mingw-w64ucrt-10.0.0-r4 (winlibs)

### <a id="Running_the_application" />Running the application

Server:
```sh
run "runServer.bat" for compile
```

Client:
```sh
run "runClient.bat" for compile
-> ./client.exe ip port filename
```

##  <a id="project" />The Project

###  <a id="design"/>Design Decisions
The purpose of the project is to build a TPC server for transferring files between client -> server, with some requesites:

- Must be Multithreaded
- Have a cache system with 64MB max capacite
- Unsure Mutex and Locking

###  <a id="lib"/>Libraries

- thread
- mutex
- winsock2.h
- map
- filesystem

##  <a id="overview" />Overview

### <a id="client" />Client

The client can communicate with the server in two ways passing in the arguments:
- "list" -> Request to show files in cache
- filename -> Name of the file that will be sent to the server

example:

```sh
./client.exe 127.0.0.1 5555 ih.txt
```

basic flow that the client will do after connecting to the server

```sh
	if(list == "list"){
		cout << "[Server] - Cache List" << endl;
	}
	else{
        #protocol 1 is for success find the filename
		if(strcmp(buffer, "1") == 0) {
        #protocol 2 is for success if the file is bigger than 64MB
				if(strcmp(buffer, "2") == 0) {
					cout << "[Error] - Data Size is bigger than 64MB" << endl;	
				}
        # else will recv the packet
				else {
					recvFileData(server,list);
				}
		} else {
        #file not found
		}
	}
```

### <a id="server" />Server

The server, will wait for client connections, after a client connects it will have the following flow::

- The client's first request for list will return the list of files that are in the cache;
- If the client's first request is a "filename", it will look to see if such a file exists;
- If it exists, it will look in the cache if the file exists;
  - If it is not in the cache, it will check if it has available space in the cache;
      - If you have space, it will add to the cache and transfer the file;
      - If there is no space, it will delete using the policy (FIFO) until it has free space;
  - If it is in the cache, it will receive the file;
- If filename not exist, will send to the client a error;

### <a id="thread" />Thread

Every time a new client enters on server, a thread will be triggered with the on_client_connect() function that has the same flow described in the previous section.

```sh
    SOCKET client;

    if ((client = accept(server, reinterpret_cast<SOCKADDR *>(&client_addr), &client_addr_size)) != INVALID_SOCKET) {
        std::thread t([=]() {
            on_client_connect(client,client_addr);
        });
        t.detach();
    }
```

### <a id="lock" />Lock

To ensure that the file data is not inconsistent, it was necessary to guarantee mutual exclusion using the "mutex" lib, where every time we use a cache, we will execute a lock so that there is no critical section between the data.

```sh
mutex clients_mtx;

clients_mtx.lock();

{
    //do stuff
}

clients_mtx.unlock();
closesocket(client);

```

### <a id="cache" />Cache

An object was created to manage cache movements.
The cache has a data structure like "map" where the key is the file name and value is the file size and has the following functions:

```sh

    void getCacheList(SOCKET client);
    void addFileOnCache(std::string name_, int size_);
    bool findFileOnCache(std::string name_);
    void removeFileOnCache();
    bool verifySpaceAvalible(int size_);
    int getCacheSize();

```