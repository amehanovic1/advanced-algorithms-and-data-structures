#include <iostream>
#include <fstream>

enum Color
{
    RED,
    BLACK
};

template <typename T>
struct Node
{
    Node<T> *parent;
    Node<T> *right;
    Node<T> *left;
    T key;
    Color color;
};

template <typename T>
class RBTree
{
private:
    Node<T> *root;
    Node<T> *TNIL;

public:
    RBTree()
    {
        TNIL = new Node<T>;
        root = TNIL;
    };
    ~RBTree()
    {
        delete TNIL;
        root = nullptr;
    }
    void leftRotate(Node<T> *node);
    void rightRotate(Node<T> *node);
    void insertFixup(Node<T> *node);
    void insert(T key);
    void printNode(std::ofstream &out, Node<T> *node);
    void printRBTree();
    void deleteFixup(Node<T> *x);
    void transplant(Node<T> *u, Node<T> *v);
    void rbDelete(T z);

    Node<T> *findNode(T key)
    {
        Node<T> *x = root;
        while (x != TNIL)
        {
            if (x->key == key)
                return x;
            if (x->key <= key)
                x = x->right;
            else
                x = x->left;
        }

        return TNIL;
    }

    void inorder(Node<T> *node);

    void printInorder()
    {
        inorder(root);
    };

    Node<T> *treeMinimum(Node<T> *node)
    {
        while (node->left != TNIL)
            node = node->left;
        return node;
    }
};

