/* Implementation of the priority queue defined in priority-queue.h*/

#include "priority-queue.h"
#include "hunffman-encoding.h"

// Restore heap property upward from index i
void PriorityQueue::heapify_up(int i) {
  while (i > 0) {
    int p = parent(i);
    if (data_[p].first > data_[i].first) {
      swap(data_[p], data_[i]);
      i = p;
    } else {
      break;
    }
  }
}

void PriorityQueue::heapify_down(int i) {
  while (i < (int)data_.size()) {
    int l = left(i);
    int r = right(i);
    int smallest = i;
    if (l < (int)data_.size() && data_[l].first < data_[smallest].first)
      smallest = l;
    if (r < (int)data_.size() && data_[r].first < data_[smallest].first)
      smallest = r;
    if (smallest == i)
      break;
    swap(data_[i], data_[smallest]);
    i = smallest;
  }
}

void PriorityQueue::push_element(pair<int, Node*> h) {
  data_.push_back(h);
  heapify_up(data_.size() - 1);
}

void PriorityQueue::build_heap(vector<pair<int, Node*>> heap) {
  data_ = heap;
  for (int i = (int)data_.size() / 2 - 1; i >= 0; i--) {
    heapify_down(i);
  }
}

pair<int, Node*> PriorityQueue::top() {
  if (empty()) {
    return {-1, nullptr};
  }
  return data_[0];
}

pair<int, Node*> PriorityQueue::pop() {
  if (empty()) {
    return {-1, nullptr};
  }
  pair<int, Node*> top = data_[0];
  data_[0] = data_.back();
  data_.pop_back();
  heapify_down(0);
  return top;
}

bool PriorityQueue::empty() { return data_.empty(); }

int PriorityQueue::size() { return data_.size(); }