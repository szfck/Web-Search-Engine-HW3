#include <iostream>
#include <unordered_map>
#include <assert.h>
#include <string>
#include <set>
#include <vector>
#include <map>
#include <algorithm>
#include <fstream>
#include <queue>
#include "helper.h"

using namespace std;

int main(int argc, char** argv) {
    // this is a test : decode info from binarry file and check value
    // Reader index_reader("../output/intermediate-output-2/index-01001.bin");
    // Reader index_reader("../output/intermediate-output-2/index-01001.bin");
    string index_path(argv[1]);
    string term_index_path(argv[2]);
    cout << index_path << endl;
    cout << term_index_path << endl;
    Reader index_reader(index_path);
    Reader term_index_reader(term_index_path);
    Reader term_reader("../output/term_table.txt");
    Reader url_reader("../output/url_table.txt");
    auto terms = term_reader.termread();
    auto urls = url_reader.urlread();
    auto term_index = term_index_reader.indexread();

    // for (auto url : urls) {
    //     cout << url.uid << " " << url.url << " " << url.length << endl;
    // }
    // for (auto term : terms) {
    //     cout << term.tid << " " << term.term << endl;
    // }

    map<string, int> term_mp;
    for (int i = 0; i < (int) terms.size(); i++) {
        term_mp[terms[i].term] = i;
        assert (i == terms[i].tid);
    }
    string query = "student";
    int qid = term_mp[query];
    Index qindex = term_index[qid];
    {
        int tid = qindex.tid, start = qindex.start, end = qindex.end, number = qindex.number;
        auto docs = index_reader.vreadList(tid, start, end, number);
        for (auto doc : docs) {
            int uid = doc.uid, freq = doc.freq;
            cout << terms[tid].term << " " << urls[uid].url << " " << freq << endl;
        }

    }
    // for (auto index: term_index) {
    //     int tid = index.tid, start = index.start, end = index.end, number = index.number;
    //     auto docs = index_reader.vreadList(tid, start, end, number);
    //     for (auto doc : docs) {
    //         int uid = doc.uid, freq = doc.freq;
    //         cout << terms[tid].term << " " << urls[uid].url << " " << freq << endl;
    //     }
    // }
    // for (auto p : term_index.read()) {
    //     int id = p.first;
    //     int start = stoi(p.second[0]);
    //     int end = stoi(p.second[1]);
    //     int number = stoi(p.second[2]);
    //     string term = terms[id][0];
    //     cout << term << endl;
    //     cout << start << " " << end << " " << number << endl;
    //     auto res = index.read(id, start, end, number);
    //     for (auto doc : res) {
    //         int did = doc.first, freq = doc.second;
    //         cout << did << endl;
    //         string url = urls[did][0];
    //         cout << "term: " << term << " in doc: " << url << " occur: " << freq << endl;
    //     }
    // }
    // int start = 49018155, end = 49018167;
    // int num = 2;
    // int id = 1102785;
    // auto list = reader.read(start, end);
    // cout << list[0] << endl;
    // cout << list.back() << endl;
    return 0;
}