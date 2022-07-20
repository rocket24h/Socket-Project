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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <time.h>
#include <sstream>
#include "date.h"
#include <ColorDlg.h>
#include <fstream>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


struct user {
	char* name = NULL;
	char* password = NULL;
	void add(char* src, char*& dst) {
		if (dst != NULL) delete[] dst;
		int size = strlen(src);
		if (size == 0) return;
		dst = new char[size + 1];
		strcpy_s(dst, size + 1, src);
	}
	char* STK = NULL;
	user* next;
};

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
	int sameday = 0;
	int inttype = 0;
	void add(char* src, char*& dst)
	{
		if (dst != NULL) delete[] dst;
		int size = strlen(src);
		dst = new char[size + 1];
		if (size == 0) return;
		strcpy_s((char*)dst, size + 1, src);
	}
	bookingDates* book = NULL;
	char* firstPhoto = NULL;
	char* secondPhoto = NULL;
	char* thirdPhoto = NULL;
};

struct bill {
	void add(char* src, char*& dst) {
		if (dst != NULL) delete[] dst;
		int size = strlen(src);
		if (size == 0) return;
		dst = new char[size + 1];
		strcpy_s((char*)dst, size + 1, src);
	}
	char* billID = NULL;
	char* nameHotel = NULL;
	char* list = NULL;
	char* dateIn = NULL;
	char* dateOut = NULL;
	char* GhiChu = NULL;
	int giaTien = 0;
	int TTL = 0;
	bill* next;
};

bill* createBill(char* billID, char* name, char* list, char* dateIn, char* dateOut, char* note, int price,int TTL)
{
	bill* b = new bill;
	b->billID = billID;
	b->nameHotel = name;
	b->list = list;
	b->dateIn = dateIn;
	b->dateOut = dateOut;
	b->GhiChu = note;
	b->giaTien = price;
	b->TTL = TTL;
	b->next = NULL;
	return b;
}

void insertBill(bill*& first, bill* b)
{
	if (first == NULL)
	{
		first = b;
		return;
	}
	else
	{
		bill* temp = first;
		while (temp->next != NULL)
		{
			temp = temp->next;
		}
		temp->next = b;
	}
}
void strCat(char*& dest, const char* src)
{
	if (dest == NULL)
	{
		if (src == NULL) return;
		else
		{
			dest = new char[strlen(src) + 1];
			strcpy_s(dest, strlen(src) + 1, src);
		}
	}
	else {
		dest = (char*)realloc(dest, strlen(dest) + strlen(src) + 1);
		strcat_s(dest, strlen(dest) + strlen(src) + 1, src);
	}
}

void writeBillData(bill* userBill, const char* filename)
{
	ptree pt, ptChild;
	if (userBill == NULL) return;
	bill* tempBill = userBill;
	while (tempBill != NULL)
	{
		ptChild.add("billID", tempBill->billID);
		ptChild.add("hotelName", tempBill->nameHotel);
		ptChild.add("dateIn", tempBill->dateIn);
		ptChild.add("dateOut", tempBill->dateOut);
		ptChild.add("notes", tempBill->GhiChu);
		ptChild.add("price", tempBill->giaTien);
		ptChild.add("subBill", tempBill->list);
		ptChild.add("TTL", tempBill->TTL);
		pt.add_child("programData.billData", ptChild);
		ptChild.clear();
		tempBill = tempBill->next;
	}
	write_xml(filename, pt);
}

char* convertString(string str);
void readBillData(bill*& userBill, const char* filename, int& billCount)
{
	ptree pt;
	string temp;
	read_xml(filename, pt);
	BOOST_FOREACH(ptree::value_type & child, pt.get_child("programData"))
	{
		temp = child.second.get<string>("billID");
		char* billID = convertString(temp);
		temp = child.second.get<string>("hotelName");
		char* name = convertString(temp);
		//cout << name << endl;
		temp = child.second.get<string>("dateIn");
		char* dateIn = convertString(temp);
		//cout << type << endl;
		temp = child.second.get<string>("dateOut");
		char* dateOut = convertString(temp);
		//cout << date << endl;
		temp = child.second.get<string>("notes");
		char* notes = convertString(temp);
		//cout << description << endl;
		int price = child.second.get<int>("price");
		temp = child.second.get<string>("subBill");
		char* bookedRooms = convertString(temp);
		int TTL = child.second.get<int>("TTL");
		//cout << bookedRooms << endl;
		billCount++;
		insertBill(userBill, createBill(billID, name, bookedRooms, dateIn, dateOut, notes, price, TTL));
	}
}

