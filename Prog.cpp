#include "Prog.h"
#include <queue>
#include <algorithm>


int getYearFromBirthdate(const char* birthdate) {
    int year = (birthdate[6] - '0') * 10 + (birthdate[7] - '0');
    if (year < 50)
        year += 2000;
    else
        year += 1900;
    return year;
}

void swap(Record*& a, Record*& b) {
    Record* temp = a;
    a = b;
    b = temp;
}

void heapify(vector<Record*>& arr, int n, int i) {
    int largest = i;
    int l = 2*i + 1;
    int r = 2*i + 2;
    
    if (l < n && getYearFromBirthdate(arr[l]->birthdate) > getYearFromBirthdate(arr[largest]->birthdate)) {
        largest = l;
    }
    
    if (r < n && getYearFromBirthdate(arr[r]->birthdate) > getYearFromBirthdate(arr[largest]->birthdate)) {
        largest = r;
    }
    
    if (largest != i) {
        swap(arr[i], arr[largest]);
        heapify(arr, n, largest);
    }
}

void heapSort(vector<Record*>& arr) {
    int n = (int)arr.size();
    for (int i = n / 2 - 1; i >= 0; i--) {
        heapify(arr, n, i);
    }
    
    for (int i = n - 1; i >= 0; i--) {
        swap(arr[0], arr[i]);
        heapify(arr, i, 0);
    }
}

bool loadDatabase(const char* filename, vector<Record*>& records) {
    ifstream file(filename, ios::binary);
    if (!file.is_open()) {
        cerr << "Cannot open file: " << filename << endl;
        return false;
    }
    
    records.reserve(TOTAL_RECORDS);
    for (int i = 0; i < TOTAL_RECORDS; ++i) {
        Record* rec = new Record();
        file.read((char*)(rec), sizeof(Record));
        if (!file) {
            delete rec;
            break;
        }
        records.push_back(rec);
    }
    
    file.close();
    return true;
}

void printCharArray(const char* arr, size_t size) {
    int length = (int)size;
    while (length > 0 && arr[length - 1] == ' ') --length;
    for (int i = 0; i < length; ++i) {
        cout << arr[i];
    }
}

void printPage(const vector<Record*>& records, int page) {
    int start = page * PAGE_SIZE;
    int end = start + PAGE_SIZE;
    
    if (start >= (int)records.size()) {
        cout << "No records found" << endl;
        return;
    }
    
    if (end > (int)records.size()) end = (int)records.size();
    
    cout << "Page " << (page + 1) << ":" << endl;
    for (int i = start; i < end; ++i) {
        cout << i + 1 << ". Name: ";
        printCharArray(records[i]->fio, 30);
        cout << ", Department: " << records[i]->department << ", Position: ";
        printCharArray(records[i]->position, 22);
        cout << ", Birthday: ";
        printCharArray(records[i]->birthdate, 10);
        cout << " (Year " << getYearFromBirthdate(records[i]->birthdate) << ")" << endl;
    }
}

int lowerBoundByYear(const vector<Record*>& records, int year) {
    int n = (int)records.size();
    int l = -1, r = n;
    
    while (r - l > 1) {
        int m = l + (r - l) / 2;
        int y = getYearFromBirthdate(records[m]->birthdate);
        
        if (y < year) {
            l = m;
        } else {
            r = m;
        }
    }
    
    if (r < n && getYearFromBirthdate(records[r]->birthdate) == year) return r;
    return -1;
}

int upperBoundExclusiveByYear(const vector<Record*>& records, int year) {
    int n = (int)records.size();
    int l = -1, r = n;
    
    while (r - l > 1) {
        int m = l + (r - l) / 2;
        int y = getYearFromBirthdate(records[m]->birthdate);
        
        if (y <= year) {
            l = m;
        } else {
            r = m;
        }
    }
    
    return r;
}

void printQueue(MyQueue<Record*> q) {
    if (q.empty()) {
        cout << "Queue is empty" << endl;
        return;
    }
    
    size_t idx = 1;
    while (!q.empty()) {
        Record* r = q.front();
        q.pop();
        
        cout << idx++ << ". Name: ";
        printCharArray(r->fio, 30);
        cout << ", Department: " << r->department << ", Position: ";
        printCharArray(r->position, 22);
        cout << ", Birthday: ";
        printCharArray(r->birthdate, 10);
        cout << " (Year " << getYearFromBirthdate(r->birthdate) << ")" << endl;
    }
}



void generateCodes(HuffmanNode* root, const string& code, unordered_map<char, string>& codes) {
    if (!root) return;
    
    // Если это листовой узел (содержит символ)
    if (!root->left && !root->right) {
        codes[root->ch] = code.empty() ? "0" : code;
        return;
    }
    
    generateCodes(root->left, code + "0", codes);
    generateCodes(root->right, code + "1", codes);
}

void destroyHuffmanTree(HuffmanNode* node) {
    if (!node) return;
    destroyHuffmanTree(node->left);
    destroyHuffmanTree(node->right);
    delete node;
}

