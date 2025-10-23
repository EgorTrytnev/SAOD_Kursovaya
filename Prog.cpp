#include "Prog.h"


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
        cerr << "Dont open file N/A " << filename << endl;
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
        cout << "Not found" << endl;
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
        Record* r = q.front(); q.pop();
        cout << idx++ << ". name ";
        printCharArray(r->fio, 30);
        cout << ", department: " << r->department << ", post: ";
        printCharArray(r->position, 22);
        cout << ", Date of birthday: ";
        printCharArray(r->birthdate, 10);
        cout << " (Year " << getYearFromBirthdate(r->birthdate) << ")" << endl;
    }
}
