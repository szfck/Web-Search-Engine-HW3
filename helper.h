#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <unordered_map>
using namespace std;

const int BLOCK = 128;

struct Url {
    int uid;
    string url;
    int length;
    long long content_start_byte, content_end_byte;

    Url(int uid, string url, int length, long long content_start_byte, long long content_end_byte) : uid(
            uid), url(url), length(length), content_start_byte(content_start_byte), content_end_byte(
            content_end_byte) {}

};

struct Term {
    int tid;
    string term;
    Term() {}
    Term(int tid, string term) : 
        tid(tid), term(term) {}
};

struct Index {
    int tid;
    long long start, end;
    int number;
    Index() {}
    Index(int tid, long long int start, long long int end, int number) :
        tid(tid), start(start), end(end), number(number) {}
};

struct Doc {
    int uid, freq;
    Doc() {}
    Doc(int uid, int freq) : uid(uid), freq(freq) {}
};

class Writer {
protected:
    long long offset;
    ofstream out;
public:
    Writer();
    Writer(string file_name);
    void open(string file_name);
    void close();
    long long getOffset();
    void vwrite(int x);
    void swrite(string s);
    void vwriteList(int tid, const vector<Doc>& list);
};

class Reader {
protected:
    ifstream in;
public:
    Reader();
    Reader(string file_name);
    void open(string file_name);
    void close();

    vector<int> vread(long long start, long long end);
    vector<Doc> vreadList(int tid, long long start, long long end, int number);
    vector<Url> urlread();
    vector<Term> termread();
    vector<Index> indexread();
};