template <typename T>
void RBTree<T>::leftRotate(Node<T> *x)
{
    Node<T> *y = x->right;
    // lijevo y podstablo kao desno x podstablo
    x->right = y->left;

    // ako y ima lijevo dijete treba postaviti da njegov roditelj pokazuje na cvor x
    if (y->left != TNIL)
        y->left->parent = x;

    y->parent = x->parent;

    if (x->parent == TNIL)
        root = y;
    else if (x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;

    y->left = x; // x kao lijevo podstablo y
    x->parent = y;
}

template <typename T>
void RBTree<T>::rightRotate(Node<T> *x)
{
    Node<T> *y = x->left;
    // desno y podstablo kao lijevo x podstablo
    x->left = y->right;

    // ako y ima desno dijete, treba da pokazuje na cvor x
    if (y->right != TNIL)
        y->right->parent = x;

    y->parent = x->parent;

    if (x->parent == TNIL)
        root = y;
    else if (x == x->parent->right)
        x->parent->right = y;
    else
        x->parent->left = y;

    y->right = x; // x kao desno podstablo y
    x->parent = y;
}

template <typename T>
void RBTree<T>::insertFixup(Node<T> *z)
{
    Node<T> *y = new Node<T>;

    while (z->parent->color == RED)
    {

        if (z->parent == z->parent->parent->left) // da li je cvor lijevo dijete roditelja
        {
            y = z->parent->parent->right; // ujak cvora z

            // ujak cvora z je crvene boje
            if (y->color == RED)
            {
                z->parent->color = BLACK;       // case 1
                y->color = BLACK;               // case 1
                z->parent->parent->color = RED; // case 1
                z = z->parent->parent;          // case 1
            }
            else
            {
                if (z == z->parent->right) // ako je z desno dijete
                {
                    // rotira se roditelj cvora z
                    z = z->parent; // case 2
                    leftRotate(z); // case 2
                }

                // rotira se djed cvora z i mijenja se boja za roditelja i djeda
                z->parent->color = BLACK;       // case 3
                z->parent->parent->color = RED; // case 3
                rightRotate(z->parent->parent); // case 3
            }
        }
        else
        {
            y = z->parent->parent->left;

            if (y->color == RED)
            {
                z->parent->color = BLACK;       // case 1
                y->color = BLACK;               // case 1
                z->parent->parent->color = RED; // case 1
                z = z->parent->parent;          // case 1
            }
            else
            {
                if (z == z->parent->left)
                {
                    z = z->parent;  // case 2
                    rightRotate(z); // case 2
                }

                z->parent->color = BLACK;       // case 3
                z->parent->parent->color = RED; // case 3
                leftRotate(z->parent->parent);  // case 3
            }
        }
    }
    root->color = BLACK;
}

template <typename T>
void RBTree<T>::insert(T newKey)
{
    Node<T> *y = TNIL;
    Node<T> *x = root;
    Node<T> *z = new Node<T>;

    while (x != TNIL)
    {
        y = x;

        if (newKey < x->key)
            x = x->left;
        else
            x = x->right;
    } // roditelj kojem se dodaje dijete

    z->parent = y;

    // provjeriti da li je cvor korijen, i da li je manji ili veći od roditelja
    if (y == TNIL)
        root = z;
    else if (newKey < y->key)
        y->left = z;
    else
        y->right = z;

    z->key = newKey;
    z->left = TNIL;
    z->right = TNIL;
    z->color = RED;

    insertFixup(z);
}

template <typename T>
void RBTree<T>::inorder(Node<T> *node)
{
    if (node != TNIL)
    {
        inorder(node->left);
        std::cout << node->key << " ";
        inorder(node->right);
    }
}

template <typename T>
void RBTree<T>::transplant(Node<T> *u, Node<T> *v)
{
    if (u->parent == TNIL)
        root = v;
    else if (u == u->parent->left)
        u->parent->left = v;
    else
        u->parent->right = v;
    v->parent = u->parent;
}

template <typename T>
void RBTree<T>::deleteFixup(Node<T> *x)
{
    Node<T> *w = new Node<T>;
    while (x != root && x->color == BLACK)
    {
        if (x == x->parent->left)
        {
            w = x->parent->right;
            if (w->color == RED) // w je crven
            {
                w->color = BLACK;       // case 1
                x->parent->color = RED; // case 1
                leftRotate(x->parent);  // case 1
                w = x->parent->right;   // case 1
            }
            if (w->left->color == BLACK && w->right->color == BLACK) // w je crn i njegova djeca
            {
                w->color = RED; // case 2
                x = x->parent;  // case 2
            }
            else
            {
                if (w->right->color == BLACK) // w je crn, lijevo dijete je crveno a desno crno
                {
                    w->left->color = BLACK; // case 3
                    w->color = RED;         // case 3
                    rightRotate(w);         // case 3
                    w = x->parent->right;   // case 3
                }
                // w je crn i njegovo desno dijete je crveno
                w->color = x->parent->color; // case 4
                x->parent->color = BLACK;    // case 4
                w->right->color = BLACK;     // case 4
                leftRotate(x->parent);       // case 4
                x = root;
            }
        }
        else
        {
            w = x->parent->left;
            if (w->color == RED)
            {
                w->color = BLACK;       // case 1
                x->parent->color = RED; // case 1
                rightRotate(x->parent); // case 1
                w = x->parent->left;    // case 1
            }
            if (w->right->color == BLACK && w->left->color == BLACK)
            {
                w->color = RED; // case 2
                x = x->parent;  // case 2
            }
            else
            {
                if (w->left->color == BLACK)
                {
                    w->right->color = BLACK; // case 3
                    w->color = RED;          // case 3
                    leftRotate(w);           // case 3
                    w = x->parent->left;     // case 3
                }

                w->color = x->parent->color; // case 4
                x->parent->color = BLACK;    // case 4
                w->left->color = BLACK;      // case 4
                rightRotate(x->parent);      // case 4
                x = root;                    // case 4
            }
        }
    }

    x->color = BLACK;
}

template <typename T>
void RBTree<T>::rbDelete(T key)
{
    Node<T> *z = findNode(key);
    Node<T> *y = z;
    Node<T> *x = new Node<T>;

    Color y_original_color = y->color;

    if (z->left == TNIL)
    {
        x = z->right;            // case 1
        transplant(z, z->right); // case 1
    }
    else if (z->right == TNIL)
    {
        x = z->left;            // case 2
        transplant(z, z->left); // case 2
    }
    else
    {
        // case 3
        //  trazi se minimum u desnom podstablu
        y = treeMinimum(z->right);
        y_original_color = y->color;
        // x mijenja cvor y
        x = y->right;
        if (y->parent == z)
        {
            x->parent = y;
        }
        else
        {
            transplant(y, y->right);
            y->right = z->right;
            y->right->parent = y;
        }
        transplant(z, y);
        y->left = z->left;
        y->left->parent = y;
        y->color = z->color;
    }

    if (y_original_color == BLACK)
    {
        deleteFixup(x);
    }
}

template <typename T>
void RBTree<T>::printNode(std::ofstream &out, Node<T> *node)
{
    if (node != TNIL)
    {
        out << "  " << node->key << " [color=" << (node->color == RED ? "red" : "black") << "];\n";

        if (node->left != TNIL)
        {
            out << "  " << node->key << " -> " << node->left->key << ";\n";
            printNode(out, node->left);
        }
        else
        {
            out << "  TNIL_LEFT" << node->key << " [color=black, shape=circle, label=\"\"];\n";
            out << "  " << node->key << " -> TNIL_LEFT" << node->key << " [color=black];\n";
        }

        if (node->right != TNIL)
        {
            out << "  " << node->key << " -> " << node->right->key << ";\n";
            printNode(out, node->right);
        }
        else
        {
            out << "  TNIL_RIGHT" << node->key << " [color=black, shape=circle, label=\"\"];\n";
            out << "  " << node->key << " -> TNIL_RIGHT" << node->key << " [color=black];\n";
        }
    }
}

template <typename T>
void RBTree<T>::printRBTree()
{
    std::ofstream out("rbtree.dot");
    out << "digraph RBTree {\n";
    printNode(out, root);
    out << "}\n";
    out.close();
    system("\"C:\\Program Files\\Graphviz\\bin\\dot\" -Tpng rbtree.dot -o rbtree.png");
    system("start rbtree.png");
}

int main()
{
    RBTree<int> rbTree;
    rbTree.insert(6);
    rbTree.insert(11);
    rbTree.insert(10);
    rbTree.insert(2);
    rbTree.insert(9);
    rbTree.insert(7);
    rbTree.insert(5);
    rbTree.insert(13);
    rbTree.insert(22);
    rbTree.insert(27);
    rbTree.insert(36);
    rbTree.insert(12);
    rbTree.insert(31);

    // rbTree.printRBTree();

    while (1)
    {
        std::cout << std::endl
                  << "--------------------------------------------" << std::endl;
        std::cout << "Odaberite opciju:" << std::endl;
        std::cout << "(1) Umetanje novog čvora:" << std::endl;
        std::cout << "(2) Inorder ispis čvorova na ekranu:" << std::endl;
        std::cout << "(3) Brisanje čvora:" << std::endl;
        std::cout << "(4) Izlaz:" << std::endl;

        int input;
        std::cin >> input;
        switch (input)
        {
        case 1:
            std::cout << "Unesite čvor: ";
            int key;
            std::cin >> key;
            rbTree.insert(key);
            break;

        case 2:
            rbTree.printInorder();
            break;
        case 3:
            std::cout << "Unesite čvor: ";
            std::cin >> key;
            rbTree.rbDelete(key);
            break;
        case 4:
            break;
        }
    };

    return 0;
}