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

//    Url(int uid, string url, int length) :
//    uid(uid), url(url), length(length) {}

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

//class IndexReader {
//private:
//    Reader index_bin;
////    Reader content_bin;
////    vector<Index> index_table;
//    vector<Doc> docs;
//    int pointer;
//
////    Reader content_bin;
////    vector<Term> term_table;
////    vector<Url> url_table;
////    unordered_map<string, int> terms;
////    unordered_map<int, vector<Doc>> cached;
//
//public:
//    IndexReader();
//    static const int NOT_FOUND = -1;
//
//    int openList(string term);
//    void closeList(string term);
//    int nextGEQ(int uid);
//    int getFreq();
//    int getLength(int uid);
//    string getUrl(int uid);
//    vector<string> getPayload(int uid);
////    int getDocNumber();
//};

// class TextWriter : public Writer {
//     public:
//         TextWriter() {};
//         TextWriter(string file_name);

//         void write(int key, const vector<string>& list);
// };

// class TextReader : public Reader {
//     public:
//         TextReader() {};
//         TextReader(string file_name);
//         map<int, vector<string>> read();
// };

// const int BLOCK = 128;
// class VByteWriter : public Writer {
//     private:
//         // ofstream out;
//         int offset;
//         // const int BLOCK = 128;
//     public:
//         VByteWriter() {};
//         VByteWriter(string file_name);

//         // get current offset
//         int getOffset();

//         // wirte long long int to a binary file with varbyte encoding
//         void write(int x);

//         // write tid with list of (docId, freq) pair
//         void writeList(int tid, const vector<pair<int, int>>& list);

//         // close file
//         // void close();
// };

// class VByteReader : public Reader {
//     private:
//         int offset;
//         // read bytes from binary file in range [start, end)
//         // and decode to long long vector
//         // const int BLOCK = 128;
//     public:
//         VByteReader() {};
//         VByteReader(string file_name);

//         int getOffset();

//         // int getNext(int& start);
//         vector<int> read(int start, int end);
//         vector<pair<int, int>> read(int tid, int start, int end, int number);

//         // // close file
//         // void close();
// };