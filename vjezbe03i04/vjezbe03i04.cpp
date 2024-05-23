#include <iostream>
#include <cmath>
#include <vector>

template <typename T>
struct Node
{
    Node<T> *parent;
    Node<T> *child;
    Node<T> *left;
    Node<T> *right;
    int degree;
    bool marked;
    T key;

    Node(T value)
    {
        key = value;
        degree = 0;
        marked = false;

        parent = nullptr;
        child = nullptr;
        left = nullptr;
        child = nullptr;
    }
};

template <typename T>
class FibHeap
{
    Node<T> *min;
    int n;

public:
    FibHeap()
    {
        n = 0;
        min = nullptr;
    }
    Node<T> *minimum()
    {
        return min;
    }

    int numberOfNodes()
    {
        return n;
    }
    void fibHeapInsert(T key);
    FibHeap fibHeapUnion(FibHeap H1, FibHeap H2);
    void fibHeapLink(Node<T> *x, Node<T> *y);
    void consolidate();
    Node<T> *fibHeapExtractMin();
    void cut(Node<T> *x, Node<T> *y);
    void cascadingCut(Node<T> *y);
    Node<T> *findNode(Node<T> *x, T k);
    void fibHeapDecreaseKey(T x, T k);
    void fibHeapDelete(T x);
    void printRootNode();
    void printChildren(Node<T> *node);
};

template <typename T>
void FibHeap<T>::fibHeapInsert(T key)
{
    Node<T> *x = new Node<T>(key);

    if (min == nullptr)
    {
        // kreiranje liste korjena koja sadrzi samo x
        x->left = x;
        x->right = x;
        min = x;
    }
    else
    {
        // dodavanje x u listu korjena
        min->left->right = x;
        x->right = min;
        x->left = min->left;
        min->left = x;
        if (x->key < min->key)
        {
            min = x;
        }
    }

    n = n + 1;
}

template <typename T>
FibHeap<T> FibHeap<T>::fibHeapUnion(FibHeap H1, FibHeap H2)
{
    FibHeap H = FibHeap();
    H.min = H1.min;

    // spajanje liste korjena gomile H2 sa listom korjena gomile H
    if (H.min != nullptr && H2.min != nullptr)
    {
        (H.min)->left->right = H2.min;
        (H2.min)->left->right = H.min;
        Node<T> *node = (H.min)->left;
        (H.min)->left = (H2.min)->left;
        (H2.min)->left = node;
    }

    if ((H1.min == nullptr) || (H2.min != nullptr && H2.min->key < H1.min->key))
    {
        H.min = H2.min;
    }

    H.n = H1.n + H2.n;
    return H;
}

template <typename T>
void FibHeap<T>::fibHeapLink(Node<T> *y, Node<T> *x)
{
    // ukloniti y iz liste korjena
    // y postaje dijete cvora x
    // povecava se s.degree
    // y.mark = false

    y->left->right = y->right;
    y->right->left = y->left;

    if (x->right == x)
    {
        min = x;
    }

    y->left = y;
    y->right = y;
    y->parent = x;

    if (x->child == nullptr)
    {
        x->child = y;
    }

    y->right = x->child;
    y->left = x->child->left;
    x->child->left->right = y;
    x->child->left = y;

    if (y->key < x->child->key)
    {
        x->child = y;
    }

    x->degree = x->degree + 1;
    y->marked = false;
}

template <typename T>
void FibHeap<T>::consolidate()
{
    int D = log2(n) + 1;
    std::vector<Node<T> *> A(n, nullptr);
    Node<T> *x = min;

    do
    {
        int d = x->degree;
        while (A[d] != nullptr)
        {
            // postoji drugi cvor y sa istim degree kao x
            Node<T> *y = A[d];
            if (x->key > y->key)
            {
                Node<T> *temp = x;
                x = y;
                y = temp;
            }
            if (y == min)
            {
                min = x;
            }

            fibHeapLink(y, x);

            if (x->right == x)
            {
                min = x;
            }
            A[d] = nullptr;
            d = d + 1;
        }

        // x ima jedinstven degree
        A[d] = x;
        x = x->right;
    } while (x != min);

    min = nullptr;
    for (int i = 0; i < D; i++)
    {
        if (A[i] != nullptr)
        {
            A[i]->left = A[i];
            A[i]->right = A[i];
            if (min != nullptr)
            {
                min->left->right = A[i];
                A[i]->right = min;
                A[i]->left = min->left;
                min->left = A[i];
                if (A[i]->key < min->key)
                {
                    min = A[i];
                }
            }
            else
            {
                min = A[i];
            }

            if (min == nullptr)
            {
                min = A[i];
            }
            else if (A[i]->key < min->key)
            {
                min = A[i];
            }
        }
    }
}

