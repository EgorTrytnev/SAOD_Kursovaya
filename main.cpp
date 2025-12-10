#include "Prog.h"

extern void init_data(const std::vector<int>& keys, const std::vector<int>& weights);

extern void build_tree();

extern void print_inorder();

extern bool contains_key(int key);

extern void destroy_tree();

void displayMenu() {
    cout << "\n" << string(50, '=') << "\n";
    cout << " ГЛАВНОЕ МЕНЮ\n";
    cout << string(50, '=') << "\n";
    cout << "1. Просмотр базы данных (постраничный просмотр)\n";
    cout << "2. Сортировка по году рождения (Heap Sort)\n";
    cout << "3. Поиск по году и просмотр очереди\n";
    cout << "4. Построение и поиск в B-дереве\n";
    cout << "5. Кодирование и сжатие Хаффмана\n";
    cout << "6. Выход\n";
    cout << string(50, '=') << "\n";
    cout << "Выберите опцию: ";
}

int main() {
#if defined(_WIN32) || defined(_WIN64)
    SetConsoleOutputCP(866);
    SetConsoleCP(866);
#endif

    vector<Record*> records;
    cout << "Загрузка базы данных..." << endl;
    if (!loadDatabase("base.dat", records)) {
        cout << "Ошибка: не удалось загрузить базу данных!" << endl;
        return 1;
    }

    cout << "Загружено записей: " << records.size() << endl;
    int choice = 0;
    bool sorted = false;
    while (true) {
        displayMenu();
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            cout << "Некорректный ввод!" << endl;
            continue;
        }

        if (choice == 1) {
            cout << "\n" << string(50, '=') << "\nПРОСМОТР БАЗЫ ДАННЫХ\n" << string(50, '=') << "\n";
            int currentPage = 0;
            char navChoice;
            while (true) {
                printPage(records, currentPage);
                int totalPages = (records.size() + PAGE_SIZE - 1) / PAGE_SIZE;
                cout << "\nСтраница " << (currentPage + 1) << " из " << totalPages << endl;
                cout << "Команды: (n)ext, (p)revious, (q)uit: ";
                cin >> navChoice;
                if (navChoice == 'n' || navChoice == 'N') {
                    if ((currentPage + 1) * PAGE_SIZE < (int)records.size()) {
                        currentPage++;
                    } else {
                        cout << "Нет больше страниц!" << endl;
                    }
                } else if (navChoice == 'p' || navChoice == 'P') {
                    if (currentPage > 0) {
                        currentPage--;
                    } else {
                        cout << "Вы уже на первой странице!" << endl;
                    }
                } else if (navChoice == 'q' || navChoice == 'Q') {
                    break;
                }
            }
        } else if (choice == 2) {
            cout << "\n" << string(50, '=') << "\nСОРТИРОВКА ПО ГОДУ РОЖДЕНИЯ\n" << string(50, '=') << "\n";
            cout << "Сортировка в процессе..." << endl;
            heapSort(records);
            cout << "Сортировка завершена!\n" << endl;
            sorted = true;
            int currentPage = 0;
            char navChoice;
            while (true) {
                printPage(records, currentPage);
                int totalPages = (records.size() + PAGE_SIZE - 1) / PAGE_SIZE;
                cout << "\nСтраница " << (currentPage + 1) << " из " << totalPages << endl;
                cout << "Команды: (n)ext, (p)revious, (q)uit: ";
                cin >> navChoice;
                if (navChoice == 'n' || navChoice == 'N') {
                    if ((currentPage + 1) * PAGE_SIZE < (int)records.size()) {
                        currentPage++;
                    } else {
                        cout << "Нет больше страниц!" << endl;
                    }
                } else if (navChoice == 'p' || navChoice == 'P') {
                    if (currentPage > 0) {
                        currentPage--;
                    } else {
                        cout << "Вы уже на первой странице!" << endl;
                    }
                } else if (navChoice == 'q' || navChoice == 'Q') {
                    break;
                }
            }
        } else if (choice == 3) {
            if (!sorted) {
                cout << "\nОшибка: сначала отсортируйте базу данных (опция 2)!" << endl;
                continue;
            }

            cout << "\n" << string(50, '=') << "\nПОИСК ПО ГОДУ РОЖДЕНИЯ\n" << string(50, '=') << "\n";
            while (true) {
                cout << "Введите год рождения (или 0 для выхода): ";
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
                    cout << "Записей не найдено за год " << year << endl;
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

                cout << "\nНайдено " << q.size() << " записей за год " << year << ":" << endl;
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
                std::cout << "\n" << string(50, '=') << "\nОПТИМАЛЬНОЕ B-ДЕРЕВО\n" << string(50, '=') << "\n";
                print_inorder();

                while (true) {
                    std::cout << "\nВведите номер отдела (или 0 для выхода): ";
                    int dept;
                    if (!(std::cin >> dept)) {
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        continue;
                    }

                    if (dept == 0) break;

                    if (!contains_key(dept)) {
                        std::cout << "Отдел не найден в дереве!" << std::endl;
                        continue;
                    }

                    MyQueue<Record*> qsub;
                    auto it = byDept.find(dept);
                    if (it != byDept.end()) {
                        for (auto* r : it->second) {
                            qsub.push(r);
                        }
                    }
                    std::cout << "\nНайдено " << qsub.size() << " записей для отдела " << dept << ":" << std::endl;
                    printQueue(qsub);
                }

                destroy_tree();
                cout << "\nПоиск завершен. Продолжить? (y/n): ";
                char cont;
                cin >> cont;
                if (cont != 'y' && cont != 'Y') break;
            }
        } else if (choice == 4) {
            cout << "\nЭта опция интегрирована в опцию 3 (Поиск по году)" << endl;
            cout << "Для использования B-дерева:" << endl;
            cout << "1. Сначала отсортируйте базу данных (опция 2)" << endl;
            cout << "2. Затем выполните поиск по году (опция 3)" << endl;
            cout << "3. B-дерево будет построено автоматически" << endl;
        } else if (choice == 5) {
            cout << "\n" << string(90, '=') << "\n";
            cout << setw(50) << "КОДИРОВАНИЕ И СЖАТИЕ ХАФФМАНА\n";
            cout << string(90, '=') << "\n";
            unordered_map<char, string> codes;
            cout << "Построение дерева Хаффмена..." << endl;
            buildHuffmanTree(records, codes);
            cout << "Готово!\n" << endl;

            // Получаем статистику
            unordered_map<char, long long> freq;
            long long totalChars = 0;
            for (const auto& rec : records) {
                for (int i = 0; i < 30; ++i) if (rec->fio[i] != '\0') { freq[rec->fio[i]]++; totalChars++; }
                for (int i = 0; i < 22; ++i) if (rec->position[i] != '\0') { freq[rec->position[i]]++; totalChars++; }
                for (int i = 0; i < 10; ++i) if (rec->birthdate[i] != '\0') { freq[rec->birthdate[i]]++; totalChars++; }
            }

            // Выводим таблицу кодов
            printHuffmanTable(codes, freq, totalChars);

            // Выводим анализ
            printHuffmanAnalysis(records, codes);

            // Сжимаем базу данных
            cout << "Сохранение сжатой базы данных..." << endl;
            saveCompressedDatabase("base_compressed.dat", records, codes);
            double ratio = calculateCompressionRatio("base.dat", "base_compressed.dat");

            cout << "\n" << string(90, '=') << "\n";
            cout << "Коэффициент сжатия: " << fixed << setprecision(2) << ratio << " %\n";
            cout << string(90, '=') << "\n\n";
        } else if (choice == 6) {
            cout << "\nВыход из программы..." << endl;
            break;
        } else {
            cout << "\nНекорректный выбор!" << endl;
        }
    }

    destroy_tree();
    for (auto r : records) {
        delete r;
    }
    records.clear();
    cout << "До свидания!" << endl;
    return 0;
}