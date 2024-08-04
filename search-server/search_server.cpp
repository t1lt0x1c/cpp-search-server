//Вставьте сюда своё решение из урока «Очередь запросов» темы «Стек, очередь, дек».‎
#include "search_server.h"


using namespace std;

SearchServer::SearchServer(const string& stop_words_text)
        : SearchServer(
            SplitIntoWords(stop_words_text))  // Invoke delegating constructor from string container
    {
    }
SearchServer::SearchServer(const string_view& stop_words_text)
        : SearchServer(
            SplitIntoWords(string(stop_words_text)))  // Invoke delegating constructor from string container
    {
    }
SearchServer::SearchServer()
    : SearchServer(
            SplitIntoWords(""s))
{}
 void SearchServer::AddDocument(const int document_id, const string_view& document, DocumentStatus status,
                     const vector<int>& ratings) {
     
        if ((document_id < 0) || (documents_.count(document_id) > 0)) {
            throw invalid_argument("Invalid document_id"s);
        }
        storage.emplace_back(document); 
        //auto words_view = SplitIntoWords(storage.back());
        const auto words = SplitIntoWordsNoStop(storage.back());
        const double inv_word_count = 1.0 / words.size();
        for (const string_view& word : words) {
            word_to_document_freqs_[word][document_id] += inv_word_count;
            document_to_word_freqs_[document_id][word] += inv_word_count;
        }
        documents_.emplace(document_id, DocumentData{ComputeAverageRating(ratings), status});
        document_ids_.insert(document_id);
    }

    vector<Document> SearchServer::FindTopDocuments(const string_view& raw_query, DocumentStatus status) const {
        return SearchServer::FindTopDocuments(std::execution::seq, raw_query, status);
    }

    /*vector<Document> SearchServer::FindTopDocuments(const string_view& raw_query) const {
        return SearchServer::FindTopDocuments(std::execution::seq, raw_query, DocumentStatus::ACTUAL);
    }*/

    int SearchServer::GetDocumentCount() const {
        return documents_.size();
    }

    set<int>::const_iterator SearchServer::begin() {
        return document_ids_.begin();
    }

    set<int>::const_iterator SearchServer::end() {
        return document_ids_.end();
    }

    tuple<vector<string_view>, DocumentStatus> SearchServer::MatchDocument(const string_view& raw_query,
                                                        int document_id) const {
        const auto query = ParseQuery(raw_query, true);

        vector<string_view> matched_words;
        for (const string_view& word : query.minus_words) {
            if (word_to_document_freqs_.count(word) != 0 && word_to_document_freqs_.at(word).count(document_id)) {
                return {matched_words, documents_.at(document_id).status};
            }
        }
        for (const string_view& word : query.plus_words) { 
            if (word_to_document_freqs_.at(word).count(document_id)) {
                matched_words.push_back(word);
            }
        }
        
        
        
        return {matched_words, documents_.at(document_id).status};
    }

tuple<vector<string_view>, DocumentStatus> SearchServer::MatchDocument(std::execution::sequenced_policy, 
                                                                  const string_view& raw_query, int document_id) const {
    return MatchDocument(raw_query, document_id);
}

tuple<vector<string_view>, DocumentStatus> SearchServer::MatchDocument(std::execution::parallel_policy, 
                                                                  const string_view& raw_query, int document_id) const {
    const auto query = ParseQuery(raw_query, false);
    vector<string_view> matched_words;
    
    const auto& word_freqs = document_to_word_freqs_.at(document_id);
    
    if(any_of(std::execution::par, query.minus_words.begin(), query.minus_words.end(), [&, document_id](const string_view& word){
        return word_freqs.count(word) > 0;
    })) {
        return {matched_words, documents_.at(document_id).status};
    }
    matched_words.reserve(query.plus_words.size());
    copy_if(std::execution::par, query.plus_words.begin(), query.plus_words.end(), matched_words.end(), [&, document_id](const string_view& word) {
        return word_freqs.count(word) > 0;
    });
    
    std::sort(std::execution::par, matched_words.begin(), matched_words.end());
    matched_words.erase(std::unique(std::execution::par, matched_words.begin(), matched_words.end()), matched_words.end());
    
    return {matched_words, documents_.at(document_id).status};
}

