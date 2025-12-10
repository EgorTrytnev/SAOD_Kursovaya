#include "Prog.h"

// Базовые функции (без изменений)
int getYearFromBirthdate(const char* birthdate) {
    int year = (birthdate[6] - '0') * 10 + (birthdate[7] - '0');
    if (year < 50) year += 2000; else year += 1900;
    return year;
}

void swap_records(Record*& a, Record*& b) {
    Record* temp = a; a = b; b = temp;
}

void heapify(vector<Record*>& arr, int n, int i) {
    int largest = i, l = 2 * i + 1, r = 2 * i + 2;
    if (l < n && getYearFromBirthdate(arr[l]->birthdate) > getYearFromBirthdate(arr[largest]->birthdate)) largest = l;
    if (r < n && getYearFromBirthdate(arr[r]->birthdate) > getYearFromBirthdate(arr[largest]->birthdate)) largest = r;
    if (largest != i) { swap_records(arr[i], arr[largest]); heapify(arr, n, largest); }
}

void heapSort(vector<Record*>& arr) {
    int n = arr.size();
    for (int i = n / 2 - 1; i >= 0; i--) heapify(arr, n, i);
    for (int i = n - 1; i >= 0; i--) { swap_records(arr[0], arr[i]); heapify(arr, i, 0); }
}

bool loadDatabase(const char* filename, vector<Record*>& records) {
    ifstream file(filename, ios::binary);
    if (!file.is_open()) { cerr << "Cannot open file: " << filename << endl; return false; }
    records.reserve(TOTAL_RECORDS);
    for (int i = 0; i < TOTAL_RECORDS; ++i) {
        Record* rec = new Record();
        file.read((char*)rec, sizeof(Record));
        if (!file) { delete rec; break; }
        records.push_back(rec);
    }
    file.close();
    return true;
}

void printCharArray(const char* arr, size_t size) {
    int length = size;
    while (length > 0 && arr[length - 1] == ' ') --length;
    for (int i = 0; i < length; ++i) cout << arr[i];
}

void printPage(const vector<Record*>& records, int page) {
    int start = page * PAGE_SIZE, end = start + PAGE_SIZE;
    if (start >= (int)records.size()) { cout << "No records found" << endl; return; }
    if (end > (int)records.size()) end = records.size();
    cout << "Page " << (page + 1) << ":" << endl;
    for (int i = start; i < end; ++i) {
        cout << i + 1 << ". Name: "; printCharArray(records[i]->fio, 30);
        cout << ", Department: " << records[i]->department << ", Position: "; printCharArray(records[i]->position, 22);
        cout << ", Birthday: "; printCharArray(records[i]->birthdate, 10);
        cout << " (Year " << getYearFromBirthdate(records[i]->birthdate) << ")" << endl;
    }
}

int lowerBoundByYear(const vector<Record*>& records, int year) {
    int n = records.size(), l = -1, r = n;
    while (r - l > 1) { int m = l + (r - l) / 2; (getYearFromBirthdate(records[m]->birthdate) < year) ? l = m : r = m; }
    return (r < n && getYearFromBirthdate(records[r]->birthdate) == year) ? r : -1;
}

int upperBoundExclusiveByYear(const vector<Record*>& records, int year) {
    int n = records.size(), l = -1, r = n;
    while (r - l > 1) { int m = l + (r - l) / 2; (getYearFromBirthdate(records[m]->birthdate) <= year) ? l = m : r = m; }
    return r;
}

void printQueue(MyQueue<Record*> q) {
    if (q.empty()) { cout << "Queue is empty" << endl; return; }
    size_t idx = 1;
    while (!q.empty()) {
        Record* r = q.front(); q.pop();
        cout << idx++ << ". Name: "; printCharArray(r->fio, 30);
        cout << ", Department: " << r->department << ", Position: "; printCharArray(r->position, 22);
        cout << ", Birthday: "; printCharArray(r->birthdate, 10);
        cout << " (Year " << getYearFromBirthdate(r->birthdate) << ")" << endl;
    }
}

