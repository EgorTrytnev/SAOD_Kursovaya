#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <cmath>
#include <algorithm>
#include <unordered_map>
#include <queue>
#include <limits>
#include <iomanip>
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
const int MAX_SYMBOLS = 256;
const int MAX_CODE_LENGTH = 64;

// Базовые функции
int getYearFromBirthdate(const char* birthdate);
void heapify(vector<Record*>& arr, int n, int i);
void heapSort(vector<Record*>& arr);
bool loadDatabase(const char* filename, vector<Record*>& records);
void printCharArray(const char* arr, size_t size);
void printPage(const vector<Record*>& records, int page);
int lowerBoundByYear(const vector<Record*>& records, int year);
int upperBoundExclusiveByYear(const vector<Record*>& records, int year);
void printQueue(MyQueue<Record*> q);

// Хаффман структуры
struct SymbolData {
    unsigned char symbol;
    double probability;
    char code[MAX_CODE_LENGTH];
    int code_length;
};

// Хаффман функции (из эталонного кода)
int Up(int n, double q, double P[]);
void Down(int n, int j, SymbolData C[], int L[]);
void Huffman(int n, double P[], SymbolData C[], int L[]);
void buildHuffmanCodes(SymbolData* table, int symbol_count);

// Анализ
double calculateEntropy(SymbolData* table, int symbol_count);
double calculateAverageLength(SymbolData* table, int symbol_count);
double checkKraftInequality(SymbolData* table, int symbol_count);
void printHuffmanTable(SymbolData* table, int symbol_count);
void printHuffmanAnalysis(SymbolData* table, int symbol_count, double entropy);

// Сжатие
void saveCompressedDatabase(const char* filename, const vector<Record*>& records, SymbolData* table, int symbol_count);
double calculateCompressionRatio(const char* originalFile, const char* compressedFile);
