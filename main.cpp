#include <iostream>
#include "helper.h"
#include <vector>
#include <queue>
#include <cmath>
#include <sstream>
using namespace std;

double cosine_measure(int length, int N, int f_t, int f_d_t) {
    if (length == 0) return 0;
    return log(1 + 1.0 * N / f_t) * (1 + log(f_d_t)) / sqrt(length);
}
void solve(string term, IndexReader& reader, map<int, double>& score) {
    reader.openList(term);
    int uid = -1;
    vector<Doc> docs;
    int f_t = 0;
    while ((uid = reader.nextGEQ(uid)) != reader.NOT_FOUND) {
        int freq = reader.getFreq();
        f_t += freq;
        docs.emplace_back(uid, freq);
    }
    reader.closeList(term);
    for (Doc doc : docs) {
        score[doc.uid] += cosine_measure(reader.getLength(doc.uid), (int) docs.size(), f_t, doc.freq);
    }
}
void Query(vector<string> query, IndexReader& reader) {
    map<int, double> score;
    for (auto term : query) {
        solve(term, reader, score);
    }
    int top = 10;
    priority_queue<pair<double, int>> pq;
    for (auto pair : score) {
        double value = pair.second, uid = pair.first;
        if (pq.size() < top) {
            pq.emplace(-value, uid);
        } else {
            if (value > -pq.top().first) {
                pq.pop();
                pq.emplace(-value, uid);
            }
        }
    }
    vector<pair<int, double>> result;
    while (pq.size()) {
        auto cur = pq.top();
        pq.pop();
        result.emplace_back(cur.second, -cur.first);
    }
    reverse(result.begin(), result.end());
    for (auto pair : result) {
        int uid = pair.first;
        double value = pair.second;
        string url = reader.getUrl(uid);
        if (url.size() < 4 || url.substr(0, 4) != "http") continue;
        auto payload = reader.getPayload(uid);
        cout << reader.getUrl(pair.first) << " " << value << endl;
        for (auto word: payload) {
            cout << word << " ";
        }
        cout << endl;
    }
}
int main() {
    cout << "search engine starting ..." << endl;
    IndexReader reader;
    cout << "search engine started" << endl;
    while (true) {
        cout << "please input your query in a line, or quit to exit" << endl;
        string query;
        getline(cin, query);
        if (query == "quit") break;
        stringstream ss(query);
        vector<string> terms;
        string word;
        while (ss >> word) {
            terms.push_back(word);
        }
        Query(terms, reader);
    }
//    Query({"student"}, reader);
    return 0;
}