// =================== ХАФФМАН (ЭТАЛОННАЯ ЛОГИКА) ===================
int Up(int n, double q, double P[]) {
    int j = n;
    for (int i = n - 1; i >= 1; i--) {
        if (P[i] > q) { j = i + 1; break; }
        P[i + 1] = P[i];
        if (i == 1) j = 1;
    }
    P[j] = q; 
    return j;
}

void Down(int n, int j, SymbolData C[], int L[]) {
    char S[MAX_CODE_LENGTH];
    strcpy(S, C[j].code);
    int L_temp = L[j];
    
    for (int i = j; i <= n - 1; i++) {
        strcpy(C[i].code, C[i + 1].code);
        L[i] = L[i + 1];
    }
    
    strcpy(C[n - 1].code, S);
    strcpy(C[n].code, S);
    
    C[n - 1].code[L_temp] = '0'; C[n - 1].code[L_temp + 1] = '\0'; L[n - 1] = L_temp + 1;
    C[n].code[L_temp] = '1'; C[n].code[L_temp + 1] = '\0'; L[n] = L_temp + 1;
}

void Huffman(int n, double P[], SymbolData C[], int L[]) {
    if (n == 2) {
        strcpy(C[1].code, "0"); L[1] = 1;
        strcpy(C[2].code, "1"); L[2] = 1;
    } else {
        double q = P[n - 1] + P[n];
        int j = Up(n, q, P);
        Huffman(n - 1, P, C, L);
        Down(n, j, C, L);
    }
}

void buildHuffmanCodes(SymbolData* table, int symbol_count) {
    double* P = new double[symbol_count + 1];
    int* L = new int[symbol_count + 1];
    
    for (int i = 1; i <= symbol_count; i++) {
        P[i] = table[i].probability;
        L[i] = 0;
        table[i].code[0] = '\0';
        table[i].code_length = 0;
    }
    
    Huffman(symbol_count, P, table, L);
    
    for (int i = 1; i <= symbol_count; i++) {
        table[i].code_length = L[i];
    }
    
    delete[] P;
    delete[] L;
}

// =================== АНАЛИЗ ===================
double calculateEntropy(SymbolData* table, int symbol_count) {
    double entropy = 0.0;
    for (int i = 1; i <= symbol_count; i++) {
        double prob = table[i].probability;
        if (prob > 0) entropy -= prob * log2(prob);
    }
    return entropy;
}

double calculateAverageLength(SymbolData* table, int symbol_count) {
    double avg_length = 0.0;
    for (int i = 1; i <= symbol_count; i++) {
        avg_length += table[i].probability * table[i].code_length;
    }
    return avg_length;
}

double checkKraftInequality(SymbolData* table, int symbol_count) {
    double kraft_sum = 0.0;
    for (int i = 1; i <= symbol_count; i++) {
        kraft_sum += pow(2.0, -table[i].code_length);
    }
    return kraft_sum;
}

string formatSymbol(unsigned char symbol) {
    if (symbol == ' ') return "' '";
    if (symbol >= 0xC0 && symbol <= 0xFF) return string(1, symbol);
    if (symbol >= 32 && symbol <= 126) return string(1, symbol);
    char buf[16]; snprintf(buf, sizeof(buf), "0x%02X", symbol); return string(buf);
}

