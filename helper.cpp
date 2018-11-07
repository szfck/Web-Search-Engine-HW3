#include "helper.h"
#include <cassert>
#include <sstream>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
using namespace std;

// Writer

Writer::Writer() {
    offset = 0;
}

Writer::Writer(string file_name) : Writer() {
    open(file_name);
    offset = 0;
}

void Writer::open(string file_name) {
    out.open(file_name);
}

void Writer::close() {
    out.close();
}

long long Writer::getOffset() {
    return offset;
}

void Writer::vwrite(int x) {
    vector<int> digits;
    digits.push_back((x % 128));
    x /= 128;
    while (x) {
        digits.push_back(128 | (x % 128));
        x /= 128;
    }
    int len = (int) digits.size();
    for (int i = len - 1; i >= 0; i--) {
        out << (unsigned char)(digits[i]);
    }
    offset += len;
}

void Writer::swrite(string s) {
    out << s;
}

void Writer::vwriteList(int tid, const vector<Doc>& list) {
    vwrite(tid);
    int number = (int) list.size();
    vwrite(number);
    for (int i = 0; i < number; i += BLOCK) {
        for (int j = 0; j < BLOCK && i + j < number; j++) {
            vwrite(list[i + j].uid);
        }
        for (int j = 0; j < BLOCK && i + j < number; j++) {
            vwrite(list[i + j].freq);
        }
    }
}

// Reader

Reader::Reader() {

}

Reader::Reader(string file_name) {
    open(file_name);
}

void Reader::open(string file_name) {
    in.open(file_name);
}

void Reader::close() {
    in.close();
}

vector<int> Reader::vread(long long start, long long end) {
    in.seekg(start);
    long long len = end - start;
    string s;
    s.resize(len);
    in.read(&s[0], len);
    vector<int> arr;
    for (int i = 0; i < len; ) {
        int val = 0;
        int j = i;
        while (j < len) {
            int ch = (unsigned char)s[j++];
            val = val * 128 + (ch & 127);
            if (ch <= 127) break;
        }
        i = j;
        arr.push_back(val);
    }
    return arr;
}

vector<Doc> Reader::vreadList(int tid, long long start, long long end, int number) {
    auto list = vread(start, end);
    int n = (int) list.size();
    if (tid != list[0]) {
        cout << "tid: " << tid << " " << list[0] << endl;
    }
     assert (tid == list[0]);
    if (number != list[1]) {
        cout << "number: " << number << " " << list[1] << endl;
    }
    assert (number == list[1]);

    assert (number == (n - 2) / 2);
    assert (n % 2 == 0);
    vector<Doc> docs(number);
    int cnt = 0;
    for (int i = 2; i < n; i += 2 * BLOCK) {
        int len = min((n - i) / 2, BLOCK);
        for (int j = 0; j < len; j++) {
            docs[(i - 2) / 2 + j] = Doc(list[i + j], list[i + len + j]);
            cnt++;
        }
    }
    assert (cnt == number);
    return docs;
}

vector<Url> Reader::urlread() {
    vector<Url> urls;
    string line = "";
    while (getline(in, line)) {
        stringstream ss(line);
        int uid, length;
        string url;
        long long start, end;
        ss >> uid >> url >> length >> start >> end;
        urls.emplace_back(uid, url, length, start, end);
    }
    return urls;
}

vector<Term> Reader::termread() {
    vector<Term> terms;
    string line = "";
    while (getline(in, line)) {
        stringstream ss(line);
        int tid;
        string term;
        ss >> tid >> term;
        terms.emplace_back(tid, term);
    }
    return terms;
}

vector<Index> Reader::indexread() {
    vector<Index> indexes;
    string line = "";
    while (getline(in, line)) {
        stringstream ss(line);
        int tid;
        long long start, end;
        int number;
        ss >> tid >> start >> end >> number;
        indexes.emplace_back(tid, start, end, number);
    }
    return indexes;
}
