#include <iostream>
#include <string>
#include <map>
#include <queue>

class HuffmanNode {
public:
    int weight; // Frequency if leaf, value if not
    bool is_leaf;
    char el = 0; // 0 when not leaf
    HuffmanNode* left;
    HuffmanNode* right;
    std::string code;
    
    HuffmanNode() = default; // Empty constructor, not use!
    HuffmanNode(char, int);
    HuffmanNode(int, HuffmanNode*, HuffmanNode*);
    HuffmanNode(const HuffmanNode&); // Copy constructor
    HuffmanNode(HuffmanNode&&) noexcept; // Move constructor
    HuffmanNode& operator=(const HuffmanNode& other); // Copy assigment operator
    HuffmanNode& operator=(HuffmanNode&& other) noexcept; // Move assignment operator
    ~HuffmanNode();
};

struct CompareHuffmanNodes{
    bool operator()(HuffmanNode* a, HuffmanNode* b){
        return a->weight > b->weight;
    }
};

typedef std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, CompareHuffmanNodes> HuffmanNodesQueue;

std::ostream& operator<<(std::ostream&, const HuffmanNode&);

HuffmanNode build_tree(std::map<char, int>);

HuffmanNodesQueue build_nodes(std::map<char, int>);

void create_codes_table(HuffmanNode*, std::map<char, std::string>& prev, std::string = "");

std::string encode(std::string, std::map<char, std::string>);

void create_decode(std::string, HuffmanNode*, std::string&);
std::string decode(std::string, HuffmanNode*);

std::string decode_file(std::ifstream&, HuffmanNode*, size_t);

std::map<char, int> build_frequencies_table(std::string);