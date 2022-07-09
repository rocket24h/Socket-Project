#pragma once
#include <math.h>
struct date
{
	int day, month, year;
};
const int monthDays[12] = { 31, 59, 90, 120, 151, 181, 212, 243,273, 304, 334, 365 };
bool checkdate(date d);
void convertchargdatetoint(char* s, date& d);
int countNoOfDays(date date1, date date2);
date nextday(date d);
int countLeapYearDays(date d);
bool checkleapyear(int year);
int maxday(date d);
int lengthint(int n);
#include <iostream>
#include "pch.h"
using namespace std;
bool checkdate(date d)
{
	if (d.day <= 0 || d.month <= 0 || d.year < 0) return false;
	if (d.month > 12) return false;
	if (d.day > maxday(d)) return false;
	return true;
}
bool checkleapyear(int year)
{
	if ((year % 400) == 0) return true;
	else
	{
		if (year % 4 == 0) return true;
	}
	return false;
}
int maxday(date d)
{
	switch (d.month)
	{
	case 1: case 3:case 5:case 7:case 8: case 10: case 12:
	{
		return 31;
	}
	break;
	case 4:case 6: case 9: case 11:
	{
		return 30;
	}
	break;
	case 2:
	{
		if (checkleapyear(d.year))
		{
			return 29;
		}
		else
		{
			return 28;
		}
	}
	break;
	default:

		break;
	}
	return -1;
}
date nextday(date d)
{
	date da;
	da.day = d.day + 1;
	da.month = d.month;
	da.year = d.year;
	if (d.month == 12)
	{
		if (d.day == maxday(d))
		{
			da.day = 1;
			da.month = 1;
			da.year = d.year + 1;
		}
	}
	else
	{
		if (d.day == maxday(d))
		{
			da.day = 1;
			da.month = d.month + 1;
		}
	}
	return da;
}
void convertchargdatetoint(char* s, date& d)
{
	d.day = (s[0] - '0') * 10 + (s[1] - '0');
	d.month = (s[3] - '0') * 10 + (s[4] - '0');
	if (s[9] != '\0')
	{
		d.year = (s[6] - '0') * 10 * 10 * 10 + (s[7] - '0') * 10 * 10 + (s[8] - '0') * 10 + (s[9] - '0');
		return;
	}
	if (s[8] != '\0')
	{
		d.year = (s[6] - '0') * 10 * 10 + (s[7] - '0') * 10 + (s[8] - '0');
		return;
	}
	if (s[7] != '\0')
	{
		d.year = (s[6] - '0') * 10 + (s[7] - '0');
		return;
	}
	if (s[6] != '\0')
	{
		d.year = (s[6] - '0');
		return;
	}
}

void convertdate(date d, char*& s)
{
	s = new char[11];
	if (d.day < 10)
	{
		s[0] = '0';
		s[1] = d.day + '0';
	}
	else
	{
		s[0] = d.day / 10 + '0';
		s[1] = d.day % 10 + '0';
	}
	s[2] = '/';
	if (d.month < 10)
	{
		s[3] = '0';
		s[4] = d.month + '0';
	}
	else
	{
		s[3] = '1';
		s[4] = d.month % 10 + '0';
	}
	s[5] = '/';

	int i = 5;
	int y = d.year;
	while (i < 10)
	{
		if ((i + lengthint(d.year)) != 9)
		{
			s[++i] = '0';
		}
		else
		{

			for (int j = lengthint(d.year); j > 0; j--)
			{
				s[i + j] = y % 10 + '0';
				y = y / 10;
			}
			i = 10;
		}
	}
	s[10] = '\0';

}
int lengthint(int n)
{
	int d = 0;
	if (n < 10) return 1;
	while (n > 0)
	{
		d++;
		n = n / 10;
	}
	return d;
}

int countLeapYearDays(date d) {
	int years = d.year;
	if (d.month <= 2)
		years--;
	return ((years / 4) - (years / 100) + (years / 400));
}
int countNoOfDays(date date1, date date2) {
	long int dayCount1 = (date1.year * 365);
	dayCount1 += monthDays[date1.month];
	dayCount1 += date1.day;
	dayCount1 += countLeapYearDays(date1);
	long int dayCount2 = (date2.year * 365);
	dayCount2 += monthDays[date2.month];
	dayCount2 += date2.day;
	dayCount2 += countLeapYearDays(date2);
	return ((dayCount2 - dayCount1));
}