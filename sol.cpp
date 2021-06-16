#include <bits/stdc++.h>
#define int unsigned long long
using namespace std;

// For Making Pair As Key Of Unordered_Map
struct hash_pair {
  template <class T1, class T2>
  size_t operator()(const pair<T1, T2> &p) const {
    auto hash1 = hash<T1>{}(p.first);
    auto hash2 = hash<T2>{}(p.second);
    return hash1 ^ hash2;
  }
};

set<string> childLeft;
struct transaction {
  string id;
  int wt;
  int profit;
  vector<string> parents;
};

void sortTransaction(vector<transaction> &tr, vector<transaction> &st);
void read(string fileName, vector<transaction> &tr);
unordered_map<pair<int, string>, bool, hash_pair> searchh;
unordered_map<string, bool> visited;
unordered_map<string, vector<transaction>> pc;
// initialize all char with false;
// ofstream fout;

/*-----------------------------------------------------------------*/
// Recursion Approx Similar As 0-1 KnapSack Problem. Ans Will Store Final Ans.Tr
// Represent Sorted Transaction
// refcurrfee Represent Max fee/profit seen So far, Max Wt Denote Maximum
// Combining Weight Of Result Transaction Allow
// Index Denote Current Index Of Transaction

// This Work Well  For Small No Of Transaction Like 1000-2000 but it may take
// 3-24hrs for printing output for actual Input File in mempool.csv

void recursion(vector<string> &ans, vector<string> &temp,
               vector<transaction> &tr, int index, int n, int &ref_curr_fee,
               int curr_fee, int maxwt) {
  if (index == n) {
    if (maxwt < 0)
      return;
    else if (ref_curr_fee < curr_fee) {
      ref_curr_fee = curr_fee;
      ans = temp;
      return;
    } else
      return;
  }
  if (maxwt < 0) return;
  bool isvis = true;
  for (auto &p : tr[index].parents) {
    if (visited[p] == false) {
      isvis = false;
      break;
    }
  }

  recursion(ans, temp, tr, index + 1, n, ref_curr_fee, curr_fee, maxwt);

  if (isvis == true) {
    visited[tr[index].id] = true;
    temp.push_back(tr[index].id);
    recursion(ans, temp, tr, index + 1, n, ref_curr_fee,
              curr_fee + tr[index].profit, maxwt - tr[index].wt);
    temp.pop_back();
  }
  visited[tr[index].id] = false;
}

// Optimal Recursion Approach By Introducing Set And DP.
void recur(vector<string> &ans, vector<string> &temp, vector<transaction> &tr,
           int index, int n, int &ref_curr_fee, int curr_fee, int maxwt) {
  if (index == n) {
    if (maxwt < 0)
      return;
    else if (ref_curr_fee < curr_fee) {
      ref_curr_fee = curr_fee;
      ans = temp;

      return;
    } else
      return;
  }

  if (maxwt < 0) return;
  bool isvis = true;
  for (auto &p : tr[index].parents) {
    if (visited[p] == false) {
      isvis = false;
      break;
    }
  }

  string childrem = "";

  for (auto &it : childLeft) {
    childrem += it;
  }
  pair<int, string> pp;
  if (childrem.length() > 0) {
    pp = {maxwt, childrem};
    // cout << childrem << " " << maxwt << " " << ref_curr_fee << " ";
    // for (auto i : temp) cout << " temp " << i << " ";
    // cout << endl;
    if (searchh[pp] == true) return;
  }
  recur(ans, temp, tr, index + 1, n, ref_curr_fee, curr_fee, maxwt);

  if (isvis == true) {
    visited[tr[index].id] = true;
    temp.push_back(tr[index].id);
    for (auto &child : pc[tr[index].id]) {
      childLeft.insert(child.id);
    }
    recur(ans, temp, tr, index + 1, n, ref_curr_fee,
          curr_fee + tr[index].profit, maxwt - tr[index].wt);
    temp.pop_back();
    for (auto &child : pc[tr[index].id]) {
      childLeft.erase(child.id);
    }
  }
  visited[tr[index].id] = false;
  searchh[pp] = true;
}
bool compare(transaction a, transaction b) { return a.id < b.id; }
signed main() {
#ifndef ONLINE_JUDGE
  freopen("block.txt", "w", stdout);
#endif
  vector<transaction> tr;
  read("mempool.csv", tr);
  // sort Id Of Each Transaction lexicographically
  sort(tr.begin(), tr.end(), compare);
  vector<transaction> sortedtr;

  // sort transaction in such a way that parent will be left of child
  sortTransaction(tr, sortedtr);
  // for (auto &i : sortedtr) {
  //   cout << i.id << " ";
  // }
  // cout << endl;

  int size = sortedtr.size();

  vector<string> ans;
  vector<string> temp;
  int ind = 0, ref_cur_fee = 0;

  // cout << " Transacion ";

  recur(ans, temp, sortedtr, ind, size, ref_cur_fee, 0, 4000000);

  for (auto i : ans) {
    cout << i << "\n";
  }
  // cout << endl;
  // cout << ref_cur_fee;
}

