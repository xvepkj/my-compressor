/* Main class to test huffman encoding */

#include "file-operator.cpp"
#include "hunffman-encoding.h"

int main() {
  HuffmanEncoding huffmanEncoding;
  huffmanEncoding.encodeFile("test.txt");
  return 0;
}
