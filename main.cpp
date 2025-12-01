#include "Prog.h"
extern void init_data(const std::vector<int>& keys, const std::vector<int>& weights);
extern void build_tree();
extern void print_inorder();
extern bool contains_key(int key);
extern void destroy_tree();

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

    std::vector<Record*> found;
    for (int i = left; i < rightExclusive; ++i)
        found.push_back(records[i]);

    MyQueue<Record*> q;
    for (auto* r : found) q.push(r);

    cout << "Found " << q.size() << " " << year << " Queue print:" << endl;
    printQueue(q);

    std::unordered_map<int, std::vector<Record*>> byDeptSub;
    for (auto* r : found)
        byDeptSub[r->department].push_back(r);

    std::vector<std::pair<int,int>> kv;
    for (auto& it : byDeptSub)
        kv.emplace_back(it.first, (int)it.second.size());
    std::sort(kv.begin(), kv.end());

    std::vector<int> keys, weights;
    for (auto& p : kv) {
        keys.push_back(p.first);
        weights.push_back(p.second);
    }

    init_data(keys, weights);
    build_tree();

    std::cout << "Б-Дерево: \n";
    print_inorder();

    while (true) {
        std::cout << "Введите номер департамента: ";
        int dept;
        if (!(std::cin >> dept)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }
        if (dept == 0) break;

        if (!contains_key(dept)) {
            std::cout << "Департамент не найден\n";
            continue;
        }

        MyQueue<Record*> qsub;
        auto it = byDeptSub.find(dept);
        if (it != byDeptSub.end()) {
            for (auto* r : it->second)
                qsub.push(r);
        }
        std::cout << "Найдено " << qsub.size() << " записей, печать очереди:\n";
        printQueue(qsub);
    }

    destroy_tree();
}





    for (auto r : records) {
        delete r;
    }
    records.clear();
    destroy_tree();

    return 0;
}