const map<string_view, double>& SearchServer::GetWordFrequencies(int document_id) const {
    if(documents_.count(document_id) > 0){
        auto it = documents_.find(document_id);
        if(it != documents_.end()){
            return document_to_word_freqs_.at(document_id);
        }
    }
    static const map<string_view, double> null;
    return null;
}

bool SearchServer::IsStopWord(const string_view& word) const {
        return stop_words_.count(word) > 0;
    }

bool SearchServer::IsValidWord(const string_view& word) {
        // A valid word must not contain special characters
        return none_of(word.begin(), word.end(), [](char c) {
            return c >= '\0' && c < ' ';
        });
    }

    vector<string_view> SearchServer::SplitIntoWordsNoStop(const string_view& text) const {
        vector<string_view> words;
        for (const string_view& word : SplitIntoWords(text)) {
            if (!IsValidWord(word)) {
                throw invalid_argument("Word "s + string(word) + " is invalid"s);
            }
            if (!IsStopWord(word)) {
                words.push_back(word);
            }
        }
        return words;
    }

int SearchServer::ComputeAverageRating(const vector<int>& ratings) {
        if (ratings.empty()) {
            return 0;
        }
        int rating_sum = std::accumulate(ratings.begin(), ratings.end(), 0);
        return rating_sum / static_cast<int>(ratings.size());
    }

SearchServer::QueryWord SearchServer::ParseQueryWord(const string_view& text) const {
        if (text.empty()) {
            throw invalid_argument("Query word is empty"s);
        }
        string_view word = text;
        bool is_minus = false;
        if (word[0] == '-') {
            is_minus = true;
            word = word.substr(1);
        }
        if (word.empty() || word[0] == '-' || !IsValidWord(word)) {
            throw invalid_argument("Query word "s + string(text) + " is invalid");
        }

        return {word, is_minus, IsStopWord(word)};
    }
SearchServer::Query SearchServer::ParseQuery(const string_view& text, const bool is_sort) const {
        Query result;
        for (const string_view& word : SplitIntoWords(text)) {
            const auto query_word = ParseQueryWord(word);
            if (!query_word.is_stop) {
                if (query_word.is_minus) {
                    result.minus_words.push_back(query_word.data);
                } else {
                    result.plus_words.push_back(query_word.data);
                }
            }
        }
        if(is_sort) {
            std::sort(result.minus_words.begin(), result.minus_words.end());
            result.minus_words.erase(std::unique(result.minus_words.begin(), result.minus_words.end()), result.minus_words.end());
            std::sort(result.plus_words.begin(), result.plus_words.end());
            result.plus_words.erase(std::unique(result.plus_words.begin(), result.plus_words.end()), result.plus_words.end());
        }
        return result;
    }
double SearchServer::ComputeWordInverseDocumentFreq(const string_view& word) const {
        return log(GetDocumentCount() * 1.0 / word_to_document_freqs_.at(word).size());
    }
void SearchServer::RemoveDocument(int document_id) { 
    for (auto& [word, freq] : document_to_word_freqs_.at(document_id)) {
        auto& freqs = word_to_document_freqs_.at(word);
        freqs.erase(document_id);
        if (freqs.empty()) {
            word_to_document_freqs_.erase(word);
        }
    }
    document_to_word_freqs_.erase(document_id);
    documents_.erase(document_id);
    document_ids_.erase(document_id);
}

void SearchServer::RemoveDocument(std::execution::sequenced_policy, int document_id) {
    SearchServer::RemoveDocument(document_id);
}

void SearchServer::RemoveDocument(std::execution::parallel_policy, int document_id) {
        if (documents_.count(document_id)) {
            vector<const string_view*> words_;
            auto& words = document_to_word_freqs_.at(document_id);
            transform(std::execution::par, words.begin(), words.end(), words_.begin(), [&](const auto& p) {
                return &(p.first);
            });
            for_each(std::execution::par, words_.begin(), words_.end(), [&, document_id](const auto& word){
                word_to_document_freqs_.at(*word).erase(document_id);
            });
            document_to_word_freqs_.erase(document_id);
            documents_.erase(document_id);
            document_ids_.erase(document_id);
        }
}    
       


void AddDocument(SearchServer& search_server, int document_id, const string_view& document, DocumentStatus status, const vector<int>& ratings) {
    search_server.AddDocument(document_id, document, status, ratings);
} 
