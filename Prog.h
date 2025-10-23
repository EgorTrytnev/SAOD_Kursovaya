#include <iostream>
#include <fstream>
#include <cstdint>
#include <cstring>
#include <vector>
#include "MyQueue.h"
#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

struct Record {
    char fio[30];
    uint16_t department;
    char position[22];
    char birthdate[10]; // формат дд-мм-гг
};

using namespace std;

const int TOTAL_RECORDS = 4000;
const int PAGE_SIZE = 20;

int getYearFromBirthdate(const char* birthdate);
void heapify(vector<Record*>& arr, int n, int i);
void heapSort(vector<Record*>& arr);
bool loadDatabase(const char* filename, vector<Record*>& records);
void printCharArray(const char* arr, size_t size);

void printPage(const vector<Record*>& records, int page);
int lowerBoundByYear(const vector<Record*>& records, int year);
int upperBoundExclusiveByYear(const vector<Record*>& records, int year);
void printQueue(MyQueue<Record*> q);
