#include <algorithm>
#include <bitset>
#include <iostream>
#include <unordered_set>
using namespace std;

const int WIDTH = 68,
          HEIGHT = 68,
          A = 3,
          B = 5,
          N_QUADRATS = (WIDTH * HEIGHT) / (A*A + B*B);

class Quadrat {
  public:
    uint8_t height = 0, nA = 0, nB = 0;
    vector<uint8_t> altura = {0}, repeats = {WIDTH};
    bitset<2*N_QUADRATS> tria;
    size_t hash_value;

    bool operator ==(const Quadrat & q) const {
      return hash_value == q.hash_value;
    }

    bool afegir(uint8_t mida) {
      bool cal_comprovar_el_minim = false;

      /* Afegir element al vector on toque */
      size_t i = distance(altura.begin(), find(altura.begin(), altura.end(), 0));

      // Massa petit
      if (repeats[i] < mida)
        return false;

      // Més gran
      if (repeats[i] > mida) {
        // Reduir mida del 0
        repeats[i] -= mida;

        // Early kill
        if (repeats[i] == 1 or repeats[i] == 2 or repeats[i] == 4 or repeats[i] == 7)
          return false;

        if (i > 0 and altura[i-1] == mida) {
          // Juntar amb l'anterior
          repeats[i-1] += mida;
        }
        else {
          // Inserir si a l'inici o l'anterior és diferent
          altura.insert(altura.begin() + i, mida);
          repeats.insert(repeats.begin() + i, mida);
        }
      }
      // Igual mida
      else if (repeats[i] == mida) {
        cal_comprovar_el_minim = true;

        // Juntar els 3
        if (i > 0 and altura[i-1] == mida and i+1 < altura.size() and altura[i+1] == mida) {
          repeats[i-1] += mida + repeats[i+1];
          altura.erase(altura.begin()+i, altura.begin()+i+2);
          repeats.erase(repeats.begin()+i, repeats.begin()+i+2);
        }
        // Juntar a l'esquerra
        else if (i > 0 and altura[i-1] == mida) {
          repeats[i-1] += mida;
          altura.erase(altura.begin() + i);
          repeats.erase(repeats.begin() + i);
        }
        // Juntar a la dreta
        else if (i+1 < altura.size() and altura[i+1] == mida) {
          repeats[i+1] += mida;
          altura.erase(altura.begin() + i);
          repeats.erase(repeats.begin() + i);
        }
        // Canviar mida
        else
          altura[i] = mida;
      }

      /* Increment nA or nB and add to tria */
      tria[mida == A ? nA++ : nB++] = (mida == A);
      if (nA > N_QUADRATS or nB > N_QUADRATS)
        return false;

      /* Posar la mínima altura a zero */
      if (cal_comprovar_el_minim) {
        uint8_t minim = *min_element(altura.begin(), altura.end());
        if (minim > 0) {
          height += minim;
          for (size_t i = 0; i < altura.size(); ++i) {
            altura[i] -= minim;
            if (altura[i] == 0 and (repeats[i] == 1 or repeats[i] == 2 or repeats[i] == 4 or repeats[i] == 7))
              return false;
          }
        }
      }

      /* Flip skyline if necessary */
      for (size_t i = 0; i < altura.size()/2; ++i)
        if (altura[i] != altura[altura.size()-i-1]) {
          if (altura[i] < altura[altura.size()-i-1]) {
            altura = vector<uint8_t>(altura.rbegin(), altura.rend());
            repeats = vector<uint8_t>(repeats.rbegin(), repeats.rend());
          }
          break;
        }

      /* Compute hash and save in object */
      string s;
      s.insert(end(s), begin(altura), end(altura));
      s.insert(end(s), begin(repeats), end(repeats));
      s.push_back(nA);
      s.push_back(height);
      hash<string> hash_fn;
      hash_value = hash_fn(s);

      return true;
    }
};

ostream& operator<<(ostream& os, const Quadrat& q) {
  os << int(q.height) << " ";
  for (size_t i = 0; i < q.altura.size(); ++i)
    os << string(q.repeats[i], '0' + q.altura[i]);
  os << " ";
  for (int i = 0; i < q.nA + q.nB; ++i)
    os << (q.tria[i] ? B : A);
  return os;
}

struct Hash {
  size_t operator() (const Quadrat &q) const {
    return q.hash_value;
  }
};


void write_square(bitset<2*N_QUADRATS> ordre) {
  cout << "TODO" << endl;
}


int main() {
  /* TODO: read sizes from argv */

  unordered_set<Quadrat, Hash> set1, set2;
  set1.insert(Quadrat());

  for (int n = 0; n < 2*N_QUADRATS; ++n) {
    cerr << "Step " << n << ": " << set1.size() << " profiles." << endl;

    for (auto it = set1.cbegin(); it != set1.cend(); set1.erase(it++)) {
      // cerr << *it << endl;
      Quadrat q1(*it), q2(*it);
      if (q1.afegir(A))
        set2.insert(q1);
      if (q2.afegir(B))
        set2.insert(q2);
    }

    swap(set1, set2);
    if (n == 40) break;
  }

  if (not set1.empty()) {
    cout << *set1.begin() << endl;
    write_square(set1.begin()->tria);
  }
}
