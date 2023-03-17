#include <iostream>
#include <thread>
#include <mutex>
#include <future>
#include <fstream>
#include <string>
#include <filesystem>
#include <winsock2.h>
#include <unistd.h>

#include "include/cache.cpp"

using namespace	std;
namespace fs = std::filesystem;
Cache cache;
mutex clients_mtx;


void sendFileData(SOCKET client, char buff[],SOCKADDR_IN server_addr) {
	fstream ifs;
	string file_name2 = "files/";
	file_name2.append(buff);

	ifs.open(file_name2, ios::in | ios::binary);

		if(ifs.is_open()){
			std::string contents((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
			cout << "[LOG:" << server_addr.sin_port << "] - Transmission Data Size " << contents.length() <<" Bytes.\n";
			int bytes_sent = send(client , contents.c_str() , contents.length() , 0);
		}else{
			cout <<"[Error:" << server_addr.sin_port << "] - Code: " << strerror(errno)<< endl;
		}

	ifs.close();
}

void on_client_connect (SOCKET client, SOCKADDR_IN server_addr){
	fstream ifs;
	string file_name = "files/";
	char buff[1024] = {};

	clients_mtx.lock();

	recv(client, buff, sizeof(buff), 0);

		if(strcmp(buff, "list") == 0){
			cout << "[LOG:" << server_addr.sin_port << "] - Seeding Cache List to client." << endl;
			cache.getCacheList(client);
		} else {
			file_name.append(buff);

			if(std::filesystem::exists(file_name)){
					send(client , "1" , 1024, 0);
					if(cache.findFileOnCache(buff)){
						send(client , "0" , 1024, 0);
        				std::cout << "[LOG:" << server_addr.sin_port << "] - Cache hit. File -> " << buff << " sent to the client." << std::endl;
						sendFileData(client,buff,server_addr);
   					} else {
						if(std::filesystem::file_size(file_name) > 64000000){
						send(client , "2" , 1024, 0);
							cout << "[Error" << server_addr.sin_port << "] - Data Size is bigger than 64MB" << endl;
						}
						else if (cache.verifySpaceAvalible(std::filesystem::file_size(file_name))){
							send(client , "0" , 1024, 0);
							std::cout << "[LOG:" << server_addr.sin_port << "] - The cache system is full" << std::endl;
							std::cout << "[LOG:" << server_addr.sin_port << "] - Erasing files from cache" << std::endl;
								while(cache.verifySpaceAvalible(std::filesystem::file_size(file_name))){
									cache.removeFileOnCache();
								}
							sendFileData(client,buff,server_addr);
							cache.addFileOnCache(buff,std::filesystem::file_size(file_name));
						}
						else {
							send(client , "0" , 1024, 0);
							std::cout << "[LOG:" << server_addr.sin_port << "] - Cache miss. File -> " << buff << " sent to the client" << std::endl;
							sendFileData(client,buff,server_addr);
							cache.addFileOnCache(buff,std::filesystem::file_size(file_name));
						}
					}
			} else {
				string text = "[Server] - File not found";
				send(client , text.c_str() , text.length() , 0);
				std::cout << "[Server:" << server_addr.sin_port << "] - File not found" << endl;
			}
			

		}
		
	clients_mtx.unlock();
	cout << "[LOG:" << server_addr.sin_port << "] - Client disconnected." << endl;
	closesocket(client);
}

int main(int argc, char *argv[]) {

	WSADATA wsa_data;
	SOCKADDR_IN server_addr, client_addr;

	WSAStartup(MAKEWORD(2, 2), &wsa_data);
	const auto server = socket(AF_INET, SOCK_STREAM, 0);

	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(5555);

	::bind(server, reinterpret_cast<SOCKADDR *>(&server_addr), sizeof(server_addr));
	listen(server, 0);

	cout << "[Server] Waiting for incoming connections." << endl;

	int client_addr_size = sizeof(client_addr);

	for (;;) {

		SOCKET client;

		if ((client = accept(server, reinterpret_cast<SOCKADDR *>(&client_addr), &client_addr_size)) != INVALID_SOCKET) {
			//auto fut = async(launch::async, on_client_connect, client);
			std::thread t([=]() {
				on_client_connect(client,client_addr);
			});
			t.detach();
		}

		const auto last_error = WSAGetLastError();
		
		if(last_error > 0) {
			cout << "Error: " << last_error << endl;
		}

	}
    return 0;
}