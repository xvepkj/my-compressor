CXX = g++
CXXFLAGS = -std=c++17

compressor: main.cpp huffman-encoding.cpp priority-queue.cpp
	$(CXX) $(CXXFLAGS) -o compressor main.cpp huffman-encoding.cpp priority-queue.cpp

run: compressor
	./compressor

clean:
	rm -f compressor
