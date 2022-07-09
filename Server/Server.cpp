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
#include "date.h"
#include <ColorDlg.h>
#include <fstream>
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
	void add(char* src, char*& dst) {
		if (dst != NULL) delete[] dst;
		int size = strlen(src);
		dst = new char[size + 1];
		strcpy_s((char*)dst, size + 1, src);
	}
	char* STK = NULL;
	user* next;
};
struct bookingDates
{
	char* booked = NULL;
	void add(char* src, char*& dst)
	{
		if (dst != NULL) delete[] dst;
		int size = strlen(src);
		dst = new char[size + 1];
		strcpy_s((char*)dst, size + 1, src);
	}
	bookingDates* next = NULL;
};
struct room
{
	char* name = new char[256];
	char* type = NULL;
	char* description = NULL;
	char* bookedList = NULL;
	char* price = NULL;
	void add(char* src, char*& dst)
	{
		if (dst != NULL) delete[] dst;
		int size = strlen(src);
		dst = new char[size + 1];
		strcpy_s((char*)dst, size + 1, src);
	}
	bookingDates* book = NULL;
};
char* convertString(string str);
struct hotel
{
	char* name;
	void add(char* src, char*& dst)
	{
		if (dst != NULL) delete[] dst;
		int size = strlen(src);
		dst = new char[size + 1];
		strcpy_s((char*)dst, size + 1, src);
	}
	room roomList[20] = {};
};
bookingDates* newdate(char* s)
{
	bookingDates* temp = new bookingDates;
	temp->booked = s;
	temp->next = NULL;
	return temp;
}
void cutstring(char* s, bookingDates*&d)
{
	if (strlen(s) != 0)
	{
		int i = 0;
		while (i<strlen(s))
		{
			char* c = new char[12];
			for (int j = 0; j < 10; j++)
				c[j] = s[i + j];
			c[10] = '\0';
			if (d == NULL)
			{
				d = newdate(c);
			}
			else
			{
				bookingDates* temp = d;
				while (temp->next != NULL)
				{
					temp = temp->next;
				}
				temp->next = newdate(c);
			}
			i = i + 11;
		}
		
	}
}
void readfilehotel(room *&list)
{
	ifstream in("hotel.txt", ios::in);
	list = new room[20];
	int i = 0;
	if (in.is_open())
	{
		while (i<3)
		{
			string s;
			getline(in, s);
			list[i].name = convertString(s);
			getline(in, s);
			list[i].bookedList = convertString(s);
			cutstring(list[i].bookedList, list[i].book);
			getline(in, s);
			list[i].type = convertString(s);
			getline(in, s);
			list[i].description = convertString(s);
			getline(in, s);
			list[i].price = convertString(s);
			i++;
		}
	}
	else
	{
		cout << "Loi mo file";
	}
	in.close();
	
	
}
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

void insertData(user*& first, user* data)
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


char* convertString(string str) {
	int size = 0;
	size = str.length();
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
	while (temp != NULL) {
		if (strcmp(name, temp->name) == 0) return true;
		temp = temp->next;
	}
	return false;
}

user* loginConfirm(user* first, char* name, char* pass) {
	user* temp = first;
	if (temp == NULL) return NULL;
	while (temp != NULL) {
		if (strcmp(name, temp->name) == 0) {
			if (strcmp(pass, temp->password) == 0) {
				return temp;
			}
			else {
				return NULL;
			}
		}
		temp = temp->next;
	}
	return NULL;
}

// The one and only application object


