#include "Prog.h"

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

    while (true) {
        cout << "Text Year birth: ";
        int year;
        if (!(cin >> year)) break;
        if (year == 0) break;

        int left = lowerBoundByYear(records, year);
        int rightExclusive = upperBoundExclusiveByYear(records, year);

        if (left == -1 || left >= rightExclusive) {
            cout << "Not found!" << endl;
            continue;
        }

        MyQueue<Record*> q;
        for (int i = left; i < rightExclusive; ++i) {
            q.push(records[i]);
        }

        cout << "Found " << q.size() << year << " Queue print:" << endl;
        printQueue(q);
    }

    // Освобождение памяти
    for (auto r : records) {
        delete r;
    }
    records.clear();

    return 0;
}
