/* Inhouse implementation of priority queue using min heap
 */

#pragma once

#include <iostream>
#include <vector>

using namespace std;

struct Node;

class PriorityQueue {
  vector<pair<int, Node*>> data_;

  int parent(int i) { return (i - 1) / 2; }
  int left(int i) { return 2 * i + 1; }
  int right(int i) { return 2 * i + 2; }

  void heapify_up(int i);
  void heapify_down(int i);

public:
  void push_element(pair<int, Node*> h);
  void build_heap(vector<pair<int, Node*>> heap);
  pair<int, Node*> top();
  pair<int, Node*> pop();
  bool empty();
  int size();
};