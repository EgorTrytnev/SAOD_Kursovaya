#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <queue>
#include <cmath>
#include <string>
#include <algorithm>
#include <iomanip>
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

// ============= Базовые функции =============

int getYearFromBirthdate(const char* birthdate);

void heapify(vector<Record*>& arr, int n, int i);

void heapSort(vector<Record*>& arr);

bool loadDatabase(const char* filename, vector<Record*>& records);

void printCharArray(const char* arr, size_t size);

void printPage(const vector<Record*>& records, int page);

int lowerBoundByYear(const vector<Record*>& records, int year);

int upperBoundExclusiveByYear(const vector<Record*>& records, int year);

void printQueue(MyQueue<Record*> q);

// ============= Структуры для Хаффмена =============

struct HuffmanNode {
    char ch;
    int freq;
    HuffmanNode* left;
    HuffmanNode* right;
    HuffmanNode(char ch, int freq) : ch(ch), freq(freq), left(nullptr), right(nullptr) {}
};

struct CompareNode {
    bool operator()(HuffmanNode* a, HuffmanNode* b) const {
        return a->freq > b->freq;
    }
};

struct HuffmanSymbol {
    char ch;
    int freq;
    double prob;
    int codeLen;
    string code;
    double kraft;
};

// ============= Функции Хаффмена =============

void buildHuffmanTree(const vector<Record*>& records, unordered_map<char, string>& codes);

void generateCodes(HuffmanNode* root, const string& code, unordered_map<char, string>& codes);

void destroyHuffmanTree(HuffmanNode* node);

// ============= Функции анализа кодирования =============

double calculateEntropy(const vector<Record*>& records);

double calculateAvgCodeLength(const vector<Record*>& records, const unordered_map<char, string>& codes);

double calculateKraftSum(const unordered_map<char, string>& codes);

// ============= Функции вывода =============

void printHuffmanTable(const unordered_map<char, string>& codes,
                       const unordered_map<char, long long>& freq, long long totalChars);

void printHuffmanAnalysis(const vector<Record*>& records, const unordered_map<char, string>& codes);

// ============= Функции сжатия =============

void saveCompressedDatabase(const char* filename, const vector<Record*>& records,
                           const unordered_map<char, string>& codes);

double calculateCompressionRatio(const char* originalFile, const char* compressedFile);