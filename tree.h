#ifndef TREE_H
#define TREE_H

#include "tipovi.h"
#include <iostream>
#include <stack>
#include <vector>

using namespace std;

struct Node {
    string nodeName;
    int line;
    string value;
    Tip tip;
    Tip ntip; // Ili NTip (nez jos)
    int brElem;
    bool lIzraz;
    vector<Tip> tipovi;
    vector<string> imena;

    vector<Node*> childrenNodes;
    Node(string value)
    {
        int spaceIndex = value.find_first_of(' ');
        // Nije naso niti jedan razmak znaci da je cvor, a ne list
        if (spaceIndex == -1) {
            this->nodeName = value;
            line = -1; // Oznacava cvor
            return;
        }

        this->nodeName = value.substr(0, spaceIndex);
        value = value.substr(spaceIndex + 1);

        spaceIndex = value.find_first_of(' ');

        string lineString = value.substr(0, spaceIndex);
        line = atoi(lineString.c_str());
        value = value.substr(spaceIndex + 1);

        value = value;
    }
};

class Tree {
private:
    Node* root;
    Node* currentNode;
    stack<Node*> dfsStack; // int je za indeks cvora
    // stack<pair<Node*, int>> dfsStack;
    void recPrint(Node* parentNode, int depth);

public:
    Tree();

    // Vraca korijen/prvi cvor
    Node* getRoot();

    // Vraca sljedeci cvor po redu
    Node* next();

    // Vraca trenutni cvor
    Node* get();

    // Vraca sljedeci cvor, ali ne prelazi na njega
    Node* peek();

    void print();
};

#endif