#include <iostream>
#include "helper.h"
#include <unordered_map>
#include <assert.h>
#include <string>
#include <set>
#include <vector>
#include <map>
#include <algorithm>
#include <fstream>
#include <queue>

using namespace std;

char ch;

const string END = "===============";
const string ENDD = "szfck" + END + "szfck" + "@@@@@";
const string OUTPUT = "../output/"; // output dir
const string URL_TABLE_PATH = OUTPUT + "url_table.txt";
const string TERM_TABLE_PATH = OUTPUT + "term_table.txt";
const string URL_CONTENT_PATH = OUTPUT + "url_content.bin";
const string PREV_OUTPUT = OUTPUT + "intermediate-output-1/";
const string CUR_OUTPUT = OUTPUT + "intermediate-output-2/";
map<string, int> url_map;
map<string, int> term_map;
vector<Url> urls;
vector<Term> terms;

//get next term from ifstream
string getTerm(string& s, int& pos) {
    string str = "";
    while (pos < (int) s.size()) {
        char ch = s[pos++];
        if (ch >= 'A' && ch <= 'Z') {
            str += ch - 'A' + 'a';
        } else if ((ch >= 'a' && ch <= 'z') || (ch >= '0' && ch <= '9')) {
            str += ch;
        } else {
            if (str != "") return str;
            continue;
        }
    }
    return str;
}

//get num with leading 0s
string getNum(int x) {
    string num = to_string(x);
    while ((int)num.size() < 5) num = '0' + num;
    return num;
}

string str(long long x) {
    return to_string(x);
}

void build(int id, Writer& url_content_writer) {
    cout << "processing: " << id << endl;
    string id_str = getNum(id);
    string input_filename = PREV_OUTPUT + id_str + ".txt";
    ifstream input_file;
    input_file.open(input_filename);

    string line = "";
    string url = "";
    map<int, vector<Doc>> words; 
    while (getline(input_file, line)) {
        if (line == "") continue;
        //get url
        url = line;
        if (url.size() < 4 || url.substr(0, 4) != "http") {
            cout << "id : " << id << " " << "wrong url : " << url << endl;
            while (true) {
                getline(input_file, line);
                if (line == ENDD) {
                    getline(input_file, line);
                    break;
                }
            }
            continue;
        }
        if (url_map.find(url) == url_map.end()) {
            url_map[url] = (int) urls.size();
        }
        int uid = url_map[url];

        map<int, int> freq; // for this doc
        // read doc

        long long content_start_byte = url_content_writer.getOffset();
        vector<int> words_id;
        while (getline(input_file, line)) {
            if (line == ENDD) {
                string len;
                getline(input_file, len);
                int length = 0;
                try {
                    length = stoi(len);
                } catch (const std::exception& e) {
                    cout << url << " " << e.what() << endl;
                    cout << len << endl;
                    break;
                }
                for (int id : words_id) {
                    url_content_writer.vwrite(id);
                }
                long long content_end_byte = url_content_writer.getOffset();
                urls.emplace_back(uid, url, length, content_start_byte, content_end_byte);
                url_map[url] = uid;
                for (auto p : freq) { // in uid
                    int tid = p.first, fre = p.second;
                    words[tid].emplace_back(uid, fre);
                }
                break;
            }

            int pos = 0;
            string term = "";
            while ((term = getTerm(line, pos)) != "") {
                if (term_map.find(term) == term_map.end()) {
                    term_map[term] = (int) terms.size();
                    terms.emplace_back(term_map[term], term);
                }
                int tid = term_map[term];
                words_id.push_back(tid);
                freq[tid]++;
            }

        }
    }

    Writer index(CUR_OUTPUT + "index-" + id_str + ".bin");
    Writer term_index(CUR_OUTPUT + "index-" + id_str + ".txt");
    for (auto& word : words) {
        long long start = index.getOffset();
        int tid = word.first;
        auto& list = word.second;
        int number = (int) list.size();
        index.vwriteList(tid, list);
        long long end = index.getOffset();
        term_index.swrite(str(tid) + " " + str(start) + " " + str(end) + " " + str(number) + '\n');
    }
    index.close();
    term_index.close();
    input_file.close();
}

int main(int argc, char *argv[]) {
    int lower = atoi(argv[1]), upper = atoi(argv[2]);
    cout << "start build index for file [" << lower << " to " << upper << ")" << endl;

    Writer url_content_writer(URL_CONTENT_PATH);
    for (int i = lower; i < upper; i++) {
        build(i, url_content_writer);
    }
    url_content_writer.close();

    cout << "write back to url_table..." << endl;

    Writer url_writer(URL_TABLE_PATH);
    for (auto url : urls) {
        url_writer.swrite(str(url.uid) + " " + url.url + " " + str(url.length) +
        + " " + str(url.content_start_byte) + " " + str(url.content_end_byte) + '\n');
    }
    url_writer.close();

    cout << "write back to term_table..." << endl;
    Writer term_writer(TERM_TABLE_PATH);
    for (auto term : terms) {
        term_writer.swrite(str(term.tid) + " " + term.term + '\n');
    }
    term_writer.close();
    return 0;
}
