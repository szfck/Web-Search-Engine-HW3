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
    // record current offset when writing
    long long offset;
    ofstream out;
public:
    Writer();
    Writer(string file_name);
    void open(string file_name);
    void close();
    long long getOffset();

    // write one integer x, compressed by varbytes
    void vwrite(int x);

    // write normal string s
    void swrite(string s);

    // write a list of Doc, compressed by varbytes
    // format: termId numberOfDoc {doc1, doc2, ... doc128} {freq1, freq2, ... freq128} {doc129 ...} {fre129 ...} ...
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

    // decompress a list of integer with varbytes in offset [start, end)
    vector<int> vread(long long start, long long end);

    // decompress a list Doc of size number with varbytes in offset[start, end)
    vector<Doc> vreadList(int tid, long long start, long long end, int number);

    // read url table
    // format: {uid, url, length, start, end} ...
    // url's content is compressed by varbytes in content.bin file, offset in [start, end)
    vector<Url> urlread();

    // read term table
    // format: {tid, term}
    vector<Term> termread();

    // read index table
    // format: {tid, start, end, numberOfDoc} ...
    // index list is compressed by varbytes in index-XXX.mergeX.bin or index-XXX.bin file, offset in [start, end)
    vector<Index> indexread();
};
