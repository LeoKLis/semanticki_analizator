#include "tree.h"
#include <iostream>

using namespace std;

int main()
{
    Tree genTree;
    // genTree.print();
    Node* node;
    while ((node = genTree.next()) && genTree.peek()) {
        cout << node->nodeName << " " << genTree.peek()->nodeName << endl;
    }

    // string ss = "\'";
    // cout << ss << endl;
}
