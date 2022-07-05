// Server.cpp : Defines the entry point for the console application.
//
#include "pch.h"
#include "framework.h"
#include <afxsock.h>
using namespace std;
#include <iostream>
#include <boost/property_tree/ptree.hpp>
using boost::property_tree::ptree;
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>
#include <Windows.h>
#include <fcntl.h>
#include <io.h>
#include <ColorDlg.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

class customer {
public:
	string username, password, bankInfo;

	customer() = default;
	customer(string username, string password, string bankInfo) :username(username), password(password), bankInfo(bankInfo) {}
};

class hotels {
public:

};

struct user {
	char* name = NULL;
	char* password = NULL;
	void add(char* src,char*& dst) {
		if (dst != NULL) delete[] dst;
		int size = strlen(src);
		dst = new char[size + 1];
		strcpy_s((char*)dst, size + 1, src);
	}
	char* STK = NULL ;
	user* next;
};

user* userList = NULL;

user* createData(char* name, char* password, char* STK)
{
	user* newUser = new user;
	newUser->add(name, newUser->name);
	newUser->add(password, newUser->password);
	newUser->add(STK, newUser->STK);
	newUser->next = NULL;
	return newUser;
}

void insertData(user*&first, user* data)
{
	if (first == NULL)
	{
		first = data;
		return;
	}
	user* temp = first;
	while (temp->next != NULL) temp = temp->next;
	temp->next = data;
}

bool checkUserName(char* name) {
	int size = strlen(name);
	if (size < 5) {
		return false;
	}
	for (int i = 0; i < size; i++) {
		if (name[i] < '0') {
			return false;
		}
		else if (name[i] > '9') {
			if (name[i] < 'a' || name[i] > 'z') {
				return false;
			}
		}
	}
	return true;
}

bool checkPassword(char* password) {
	int size = strlen(password);
	if (size < 3)
		return false;
	return true;
}

bool checkBankInfo(char* STK)
{
	int size = strlen(STK);
	if (size != 10) return false;
	for (int i = 0; i < size; i++)
	{
		if (STK[i] < '0' || STK[i] > '9') return false;
	}
	return true;
}


char* convertString(string str)
{
	int size = str.length();
	char* obj = new char[size + 1];
	strcpy_s(obj, size + 1, str.c_str());
	return obj;
}

void readFile(const char* filename, user*& first)
{
	ptree pt;
	read_xml(filename, pt);

	BOOST_FOREACH(ptree::value_type & child, pt.get_child("programData.userdata"))
	{
		string temp = child.second.get<string>("username");
		char* name = convertString(temp);
		temp = child.second.get<string>("password");
		char* password = convertString(temp);
		temp = child.second.get<string>("bankInfo");
		char* bankInfo = convertString(temp);
		insertData(first, createData(name, password, bankInfo));
	}
}

bool testClientInput(user* first, char* name)
{
	user* temp = first;
	if (temp == NULL) return false;
	while (temp != NULL)
	{
		//cout << "Username: " << temp->name << endl;
		//cout << "Password: " << temp->password << endl;
		//cout << "STK: " << temp->STK << endl;
		if (strcmp(name, temp->name) != 0) return true;
		temp = temp->next;
	}
	return false;
}

// The one and only application object


DWORD WINAPI function_cal(LPVOID arg)
{
	SOCKET* hConnected = (SOCKET*)arg;
	CSocket client;
	client.Attach(*hConnected);
	int number_continue = 1;
	do {
		cout << "Da co mot client ket noi!" << endl;
		user user1;
		int fetures;
		int flag;
		client.Receive((char*)&fetures, sizeof(int), 0);
		switch (fetures) {
		case 0:
			flag = 1;
			while (flag <= 3) {
				int size = 0;
				int tempSize = 0;
				char* buffer;
				client.Receive((char*)&size, sizeof(int), 0);
				buffer = new char[size + 1];
				do {
					client.Receive((char*)&buffer[tempSize], size, 0);
					tempSize = tempSize + strlen(buffer);
				} while (tempSize < size);
				buffer[size] = '\0';
				readFile("temp.xml", userList);
				switch (flag) {
				case 1: user1.add(buffer, user1.name); // Phần này đáng ra phải có 1 hàm đọc xml rồi so sánh với cái username mà mình nhận được ổn thì mới cho cái flag di chuyển tiếp không thì báo lại cho bên client là userName có người xài rồi gửi lại đi
					printf("%s \n", user1.name);
					if (testClientInput(userList, user1.name))
					{
						bool checker = 1;
						cout << "user existed" << endl;
						client.Send(&checker, sizeof(checker), 0);
						break;
					}
					else
					{
						bool checker = 0;
						client.Send(&checker, sizeof(checker), 0);
						break;
					}
				case 2:user1.add(buffer, user1.password);
					printf("%s \n", user1.password);
					break;
				case 3:user1.add(buffer, user1.STK);
					printf("%s \n", user1.STK);
				default: break;
				}
				flag++;
			}
			break;
		default:
			break;
		}
		client.Receive(&number_continue, sizeof(int), 0);
	} while (number_continue);
	delete hConnected;
	return 0;
}

CWinApp theApp;

using namespace std;



int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{

	readFile("temp.xml", userList);
	//cout << userList->name << "/" << userList->password << "/" << userList->STK << endl;
	//testClientOutput(userList);

	int nRetCode = 0;
	HMODULE hModule = ::GetModuleHandle(NULL);
	// initialize MFC and print and error on failure
	if (hModule != NULL)
	{
		if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
		{
			// TODO: change error code to suit your needs
			_tprintf(_T("Fatal Error: MFC initialization failed\n"));
			nRetCode = 1;
		}
		else
		{
			// TODO: code your application's behavior here.
			// Khoi tao thu vien Socket
			AfxSocketInit(NULL);
			CSocket main, client;
			DWORD threadID;
			HANDLE threadStatus;
			main.Create(1234);
			do {
				printf("Server lang nghe ket noi tu client\n");
				main.Listen();
				main.Accept(client);
				//Khoi tao con tro Socket
				SOCKET* hConnected = new SOCKET();
				//Chuyển đỏi CSocket thanh Socket
				*hConnected = client.Detach();
				//Khoi tao thread tuong ung voi moi client Connect vao server.
				//Nhu vay moi client se doc lap nhau, khong phai cho doi tung client xu ly rieng
				threadStatus = CreateThread(NULL, 0, function_cal, hConnected, 0, &threadID);
			} while (1);
		}
	}
	else {
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: GetModuleHandle failed\n"));
		nRetCode = 1;
	}
	return nRetCode;
}
