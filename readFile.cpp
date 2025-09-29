#include <iostream>
#include <fstream>
#include <cstdint>
#include <cstring>
#include <vector>
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
    int n = arr.size();
    for (int i = n / 2 - 1; i >= 0; i--) {
        heapify(arr, n, i);
    }
    for (int i = n-1; i >= 0; i--) {
        swap(arr[0], arr[i]);
        heapify(arr, i, 0);
    }
}

bool loadDatabase(const char* filename, vector<Record*>& records) {
    ifstream file(filename, ios::binary);
    if (!file.is_open()) {
        cerr << "Не удалось открыть файл " << filename << endl;
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
    int length = size;
    while (length > 0 && arr[length - 1] == ' ') --length;
    for (int i = 0; i < length; ++i) {
        cout << arr[i];
    }
}

void printPage(const vector<Record*>& records, int page) {
    int start = page * PAGE_SIZE;
    int end = start + PAGE_SIZE;
    if (start >= (int)records.size()) {
        cout << "Нет записей для вывода на этой странице" << endl;
        return;
    }
    if (end > (int)records.size()) end = (int)records.size();
    cout << "Page " << (page + 1) << ":" << endl;
    for (int i = start; i < end; ++i) {
        cout << i + 1 << ". name ";
        printCharArray(records[i]->fio, 30);
        cout << ", department: " << records[i]->department << ", post: ";
        printCharArray(records[i]->position, 22);
        cout << ", Date of birthday: ";
        printCharArray(records[i]->birthdate, 10);
        cout << " (Year " << getYearFromBirthdate(records[i]->birthdate) << ")" << endl;
    }
}

int main() {
    #if defined(_WIN32) || defined(_WIN64)
    SetConsoleOutputCP(866);
    SetConsoleCP(866);
    #endif
    vector<Record*> records;
    if (!loadDatabase("base.dat", records)) {
        return 1;
    }

    int currentPage = 0;
    char choice;

    do {
        printPage(records, currentPage);
        cout << "Show next page? (y/n): ";
        cin >> choice;
        if (choice == 'y' || choice == 'Y') {
            currentPage++;
        } else {
            break;
        }
    } while ((currentPage * PAGE_SIZE) < (int)records.size());

    cout << "Sort by birthday year..." << endl;
    heapSort(records);

    currentPage = 0;
    do {
        printPage(records, currentPage);
        cout << "Show next page? (y/n): ";
        cin >> choice;
        if (choice == 'y' || choice == 'Y') {
            currentPage++;
        } else {
            break;
        }
    } while ((currentPage * PAGE_SIZE) < (int)records.size());

    // Освобождение памяти
    for (auto r : records) {
        delete r;
    }
    records.clear();

    return 0;
}