template <typename T>
Node<T> *FibHeap<T>::fibHeapExtractMin()
{
    Node<T> *z = min;
    // provjera da li je gomila prazna
    if (z != nullptr)
    {
        Node<T> *x = z->child;
        // ukoliko cvor ima djece ubacuju se u listu korjena prije izbacivanja min
        if (x != nullptr)
        {
            Node<T> *node;
            Node<T> *ptr = x;
            do
            {
                node = x->right;
                min->left->right = x;
                x->right = min;
                x->left = min->left;
                min->left = x;

                if (x->key < min->key)
                {
                    min = x;
                }

                x->parent = nullptr;
                x = node;
            } while (node != ptr);
        }

        // izbacivanje min iz liste korjena
        min->left->right = min->right;
        min->right->left = min->left;

        if (z == z->right && z->child == nullptr)
        {
            // z jedini cvor gomile
            min = nullptr;
        }
        else
        {
            min = z->right;
            // smanjuje broj stabala i postavlja pravi min
            consolidate();
        }
        n = n - 1;
    }

    return z;
}

template <typename T>
void FibHeap<T>::cut(Node<T> *x, Node<T> *y)
{
    // ukloniti x iz liste djece čvora y
    if (x == x->right)
    {
        y->child = nullptr;
    }
    x->left->right = x->right;
    x->right->left = x->left;
    x->parent = nullptr;

    if (x == y->child)
    {
        y->child = x->right;
    }

    // dekrementirati y.degree
    y->degree = y->degree - 1;
    x->right = x;
    x->left = x;

    // dodati x u listu korijena od H
    min->left->right = x;
    x->left = min->left;
    x->right = min;
    min->left = x;

    x->parent = nullptr;
    x->marked = false;
}

template <typename T>
void FibHeap<T>::cascadingCut(Node<T> *y)
{
    Node<T> *z = y->parent;
    if (z != nullptr)
    {
        if (y->marked == false)
        {
            y->marked = true;
        }
        else
        {
            cut(y, z);
            cascadingCut(z);
        }
    }
}

template <typename T>
Node<T> *FibHeap<T>::findNode(Node<T> *y, T k)
{
    Node<T> *x = y;
    x->marked = true;
    Node<T> *ptr = nullptr;

    if (x->key == k)
    {
        ptr = x;
        x->marked = false;
        return ptr;
    }

    if (x->child != nullptr)
    {
        auto z = findNode(x->child, k);
        if (z)
            ptr = z;
    }
    if (x->right->marked != true)
    {
        auto z = findNode(x->right, k);
        if (z)
            ptr = z;
    }

    x->marked = false;
    return ptr;
}

template <typename T>
void FibHeap<T>::fibHeapDecreaseKey(T currentKey, T newKey)
{
    if (min == nullptr)
    {
        std::cout << "Gomila je prazna" << std::endl;
        return;
    }

    Node<T> *x = findNode(min, currentKey);
    if (x == nullptr)
    {
        std::cout << "Ne postoji čvor sa unesenim ključem" << std::endl;
        return;
    }

    if (newKey > currentKey)
    {
        std::cout << "Novi ključ je veći od trenutnog ključa" << std::endl;
        return;
    }

    x->key = newKey;
    Node<T> *y = x->parent;

    // dijete ima manji kljuc od roditelja
    if (y != nullptr && x->key < y->key)
    {
        cut(x, y);
        cascadingCut(y);
    }
    // provjera da li je novi kljuc min
    if (x->key < min->key)
    {
        min = x;
    }
}

template <typename T>
void FibHeap<T>::fibHeapDelete(T x)
{
    fibHeapDecreaseKey(x, -100000);
    Node<T> *node = fibHeapExtractMin();

    if (node == nullptr)
    {
        std::cout << "Čvor sa ključem " << node->key << " nije izbrisan" << std::endl;
    }
}

template <typename T>
void FibHeap<T>::printRootNode()
{
    Node<T> *node = min;
    do
    {
        std::cout << "{" << node->key << "}";
        printChildren(node->child);
        node = node->right;
        if (node != min)
        {
            std::cout << "\n";
        }
    } while (node != min && node->right != nullptr);
    std::cout << std::endl
              << std::endl;
}

template <typename T>
void FibHeap<T>::printChildren(Node<T> *node)
{
    if (node == nullptr)
        return;

    Node<T> *temp = node;
    temp->marked = true;

    if (temp->child != nullptr)
    {
        std::cout << "|" << temp->key << "|";
        printChildren(temp->child);
    }
    else
    {
        std::cout << " - " << temp->key;
    }
    if (temp->right->marked != true)
    {
        std::cout << " - ";
        printChildren(temp->right);
    }

    temp->marked = false;
}

