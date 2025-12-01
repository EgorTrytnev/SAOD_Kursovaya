#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iomanip>
using namespace std;
struct Node {
    int data{};
    int h{};
    int w{};
    Node* left{nullptr};
    Node* right{nullptr};
};

void AW_();
void AP_AR_();
void add(Node*& p, int x, int w);
void create_tree(int L, int R);
void seth(Node* p);
void LRprint(Node* x, int level);
void destroy(Node* p);




Node* root = nullptr;

static int N = 0;
vector<int> mas, W;
vector<vector<int>> AW, AP, AR;

void init_data(const vector<int>& keys, const vector<int>& weights) {
    root = nullptr;
    N = (int)keys.size();
    mas = keys;
    W = weights;
    AW.assign(N + 1, vector<int>(N + 1, 0));
    AP.assign(N + 1, vector<int>(N + 1, 0));
    AR.assign(N + 1, vector<int>(N + 1, 0));
}

void build_tree() {
    if (N == 0) return;
    AW_();
    AP_AR_();
    create_tree(0, N);
    if (root) { root->h = 1; seth(root); }
}

bool bst_contains_value(Node* p, int key) {
    if (!p) return false;
    if (key == p->data) return true;
    if (key < p->data) return bst_contains_value(p->left, key);
    return bst_contains_value(p->right, key);
}

bool contains_key(int key) { return bst_contains_value(root, key); }
void print_inorder() { LRprint(root, 0); }
void destroy_tree() { destroy(root); root = nullptr; }


int csum = 0;

void AW_() {
    for (int i = 0; i <= N; ++i) {
        AW[i][i] = 0;
        for (int j = i + 1; j <= N; ++j) {
            AW[i][j] = AW[i][j - 1] + W[j - 1];
        }
    }
}

void AP_AR_() {
    for (int i = 0; i < N; ++i) {
        int j = i + 1;
        AP[i][j] = AW[i][j];
        AR[i][j] = j;
    }
    for (int H = 2; H <= N; ++H) {
        for (int i = 0; i <= N - H; ++i) {
            int j = i + H;
            int m = AR[i][j - 1];
            int best = AP[i][m - 1] + AP[m][j];
            for (int k = m + 1; k <= AR[i + 1][j]; ++k) {
                int val = AP[i][k - 1] + AP[k][j];
                if (val < best) { best = val; m = k; }
            }
            AP[i][j] = best + AW[i][j];
            AR[i][j] = m;
        }
    }
}

void add(Node*& p, int x, int w) {
    if (!p) {
        p = new Node();
        p->data = mas[x - 1];
        p->w = w;
        return;
    }
    if (mas[x - 1] < p->data) add(p->left, x, w);
    else add(p->right, x, w);
}

void create_tree(int L, int R) {
    if (L < R) {
        int k = AR[L][R];
        add(root, k, W[k - 1]);
        create_tree(L, k - 1);
        create_tree(k, R);
    }
}

void seth(Node* p) {
    if (!p) return;
    if (p->left)  { p->left->h  = p->h + 1; }
    if (p->right) { p->right->h = p->h + 1; }
    seth(p->left);
    seth(p->right);
}

void midh(Node* p) {
    if (!p) return;
    csum += p->w * p->h;
    midh(p->left);
    midh(p->right);
}

int size(Node* p) {
    if (!p) return 0;
    return 1 + size(p->left) + size(p->right);
}

long long summ(Node* p) {
    if (!p) return 0;
    return p->data + summ(p->left) + summ(p->right);
}

void LRprint(Node* x, int level) {
    if (!x) return;
    LRprint(x->left, level + 1);
    cout << "Key = " << x->data << ", Weight = " << x->w << '\n';
    
    LRprint(x->right, level + 1);
}

void destroy(Node* p) {
    if (!p) return;
    destroy(p->left);
    destroy(p->right);
    delete p;
}

