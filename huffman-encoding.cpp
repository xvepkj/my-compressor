/*
 Implementation of huffmain-encoding.h functions
*/

#include "file-operator.cpp"
#include "hunffman-encoding.h"
#include "priority-queue.h"
#include <algorithm>

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
  vector<pair<char, int>> sorted(freqTable.begin(), freqTable.end());
  sort(sorted.begin(), sorted.end(), [](auto &a, auto &b) {
    return a.second != b.second ? a.second < b.second : a.first < b.first;
  });
  for (auto &entry : sorted) {
    pq.push_element({entry.second, new Node(entry.first)});
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

vector<char> HuffmanEncoding::decodeFile(string path) {
  FileOperator fileOperator;
  vector<char> bytes = fileOperator.readFromFile(path);

  // Read header
  uint32_t numChars = 0;
  for (int i = 0; i < 4; i++) {
    numChars |= (bytes[i] & 0xFF) << (24 - i * 8);
  }

  unordered_map<char, int> freqTable;
  for (int i = 0; i < numChars; i++) {
    char c = bytes[i * 5 + 4];
    uint32_t freq = 0;
    for (int j = 0; j < 4; j++) {
      freq |= (bytes[i * 5 + 5 + j] & 0xFF) << (24 - j * 8);
    }
    freqTable[c] = freq;
  }

  Node *root = constructTree(freqTable);
  unordered_map<char, vector<int>> codes = generateCodes(root, freqTable);

  // Read total number of bits
  uint32_t numBits = 0;
  for (int i = 0; i < 4; i++) {
    numBits |= (bytes[i + 4 + numChars * 5] & 0xFF) << (24 - i * 8);
  }

  // Decode bits
  vector<int> decodedBits;
  int byteIndex = 4 + numChars * 5 + 4;
  for (int i = 0; i < numBits; i++) {
    int byteOffset = i / 8;
    int bitPosition = 7 - (i % 8); // MSB first
    int bit = (bytes[byteIndex + byteOffset] >> bitPosition) & 1;
    decodedBits.push_back(bit);
  }

  // Decode characters
  vector<char> decodedChars;
  Node *currentNode = root;
  for (int bit : decodedBits) {
    if (bit == 0) {
      currentNode = currentNode->left;
    } else {
      currentNode = currentNode->right;
    }
    if (!currentNode->left && !currentNode->right) {
      decodedChars.push_back(currentNode->data);
      currentNode = root;
    }
  }

  fileOperator.writeToFile("decoded.txt", decodedChars);
  return decodedChars;
}