void insertTest()
{
    std::cout << "TEST 1" << std::endl;
    std::cout << "------------------------------------\n";

    FibHeap<int> fibHeap = FibHeap<int>();
    fibHeap.fibHeapInsert(48);
    fibHeap.fibHeapInsert(33);
    fibHeap.fibHeapInsert(5);

    fibHeap.printRootNode();

    std::cout << "Min čvor: " << fibHeap.minimum()->key << std::endl;
    std::cout << "Broj čvorova: " << fibHeap.numberOfNodes() << std::endl;
    std::cout << "------------------------------------\n";
}

void unionTest()
{
    std::cout << "TEST 2" << std::endl;
    std::cout << "------------------------------------\n";

    FibHeap<int> fibHeap_1 = FibHeap<int>();
    fibHeap_1.fibHeapInsert(48);
    fibHeap_1.fibHeapInsert(33);
    fibHeap_1.fibHeapInsert(5);
    std::cout << "Prva gomila: " << std::endl;
    fibHeap_1.printRootNode();

    FibHeap<int> fibHeap_2 = FibHeap<int>();
    fibHeap_2.fibHeapInsert(16);
    fibHeap_2.fibHeapInsert(1);
    std::cout << "Druga gomila: " << std::endl;
    fibHeap_2.printRootNode();

    FibHeap<int> fibHeap = fibHeap_1.fibHeapUnion(fibHeap_1, fibHeap_2);
    std::cout << "Spojene gomile: " << std::endl;
    fibHeap.printRootNode();

    std::cout << "Min čvor: " << fibHeap.minimum()->key << std::endl;
    std::cout << "Broj čvorova: " << fibHeap.numberOfNodes() << std::endl;
    std::cout << "------------------------------------\n";
}

void extractMinTest()
{
    std::cout << "TEST 3" << std::endl;
    std::cout << "------------------------------------\n";

    FibHeap<int> fibHeap = FibHeap<int>();
    fibHeap.fibHeapInsert(48);
    fibHeap.fibHeapInsert(33);
    fibHeap.fibHeapInsert(5);

    fibHeap.printRootNode();
    std::cout << "Min čvor: " << fibHeap.minimum()->key << std::endl;
    std::cout << "Broj čvorova: " << fibHeap.numberOfNodes() << std::endl
              << std::endl;

    Node<int> *minNode = fibHeap.fibHeapExtractMin();

    std::cout << "Nakon izbacivanja čvora sa najmanjim ključem" << std::endl;
    fibHeap.printRootNode();
    std::cout << "Min čvor: " << fibHeap.minimum()->key << std::endl;
    std::cout << "Broj čvorova: " << fibHeap.numberOfNodes() << std::endl;
    std::cout << "------------------------------------\n";
}

void decreaseKeyTest()
{
    std::cout << "TEST 4" << std::endl;
    std::cout << "------------------------------------\n";

    FibHeap<int> fibHeap = FibHeap<int>();
    fibHeap.fibHeapInsert(48);
    fibHeap.fibHeapInsert(33);
    fibHeap.fibHeapInsert(5);
    fibHeap.printRootNode();

    fibHeap.fibHeapDecreaseKey(33, 16);

    std::cout << "Nakon smanjivanja ključa" << std::endl;
    fibHeap.printRootNode();
    std::cout << "Min čvor: " << fibHeap.minimum()->key << std::endl;
    std::cout << "Broj čvorova: " << fibHeap.numberOfNodes() << std::endl;
    std::cout << "------------------------------------\n";
}

void deleteTest()
{
    std::cout << "TEST 5" << std::endl;
    std::cout << "------------------------------------\n";

    FibHeap<int> fibHeap = FibHeap<int>();
    fibHeap.fibHeapInsert(4);
    fibHeap.fibHeapInsert(6);
    fibHeap.fibHeapInsert(8);
    fibHeap.fibHeapInsert(12);
    fibHeap.fibHeapInsert(13);

    fibHeap.printRootNode();

    Node<int> *minNode = fibHeap.fibHeapExtractMin();
    std::cout << "Izvlacenje minimalnog cvora: " << minNode->key << std::endl;
    fibHeap.printRootNode();

    fibHeap.fibHeapDelete(12);

    std::cout << "Nakon brisanja čvora " << std::endl;
    fibHeap.printRootNode();
    std::cout << "Min čvor: " << fibHeap.minimum()->key << std::endl;
    std::cout << "Broj čvorova: " << fibHeap.numberOfNodes() << std::endl;
    std::cout << "------------------------------------\n";
}

int main()
{
    insertTest();
    unionTest();
    extractMinTest();
    decreaseKeyTest();
    deleteTest();

    return 0;
}