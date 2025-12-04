#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <cmath>
#include <algorithm>
#include <unordered_map>
#include <queue>
#include <limits>
#include "MyQueue.h"

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

struct Record {
    char fio[30];
    uint16_t department;
    char position[22];
    char birthdate[10];
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

struct HuffmanNode {
    char ch;
    int freq;
    HuffmanNode* left;
    HuffmanNode* right;
    HuffmanNode(char c, int f) : ch(c), freq(f), left(nullptr), right(nullptr) {}
};

struct CompareNode {
    bool operator()(HuffmanNode* a, HuffmanNode* b) {
        return a->freq > b->freq;
    }
};

void buildHuffmanTree(const vector<Record*>& records, unordered_map<char, string>& codes);
void generateCodes(HuffmanNode* root, const string& code, unordered_map<char, string>& codes);
double calculateEntropy(const vector<Record*>& records);
double calculateAvgCodeLength(const vector<Record*>& records, const unordered_map<char, string>& codes);
void destroyHuffmanTree(HuffmanNode* node);
void saveCompressedDatabase(const char* filename, const vector<Record*>& records, const unordered_map<char, string>& codes);
double calculateCompressionRatio(const char* originalFile, const char* compressedFile);