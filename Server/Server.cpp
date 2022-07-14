#include "pch.h"
#include "framework.h"
#include <afxsock.h>
using namespace std;
#include <iostream>
#include <boost/property_tree/ptree.hpp>
using boost::property_tree::ptree;
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
using namespace cv;
#include <Windows.h>
#include <fcntl.h>
#include <io.h>
#include "date.h"
#include <ColorDlg.h>
#include <fstream>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


struct  image {
	Mat data;
	int width;
	int height;
	void readImageFile(string fileName) {
		this->data = imread(fileName);
		height = data.size[0];
		width = data.size[1];
	}
};


struct user {
	char* name = NULL;
	char* password = NULL;
	void add(char* src, char*& dst) {
		if (dst != NULL) delete[] dst;
		int size = strlen(src);
		if (size == 0) return;
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
		if (size == 0) return;
		strcpy_s((char*)dst, size + 1, src);
	}
	bookingDates* next = NULL;
};
struct room {
	char* name = new char[256];
	char* type = NULL;
	char* description = NULL;
	char* bookedList = NULL;
	char* price = NULL;
	void add(char* src, char*& dst)
	{
		if (dst != NULL) delete[] dst;
		int size = strlen(src);
		if (size == 0) return;
		dst = new char[size + 1];
		strcpy_s((char*)dst, size + 1, src);
	}
	bookingDates* book = NULL;
	//string imagesFolder[3] ;
	/*
	        ("photo\\hotelOne\\single\\phongNgu.jpeg");
			("photo\\hotelOne\\single\\phongKhach.jpeg");
			("photo\\hotelOne\\double\\phongTam.jpeg");
			Lan luot 3 dong tren cho index 0 1 2 Ong them 3 dong nay vao cai cau truc cua moi phong roi luc doc file ma tao ra list ay thi ong doc them 3 dong nay nua cho moi phong nha
	*/
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
void cutstring(char* s, bookingDates*& d)
{
	if (strlen(s) != 0)
	{
		int i = 0;
		while (i < strlen(s))
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
void readHotelData(const char* hotelname, room*& list)
{
	ptree pt;
	read_xml(hotelname, pt);
	list = new room[20];
	int i = 0;
	BOOST_FOREACH(ptree::value_type & child, pt.get_child("programData.hoteldata"))
	{
		string temp = child.second.get<string>("name");
		char* name = convertString(temp);
		//cout << name << endl;
		temp = child.second.get<string>("type");
		char* type = convertString(temp);
		//cout << type << endl;
		temp = child.second.get<string>("date");
		char* date = convertString(temp);
		//cout << date << endl;
		temp = child.second.get<string>("description");
		char* description = convertString(temp);
		//cout << description << endl;
		temp = child.second.get<string>("price");
		char* price = convertString(temp);
		//cout << price << endl;
		list[i].name = name;
		list[i].bookedList = date;
		cutstring(list[i].bookedList, list[i].book);
		list[i].type = type;
		list[i].description = description;
		list[i].price = price;
		// Doc them 3 dong link cho photo nua
		i++;
	}
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
	if (first == NULL) {
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
	char* obj = NULL;
	if (size == 0) return obj;
	obj = new char[size + 1];
	strcpy_s(obj, size + 1, str.c_str());
	return obj;
}

void readUserData(const char* filename, user*& first)
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

bool testClientInput(user* first, char* name) {
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

void writeUserData(const char* filename, user* first)
{
	ptree pt, ptChild;
	if (first == NULL) return;
	user* temp = first;
	while (temp != NULL)
	{
		ptChild.add("name", temp->name);
		ptChild.add("password", temp->name);
		ptChild.add("bankInfo", temp->STK);
		pt.add_child("programData.userData", ptChild);
		ptChild.clear();
		temp = temp->next;
	}

	write_xml(filename, pt);
}

room* truyTiemPhong(char* roomNumber, room* list) {
	room* temp = list;
	int size = strlen(roomNumber);
	while (temp != NULL) {
		if (strcmp(temp->name, roomNumber) == 0) {
			return temp;
		}
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
	user* user1 = new user;
	user* trueUser = NULL;
	int features;
	int flag;
	int featuresSwitch = 0;
	room* list = NULL;
	char* hotelname = NULL;
	readHotelData("hotel.xml", list);
	readUserData("temp.xml", userList);
	do {
		client.Receive((char*)&features, sizeof(int), 0);
		switch (features) {
		case 0:
			TTL++;
			if (featuresSwitch == 0) {
				flag = 1;
				while (flag <= 3) {
					int size = 0;
					int tempSize = 10;
					char* buffer = NULL;
					client.Receive((char*)&size, sizeof(int), 0);
					if (size == 0) {
						break;
					}
					buffer = new char[size + 1];
					if (buffer == NULL) break;
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
					case 1: user1->add(buffer, user1->name); // Phần này đáng ra phải có 1 hàm đọc xml rồi so sánh với cái username mà mình nhận được ổn thì mới cho cái flag di chuyển tiếp không thì báo lại cho bên client là userName có người xài rồi gửi lại đi
						printf("%s \n", user1->name);
						break;
					case 2:user1->add(buffer, user1->password);
						printf("%s \n", user1->password);
						break;
					case 3:user1->add(buffer, user1->STK);
						printf("%s \n", user1->STK);
					default: break;
					}
					flag++;
				}
				insertData(userList, createData(user1->name, user1->password, user1->STK));
				writeUserData("userData.xml", userList);
			}
			else {
				//Chuc nang tra cuu
				char* categoryRoom = 0;
				room* roomToCheck = NULL;
				image imageToSent[3];
				vector <uchar> dataStorage;
				int size = 0;
				int tempSize = 10;
				char* buffer = NULL;
				client.Receive((char*)&size, sizeof(int), 0);
				if (size == 0) {
					break;
				}
				buffer = new char[size + 1];
				for (int i = 0; i < size; i = i + tempSize) {
					if (i + tempSize >= size) {
						tempSize = size - i;
					}
					client.Receive((char*)&buffer[i], tempSize, 0);
				}
				buffer[size] = '\0';

				char* buffer2 = NULL;
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
				int a[10] = { 0 };
				date d1, d2;
				convertchargdatetoint(buffer, d1);
				convertchargdatetoint(buffer2, d2);
				for (int j = 0; j <= countNoOfDays(d1, d2); j++) {

					for (int i = 0; i < 10; i++) {
						bookingDates* temp = list[i].book;

						if (temp != NULL)
						{

							while (temp != NULL)
							{
								if (strcmp(temp->booked, t) == 0)
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
				for (int i = 0; i < 10; i++) {
					if (a[i] == 0) c++;
				}
				client.Send(&c, sizeof(c), 0);
				
				for (int i = 0; i < 10; i++) {
					if (a[i] == 0) {
						size = strlen(list[i].name);
						client.Send(&size, sizeof(size), 0);
						client.Send(&list[i].name[0], size, 0);
						size = strlen(list[i].type);
						client.Send(&size, sizeof(size), 0);
						client.Send(&list[i].type[0], size, 0);
						size = strlen(list[i].description);
						client.Send(&size, sizeof(size), 0);
						client.Send(&list[i].description[0], size, 0);
						size = strlen(list[i].price);
						client.Send(&size, sizeof(size), 0);
						client.Send(&list[i].price[0], size, 0);
					}
				}
				/*
				client.Receive((char*)&categoryRoom, sizeof(int), 0);
				roomToCheck = truyTiemPhong(categoryRoom, list);
				*/
				int type = 0;
				client.Receive((char*)&type, sizeof(int), 0);
				cout << type << endl;
				imageToSent[0].readImageFile("photo\\hotelOne\\single\\phongNgu.jpeg");
				imageToSent[1].readImageFile("photo\\hotelOne\\single\\phongKhach.jpeg");
				imageToSent[2].readImageFile("photo\\hotelOne\\single\\phongTam.jpg");
				flag = 1;
				for (int i = 0; i < 3; i++) {
					imencode(".jpg", imageToSent[i].data, dataStorage);
					size = dataStorage.size();
					client.Send(&size, sizeof(int), 0);
					client.Send(&dataStorage[0], size, 0);
				}
				break;
			}
			break;
		case 1:
			TTL++;
			if (featuresSwitch == 0) {
				flag = 1;
				while (flag <= 2) {
					int size = 0;
					int tempSize = 10;
					char* buffer = NULL;
					client.Receive((char*)&size, sizeof(int), 0);
					if (size == 0) {
						return 0;
					}
					buffer = new char[size + 1];
					if (buffer == NULL) break;
					for (int i = 0; i < size; i = i + tempSize) {
						if (i + tempSize >= size) {
							tempSize = size - i;
						}
						client.Receive((char*)&buffer[i], tempSize, 0);
					}
					buffer[size] = '\0';
					switch (flag) {
					case 1: user1->add(buffer, user1->name);
						printf("%s \n", user1->name);
						break;
					case 2:user1->add(buffer, user1->password);
						printf("%s \n", user1->password);
						break;
					default: break;
					}
					flag++;
				}
				trueUser = loginConfirm(userList, user1->name, user1->password);
				if (trueUser == NULL) {
					bool checker = 0;
					client.Send(&checker, sizeof(bool), 0);
				}
				else {
					bool checker = 1;
					featuresSwitch = 1;
					client.Send(&checker, sizeof(bool), 0);
				}
			}
			else {
				// Phan code cho dat phong
				cout << "hello" << endl;
			}
			break;
		case 3:
			if (featuresSwitch != 0) {
				client.Receive((char*)&featuresSwitch, sizeof(int), 0);
				delete trueUser;
				delete user1;
				trueUser = NULL; 
				user1 = new user;
			}
			break;
		default:
			break;
		}
		TTL--;
		if (TTL == 0) break;
		client.Receive((char*)&number_continue, sizeof(int), 0);
	} while (number_continue == 1);
	delete hConnected;
	return 0;
}

CWinApp theApp;

using namespace std;



int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{

	//readUserData("temp.xml", userList);

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