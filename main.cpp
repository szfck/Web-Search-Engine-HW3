#include <iostream>
#include "helper.h"
#include <vector>
#include <queue>
#include <cmath>
#include <sstream>
#include <set>
using namespace std;

double cosine_measure(int length, int N, int f_t, int f_d_t) {
    if (length == 0) return 0;
    return log(1 + 1.0 * N / f_t) * (1 + log(f_d_t)) / sqrt(length);
}

string getSnippet(const vector<string>& doc, const vector<string>& query, int shift = 10) {
    vector<pair<int, int>> segments;
    set<string> vis;
    for (auto str : query) {
        vis.insert(str);
    }
    int len = doc.size();
    for (int i = 0; i < len; i++) {
        string str = doc[i];
        if (vis.find(str) != vis.end()) {
            vis.erase(str);
            segments.emplace_back(max(0, i - shift), min(len - 1, i + shift));
        }
    }
    sort(segments.begin(), segments.end());
    int ptr = 1;
    for (int i = 1; i < (int) segments.size(); i++) {
        if (segments[i].first <= segments[ptr - 1].second + 1) {
            segments[ptr - 1].second = max(segments[ptr - 1].second, segments[i].second);
        } else {
            segments[ptr++] = segments[i];
        }
    }
    string snnipet = "";
    const string ellipsis = "...... ";
    for (int i = 0; i < ptr; i++) {
        int l = segments[i].first, r = segments[i].second;
        if (i > 0) snnipet += ellipsis;
        for (int j = l; j <= r; j++) {
            snnipet += doc[j] + " ";
        }
    }
    return snnipet;
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
        string snnipet = getSnippet(payload, query);
        cout << snnipet << endl;
//        for (auto word: payload) {
//            cout << word << " ";
//        }
//        cout << endl;
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