//
// A simple implementation of scapegoat tree.
// Alpha is 0.75.
//
#pragma once
#ifndef SCAPEGOATTREESCAPEGOATTREE_H
#define SCAPEGOATTREESCAPEGOATTREE_H

#include <cstdint>
#include <vector>
#include <fstream>
#include <sstream>
#include <queue>

template <class KeyType,class ValueType>
class key_value_pair{
public:
    KeyType key;
    ValueType value;
    key_value_pair() = default;
    key_value_pair(const KeyType &k,const ValueType &v):key(k),value(v){}
};

template <class KeyType,class ValueType>
class ScapegoatTree{
private:
    class TreeNode{
    public:
        key_value_pair<KeyType,ValueType> data;
        //in one tree, a key should only appear once at most.
        TreeNode *leftChild;
        TreeNode *rightChild;
        bool isDeleted;
        //when we need to delete a node,set this to true and then adjust size then check the balance.


        TreeNode():leftChild(nullptr),rightChild(nullptr),isDeleted(false){}
        TreeNode(const key_value_pair<KeyType,ValueType> &content,
                 TreeNode *left = nullptr,
                 TreeNode *right = nullptr):
                 data(content),leftChild(left),rightChild(right),isDeleted(false){}
    };

    TreeNode *root; //root of tree
    double alpha; //factor of balance;
    TreeNode *check(TreeNode *target)const;
    //return nullptr if is all right;or return the first unbalanced node from the root.

public:

    ScapegoatTree():root(nullptr),alpha(0.75){}
    ScapegoatTree(double factor,
                  const std::vector<key_value_pair<KeyType,ValueType>> &dataSet);
    ScapegoatTree(double factor,const std::string &filepath);
    ~ScapegoatTree(){clear();};


    int getNodeSize(TreeNode *target)const;
    int getTreeSize()const;

    void getArray(TreeNode *target,std::vector<key_value_pair<KeyType,ValueType>> &array);

    bool isEmpty()const;
    bool searchKey(const KeyType &key)const;

    void clear(TreeNode *target);
    void clear();

    TreeNode *getRoot(){
        return this->root != nullptr ? this->root : nullptr;
    }

    bool insert(const key_value_pair<KeyType,ValueType> &newData);
    bool remove(const KeyType &key);
    void rebuild(TreeNode *&target);
    void build(TreeNode *&target,
               const std::vector<key_value_pair<KeyType, ValueType>> &data);
    TreeNode* buildTree(const std::vector<key_value_pair<KeyType, ValueType>>& data, int start, int end);

    void printTreeByLevel() const;
};

template<class KeyType, class ValueType>
ScapegoatTree<KeyType, ValueType>::ScapegoatTree(double factor, const std::string &filepath)
        : alpha(factor), root(nullptr) { // 初始化 alpha 和 root
    std::vector<key_value_pair<KeyType, ValueType>> Set; // 存储键值对
    std::ifstream file(filepath); // 打开文件
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filepath << std::endl;
        return; // 如果文件无法打开，直接返回
    }

    std::string line;
    while (std::getline(file, line)) { // 逐行读取文件
        std::istringstream iss(line);
        KeyType key;
        ValueType value;
        char comma; // 用于读取逗号

        // 解析每一行的格式：整数,字符串
        if (iss >> key >> comma >> value && comma == ',') {
            Set.emplace_back(key, value); // 将键值对添加到集合中
        }
    }
    file.close(); // 关闭文件

    build(root, Set); // 使用集合构建平衡二叉树
}


template<class KeyType, class ValueType>
ScapegoatTree<KeyType,ValueType>::TreeNode * ScapegoatTree<KeyType, ValueType>
        ::check(ScapegoatTree::TreeNode *target) const {
    if(target == nullptr){
        return nullptr;
    }
    TreeNode *position = root;
    while(position!=target){
        if(getNodeSize(position->leftChild) > getNodeSize(position) *alpha
        ||getNodeSize(position->rightChild) > getNodeSize(position) *alpha){
            return position;
        }

        else if(position->data.key < target->data.key){
            if(position->rightChild == nullptr){
                return nullptr;
            }
            position = position->rightChild;
        }

        else if(position->data.key > target->data.key){
            if(position->leftChild == nullptr){
                return nullptr;
            }
            position = position->leftChild;
        }
    }
    if(getNodeSize(position->leftChild) > getNodeSize(position) *alpha
       ||getNodeSize(position->rightChild) > getNodeSize(position) *alpha){
        return position;
    }
    else{
        return nullptr;
    }
}


template<class KeyType, class ValueType>
bool ScapegoatTree<KeyType, ValueType>::remove(const KeyType &key) {
    //if not existed,then return false;
    TreeNode *position = this->root;
    while(true){
        if(position->data.key == key){
            if(position->isDeleted){return false;}
            else{
                position->isDeleted = true;
            }
            break;
        }

        else if(position->data.key < key){
            if(position->rightChild == nullptr){
                return false;
            }
            position = position->rightChild;
        }

        else if(position->data.key > key){
            if(position->leftChild == nullptr){
                return false;
            }
            position = position->leftChild;
        }
    }
    if(check(position) == nullptr){
        return true;
    }
    else{
        rebuild(check(position));
        return true;
    }
}

