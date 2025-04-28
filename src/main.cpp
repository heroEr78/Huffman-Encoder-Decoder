#include <iostream>
#include "./headers/huffman/hufmman.h"
#include <fstream>
#include <cstring>
#include <bitset> 

struct Params
{
    std::string input_file_location;
    std::string output_file_location;
    char flag;
} config;

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "Not enough args, see usage ";
        return 1;
    }

    if (strlen(argv[1]) < 2 || strlen(argv[3]) < 2)
    {
        std::cerr << "Strange input, see usage";
    }

    config.flag = argv[1][1]; // We don't take -
    
    if (strcmp("-o", argv[3]) != 0)
    {
        std::cerr << "Needs output, see usage";
    }

    config.output_file_location = argv[4];
    config.input_file_location = argv[2];

    // Encode
    if (config.flag == 'e')
    {
        std::string input;

        std::ifstream ifs(config.input_file_location, std::ios::binary);

        if (!ifs.is_open())
        {
            std::cerr << "Cannot open file. ABORTING";
            return 1;
        }

        char tmp_c;
        while (ifs.get(tmp_c))
        {
            input += tmp_c;
        }

        ifs.close();

        std::map<char, std::string> codes;
        std::map<char, int> freqs = build_frequencies_table(input);
        HuffmanNode tree_root = build_tree(freqs);

        create_codes_table(&tree_root, codes);

        std::string encoded = encode(input, codes);

        std::ofstream ofs(config.output_file_location, std::ios::binary | std::ios::out);

        size_t n_cells = freqs.size();
        ofs.write(reinterpret_cast<char *>(&n_cells), sizeof(size_t));

        for (auto &pair : freqs)
        {                                                                           // Write all the pairs 
            ofs.write(&pair.first, sizeof(pair.first));                             // 1
            ofs.write(reinterpret_cast<char *>(&pair.second), sizeof(pair.second)); // 4
        }

        size_t encoded_lenght = encoded.size();
        ofs.write(reinterpret_cast<char *>(&encoded_lenght), sizeof(size_t));

        unsigned char buffer = 0;
        int bit_count = 0;

        for (char c : encoded)
        {
            if (c == '1')
            {
                buffer |= 1 << bit_count; // Insert 1 at bit_count pos // Writes bits reversed
            }

            bit_count++;

            if (bit_count == 8)
            {
                ofs.write(reinterpret_cast<char *>(&buffer), 1);
                bit_count = 0;
                buffer = 0;
            }
        }

        if (bit_count != 0)
        { // Write remaining bits
            ofs.write(reinterpret_cast<char *>(&buffer), 1);
        }

        ofs.close();

        return 0;
    }

    if(config.flag != 'd') return 1;
    
    // Decode
    std::ifstream ifs(config.input_file_location, std::ios::binary | std::ios::in);
    std::ofstream ofs(config.output_file_location, std::ios::binary | std::ios::out);

    std::map<char, int> frequencies;

    size_t n_entries;
    ifs.seekg(0, std::ios::beg);
    ifs.read(reinterpret_cast<char*>(&n_entries), sizeof(size_t));

    size_t n_entries_bytes = (sizeof(int) + sizeof(char)) * n_entries + sizeof(int) + sizeof(char);

    while(ifs.tellg() < n_entries_bytes)
    {
        char c;
        int its_freq;

        ifs.read(&c, 1);
        ifs.read(reinterpret_cast<char *>(&its_freq), 4);

        frequencies[c] = its_freq;
    }

    size_t len;
    ifs.read(reinterpret_cast<char*>(&len), sizeof(size_t));

    HuffmanNode original_tree = build_tree(frequencies);
    ofs << decode_file(ifs, &original_tree, len);

    ifs.close();
    ofs.close();

    return 0;
}