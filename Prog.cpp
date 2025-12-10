#include "Prog.h"

#include <cstring>
#include <algorithm>
#include <map>

// ============= БАЗОВЫЕ ФУНКЦИИ =============

int getYearFromBirthdate(const char* birthdate) {
    int year = (birthdate[6] - '0') * 10 + (birthdate[7] - '0');
    if (year < 50)
        year += 2000;
    else
        year += 1900;
    return year;
}

void swap_records(Record*& a, Record*& b) {
    Record* temp = a;
    a = b;
    b = temp;
}

void heapify(vector<Record*>& arr, int n, int i) {
    int largest = i;
    int l = 2 * i + 1;
    int r = 2 * i + 2;
    if (l < n && getYearFromBirthdate(arr[l]->birthdate) > getYearFromBirthdate(arr[largest]->birthdate)) {
        largest = l;
    }
    if (r < n && getYearFromBirthdate(arr[r]->birthdate) > getYearFromBirthdate(arr[largest]->birthdate)) {
        largest = r;
    }
    if (largest != i) {
        swap_records(arr[i], arr[largest]);
        heapify(arr, n, largest);
    }
}

void heapSort(vector<Record*>& arr) {
    int n = (int)arr.size();
    for (int i = n / 2 - 1; i >= 0; i--) {
        heapify(arr, n, i);
    }
    for (int i = n - 1; i >= 0; i--) {
        swap_records(arr[0], arr[i]);
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

// ============= SHANNON-FANO КОДИРОВАНИЕ =============

struct ShannonSymbol {
    unsigned char value;
    unsigned long long count;
    double p;
    double Q;
    int L;
    std::string code;
};

// Перевод дроби в двоичную строку длины L
std::string frac_to_binary(double x, int L) {
    std::string res;
    double v = x;
    for (int i = 0; i < L; ++i) {
        v *= 2.0;
        if (v >= 1.0) {
            res += '1';
            v -= 1.0;
        } else {
            res += '0';
        }
    }
    return res;
}

// Сравнение для сортировки по вероятности (убыванию)
int compare_shannon_symbols(const ShannonSymbol& a, const ShannonSymbol& b) {
    if (a.p > b.p) return -1;
    if (a.p < b.p) return 1;
    if (a.value < b.value) return -1;
    if (a.value > b.value) return 1;
    return 0;
}

void swap_shannon_symbols(ShannonSymbol& a, ShannonSymbol& b) {
    ShannonSymbol tmp = a;
    a = b;
    b = tmp;
}

int partition_shannon_symbols(std::vector<ShannonSymbol>& A, int L, int R) {
    ShannonSymbol X = A[L];
    int i = L;
    int j = R;
    while (true) {
        while (compare_shannon_symbols(A[i], X) < 0) {
            i++;
        }
        while (compare_shannon_symbols(A[j], X) > 0) {
            j--;
        }
        if (i >= j) {
            return j;
        }
        swap_shannon_symbols(A[i], A[j]);
        i++;
        j--;
    }
}

void shannon_quick_sort(std::vector<ShannonSymbol>& A, int L, int R) {
    while (L < R) {
        int j = partition_shannon_symbols(A, L, R);
        if (j - L < R - j) {
            shannon_quick_sort(A, L, j);
            L = j + 1;
        } else {
            shannon_quick_sort(A, j + 1, R);
            R = j;
        }
    }
}

void build_shannon_codes(const vector<Record*>& records,
                         std::vector<ShannonSymbol>& symbols,
                         std::map<unsigned char, std::string>& code_map,
                         unsigned long long& total_bytes)
{
    unsigned long long freq[256] = {0};

    // Подсчитываем частоты символов
    total_bytes = 0;
    for (const auto& rec : records) {
        for (int i = 0; i < 30; ++i) if (rec->fio[i] != '\0') {
            freq[(unsigned char)rec->fio[i]]++;
            total_bytes++;
        }
        for (int i = 0; i < 22; ++i) if (rec->position[i] != '\0') {
            freq[(unsigned char)rec->position[i]]++;
            total_bytes++;
        }
        for (int i = 0; i < 10; ++i) if (rec->birthdate[i] != '\0') {
            freq[(unsigned char)rec->birthdate[i]]++;
            total_bytes++;
        }
    }

    if (total_bytes == 0) {
        std::cout << "Source is empty!\n";
        return;
    }

    // Список реально встречающихся символов
    symbols.clear();
    for (int i = 0; i < 256; ++i) {
        if (freq[i] > 0) {
            ShannonSymbol s;
            s.value = static_cast<unsigned char>(i);
            s.count = freq[i];
            s.p = static_cast<double>(freq[i]) / static_cast<double>(total_bytes);
            s.Q = 0.0;
            s.L = 0;
            s.code = "";
            symbols.push_back(s);
        }
    }

    if (symbols.empty()) {
        return;
    }

    // Сортировка по вероятности (убыванию)
    shannon_quick_sort(symbols, 0, (int)symbols.size() - 1);

    // Вычисление Q_i и L_i
    double Qprev = 0.0;
    code_map.clear();

    for (size_t i = 0; i < symbols.size(); ++i) {
        symbols[i].Q = Qprev;
        Qprev += symbols[i].p;

        if (symbols[i].p > 0.0) {
            double Li = -std::log2(symbols[i].p);
            symbols[i].L = (int)std::ceil(Li);
        } else {
            symbols[i].L = 1;
        }

        symbols[i].code = frac_to_binary(symbols[i].Q, symbols[i].L);
        code_map[symbols[i].value] = symbols[i].code;
    }
}

void destroyHuffmanTree(HuffmanNode* node) {
    if (!node) return;
    destroyHuffmanTree(node->left);
    destroyHuffmanTree(node->right);
    delete node;
}

void buildHuffmanTree(const vector<Record*>& records, unordered_map<char, string>& codes) {
    unordered_map<char, int> freq;

    // Подсчитываем частоты символов
    for (const auto& rec : records) {
        for (int i = 0; i < 30; ++i) if (rec->fio[i] != '\0') freq[rec->fio[i]]++;
        for (int i = 0; i < 22; ++i) if (rec->position[i] != '\0') freq[rec->position[i]]++;
        for (int i = 0; i < 10; ++i) if (rec->birthdate[i] != '\0') freq[rec->birthdate[i]]++;
    }

    if (freq.empty()) return;

    // Случай одного символа
    if (freq.size() == 1) {
        codes[freq.begin()->first] = "0";
        return;
    }

    // Создаем приоритетную очередь
    priority_queue<HuffmanNode*, vector<HuffmanNode*>, CompareNode> pq;
    for (auto& p : freq) {
        pq.push(new HuffmanNode(p.first, p.second));
    }

    // Строим дерево Хаффмена
    while (pq.size() > 1) {
        HuffmanNode* left = pq.top(); pq.pop();
        HuffmanNode* right = pq.top(); pq.pop();
        HuffmanNode* parent = new HuffmanNode('\0', left->freq + right->freq);
        parent->left = left;
        parent->right = right;
        pq.push(parent);
    }

    HuffmanNode* root = pq.top(); pq.pop();
    if (root) {
        generateCodes(root, "", codes);
        destroyHuffmanTree(root);
    }
}

void generateCodes(HuffmanNode* root, const string& code, unordered_map<char, string>& codes) {
    if (!root) return;
    // Листовой узел
    if (!root->left && !root->right) {
        codes[root->ch] = code.empty() ? "0" : code;
        return;
    }
    generateCodes(root->left, code + "0", codes);
    generateCodes(root->right, code + "1", codes);
}

// ============= ФУНКЦИИ АНАЛИЗА =============

double calculateEntropy(const vector<Record*>& records) {
    unordered_map<char, long long> freq;
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
    unordered_map<char, long long> freq;
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

double calculateKraftSum(const unordered_map<char, string>& codes) {
    double kraft = 0.0;
    for (const auto& p : codes) {
        kraft += pow(2.0, -p.second.length());
    }
    return kraft;
}

// ============= ФУНКЦИИ ВЫВОДА =============

string formatSymbol(char ch) {
    if (ch == '\0') return "\\0";
    if (ch == ' ') return "SPACE";
    if (ch < 32 || ch > 126) {
        char buf[8];
        snprintf(buf, sizeof(buf), "0x%02X", (unsigned char)ch);
        return string(buf);
    }
    return string(1, ch);
}

void printHuffmanTable(const unordered_map<char, string>& codes,
                       const unordered_map<char, long long>& freq, long long totalChars) {
    cout << "\n" << string(90, '=') << "\n";
    cout << setw(40) << "ТАБЛИЦА КОДОВ ХАФФМЕНА" << "\n";
    cout << string(90, '=') << "\n";
    cout << left << setw(10) << "Символ"
         << setw(12) << "Частота"
         << setw(15) << "Вероятность"
         << setw(8) << "Длина"
         << setw(20) << "Крафт"
         << setw(20) << "Код" << endl;
    cout << string(90, '-') << "\n";

    struct HuffmanSymbol {
        char ch;
        long long freq;
        double prob;
        int codeLen;
        string code;
        double kraft;
    };

    vector<HuffmanSymbol> symbols;
    double totalKraft = 0.0;
    for (const auto& p : freq) {
        auto it = codes.find(p.first);
        if (it != codes.end()) {
            HuffmanSymbol sym;
            sym.ch = p.first;
            sym.freq = p.second;
            sym.prob = (double)p.second / totalChars;
            sym.codeLen = it->second.length();
            sym.code = it->second;
            sym.kraft = pow(2.0, -sym.codeLen);
            totalKraft += sym.kraft;
            symbols.push_back(sym);
        }
    }

    // Сортируем по частоте
    sort(symbols.begin(), symbols.end(), [](const HuffmanSymbol& a, const HuffmanSymbol& b) {
        return a.freq > b.freq;
    });

    for (const auto& sym : symbols) {
        cout << left << setw(10) << formatSymbol(sym.ch)
             << setw(12) << sym.freq
             << fixed << setprecision(6) << setw(15) << sym.prob
             << setw(8) << sym.codeLen
             << setw(20) << sym.kraft
             << setw(20) << sym.code << endl;
    }

    cout << string(90, '=') << "\n";
    cout << "ВСЕГО СИМВОЛОВ: " << symbols.size() << endl;
    cout << "Сумма Крафта (должна быть ≤ 1): " << fixed << setprecision(6) << totalKraft << endl;
    cout << string(90, '=') << "\n\n";
}

void printHuffmanAnalysis(const vector<Record*>& records, const unordered_map<char, string>& codes) {
    double entropy = calculateEntropy(records);
    double avgLen = calculateAvgCodeLength(records, codes);
    double kraft = calculateKraftSum(codes);
    double efficiency = (entropy / avgLen) * 100.0;
    double redundancy = avgLen - entropy;

    cout << "\n" << string(90, '=') << "\n";
    cout << setw(40) << "АНАЛИЗ КОДА ХАФФМЕНА" << "\n";
    cout << string(90, '=') << "\n";
    cout << left << setw(40) << "Энтропия источника (H):"
         << fixed << setprecision(6) << setw(15) << entropy << " бит/символ\n";
    cout << left << setw(40) << "Средняя длина кода (L):"
         << fixed << setprecision(6) << setw(15) << avgLen << " бит/символ\n";
    cout << left << setw(40) << "Неравенство Крафта (∑2^-li):"
         << fixed << setprecision(6) << setw(15) << kraft
         << (kraft <= 1.0 ? " ✓ ВЫПОЛНЕНО" : " ✗ НЕ ВЫПОЛНЕНО") << "\n";
    cout << left << setw(40) << "Эффективность кодирования:"
         << fixed << setprecision(2) << setw(15) << efficiency << " %\n";
    cout << left << setw(40) << "Избыточность:"
         << fixed << setprecision(6) << setw(15) << redundancy << " бит/символ\n";
    cout << string(90, '=') << "\n\n";
}

// ============= ФУНКЦИИ СЖАТИЯ =============

void saveCompressedDatabase(const char* filename, const vector<Record*>& records,
                           const unordered_map<char, string>& codes) {
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
    cout << "Сжатая база данных сохранена: " << filename << endl;
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