template<class KeyType, class ValueType>
bool ScapegoatTree<KeyType, ValueType>::insert(const key_value_pair<KeyType, ValueType> &newData) {
    //if the same key is existed,then take the place.
    TreeNode *position = this->root;
    KeyType key = newData.key;
    while(true){
        if(position->data.key == key){
            auto *newNode = new TreeNode (newData,position->leftChild,position->rightChild);
            position = newNode;
            break;
        }

        else if(position->data.key < key){
            if(position->rightChild == nullptr){
                auto *newNode = new TreeNode (newData);
                position->rightChild = newNode;
                break;
            }
            position = position->rightChild;
        }

        else if(position->data.key > key){
            if(position->leftChild == nullptr){
                auto *newNode = new TreeNode (newData);
                position->leftChild = newNode;
                break;
            }
            position = position->leftChild;
        }
    }
    TreeNode* unbalancedNode = check(position);
    if(unbalancedNode == nullptr){
        return true;
    }
    else{
        rebuild(unbalancedNode);
        return true;
    }
}

template<class KeyType, class ValueType>
bool ScapegoatTree<KeyType, ValueType>::searchKey(const KeyType &key) const {
    TreeNode *position = this->root;
    while(true){
        if(position->data.key == key){
            return !position->isDeleted;
        }

        else if(position->data.key < key){
            if(position->rightChild == nullptr){
                return false;
            }
            position = position->rightChild;
        }

        else if(position->data.key > key){
            if(position->leftChild == nullptr){
                return false;
            }
            position = position->leftChild;
        }
    }
}

template<class KeyType, class ValueType>
void ScapegoatTree<KeyType, ValueType>::getArray(ScapegoatTree::TreeNode *target,
                                                 std::vector<key_value_pair<KeyType, ValueType>> &array) {
    if(target == nullptr){
        return;
    }
    getArray(target->leftChild,array);
    if(!target->isDeleted){
        key_value_pair<KeyType,ValueType> tmp(target->data.key,target->data.value);
        array.push_back(tmp);
    }
    getArray(target->rightChild,array);
}

template<class KeyType, class ValueType>
bool ScapegoatTree<KeyType, ValueType>::isEmpty() const {
    return (root == nullptr);
}

template<class KeyType, class ValueType>
int ScapegoatTree<KeyType, ValueType>::getTreeSize() const {
    return getNodeSize(root);
}

template<class KeyType, class ValueType>
int ScapegoatTree<KeyType, ValueType>::getNodeSize(ScapegoatTree::TreeNode *target) const {
    if(target == nullptr){return 0;}
    if(target->isDeleted){
        return getNodeSize(target->leftChild)+ getNodeSize(target->rightChild);
    }
    else if(!target->isDeleted){
        return getNodeSize(target->leftChild)+ getNodeSize(target->rightChild)+1;
    }
}

template<class KeyType, class ValueType>
void ScapegoatTree<KeyType, ValueType>::clear() {
    clear(this->root);
}

template<class KeyType, class ValueType>
void ScapegoatTree<KeyType, ValueType>::clear(ScapegoatTree::TreeNode *target) {
    if(!target){
        return;
    }
    clear(target->leftChild);
    clear(target->rightChild);
    delete target;
}

template<class KeyType, class ValueType>
void ScapegoatTree<KeyType, ValueType>::rebuild(TreeNode *&target) {
    std::vector<key_value_pair<KeyType,ValueType>> allData;
    this->getArray(target,allData);
    build(target,allData);
}

template<class KeyType, class ValueType>
ScapegoatTree<KeyType, ValueType>::ScapegoatTree(double factor,
                                                 const std::vector<key_value_pair<KeyType, ValueType>> &dataSet) {
    this->alpha = factor;
    build(root,dataSet);
}

template<class KeyType, class ValueType>
void ScapegoatTree<KeyType, ValueType>::build(ScapegoatTree::TreeNode *&target,
                                              const std::vector<key_value_pair<KeyType, ValueType>> &data) {
    if(data.empty()){return;}
    target = buildTree(data,0,data.size()-1);
}

template<class KeyType, class ValueType>
ScapegoatTree<KeyType,ValueType>::TreeNode *
ScapegoatTree<KeyType, ValueType>::buildTree(const std::vector<key_value_pair<KeyType, ValueType>> &data, int start,
                                             int end) {
    if (start > end) {
        return nullptr;
    }
    int mid = (start + end) / 2;
    key_value_pair<KeyType,ValueType> newData(data[mid].key, data[mid].value);
    auto* node = new TreeNode(newData);

    node->leftChild = buildTree(data, start, mid - 1);
    node->rightChild = buildTree(data, mid + 1, end);

    return node;
}

template <class KeyType, class ValueType>
void ScapegoatTree<KeyType, ValueType>::printTreeByLevel() const {
    if (!root) {
        std::cout << "Tree is empty." << std::endl;
        return;
    }
    std::queue<TreeNode*> q;
    q.push(root);
    while (!q.empty()) {
        int levelSize = q.size();
        for (int i = 0; i < levelSize; ++i) {
            TreeNode* node = q.front();
            q.pop();

            if (node && !node->isDeleted) {
                std::cout << "(" << node->data.key << ", " << node->data.value << ") ";
                q.push(node->leftChild);
                q.push(node->rightChild);
            }
            else {
                std::cout << "(null, null) ";
            }
        }
        std::cout << std::endl;
    }
}
#endif //SCAPEGOATTREESCAPEGOATTREE_H
