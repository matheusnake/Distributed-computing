#include <iostream>
#include <fstream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>

using namespace std;

fstream outfile;
char file_name[1024] = {};

void recvFileData(SOCKET client, string file_name) {
	
	char file_buffer[1024] = {};
	outfile.open(file_name, ios::out | ios::binary);
	outfile << std::unitbuf;

	cout << "[LOG] - File Transfer Begun." << endl;
	
	while(1){
		int data = recv(client, file_buffer, 1024, 0);
		if(data == 0) break;
		outfile.write(file_buffer, data);
	}
	
	outfile.close();
	closesocket(client);
	cout << "[LOG] - File Transfered. " << endl;
}

int main(int argc, char *argv[]) {

	char buffer[1024];

	string 	server_host = "127.0.0.1",
			server_port = "5555",
			list = "img1.png",
			buf;

	if(argc > 1) {
		server_host = argv[1];
		server_port = argv[2];
		list = argv[3];
	}

	WSADATA wsa_data;
	SOCKADDR_IN addr;

	WSAStartup(MAKEWORD(2, 0), &wsa_data);
	const auto server = socket(AF_INET, SOCK_STREAM, 0);

	InetPton(AF_INET, server_host.c_str(), &addr.sin_addr.s_addr);

	addr.sin_family = AF_INET;
	addr.sin_port = htons(5555);

	if(connect(server, reinterpret_cast<SOCKADDR *>(&addr), sizeof(addr)) < 0){
		cout << "[Error] - Not reached the server!" << endl;
		return 0;
	}

	cout << "[Server] - Connected to server!" << endl;

	if(list == "list"){
		buf = list;
		cout << "[Server] - Cache List" << endl;
		send(server , buf.c_str() , buf.length() , 0);
		recv(server, buffer, sizeof(buffer), 0);
		cout << buffer << endl;
	}
	else{
		send(server,list.c_str(), list.length(),0);
		recv(server, buffer, sizeof(buffer), 0);

		if(strcmp(buffer, "1") == 0) {
			recv(server, buffer, sizeof(buffer), 0);
				if(strcmp(buffer, "2") == 0) {
					cout << "[Error] - Data Size is bigger than 64MB" << endl;	
				}
				else {
					recvFileData(server,list);
				}
		} else {
			recv(server, buffer, sizeof(buffer), 0);
			cout << buffer << endl;
		}
	}

	WSACleanup();
	cout << "[SERVER] - Communication closed." << endl;

}