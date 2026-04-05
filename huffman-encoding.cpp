/*
 Implementation of huffmain-encoding.h functions
*/

#include "file-operator.cpp"
#include "hunffman-encoding.h"
#include "priority-queue.h"

unordered_map<char, int>
HuffmanEncoding::getFrequencyTable(vector<char> bytes) {
  unordered_map<char, int> freqTable;
  for (auto byte : bytes) {
    freqTable[byte]++;
  }
  return freqTable;
}

Node *HuffmanEncoding::constructTree(unordered_map<char, int> freqTable) {
  PriorityQueue pq;
  for (auto &freq : freqTable) {
    pq.push_element({freq.second, new Node(freq.first)});
  }

  while (pq.size() > 1) {
    auto [leftFreq, left] = pq.top();
    pq.pop();
    auto [rightFreq, right] = pq.top();
    pq.pop();

    Node *parent = new Node('\0');
    parent->left = left;
    parent->right = right;

    pq.push_element({leftFreq + rightFreq, parent});
  }

  return pq.top().second;
}

static void traverseTree(Node *node, vector<int> &currentCode,
                         unordered_map<char, vector<int>> &codes) {
  if (!node)
    return;
  if (!node->left && !node->right) {
    codes[node->data] = currentCode;
    return;
  }
  currentCode.push_back(0);
  traverseTree(node->left, currentCode, codes);
  currentCode.pop_back();
  currentCode.push_back(1);
  traverseTree(node->right, currentCode, codes);
  currentCode.pop_back();
}

unordered_map<char, vector<int>>
HuffmanEncoding::generateCodes(Node *root, unordered_map<char, int> freqTable) {
  unordered_map<char, vector<int>> codes;
  vector<int> currentCode;
  traverseTree(root, currentCode, codes);
  return codes;
}

vector<int> HuffmanEncoding::encodeFile(string path) {
  FileOperator fileOperator;
  vector<char> bytes = fileOperator.readFromFile(path);
  unordered_map<char, int> freqTable = getFrequencyTable(bytes);
  Node *root = constructTree(freqTable);
  unordered_map<char, vector<int>> codes = generateCodes(root, freqTable);

  // Build encoded bits
  vector<int> encodedBits;
  for (auto byte : bytes) {
    for (int bit : codes[byte]) {
      encodedBits.push_back(bit);
    }
  }

  // Serialize into output bytes
  vector<char> output;

  // 4 bytes: number of unique chars
  uint32_t numChars = freqTable.size();
  output.push_back((numChars >> 24) & 0xFF);
  output.push_back((numChars >> 16) & 0xFF);
  output.push_back((numChars >> 8) & 0xFF);
  output.push_back(numChars & 0xFF);

  // For each char: 1 byte char + 4 bytes frequency
  for (auto &entry : freqTable) {
    output.push_back(entry.first);
    uint32_t freq = entry.second;
    output.push_back((freq >> 24) & 0xFF);
    output.push_back((freq >> 16) & 0xFF);
    output.push_back((freq >> 8) & 0xFF);
    output.push_back(freq & 0xFF);
  }

  // 4 bytes: total number of bits (needed to strip padding on decode)
  uint32_t numBits = encodedBits.size();
  output.push_back((numBits >> 24) & 0xFF);
  output.push_back((numBits >> 16) & 0xFF);
  output.push_back((numBits >> 8) & 0xFF);
  output.push_back(numBits & 0xFF);

  // Pack bits into bytes (MSB first)
  int i = 0;
  while (i < (int)encodedBits.size()) {
    char packedByte = 0;
    for (int b = 7; b >= 0 && i < (int)encodedBits.size(); b--, i++) {
      packedByte |= (encodedBits[i] & 1) << b;
    }
    output.push_back(packedByte);
  }

  fileOperator.writeToFile("encoded.bin", output);
  return encodedBits;
}
