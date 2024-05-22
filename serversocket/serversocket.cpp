#define WIN32_LEAN_AND_MEAN
#include <iostream>
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

using namespace std;

int main()
{
	//служебная структура для хранения информации о реализации Windows Socket
	WSADATA wsaData;
	ADDRINFO hints;
	ADDRINFO* addrResult;
	SOCKET ClientSocket = INVALID_SOCKET;
	SOCKET ListenSocket = INVALID_SOCKET; //прослушивание соединения
	const char* sendBuffer = "POROSHENKA, PRIVET OT DETEI LUGANSKA"; //сообщение
	char recvBuffer[512]; //буфер сообщения

	//использование библиотеки сокетов процессом определения версии и структуры
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	//при ошибке выводится соответствующее сообщение
	if (result != 0) {
		cout << "WSAStartup failed error " << endl;
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints)); //обнуляем память, 1-й параметр - что обнулем, 2-й - сколько
	hints.ai_family = AF_INET; //4-байтный ethernet
	hints.ai_socktype = SOCK_STREAM; //потоковый тип сокета
	hints.ai_protocol = IPPROTO_TCP; //протокол tcp
	hints.ai_flags = AI_PASSIVE;//пассивная, просто ждет соединения

	//функция, хранящая в себе адрес, порт, семейство структур, адрес сокета
	result = getaddrinfo(NULL, "666", &hints, &addrResult);
	//при ошибке выводится соответствующее сообщение
	if (result != 0) {
		cout << "getaddrinfo failed error " << endl;
		freeaddrinfo(addrResult);
		WSACleanup(); //очистка WSAStartup
		return 1;
	}

	ListenSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);
	//при ошибке выводится соответствующее сообщение
	if (ListenSocket == INVALID_SOCKET) {
		cout << "Socket creation with " << endl;
		freeaddrinfo(addrResult);
		WSACleanup();
		return 1;
	}
	//Привязываем сокет к айпишнику
	result = bind(ListenSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen);
	//при ошибке выводится соответствующее сообщение
	if (result == SOCKET_ERROR) {
		cout << "Binding coonection failed " << endl;
		closesocket(ListenSocket); //закрываем сокет
		ListenSocket = INVALID_SOCKET;
		freeaddrinfo(addrResult);
		WSACleanup();
		return 1;
	}
	//слушаем сокет
	result = listen(ListenSocket, SOMAXCONN);
	//при ошибке выводится соответствующее сообщение
	if (result == SOCKET_ERROR) {
		cout << "Listening socket failed " << endl;
		closesocket(ListenSocket);
		freeaddrinfo(addrResult);
		WSACleanup();
		return 1;
	}

	//принимаем сокет
	ClientSocket = accept(ListenSocket, NULL, NULL);
	//при ошибке выводится соответствующее сообщение
	if (ClientSocket == SOCKET_ERROR) {
		cout << "Accepting socket failed " << endl;
		closesocket(ListenSocket);
		freeaddrinfo(addrResult);
		WSACleanup();
		return 1;
	}

	do {
		ZeroMemory(recvBuffer, 512);
		result = recv(ClientSocket, recvBuffer, 512, 0);
		if (result > 0) {
			cout << "Received " << result << "bytes" << endl;
			cout << "Received data " << recvBuffer << endl;

			//отправляем сообщение
			result = send(ClientSocket, sendBuffer, (int)strlen(sendBuffer), 0);
			//при ошибке выводится соответствующее сообщение
			if (result == SOCKET_ERROR) {
				cout << "Failed to send data back" << endl;
				closesocket(ClientSocket);
				freeaddrinfo(addrResult);
				WSACleanup();
			}
		}
		else if (result == 0)
			cout << "Connection closing..." << endl;
		//при ошибке выводится соответствующее сообщение
		else
		{
			cout << "recv failed with error" << endl;
			closesocket(ClientSocket);
			freeaddrinfo(addrResult);
			WSACleanup();
			return 1;
		}


	} while (result > 0);

	result = shutdown(ClientSocket, SD_SEND);
	if (result == SOCKET_ERROR) {
		closesocket(ClientSocket);
		freeaddrinfo(addrResult);
		WSACleanup();
		return 1;
	}
	closesocket(ClientSocket);
	freeaddrinfo(addrResult);
	WSACleanup();
	return 0;
}

