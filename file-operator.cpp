/*
Class to read/write bits from file
Methods :
1. readFromFile(path) -> returns the bits in file
2. writeToFile(path, bits) -> returns bool for success/failure
*/

#include <fstream>
#include <string>
#include <vector>

using namespace std;

class FileOperator {
public:
  // Reads all bytes from file and returns them as a vector of bits (0s and 1s)
  vector<char> readFromFile(const string &path) {
    ifstream file(path, ios::binary);
    vector<char> bytes;

    if (!file.is_open()) {
      return bytes;
    }

    char byte;
    while (file.read(&byte, 1)) {
      bytes.push_back(byte);
    }

    file.close();
    return bytes;
  }

  // Writes a vector of bits (0s and 1s) to a file, pads to full byte if needed
  void writeToFile(const string &path, const vector<char> &bytes) {
    ofstream file(path, ios::binary);

    if (!file.is_open()) {
      return;
    }

    file.write(bytes.data(), bytes.size());
    file.close();
  }
};
