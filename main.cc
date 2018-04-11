#include <algorithm>
#include <bitset>
#include <iostream>
#include <unordered_set>
using namespace std;

const int WIDTH = 68,
          HEIGHT = 68,
          A = 3,
          B = 5,
          N_SQUARES = (WIDTH * HEIGHT) / (A*A + B*B);

struct SquareFilling {
    uint8_t nA = 0, nB = 0;
    vector<uint8_t> height = {0}, length = {WIDTH}; // Intervals
    bitset<2*N_SQUARES> insertions;
    size_t hash_value;
    bool did_flip = false; // Helper variable to reconstruct square

    bool operator ==(const SquareFilling & q) const {
      return hash_value == q.hash_value;
    }

    bool add_square(uint8_t size) {
      did_flip = false;

      /* Find min height interval */
      size_t i = distance(height.begin(), min_element(height.begin(), height.end()));

      /* Interval is too small */
      if (length[i] < size)
        return false;

      /* Interval is bigger: the square will be added to the left */
      if (length[i] > size) {
        /* Reduce current interval length */
        length[i] -= size;

        /* Early kill if the new minimum interval length can't be reached */
        if (length[i] == 1 or length[i] == 2 or length[i] == 4 or length[i] == 7)
          return false;

        if (i > 0 and height[i-1] == height[i] + size) {
          /* Join the previous interval if heights match */
          length[i-1] += size;
        }
        else {
          /* Create a new interval otherwise */
          height.insert(height.begin() + i, height[i] + size);
          length.insert(length.begin() + i, size);
        }
      }
      /* Interval is the same size as the square to be added */
      else if (length[i] == size) {
        if (i > 0 and height[i-1] == height[i] + size and i+1 < height.size() and height[i+1] == height[i] + size) {
          /* Combine all three intervals if their height match */
          length[i-1] += size + length[i+1];
          height.erase(height.begin()+i, height.begin()+i+2);
          length.erase(length.begin()+i, length.begin()+i+2);
        }
        else if (i > 0 and height[i-1] == height[i] + size) {
          /* Combine with the left interval */
          length[i-1] += size;
          height.erase(height.begin() + i);
          length.erase(length.begin() + i);
        }
        else if (i+1 < height.size() and height[i+1] == height[i] + size) {
          /* Combine with the right interval */
          length[i+1] += size;
          height.erase(height.begin() + i);
          length.erase(length.begin() + i);
        }
        else {
          /* Increase the interval's height otherwise */
          height[i] += size;
        }
      }

      /* Increment nA or nB and add to insertions bitset */

      insertions[nA + nB] = (size == B);
      size == A ? nA++ : nB++;
      if (nA > N_SQUARES or nB > N_SQUARES)
        return false;

      /* Flip intervals if necessary */
      for (size_t i = 0; i < height.size()/2; ++i)
        if (height[i] != height[height.size()-i-1]) {
          if (height[i] > height[height.size()-i-1]) {
            did_flip = true;
            height = vector<uint8_t>(height.rbegin(), height.rend());
            length = vector<uint8_t>(length.rbegin(), length.rend());
          }
          break;
        }

      /* Compute hash and save in object */
      string s;
      s.insert(end(s), begin(height), end(height));
      s.insert(end(s), begin(length), end(length));
      s.push_back(nA);
      hash<string> hash_fn;
      hash_value = hash_fn(s);

      return true;
    }
};

ostream& operator<<(ostream& os, const SquareFilling& q) {
  for (size_t i = 0; i < q.height.size(); ++i)
    for (int j = 0; j < q.length[i]; ++j)
      os << int(q.height[i]) << " ";
  for (int i = 0; i < q.nA + q.nB; ++i)
    os << (q.insertions[i] ? B : A);
  return os;
}

struct Hash {
  size_t operator() (const SquareFilling &q) const {
    return q.hash_value;
  }
};

void write_square(bitset<2*N_SQUARES> insertions, int n) {
  SquareFilling sq;
  vector<string> v(HEIGHT, string(WIDTH, '.'));
  int step = 0;
  while (step < n) {
    for (int i = 0; i < HEIGHT; ++i)
      for (int j = 0; j < WIDTH and step < n; ++j)
        if (v[i][j] == '.') {
          int size = (insertions[step++] ? B : A);
          for (int k = 0; k < size; ++k)
            for (int l = 0; l < size; ++l)
              v[i+k][j+l] = '0' + size;
          sq.add_square(size);
          if (sq.did_flip) {
            for (auto &s : v)
              s = string(s.rbegin(), s.rend());
            goto next_step;
          }
        }
    next_step:;
  }

  for (auto& s : v)
    cerr << s << endl;
}


int main() {
  /* TODO: read sizes from argv */

  unordered_set<SquareFilling, Hash> set1, set2;
  set1.insert(SquareFilling());

  for (int n = 0; n < 2*N_SQUARES; ++n) {
    cerr << "Step " << n << ": " << set1.size() << " profiles." << endl;

    for (auto it = set1.cbegin(); it != set1.cend(); set1.erase(it++)) {
      // cerr << *it << endl;
      SquareFilling q1(*it), q2(*it);
      if (q1.add_square(A))
        set2.insert(q1);
      if (q2.add_square(B))
        set2.insert(q2);
    }

    swap(set1, set2);
    if (n == 40) break;
  }

  if (not set1.empty()) {
    auto it = set1.begin();
    cout << *it << endl;
    write_square(it->insertions, int(it->nA) + it->nB);
  }
}