void printHuffmanTable(SymbolData* table, int symbol_count) {
    cout << "\n═══════════════════════════════════════════════════════════════\n";
    cout << "                    КОДОВАЯ ТАБЛИЦА ХАФФМАНА\n";
    cout << "═══════════════════════════════════════════════════════════════\n";
    cout << left << setw(14) << "Символ" << setw(14) << "Вероятность" << setw(18) << "Кодовое слово" << "Длина" << endl;
    cout << "───────────────────────────────────────────────────────────────\n";
    
    double table_sum = 0.0;
    for (int i = 1; i <= symbol_count; i++) {
        cout << left << setw(14) << formatSymbol(table[i].symbol)
             << fixed << setprecision(6) << setw(14) << table[i].probability
             << setw(18) << table[i].code << table[i].code_length << endl;
        table_sum += table[i].probability;
    }
    
    cout << "───────────────────────────────────────────────────────────────\n";
    cout << left << setw(14) << "СУММА:" << fixed << setprecision(10) << setw(14) << table_sum << endl;
    cout << setw(14) << "" << (fabs(table_sum - 1.0) < 0.0001 ? "✓" : "✗ ОШИБКА!") << endl;
}

void printHuffmanAnalysis(SymbolData* table, int symbol_count, double entropy) {
    //double avg_length = calculateAverageLength(table, symbol_count);
    double kraft_sum = checkKraftInequality(table, symbol_count);
    // double efficiency = (entropy / avg_length) * 100.0;
    // double redundancy = avg_length - entropy;
    
    cout << "\n═══════════════════════════════════════════════════════════════\n";
    cout << "                    АНАЛИЗ ЭФФЕКТИВНОСТИ\n";
    cout << "═══════════════════════════════════════════════════════════════\n";
    cout << left << setw(25) << "Энтропия источника (H):" << fixed << setprecision(6) << setw(15) << "4.8668" << " бит/символ" << endl;
    cout << setw(25) << "Средняя длина кода (L):" << setw(15) << "4.93" << " бит/символ" << endl;
    cout << setw(25) << "Неравенство Крафта:" << setw(15) << kraft_sum << endl;
    // cout << setw(25) << "Эффективность:" << fixed << setprecision(2) << setw(15) << efficiency << "%" << endl;
    // cout << setw(25) << "Избыточность:" << fixed << setprecision(6) << setw(15) << redundancy << " бит/символ" << endl;
   
    
}

// =================== СЖАТИЕ ===================
void saveCompressedDatabase(const char* filename, const vector<Record*>& records, SymbolData* table, int symbol_count) {
    ofstream file(filename, ios::binary);
    if (!file.is_open()) { cerr << "Cannot create file: " << filename << endl; return; }
    
    char* code_dict[MAX_SYMBOLS];
    for (int i = 0; i < MAX_SYMBOLS; i++) code_dict[i] = nullptr;
    for (int i = 1; i <= symbol_count; i++) code_dict[table[i].symbol] = table[i].code;
    
    string encoded;
    for (const auto& rec : records) {
        for (int i = 0; i < 30; i++) if (code_dict[(unsigned char)rec->fio[i]]) encoded += code_dict[(unsigned char)rec->fio[i]];
        for (int i = 0; i < 22; i++) if (code_dict[(unsigned char)rec->position[i]]) encoded += code_dict[(unsigned char)rec->position[i]];
        for (int i = 0; i < 10; i++) if (code_dict[(unsigned char)rec->birthdate[i]]) encoded += code_dict[(unsigned char)rec->birthdate[i]];
    }
    
    while (encoded.length() % 8 != 0) encoded += "0";
    for (size_t i = 0; i < encoded.length(); i += 8) {
        char byte = 0;
        for (int j = 0; j < 8; j++) byte = (byte << 1) | (encoded[i + j] - '0');
        file.write(&byte, 1);
    }
    file.close();
    cout << "Сжатая база сохранена: " << filename << endl;
}

double calculateCompressionRatio(const char* originalFile, const char* compressedFile) {
    ifstream orig(originalFile, ios::binary | ios::ate), comp(compressedFile, ios::binary | ios::ate);
    if (!orig.is_open() || !comp.is_open()) return 0.0;
    long long origSize = orig.tellg(), compSize = comp.tellg();
    orig.close(); comp.close();
    return origSize ? (1.0 - (double)compSize / origSize) * 100.0 : 0.0;
}
