#include "tree.h"

Tree::Tree()
{
    vector<Node*> level;
    string line;
    while (getline(cin, line)) {
        int depth = line.find_first_not_of(" ");
        string nodeName = line.substr(depth);
        Node* currentNode = new Node(nodeName);
        if (level.size() <= depth) {
            level.push_back(currentNode);
            if (depth == 0) {
                root = currentNode;
                continue;
            }
        }
        else {
            level[depth] = currentNode;
        }
        Node* parentNode = level[depth - 1];
        parentNode->childrenNodes.push_back(currentNode);
    }
    dfsStack.push(root);
}

Node* Tree::getRoot()
{
    return root;
}

Node* Tree::next()
{
    if (dfsStack.empty()) return nullptr;
    Node* top = dfsStack.top();
    dfsStack.pop();
    for (int i = top->childrenNodes.size() - 1; i >= 0; i--) {
        dfsStack.push(top->childrenNodes[i]);
    }
    currentNode = top;
    return top;
    // if (dfsStack.empty()) {
    //     dfsStack.push({ root, 0 });
    //     return root;
    // }
    // pair<Node*, int> top = dfsStack.top();
    // dfsStack.pop();
    // if (top.second >= top.first->childrenNodes.size()) {
    //     if(dfsStack.empty()) return nullptr;
    //     top = dfsStack.top();
    //     return top.first;
    // }
    // Node* next = top.first->childrenNodes[top.second];
    // top.second += 1;
    // dfsStack.push(top);
    // dfsStack.push({ next, 0 });
    // return next;
}

Node* Tree::get()
{
    return currentNode;
}

Node* Tree::peek()
{
    if (dfsStack.empty()) return nullptr;
    Node* top = dfsStack.top();
    return top;
}

void Tree::print()
{
    recPrint(root, 0);
}

void Tree::recPrint(Node* parentNode, int depth)
{
    cout << string(depth, ' ') << parentNode->nodeName << endl;
    for (Node* node : parentNode->childrenNodes) {
        recPrint(node, depth + 1);
    }
}