template<typename T>
void clearBill(T* pHead) {
	if (pHead == NULL) return;
	bill* temp = pHead;
	while (pHead != NULL) {
		pHead = pHead->next;
		delete temp;
		temp = pHead;
	}
}

void deleteBill(bill*& pHead,bill*& deleteOne) {
	bill* temp = pHead;
	bill* previous = NULL;
	while (temp->next != NULL) {
		if (temp == deleteOne) {
			if (previous != NULL) {
				previous->next = temp->next;
			}
			else {
				pHead = temp->next;
			}
			delete [] deleteOne->nameHotel;
			delete[] deleteOne->billID;
			delete[] deleteOne->dateIn;
			delete[] deleteOne->dateOut;
			delete[] deleteOne->GhiChu;
			delete[] deleteOne->list;
			delete deleteOne;
			deleteOne = NULL;
			break;
		}
		previous = temp;
		temp = temp->next;
	}
}

int timeTL() {
	time_t now;
	time(&now);
	return now;
}

bookingDates* newdate(char* s)
{
	bookingDates* temp = new bookingDates;
	temp->booked = s;
	temp->next = NULL;
	return temp;
}

void cutstring(char* s, bookingDates*& d)
{
	if (s == NULL) return;
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
	list = new room[10];
	int i = 0;
	BOOST_FOREACH(ptree::value_type & child, pt.get_child("programData.hotelData"))
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
		temp = child.second.get<string>("firstPhoto");
		char* firstPhoto = convertString(temp);
		temp = child.second.get<string>("secondPhoto");
		char* secondPhoto = convertString(temp);
		temp = child.second.get<string>("thirdPhoto");
		char* thirdPhoto = convertString(temp);
		//cout << price << endl;
		list[i].name = name;
		list[i].bookedList = date;
		cutstring(list[i].bookedList, list[i].book);
		list[i].type = type;
		list[i].description = description;
		list[i].price = price;
		list[i].firstPhoto= firstPhoto;
		list[i].secondPhoto = secondPhoto;
		list[i].thirdPhoto = thirdPhoto;
		i++;
	}
}

int inttype(char* s)
{
	const char* sing = "single";
	const char* doub = "double";
	if (strcmp(s, doub) == 0) return 2;
	if (strcmp(s, sing) == 0) return 1;
	return 0;
}


int findRoomByName(room* list, char* nameRoom, int size, room & trueRoom) {
	for (int i = 0; i < size; i++) {
		if (strcmp(list[i].name, nameRoom) == 0) {
			trueRoom = list[i];
			return 1;
		}
   }
	return 0;
}

user* createData(char* name, char* password, char* STK)
{
	user* newUser = new user;
	newUser->add(name, newUser->name);
	newUser->add(password, newUser->password);
	newUser->add(STK, newUser->STK);
	newUser->next = NULL;
	return newUser;
}

