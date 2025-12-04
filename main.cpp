#include "Prog.h"

extern void init_data(const std::vector<int>& keys, const std::vector<int>& weights);
extern void build_tree();
extern void print_inorder();
extern bool contains_key(int key);
extern void destroy_tree();

void displayMenu() {
    cout << "\n=== MENU ===" << endl;
    cout << "1. View database (pagination)" << endl;
    cout << "2. Sort by birth year (Heap Sort)" << endl;
    cout << "3. Search by year and view queue" << endl;
    cout << "4. Build and search in B-tree" << endl;
    cout << "5. Huffman Coding and Compression" << endl;
    cout << "6. Exit" << endl;
    cout << "Choose option: ";
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

    int choice = 0;
    bool sorted = false;
    
    while (true) {
        displayMenu();
        
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            cout << "Invalid input!" << endl;
            continue;
        }
        
        if (choice == 1) {
            cout << "\n=== DATABASE VIEW ===" << endl;
            int currentPage = 0;
            char navChoice;
            
            while (true) {
                printPage(records, currentPage);
                
                int totalPages = (records.size() + PAGE_SIZE - 1) / PAGE_SIZE;
                cout << "\nPage " << (currentPage + 1) << " of " << totalPages << endl;
                cout << "Commands: (n)ext, (p)revious, (q)uit: ";
                
                cin >> navChoice;
                
                if (navChoice == 'n' || navChoice == 'N') {
                    if ((currentPage + 1) * PAGE_SIZE < (int)records.size()) {
                        currentPage++;
                    } else {
                        cout << "No more pages!" << endl;
                    }
                } else if (navChoice == 'p' || navChoice == 'P') {
                    if (currentPage > 0) {
                        currentPage--;
                    } else {
                        cout << "Already on first page!" << endl;
                    }
                } else if (navChoice == 'q' || navChoice == 'Q') {
                    break;
                }
            }
        }
        
        else if (choice == 2) {
            cout << "\n=== SORTING BY BIRTH YEAR ===" << endl;
            cout << "Sorting in progress..." << endl;
            heapSort(records);
            cout << "Sorting complete!" << endl;
            sorted = true;
            
            int currentPage = 0;
            char navChoice;
            
            while (true) {
                printPage(records, currentPage);
                
                int totalPages = (records.size() + PAGE_SIZE - 1) / PAGE_SIZE;
                cout << "\nPage " << (currentPage + 1) << " of " << totalPages << endl;
                cout << "Commands: (n)ext, (p)revious, (q)uit: ";
                
                cin >> navChoice;
                
                if (navChoice == 'n' || navChoice == 'N') {
                    if ((currentPage + 1) * PAGE_SIZE < (int)records.size()) {
                        currentPage++;
                    } else {
                        cout << "No more pages!" << endl;
                    }
                } else if (navChoice == 'p' || navChoice == 'P') {
                    if (currentPage > 0) {
                        currentPage--;
                    } else {
                        cout << "Already on first page!" << endl;
                    }
                } else if (navChoice == 'q' || navChoice == 'Q') {
                    break;
                }
            }
        }
        
        else if (choice == 3) {
            if (!sorted) {
                cout << "Please sort the database first (option 2)!" << endl;
                continue;
            }
            
            cout << "\n=== SEARCH BY BIRTH YEAR ===" << endl;
            
            while (true) {
                cout << "Enter birth year (or 0 to exit): ";
                int year;
                
                if (!(cin >> year)) {
                    cin.clear();
                    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    continue;
                }
                
                if (year == 0) break;
                
                int left = lowerBoundByYear(records, year);
                int rightExclusive = upperBoundExclusiveByYear(records, year);
                
                if (left == -1 || left >= rightExclusive) {
                    cout << "No records found for year " << year << endl;
                    continue;
                }
                
                std::vector<Record*> found;
                for (int i = left; i < rightExclusive; ++i) {
                    found.push_back(records[i]);
                }
                
                MyQueue<Record*> q;
                for (auto* r : found) {
                    q.push(r);
                }
                
                cout << "\nFound " << q.size() << " records for year " << year << ":" << endl;
                printQueue(q);
                
                std::unordered_map<int, std::vector<Record*>> byDept;
                for (auto* r : found) {
                    byDept[r->department].push_back(r);
                }
                
                std::vector<std::pair<int, int>> kv;
                for (auto& it : byDept) {
                    kv.emplace_back(it.first, (int)it.second.size());
                }
                std::sort(kv.begin(), kv.end());
                
                std::vector<int> keys, weights;
                for (auto& p : kv) {
                    keys.push_back(p.first);
                    weights.push_back(p.second);
                }
                
                init_data(keys, weights);
                build_tree();
                
                std::cout << "\n=== OPTIMAL B-TREE ===" << std::endl;
                print_inorder();
                
                while (true) {
                    std::cout << "\nEnter department number (or 0 to go back): ";
                    int dept;
                    
                    if (!(std::cin >> dept)) {
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        continue;
                    }
                    
                    if (dept == 0) break;
                    
                    if (!contains_key(dept)) {
                        std::cout << "Department not found in tree!" << std::endl;
                        continue;
                    }
                    
                    MyQueue<Record*> qsub;
                    auto it = byDept.find(dept);
                    if (it != byDept.end()) {
                        for (auto* r : it->second) {
                            qsub.push(r);
                        }
                    }
                    
                    std::cout << "\nFound " << qsub.size() << " records for department " << dept << ":" << std::endl;
                    printQueue(qsub);
                }
                
                destroy_tree();
                
                cout << "\nSearch completed. Continue? (y/n): ";
                char cont;
                cin >> cont;
                if (cont != 'y' && cont != 'Y') break;
            }
        }
        
        else if (choice == 4) {
            cout << "\nThis option is integrated with option 3 (Search by year)" << endl;
            cout << "To use B-tree search:" << endl;
            cout << "1. First sort database (option 2)" << endl;
            cout << "2. Then search by year (option 3)" << endl;
            cout << "3. B-tree will be built automatically" << endl;
        }
        
        else if (choice == 5) {
            cout << "\n=== HUFFMAN CODING AND COMPRESSION ===" << endl;
            
            unordered_map<char, string> codes;
            buildHuffmanTree(records, codes);
            
            cout << "\nHuffman Codes:" << endl;
            cout << "Character | Code" << endl;
            for (auto& p : codes) {
                if (p.first == ' ') {
                    cout << "(space)   | " << p.second << endl;
                } else {
                    cout << p.first << "         | " << p.second << endl;
                }
            }
            
            double entropy = calculateEntropy(records);
            double avgCodeLength = calculateAvgCodeLength(records, codes);
            
            cout << "\n=== STATISTICS ===" << endl;
            cout << "Entropy: " << entropy << " bits/symbol" << endl;
            cout << "Average Code Length: " << avgCodeLength << " bits/symbol" << endl;
            cout << "Efficiency: " << (entropy / avgCodeLength * 100.0) << " %" << endl;
            
            cout << "\nSaving compressed database..." << endl;
            saveCompressedDatabase("base_compressed.dat", records, codes);
            
            double ratio = calculateCompressionRatio("base.dat", "base_compressed.dat");
            cout << "Compression Ratio: " << ratio << " %" << endl;
        }
        
        else if (choice == 6) {
            cout << "Exiting program..." << endl;
            break;
        }
        
        else {
            cout << "Invalid choice!" << endl;
        }
    }
    
    destroy_tree();
    
    for (auto r : records) {
        delete r;
    }
    records.clear();
    
    return 0;
}