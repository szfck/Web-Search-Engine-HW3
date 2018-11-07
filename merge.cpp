#include <iostream>
#include <unordered_map>
#include <string>
#include "helper.h"
#include <set>
#include <vector>
#include <map>
#include <algorithm>
#include <fstream>
#include <queue>

using namespace std;

const string OUTPUT = "../output/"; // output dir
const string PREV_OUTPUT = OUTPUT + "intermediate-output-2/";
const string CUR_OUTPUT = OUTPUT + "intermediate-output-3/";

struct Node {
    int tid, fid;
    Node(int tid, int fid) : tid(tid), fid(fid) {}
    Node() {}
    bool operator < (const Node& o) const {
        return tid > o.tid; // sorted by term id
    }
};

string str(long long x) {
    return to_string(x);
}
// add term to priority queue
void addToQue(int fid, priority_queue<Node>& pq, map<int, vector<Doc>>& mp, Reader& indexes, const  vector<Index>& term_index_vec, int& counter) {
    if (counter >= (int) term_index_vec.size()) return;
    int idx = counter++;
    auto index = term_index_vec[idx];
    int tid = index.tid, start = index.start, end = index.end, number = index.number;
    auto docs = indexes.vreadList(tid, start, end, number);
    for (auto doc : docs) {
        mp[tid].push_back(doc);
    }
    pq.push(Node(tid, fid));
}

void output(int tid, map<int, vector<Doc>>& mp, Writer& merged_index, Writer& merged_term_index) {
    long long start = merged_index.getOffset();
    auto& list = mp[tid];
    sort(list.begin(), list.end(), [] (const Doc& doc1, const Doc& doc2) {
        return doc1.uid < doc2.uid;
    });
    merged_index.vwriteList(tid, mp[tid]);
    long long end = merged_index.getOffset();
    merged_term_index.swrite(str(tid) + " " + str(start) + " " + str(end) + " " + str(mp[tid].size()) + '\n');
}

// n way merge 
void nway_merge(Reader indexes[], Reader term_indexes[], int n, Writer& merged_index, Writer& merged_term_index) {
    priority_queue<Node> pq;
    map<int, vector<Doc>> mp; // term_id [(doc_id, freq) ..]
    vector<Index> term_indexes_vec[n];
    vector<int> counter(n, 0);
    for (int i = 0; i < n; i++) {
        term_indexes_vec[i] = term_indexes[i].indexread();
    }
    for (int i = 0; i < n; i++) {
        addToQue(i, pq, mp, indexes[i], term_indexes_vec[i], counter[i]);
    }
    while (pq.size()) {
        auto cur = pq.top();
        int tid = cur.tid;

        // output merged list for term 
        output(tid, mp, merged_index, merged_term_index);

        while (pq.size() && tid == pq.top().tid) { // remove all the same term id
            int fid = pq.top().fid;
            pq.pop();
            addToQue(fid, pq, mp, indexes[fid], term_indexes_vec[fid], counter[fid]);
        }

        mp.erase(tid);
    }
}

// get a number with leading 0s
string getNum(int x) {
    string num = to_string(x);
    while ((int)num.size() < 5) num = '0' + num;
    return num;
}

// merge files in [start, start + number) to a target file
void merge_(int step, int start, int number, int target) {
    cout << "merge step: " << step << " from " << start << " to " << start + number - 1 << endl;
    Reader indexes[number];
    Reader term_indexes[number];
    for (int j = 0; j < number; j++) {
        string index_path = "";
        string term_index_path = "";
        if (step == 1) {
            index_path = PREV_OUTPUT + "index-" + getNum(start + j) + ".bin";
            term_index_path = PREV_OUTPUT + "index-" + getNum(start + j) + ".txt";
        } else {
            index_path = CUR_OUTPUT + "index-" + getNum(start + j) + ".merge" + to_string(step - 1) + ".bin";
            term_index_path = CUR_OUTPUT + "index-" + getNum(start + j) + ".merge" + to_string(step - 1) + ".txt";
        }
        indexes[j].open(index_path);
        term_indexes[j].open(term_index_path);
    }
    string merge_index_path = CUR_OUTPUT + "index-" + getNum(target) + ".merge" + to_string(step) + ".bin";
    string merge_term_index_path = CUR_OUTPUT + "index-" + getNum(target) + ".merge" + to_string(step) + ".txt";
    Writer merged_index(merge_index_path);
    Writer merged_term_index(merge_term_index_path);

    nway_merge(indexes, term_indexes, number, merged_index, merged_term_index);

    cout << "merge finished .. " << endl;
    for (int j = 0; j < number; j++) {
        indexes[j].close();
        term_indexes[j].close();
    }
    merged_index.close();
    merged_term_index.close();
}
int main(int argc, char** argv) {
    // merge files in [start, end) to target file
    int step = atoi(argv[1]);
    int start = atoi(argv[2]);
    int end = atoi(argv[3]);
    int target = atoi(argv[4]);
    merge_(step, start, end - start, target);
    return 0;
}
