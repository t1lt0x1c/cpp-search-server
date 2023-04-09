#include <algorithm>
#include <iostream>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include <map>
#include <cmath>

using namespace std;

const int MAX_RESULT_DOCUMENT_COUNT = 5;

string ReadLine() {
    string s;
    getline(cin, s);
    return s;
}

int ReadLineWithNumber() {
    int result = 0;
    cin >> result;
    ReadLine();
    return result;
}


vector<string> SplitIntoWords(const string& text) {
    vector<string> words;
    string word;
    for (const char c : text) {
        if (c == ' ') {
            if (!word.empty()) {
                words.push_back(word);
                word.clear();
            }
        }
        else {
            word += c;
        }
    }
    if (!word.empty()) {
        words.push_back(word);
    }

    return words;
}

struct Document {
    int id;
    double relevance;
};

class SearchServer {
public:



    void SetStopWords(const string& text) {
        for (const string& word : SplitIntoWords(text)) {
            stop_words_.insert(word);
        }
    }

    void AddDocument(int document_id, const string& document) {
        ++document_count_;
        const vector<string> no_stop_words = SplitIntoWordsNoStop(document);
        for (const string& word : no_stop_words) {
            word_to_document_freqs_[word][document_id] += 1 / static_cast<double>(no_stop_words.size());
        }
    }

    vector<Document> FindTopDocuments(const string& raw_query) const {
        const Query query_words = ParseQuery(raw_query);
        auto matched_documents = FindAllDocuments(query_words);

        sort(matched_documents.begin(), matched_documents.end(),
            [](const Document& lhs, const Document& rhs) {
                return lhs.relevance > rhs.relevance;
            });
        if (matched_documents.size() > MAX_RESULT_DOCUMENT_COUNT) {
            matched_documents.resize(MAX_RESULT_DOCUMENT_COUNT);
        }
        return matched_documents;
    }

private:
    struct Query {
        set<string> plus_words;
        set<string> minus_words;
    };

    struct Qword {
        string value;
        bool minus;
        bool stop;
    };

    map<string, map<int, double>> word_to_document_freqs_;

    set<string> stop_words_;

    int document_count_ = 0;

    bool IsStopWord(const string& word) const {
        return stop_words_.count(word) > 0;
    }

    vector<string> SplitIntoWordsNoStop(const string& text) const {
        vector<string> words;
        for (const string& word : SplitIntoWords(text)) {
            if (!IsStopWord(word)) {
                words.push_back(word);
            }
        }
        return words;
    }

    Qword ParseQword(string text) const {
        Qword word;
        word.minus = false;
        if (text[0] == '-') {
            word.value = text.substr(1);
            word.minus = true;
        }
        else {
            word.value = text;
        }
        word.stop = IsStopWord(text);
        return word;
    }

    Query ParseQuery(const string& text) const {
        Query query_words;
        for (const string& word : SplitIntoWords(text)) {
            const Qword qword = ParseQword(word);
            if (!qword.stop) {
                if (qword.minus) {
                    (query_words.minus_words).insert(qword.value);
                }
                else {
                    (query_words.plus_words).insert(qword.value);
                }
            }
        }
        return query_words;
    }

    vector<Document> FindAllDocuments(const Query& query_words) const {
        vector<Document> matched_documents;
        map<int, double> res;
        for (const string& word : query_words.plus_words) {
            if (word_to_document_freqs_.count(word)) {
                double itf = log(static_cast<double>(document_count_) / static_cast<double>(word_to_document_freqs_.at(word).size()));
                for (const auto& id : word_to_document_freqs_.at(word)) {
                    res[id.first] += (itf * id.second);
                }
            }
        }
        for (const string& word : query_words.minus_words) {
            if (word_to_document_freqs_.count(word)) {
                for (const auto& id : word_to_document_freqs_.at(word)) {
                    res.erase(id.first);
                }
            }
        }
        for (const auto& w : res) {
            matched_documents.push_back({ w.first, w.second });
        }
        return matched_documents;
    }
};

SearchServer CreateSearchServer() {
    SearchServer search_server;
    search_server.SetStopWords(ReadLine());

    const int document_count = ReadLineWithNumber();
    for (int document_id = 0; document_id < document_count; ++document_id) {
        search_server.AddDocument(document_id, ReadLine());
    }

    return search_server;
}

int main() {
    const SearchServer search_server = CreateSearchServer();

    const string query = ReadLine();
    for (const auto& [document_id, relevance] : search_server.FindTopDocuments(query)) {
        cout << "{ document_id = "s << document_id << ", "
            << "relevance = "s << relevance << " }"s << endl;
    }
}