void insertData(user*& first, user* data) {
	if (first == NULL) {
		first = data;
	}
	else {
		user* temp = first;
		while (temp->next != NULL) {
			temp = temp->next;
		}
		temp->next = data;
	}
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

char* mergeCharStr(char* src, const char* suffix)
{
	char* temp = new char[strlen(src) + strlen(suffix) + 1];
	for (int i = 0; i < strlen(src); i++)
	{
		temp[i] = src[i];
	}
	for (int j = 0; j < strlen(suffix); j++)
	{
		temp[strlen(src) + j] = suffix[j];
	}
	temp[strlen(src) + strlen(suffix)] = '\0';
	return temp;
}

void upperCase(char*& src)
{
	for (int i = 0; i < strlen(src); i++)
	{
		src[i] = toupper(src[i]);
	}
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
	BOOST_FOREACH(ptree::value_type & child, pt.get_child("programData.userData"))
	{
		string temp = child.second.get<string>("name");
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
	if (temp == NULL || name == NULL) return false;
	while (temp != NULL) {
		if (strcmp(name, temp->name) == 0) {
			return true;
		}
		temp = temp->next;
	}
	return false;
}

user* loginConfirm(user* first,const char* name,const char* pass) {
	user* temp = first;
	if (temp == NULL) return NULL;
	cout << name << " " << pass << endl;
	while (temp != NULL) {
		cout << temp->name << " " << temp->password << endl;
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

char* tproom(char* s, room* list)
{
	for (int i = 0; i < 10; i++)
	{
		if (strcmp(s, list[i].name)) return list[i].type;
	}
}

void writeUserData(const char* filename, user* first)
{
	ptree pt, ptChild;
	if (first == NULL) return;
	user* temp = first;
	while (temp != NULL) {
		ptChild.add("name", temp->name);
		ptChild.add("password", temp->password);
		ptChild.add("bankInfo", temp->STK);
		pt.add_child("programData.userData.user", ptChild);
		ptChild.clear();
		temp = temp->next;
	}
	write_xml(filename, pt);
}

void writeHotelData(const char* filename, room* roomList)
{
	ptree pt, ptChild;
	if (roomList == NULL) return;
	for (int i = 0; i < 10; i++)
	{
		ptChild.add("name", roomList[i].name);
		ptChild.add("type", roomList[i].type);
		ptChild.add("date", roomList[i].bookedList);
		ptChild.add("description", roomList[i].description);
		ptChild.add("price", roomList[i].price);
		ptChild.add("firstPhoto", roomList[i].firstPhoto);
		ptChild.add("secondPhoto", roomList[i].secondPhoto);
		ptChild.add("thirdPhoto", roomList[i].thirdPhoto);
		pt.add_child("programData.hotelData.room", ptChild);
		ptChild.clear();
	}
	write_xml(filename, pt);
}

void addtail(bookingDates*&pHead, char* x)
{
	if (pHead == NULL)
	{
		pHead = newdate(x);
	}
	else
	{
		bookingDates* temp = pHead;
		while (temp->next != NULL)
		{
			temp = temp->next;
		}
		temp->next = newdate(x);
	}
}

void copy(char*& des, char* src) {
	if (src == NULL) return;
	int size = strlen(src);
	strcpy_s(des, size + 1, src);
}


// The one and only application object
DWORD WINAPI function_cal(LPVOID arg) {
	SOCKET* hConnected = (SOCKET*)arg;
	CSocket client;
	client.Attach(*hConnected);
	user* userList = NULL;
	int TTL = 1000;
	int number_continue = 1;
	cout << "Da co mot client ket noi!" << endl;
	user* user1 = new user;
	user* trueUser = NULL;
	int features;
	int flag;
	int makeSure = 1;
	int featuresSwitch = 0;
	readUserData("userData.xml", userList);
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
					int makeSure = 1;
					if (flag == 1 && testClientInput(userList, buffer)) {
						bool checker = 1;
						cout << "user existed" << endl;
						client.Send(&makeSure, sizeof(int), 0);
						client.Send(&checker, sizeof(checker), 0);
						continue;
					}
					else if(flag == 1) {
						bool checker = 0;
						client.Send(&makeSure, sizeof(int), 0);
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
					if (flag == 4) {
						user* temp = createData(user1->name, user1->password, user1->STK);
						insertData(userList, temp);
						writeUserData("userData.xml", userList);
					}
				}
			}
			else {
				//Chuc nang tra cuu
				char* categoryRoom = 0;
				room* roomToCheck = NULL;
				image imageToSent[3];
				vector <uchar> dataStorage;
				int size = 0;
				int tempSize = 10;
				room* list = NULL;
				char* hotelFile = NULL;
				client.Receive((char*)&size, sizeof(int), 0);
				if (size == 0) {
					break;
				}
				hotelFile = new char[size + 1];
				for (int i = 0; i < size; i = i + tempSize) {
					if (i + tempSize >= size) {
						tempSize = size - i;
					}
					client.Receive((char*)&hotelFile[i], tempSize, 0);
				}
				hotelFile[size] = '\0';
				upperCase(hotelFile);
				hotelFile = mergeCharStr(hotelFile, ".xml");
				readHotelData(hotelFile, list);
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
				for (int j = 0; j <= countNoOfDays(d1, d2); j++)
				{
					for (int i = 0; i < 10; i++)
					{
						bookingDates* temp = list[i].book;

						if (temp != NULL)
						{

							while (temp != NULL)
							{
								if (strcmp(temp->booked, t) == 0)
								{
									list[i].sameday++;
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

				for (int i = 0; i < 10; i++)
				{
					client.Send(&list[i].sameday, sizeof(int), 0);
					if (list[i].sameday == 0)
					{
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
				int flag = 0;
				char* nameRoom = NULL;
				room trueRoom;
				client.Receive((char*)&size, sizeof(int), 0);
				nameRoom = new char[size + 1];
				client.Receive((char*)&nameRoom[0], size, 0);
				nameRoom[size] = '\0';
				flag = findRoomByName(list, nameRoom, 10, trueRoom);
				client.Send(&flag, sizeof(flag), 0);
				if (flag == 0) {
					break;
				}			
				imageToSent[0].readImageFile(trueRoom.firstPhoto);
				imageToSent[1].readImageFile(trueRoom.secondPhoto);
				imageToSent[2].readImageFile("photo\\hotelOne\\single\\phongTam.jpg");
				flag = 1;
				for (int i = 0; i < 3; i++) {
					imencode(".jpg", imageToSent[i].data, dataStorage);
					size = dataStorage.size();
					client.Send(&size, sizeof(int), 0);
					client.Send(&dataStorage[0], size, 0);
				}
				delete[] list;
			}
			break;
		case 1:
			TTL++;
			if (featuresSwitch == 0) {
				int checker = 1;
				flag = 1;
				delete user1;
				user1 = new user;
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
						break;
					case 2:user1->add(buffer, user1->password);
					default: break;
					}
					flag++;
				}
				trueUser = loginConfirm(userList, user1->name, user1->password);
				user* temp = userList;
				while (temp != NULL) {
					cout << temp->name << " " << temp->password << " " << temp->STK << endl;
					temp = temp->next;
				}
				if (trueUser == NULL){
					checker = 0;
				}
				else {
					cout << trueUser->name << endl;
					checker = 1;
					featuresSwitch = 1;
				}
				client.Send(&makeSure, sizeof(int), 0);
				client.Send(&checker, sizeof(int), 0);
			}
			else {
				bill* totalBill = NULL;
				room* list = NULL;
				int size = 0, billCount = 0;
				int tempSize = 10;
				char* buffer = NULL, * hotelFile = NULL, * hotelName = NULL;
				int typeroom;
				bill hoadon;
				char* billFile = NULL;
				strCat(billFile, user1->name);
				upperCase(billFile);
				strCat(billFile, ".xml");
				readBillData(totalBill, billFile, billCount);

				client.Receive((char*)&size, sizeof(int), 0);
				if (size == 0) {
					break;
				}
				hotelFile = new char[size + 1];
				for (int i = 0; i < size; i = i + tempSize) {
					if (i + tempSize >= size) {
						tempSize = size - i;
					}
					client.Receive((char*)&hotelFile[i], tempSize, 0);
				}
				hotelFile[size] = '\0';
				strCat(hotelName, hotelFile);
				upperCase(hotelFile);
				hotelFile = mergeCharStr(hotelFile, ".xml");
				cout << hotelFile << endl;
				readHotelData(hotelFile, list);
				client.Receive((char*)&typeroom, sizeof(int), 0);
				if (typeroom == 0) {
					break;
				}
				cout << "type" << typeroom;
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

				char* note = NULL;
				client.Receive((char*)&size, sizeof(int), 0);
				if (size == 0) {
					return 0;
				}
				note = new char[size + 1];
				for (int i = 0; i < size; i = i + tempSize) {
					if (i + tempSize >= size) {
						tempSize = size - i;
					}
					client.Receive((char*)&note[i], tempSize, 0);
				}
				note[size] = '\0';

				char* t = buffer;
				cout << buffer << "/" << buffer2 << "/" << note << endl;
				hoadon.GhiChu = note;
				hoadon.nameHotel = hotelName;
				hoadon.dateIn = buffer;
				hoadon.dateOut = buffer2;
				date d1, d2;
				const char* sing = "single";
				const char* doub = "double";
				convertchargdatetoint(buffer, d1);
				convertchargdatetoint(buffer2, d2);
				int diseday = countNoOfDays(d1, d2);
				for (int j = 0; j <= countNoOfDays(d1, d2); j++)
				{

					for (int i = 0; i < 10; i++)
					{
						bookingDates* temp = list[i].book;
						if (typeroom == 1 && strcmp(list[i].type, doub) == 0)
						{
							list[i].sameday++;
						}
						if (typeroom == 2 && strcmp(list[i].type, sing) == 0)
						{
							list[i].sameday++;
						}

						if (temp != NULL)
						{
							while (temp != NULL)
							{
								if (strcmp(temp->booked, t) == 0)
								{
									list[i].sameday++;
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
				for (int i = 0; i < 10; i++) {
					client.Send(&list[i].sameday, sizeof(int), 0);
					if (list[i].sameday == 0)
					{
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
				int numroom;
				client.Receive((char*)&numroom, sizeof(int), 0);
				cout << numroom << endl;
				int* listroom = new int[numroom];
				long int tien = 0;
				long int priceroom;
				char* bookedRooms = NULL;
				billCount += 1;
				char* userBillID = new char[100];
				_itoa(billCount, userBillID, 10);
				if (numroom == 0)
				{
					cout << "Khach hang khong dat phong\n";
				}
				else
				{
					cout << "Cac phong khach da dat : ";
					for (int i = 0; i < numroom; i++)
					{
						client.Receive((char*)&listroom[i], sizeof(size), 0);
						cout << listroom[i];
						strCat(hoadon.list, list[listroom[i]].name);
						strCat(hoadon.list, " ");
						cout << list[listroom[i]].name << " ";
						char* temp = buffer;
						int lenb = strlen(buffer);
						for (int j = 0; j <= countNoOfDays(d1, d2); j++)
						{
							int len = 0;
							if (list[listroom[i]].bookedList != NULL)
							{
								len = strlen(list[listroom[i]].bookedList);
								list[listroom[i]].bookedList[len] = ' ';
								for (int j = 0; j < lenb; j++)
								{
									list[listroom[i]].bookedList[len + j + 1] = temp[j];
								}

								list[listroom[i]].bookedList[len + lenb + 1] = '\0';
							}
							else
							{
								list[listroom[i]].bookedList = temp;
							}
						}
						priceroom = atoi(list[listroom[i]].price);
						tien = (diseday + 1) * priceroom + tien;
					}
					char* tempID = user1->name;
					strCat(tempID, "_");
					strCat(tempID, userBillID);
					int tempSz = strlen(tempID);
					client.Send(&tempSz, sizeof(tempSz), 0);
					client.Send(tempID, tempSz, 0);
					client.Send(&tien, sizeof(size), 0);
					int tTL = timeTL();
					bill* tmpbill = createBill(tempID, hotelName, hoadon.list, hoadon.dateIn, hoadon.dateOut, note, tien, tTL);
					cout << tmpbill->billID << " " << tmpbill->nameHotel << " " << tmpbill->list << " " << tmpbill->dateIn << " " << tmpbill->dateOut << " " << tmpbill->GhiChu << " " << tmpbill->giaTien << endl;
					insertBill(totalBill, tmpbill);
				}
				cout << "Tien phong da dat " << tien;
				writeBillData(totalBill, billFile);
				writeHotelData(hotelFile, list);
				// Phan code cho dat phong
			}
			break;
		case 2:
			if (featuresSwitch != 0) {
				int size = 0;
				int billcount = 0;
				int flag = 0;
				room* list = NULL;
				bill* userBill = NULL;
				bill* currentBill = NULL;
				char* tempName = NULL;
				size = strlen(trueUser->name);
				tempName = new char[size + 1];
				strcpy_s(tempName, size + 1, trueUser->name);
				upperCase(tempName);
				strCat(tempName, ".xml");
				cout << tempName << endl;
				readBillData(userBill,tempName, billcount);
				cout << 2 << endl;
				char* billID = NULL;
				client.Receive((char*)&size, sizeof(int), 0);
				if (size == 0) break;
				billID = new char[size+1];
				client.Receive((char*)&billID[0], size, 0);
				billID[size] = '\0';
				bill* temp = userBill;
				cout << billID << endl;
				while(temp != NULL) {
					if (strcmp(temp->billID, billID) == 0) {
						currentBill = temp;
						break;
					}
					temp = temp->next;
				}
				if (currentBill == NULL) {
					client.Send(&flag, sizeof(flag), 0);
				}
				else {
					flag = 1;
					client.Send(&flag, sizeof(flag), 0);
					if (timeTL() - currentBill->TTL >= 24 * 3600) {
						flag = 0;
						client.Send(&flag, sizeof(flag), 0);
					}
					else {
						client.Send(&flag, sizeof(flag), 0);
						char* tempHotel = NULL;
						size = strlen(currentBill->nameHotel);
						tempHotel = new char[size + 1];
						strcpy_s(tempHotel, size + 1, currentBill->nameHotel);
						strCat(tempHotel, ".xml");
						upperCase(tempHotel);
						cout << tempHotel << endl;
						cout << tempName << endl;
						readHotelData(tempHotel, list);
						//Phan update ngay 
						cout << billcount << endl;
						cout << currentBill->billID << endl;
						deleteBill(userBill, currentBill);
						temp = userBill;
						while (temp != NULL) {
							cout << temp->billID << " ";
							temp = temp->next;	
						}
						writeBillData(userBill, tempName);
					}
				}
			}
			break;
		case 3:
			if (featuresSwitch != 0) {
				featuresSwitch = 0;
				trueUser = NULL;
				delete user1;
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
