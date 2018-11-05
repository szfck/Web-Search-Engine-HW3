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
//    for (auto doc : docs) {
//        cout << doc.uid << " " << doc.freq << " ";
//    }
//    cout << endl;
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
//        while (tid > (int) indexes.size()) { // non exist tid
//            indexes.emplace_back(0, 0, 0, 0);
//        }
//        assert (tid == (int) indexes.size());
        indexes.emplace_back(tid, start, end, number);
    }
    return indexes;
}

IndexReader::IndexReader() {
    const string OUTPUT = "../output/";
    const string index_txt_path = OUTPUT + "intermediate-output-3/" + "index-00000.merge1.txt";
    const string index_bin_path = OUTPUT + "intermediate-output-3/" + "index-00000.merge1.bin";
    const string term_table_path = OUTPUT + "term_table.txt";
    const string url_table_path = OUTPUT + "url_table.txt";
    const string content_bin_path = OUTPUT + "url_content.bin";

    Reader term_table_reader(term_table_path);
    term_table = term_table_reader.termread();
    term_table_reader.close();
    for (auto term : term_table) {
        terms[term.term] = term.tid;
    }

    Reader url_table_reader(url_table_path);
    url_table = url_table_reader.urlread();
    url_table_reader.close();

    Reader index_reader(index_txt_path);
    index_table = index_reader.indexread();
    index_reader.close();

    index_bin.open(index_bin_path);
    content_bin.open(content_bin_path);
}

int IndexReader::openList(string term) {
    if (terms.find(term) == terms.end()) {
        cout << "term not found !" << endl;
        return NOT_FOUND;
    }
    int tid = terms[term];
    if ((int) index_table.size() <= tid) return NOT_FOUND;

    // cached index list
    if (cached.find(tid) != cached.end()) {
        docs = cached[tid];
    } else { // read from binary file
        docs = index_bin.vreadList(tid,
                                   index_table[tid].start,
                                   index_table[tid].end,
                                   index_table[tid].number);

    }
    pointer = 0;
    return (int) docs.size();
}

void IndexReader::closeList(string term) {
    if (terms.find(term) == terms.end()) {
        cout << "term not found !" << endl;
        return;
    }
    int tid = terms[term];
    docs.clear();
    pointer = -1;
}

int IndexReader::nextGEQ(int uid) {
    if (pointer >= (int) docs.size()) return NOT_FOUND;
    int next = docs[pointer++].uid;
    if (uid > next) {
        cout << "uid : " << uid << " next: " << next << endl;
    }
    assert (uid <= next);
    return next;
}

int IndexReader::getFreq() {
    return docs[pointer - 1].freq;
}

vector<string> IndexReader::getPayload(int uid) {
    vector<int> list = content_bin.vread(
            url_table[uid].content_start_byte,
            url_table[uid].content_end_byte);
    vector<string> words;
    for (int tid : list) {
        words.push_back(term_table[tid].term);
    }
    return words;
//    return {"hello", "world"};
}

int IndexReader::getLength(int uid) {
    return url_table[uid].length;
}

string IndexReader::getUrl(int uid) {
    return url_table[uid].url;
}