/**
 * This Function read CSV File and parse them in structure of Transation
 * param: CSV File , vector to store all transaction detail
 * return type:void
 * Time Complexity O(N^2) : Assuming worst case as each transaction has n-1
 * parent transaction space Complexity O(N^2) worst case
 * */
void read(string fileName, vector<transaction> &tr) {
  ifstream myFile;
  myFile.open(fileName);
  vector<string> row;
  int count = 0;
  while (myFile.good()) {
    count++;
    row.clear();
    string line;
    getline(myFile, line);
    if (count == 1) {
      continue;
    }
    // if (count == 7) break;
    stringstream s(line);
    string str;
    transaction t;
    while (getline(s, str, ',')) {
      row.push_back(str);
    }
    t.id = row[0];
    t.profit = stoi(row[1]);
    t.wt = stoi(row[2]);
    int size = row.size();
    for (int parent = 3; parent < size; parent++) {
      t.parents.push_back(row[parent]);
    }
    tr.push_back(t);
  }
}

/**
 *This Function WillSort All Transaction in such a way that parent will come
 before child transaction *This is same as topological sort *Param : original
 transactions,vector to store storted transations *return type:void *Time
 complexity O(N^2) in worst case,where N is no of transaction *Space complexity
 O(N^2) considering each transation has n-1 parent and total transaction is
 n.(Worst case)

*/
void sortTransaction(vector<transaction> &tr, vector<transaction> &st) {
  unordered_map<string, int> freqParent;

  unordered_map<string, transaction> mp;
  for (auto &item : tr) {
    mp.insert({item.id, item});
  }
  for (auto &child : tr) {
    vector<string> parent = child.parents;

    for (auto &parent : parent) {
      pc[parent].push_back(child);
    }
  }

  for (auto &item : tr) {
    freqParent.insert({item.id, item.parents.size()});
  }

  queue<string> q;
  for (auto &item : freqParent) {
    if (item.second == 0) q.push(item.first);
  }
  while (!q.empty()) {
    string top = q.front();
    st.push_back(mp[top]);
    q.pop();
    for (auto &child : pc[top]) {
      freqParent[child.id] -= 1;
      if (freqParent[child.id] == 0) q.push(child.id);
    }
  }
}

// ---------------------------COMMENT---------------------------------------------------------------------
// cout << endl;
// int wtt[] = {5, 4, 1, 7, 19, 40, 45, 33, 32, 31};
// int sz = sizeof(wtt) / sizeof(wtt[0]);
// for (int i = 0; i < sz; i++) {
//   ans.clear();
//   temp.clear();
//   ref_cur_fee = 0;

// }
// stack<string> stac;
// for (auto &item : tr) {
//   if (visited.find(item.id) == visited.end()) {
//     cout << item.id << endl;
//     stac.push(item.id);
//     visited.insert(item.id);
//     while (!stac.empty()) {
//       string top = stac.top();
//       bool found = false;
//       for (auto &parent : freqParent[top].parents) {
//         if (visited.find(parent) == visited.end()) {
//           stac.push(parent);
//           visited.insert(parent);
//           found = true;
//         }
//       }
//       if (!found) {
//         st.push_back(freqParent[top]);
//         stac.pop();
//       }
//     }
//   }
// }
// bool visited[];
// // a d c b
// int recur(char arr[], int index, vector<char> &res, int n, int maxwt) {
//   if (index == n && maxwt > 0) {
//   }
//   if (index == n) return 0;
//   if (maxwt < 0) {
//     return INT_MIN;
//   }
//   if (arr[index].parent.size > 0) {
//     bool visit = true;
//     for (auto parent : arr[index].parent) {
//       if (visited[parent] == false) {
//         visit = false;
//         break;
//       }
//     }
//     if (visit) {
//       if (arr[index].profit +
//               recur(arr, index + 1, res, n, maxwt - arr[index].wt) >
//           recur(arr, index + 1, res, n, maxw)) {
//         res.push_back(arr[index])
//       } else {
//       }
//     } else {
//       return recur(arr, index + 1, res, n, maxw);
//     }
//   } else {
//     return max(arr[index].profit +
//                    recur(arr, index + 1, res, n, maxwt - arr[index].wt),
//                recur(arr, index + 1, res, n, maxw));
//   }
// }