DWORD WINAPI function_cal(LPVOID arg) {
	SOCKET* hConnected = (SOCKET*)arg;
	CSocket client;
	client.Attach(*hConnected);
	int TTL = 1000;
	int number_continue = 1;
	cout << "Da co mot client ket noi!" << endl;
	user user1;
	user* trueUser = NULL;
	int fetures;
	int flag;
	int feturesSwitch = 0;
	room* list;
	readfilehotel(list);
	readFile("temp.xml", userList);
	do {
		client.Receive((char*)&fetures, sizeof(int), 0);
		switch (fetures) {
		case 0:
			if (feturesSwitch == 0) 
			{
				
				TTL++;
				flag = 1;
				while (flag <= 3) {
					int size = 0;
					int tempSize = 10;
					char* buffer;
					client.Receive((char*)&size, sizeof(int), 0);
					if (size == 0) {
						return 0;
					}
					buffer = new char[size + 1];
					for (int i = 0; i < size; i = i + tempSize) {
						if (i + tempSize >= size) {
							tempSize = size - i;
						}
						client.Receive((char*)&buffer[i], tempSize, 0);
					}
					buffer[size] = '\0';
					if (flag == 1 && testClientInput(userList, buffer)) {
						bool checker = 1;
						cout << "user existed" << endl;
						client.Send(&checker, sizeof(checker), 0);
						continue;
					}
					else {
						bool checker = 0;
						client.Send(&checker, sizeof(checker), 0);
					}
					switch (flag) {
					case 1: user1.add(buffer, user1.name); // Phần này đáng ra phải có 1 hàm đọc xml rồi so sánh với cái username mà mình nhận được ổn thì mới cho cái flag di chuyển tiếp không thì báo lại cho bên client là userName có người xài rồi gửi lại đi
						printf("%s \n", user1.name);
						break;
					case 2:user1.add(buffer, user1.password);
						printf("%s \n", user1.password);
						break;
					case 3:user1.add(buffer, user1.STK);
						printf("%s \n", user1.STK);
					default: break;
					}
					flag++;
					
				}
			}
			else
			{
				TTL++;
				{
					
					int size = 0;
					int tempSize = 10;
					char* buffer;
					client.Receive((char*)&size, sizeof(int), 0);
					if (size == 0) {
						return 0;
					}
					buffer = new char[size + 1];
					for (int i = 0; i < size; i = i + tempSize) {
						if (i + tempSize >= size) {
							tempSize = size - i;
						}
						client.Receive((char*)&buffer[i], tempSize, 0);
						
					}
					buffer[size] = '\0';
					
					char* buffer2;
					client.Receive((char*)&size, sizeof(int), 0);
					if (size == 0) {
						return 0;
					}
					buffer2 = new char[size + 1];
					for (int i = 0; i < size; i = i + tempSize) {
						if (i + tempSize >= size) {
							tempSize = size - i;
						}
						client.Receive((char*)&buffer2[i], tempSize, 0);
					}
					buffer2[size] = '\0';
					
					char* t = buffer;
					
					int a[4] = { 0 };
					date d1, d2;
					convertchargdatetoint(buffer, d1);
					convertchargdatetoint(buffer2, d2);
					for (int j=0;j<=countNoOfDays(d1,d2);j++)
					{
						
						for (int i = 0; i < 3; i++)
						{
							bookingDates* temp = list[i].book;

							if (temp != NULL)
							{

								while (temp != NULL)
								{
									if (strcmp(temp->booked, t)==0)
									{
										a[i]++;
										break;
									}
									temp = temp->next;
								}
							}
							
						}
						date d;
						
						convertchargdatetoint(t, d);
						d = nextday(d);
						convertdate(d, t);
					}
					int c = 0;
					for (int i = 0; i < 3; i++)
					{
						if (a[i] == 0) c++;
						
					}
					
					client.Send(&c, sizeof(c), 0);
					
					for (int i = 0; i < 3; i++)
					{
						if (a[i] == 0)
						{
							size = strlen(list[i].type);
							client.Send(&size, sizeof(size), 0);
							client.Send(list[i].type, size, 0);
							cout << list[i].type;
							size = strlen(list[i].description);
							client.Send(&size, sizeof(size), 0);
							client.Send(list[i].description, size, 0);
							cout << list[i].description;
							size = strlen(list[i].price);
							client.Send(&size, sizeof(size), 0);
							client.Send(list[i].price, size, 0);
							cout << list[i].price;
							cout << endl;
						}
					}

				}
			}
			break;
		case 1:
			TTL++;
			flag = 1;
			
			while (flag <= 2) {
				int size = 0;
				int tempSize = 10;
				char* buffer;
				client.Receive((char*)&size, sizeof(int), 0);
				if (size == 0) {
					return 0;
				}
				buffer = new char[size + 1];
				for (int i = 0; i < size; i = i + tempSize) {
					if (i + tempSize >= size) {
						tempSize = size - i;
					}
					client.Receive((char*)&buffer[i], tempSize, 0);
				}
				buffer[size] = '\0';
				switch (flag) {
				case 1: user1.add(buffer, user1.name);
					printf("%s \n", user1.name);
					break;
				case 2:user1.add(buffer, user1.password);
					printf("%s \n", user1.password);
					break;
				default: break;
				}
				flag++;
			}
			trueUser = loginConfirm(userList, user1.name, user1.password);
			if (trueUser == NULL) {
				bool checker = 0;
				client.Send(&checker, sizeof(bool), 0);
			}
			else {
				bool checker = 1;
				feturesSwitch = 1;
				client.Send(&checker, sizeof(bool), 0);
			}
			break;
		
		break;
		default:
			break;
		}
		client.Receive((char*)&number_continue, sizeof(int), 0);
		TTL--;
		if (TTL == 0) break;
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
	room* list;
	
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
			printf("Server lang nghe ket noi tu client\n");
			
			do {
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