void buildHuffmanTree(const vector<Record*>& records, unordered_map<char, string>& codes) {
    // Подсчитываем частоты символов
    unordered_map<char, int> freq;
    
    for (const auto& rec : records) {
        for (int i = 0; i < 30; ++i) if (rec->fio[i] != '\0') freq[rec->fio[i]]++;
        for (int i = 0; i < 22; ++i) if (rec->position[i] != '\0') freq[rec->position[i]]++;
        for (int i = 0; i < 10; ++i) if (rec->birthdate[i] != '\0') freq[rec->birthdate[i]]++;
    }
    
    // Если нет символов, выходим
    if (freq.empty()) {
        return;
    }
    
    // Если только один уникальный символ
    if (freq.size() == 1) {
        codes[freq.begin()->first] = "0";
        return;
    }
    
    // Создаём приоритетную очередь с листовыми узлами
    priority_queue<HuffmanNode*, vector<HuffmanNode*>, CompareNode> pq;
    
    for (auto& p : freq) {
        pq.push(new HuffmanNode(p.first, p.second));
    }
    
    // Строим дерево Хаффмена
    while (pq.size() > 1) {
        HuffmanNode* left = pq.top();
        pq.pop();
        
        HuffmanNode* right = pq.top();
        pq.pop();
        
        // Создаём внутренний узел с суммарной частотой
        HuffmanNode* parent = new HuffmanNode('\0', left->freq + right->freq);
        parent->left = left;
        parent->right = right;
        
        pq.push(parent);
    }
    
    // Получаем корень дерева
    HuffmanNode* root = pq.top();
    pq.pop();
    
    // Генерируем коды
    if (root) {
        generateCodes(root, "", codes);
        destroyHuffmanTree(root);
    }
}

double calculateEntropy(const vector<Record*>& records) {
    unordered_map<char, int> freq;
    long long totalChars = 0;
    
    for (const auto& rec : records) {
        for (int i = 0; i < 30; ++i) if (rec->fio[i] != '\0') { freq[rec->fio[i]]++; totalChars++; }
        for (int i = 0; i < 22; ++i) if (rec->position[i] != '\0') { freq[rec->position[i]]++; totalChars++; }
        for (int i = 0; i < 10; ++i) if (rec->birthdate[i] != '\0') { freq[rec->birthdate[i]]++; totalChars++; }
    }
    
    double entropy = 0.0;
    for (auto& p : freq) {
        double prob = (double)p.second / totalChars;
        entropy -= prob * log2(prob);
    }
    
    return entropy;
}

double calculateAvgCodeLength(const vector<Record*>& records, const unordered_map<char, string>& codes) {
    unordered_map<char, int> freq;
    long long totalChars = 0;
    
    for (const auto& rec : records) {
        for (int i = 0; i < 30; ++i) if (rec->fio[i] != '\0') { freq[rec->fio[i]]++; totalChars++; }
        for (int i = 0; i < 22; ++i) if (rec->position[i] != '\0') { freq[rec->position[i]]++; totalChars++; }
        for (int i = 0; i < 10; ++i) if (rec->birthdate[i] != '\0') { freq[rec->birthdate[i]]++; totalChars++; }
    }
    
    double avgLength = 0.0;
    for (auto& p : freq) {
        if (codes.find(p.first) != codes.end()) {
            double prob = (double)p.second / totalChars;
            avgLength += prob * codes.at(p.first).length();
        }
    }
    
    return avgLength;
}

void saveCompressedDatabase(const char* filename, const vector<Record*>& records, const unordered_map<char, string>& codes) {
    ofstream file(filename, ios::binary);
    if (!file.is_open()) {
        cerr << "Cannot create file: " << filename << endl;
        return;
    }
    
    string encoded = "";
    for (const auto& rec : records) {
        for (int i = 0; i < 30; ++i) {
            if (rec->fio[i] != '\0' && codes.find(rec->fio[i]) != codes.end()) {
                encoded += codes.at(rec->fio[i]);
            }
        }
        for (int i = 0; i < 22; ++i) {
            if (rec->position[i] != '\0' && codes.find(rec->position[i]) != codes.end()) {
                encoded += codes.at(rec->position[i]);
            }
        }
        for (int i = 0; i < 10; ++i) {
            if (rec->birthdate[i] != '\0' && codes.find(rec->birthdate[i]) != codes.end()) {
                encoded += codes.at(rec->birthdate[i]);
            }
        }
    }
    
    // Дополняем нулями до полного байта
    while (encoded.length() % 8 != 0) encoded += "0";
    
    // Записываем закодированные данные
    for (size_t i = 0; i < encoded.length(); i += 8) {
        char byte = 0;
        for (int j = 0; j < 8; ++j) {
            byte = (byte << 1) | (encoded[i + j] - '0');
        }
        file.write(&byte, 1);
    }
    
    file.close();
    cout << "Compressed database saved to: " << filename << endl;
}

double calculateCompressionRatio(const char* originalFile, const char* compressedFile) {
    ifstream orig(originalFile, ios::binary | ios::ate);
    ifstream comp(compressedFile, ios::binary | ios::ate);
    
    if (!orig.is_open() || !comp.is_open()) {
        cerr << "Cannot open files for comparison" << endl;
        return 0.0;
    }
    
    long long origSize = orig.tellg();
    long long compSize = comp.tellg();
    
    orig.close();
    comp.close();
    
    if (origSize == 0) return 0.0;
    return (1.0 - (double)compSize / origSize) * 100.0;
}