/*
    Header file for huffman encoding with functions for :

    1. Getting frequency table from file bits
    2. Constructing the tree
    3. Generating codes from the tree
    4. Function that calls above three internal functions to get encoded bits
   from file bits
*/

#pragma once
#include <unordered_map>
#include <vector>

struct Node {
  int data;
  Node *left;
  Node *right;
  Node(int data) : data(data), left(nullptr), right(nullptr) {}
};

using namespace std;

class HuffmanEncoding {
  unordered_map<char, int> getFrequencyTable(vector<char> bytes);
  Node *constructTree(unordered_map<char, int> freqTable);
  unordered_map<char, vector<int>>
  generateCodes(Node *root, unordered_map<char, int> freqTable);

public:
  vector<int> encodeFile(string path);
};
