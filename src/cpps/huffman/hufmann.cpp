#include "../../headers/huffman/hufmman.h"
#include <iostream>
#include <string>
#include <queue>
#include <fstream>
#include <bitset>

HuffmanNode::HuffmanNode(char el, int freq){
    this->el = el;
    this->weight = freq;
    this->left = nullptr;
    this->right = nullptr;
    this->is_leaf = true;
}

HuffmanNode::HuffmanNode(int val, HuffmanNode* left, HuffmanNode* right){
    this->weight = val;
    this->left = left;
    this->right = right;
    this->is_leaf = false;
}

HuffmanNode::~HuffmanNode(){
    delete left;
    delete right;
}

HuffmanNode::HuffmanNode(const HuffmanNode& other){
    this->el = other.el;
    this->is_leaf = other.is_leaf;
    this->left = other.left;
    this->right = other.right;
    this->weight = other.weight;
}

HuffmanNode::HuffmanNode(HuffmanNode&& other) noexcept{
    this->el = other.el;
    this->is_leaf = other.is_leaf;
    this->left = other.left;
    this->right = other.right;
    this->weight = other.weight;

    other.el = 0;
    other.is_leaf = 0;
    other.left = nullptr;
    other.right = nullptr;
    other.weight = 0;
}

// Operators

HuffmanNode& HuffmanNode::operator=(const HuffmanNode& other){
    if(this == &other) return *this;


    this->el = other.el;
    this->is_leaf = other.is_leaf;
    this->left = other.left;
    this->right = other.right;
    this->weight = other.weight;

    return *this;
}

HuffmanNode& HuffmanNode::operator=(HuffmanNode&& other) noexcept{
    this->el = other.el;
    this->is_leaf = other.is_leaf;
    this->left = other.left;
    this->right = other.right;
    this->weight = other.weight;

    other.el = 0;
    other.is_leaf = 0;
    other.left = nullptr;
    other.right = nullptr;
    other.weight = 0;

    return *this;
}

// Utils

std::ostream& operator<<(std::ostream& os, const HuffmanNode& node){
    if (node.is_leaf){
        os << node.el << node.weight;
        return os;
    }

    os << node.weight << '(' << "r = ";

    if(node.right != nullptr){
        os << *node.right;
    }
    else{
        os << "nullptr";
    }
    os << " l = ";
    if(node.left != nullptr){
        os << *node.left;
    }
    else{
        os << "nullptr";
    }
    os << ")";

    return os;
}

HuffmanNode build_tree(std::map<char, int> freqs){
    HuffmanNodesQueue nodes = build_nodes(freqs);
    
    while(nodes.size() > 1){
        HuffmanNode* left = nodes.top();
        nodes.pop();
        
        HuffmanNode* right = nodes.top();
        nodes.pop();


        HuffmanNode* merged = new HuffmanNode(left->weight + right->weight, left, right);
        nodes.push(merged);
    }

    return *nodes.top();
}

HuffmanNodesQueue build_nodes(std::map<char, int> queue_builder){
    HuffmanNodesQueue result;

    for(auto& pair : queue_builder){
        result.push(new HuffmanNode(pair.first, pair.second));
    }

    return result;
}

std::map<char, int> build_frequencies_table(std::string s){
    std::map<char, int> result;

    for (char c : s){
        if(result.find(c) != result.end()){
            result[c]++;
            continue;
        }

        result[c] = 1;
    }

    return result;
}


void create_codes_table(HuffmanNode* root, std::map<char, std::string>& prev, std::string code){
    if(root->is_leaf){
        prev[root->el] = code;
        return;
    };

    if(root->left != nullptr) create_codes_table(root->left, prev, code + '0');
    if(root->right != nullptr) create_codes_table(root->right, prev, code + '1');
}

std::string encode(std::string tree_string, std::map<char, std::string> table){
    std::string result;

    for(char c : tree_string){
        result += table[c];
    }

    return result;
}

void create_decode(std::string s, HuffmanNode* tree_root, std::string& out){
    if(tree_root == nullptr){
        return;
    }

    HuffmanNode* curr = tree_root;

    while(!curr->is_leaf){
        if(s[0] == '0') curr = curr->left;
        else curr = curr->right;

        if(s.length() == 0){
            return;
        }

        s = s.substr(1);
    }
    // Is leaf
    out += curr->el;

    create_decode(s, tree_root, out); // Process the rest of the string
}

std::string decode(std::string s, HuffmanNode* tree_root){
    std::string result;
    create_decode(s, tree_root, result);

    return result;
}

std::string decode_file(std::ifstream& ifs, HuffmanNode* tree_root, size_t len) {
    static auto reverse_string = [](std::string s) -> std::string {
        std::string result;
        for (int i = s.length() - 1; i >= 0; i--) {
            result += s[i];
        }
        return result;
    };

    std::string result;

    char byte;
    std::string s; 

    HuffmanNode* curr = tree_root;
    
    int bits_read = 0;
    while(!ifs.eof()){
        ifs.read(&byte, 1);
        s = reverse_string(std::bitset<8>(byte).to_string());

        for(char c : s){
            if(c == '0') curr = curr->left;
            else curr = curr->right;

            if(curr->is_leaf){
                result += curr->el;
                curr = tree_root;
            }

            bits_read++;

            if(bits_read >= len) return result;
        }
    }

    return result;
}