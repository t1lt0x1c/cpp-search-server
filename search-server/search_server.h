#pragma once

#include "log_libs/log_duration.h"
#include "document.h"
#include "string_processing.h"
#include "concurrent_map.h"

#include <iostream>
#include <algorithm>
#include <vector>
#include <set>
#include <map>
#include <tuple>
#include <cmath>
#include <numeric>
#include <execution>
#include <list>
#include <deque>




const int MAX_RESULT_DOCUMENT_COUNT = 5;

enum class DocumentStatus {
    ACTUAL,
    IRRELEVANT,
    BANNED,
    REMOVED,
};

class SearchServer {
public:
    
    template <typename StringContainer>
    explicit SearchServer(const StringContainer& stop_words)
        : stop_words_(MakeUniqueNonEmptyStrings(stop_words))  // Extract non-empty stop words
    {
        using namespace std;  //использовал, чтобы написать строку с литералом s
        if (!all_of(stop_words_.begin(), stop_words_.end(), IsValidWord)) {
            throw invalid_argument("Some of stop words are invalid"s);     
        }
    }

    explicit SearchServer(const std::string& stop_words_text);
    
    explicit SearchServer(const std::string_view& stop_words_text);
    //SearchServer& operator=(const SearchServer& server);
    
    explicit SearchServer();

    void AddDocument(int document_id, const std::string_view& document, DocumentStatus status,
                     const std::vector<int>& ratings);
    
    //std::vector<Document> FindTopDocuments(const std::string_view& raw_query) const;
    
    std::vector<Document> FindTopDocuments(const std::string_view& raw_query, DocumentStatus status = DocumentStatus::ACTUAL) const;
    
    template <typename DocumentPredicate>
    std::vector<Document> FindTopDocuments(const std::string_view& raw_query,
        DocumentPredicate document_predicate) const;


    template <typename ExecutionPolicy, typename DocumentPredicate>
    std::vector<Document> FindTopDocuments(ExecutionPolicy&& policy, const std::string_view& raw_query,
        DocumentPredicate document_predicate) const;  
    
    template <typename ExecutionPolicy>
    std::vector<Document> FindTopDocuments(ExecutionPolicy&& policy, const std::string_view& raw_query,  
                                                         DocumentStatus status = DocumentStatus::ACTUAL) const;
    
    //std::vector<Document> FindTopDocuments(const std::string& raw_query) const ;

    int GetDocumentCount() const ;

    //int GetDocumentId(int index) const ;
    
    std::set<int>::const_iterator begin();
    
    std::set<int>::const_iterator end();

    
    std::tuple<std::vector<std::string_view>, DocumentStatus> MatchDocument(const std::string_view& raw_query, 
                                                        int document_id) const ;
    std::tuple<std::vector<std::string_view>, DocumentStatus> MatchDocument(std::execution::sequenced_policy, 
                                                                       const std::string_view& raw_query, int document_id) const ;
    std::tuple<std::vector<std::string_view>, DocumentStatus> MatchDocument(std::execution::parallel_policy, 
                                                                       const std::string_view& raw_query, int document_id) const ;
    void RemoveDocument(int document_id);
    void RemoveDocument(std::execution::sequenced_policy, int document_id);
    void RemoveDocument(std::execution::parallel_policy, int document_id);
    const std::map<std::string_view, double>& GetWordFrequencies(int document_id) const;

    DocumentStatus StrToStatus(const std::string& status) const {
        return str_to_status.at(status);
    }
    
private:
    struct DocumentData {
        int rating;
        DocumentStatus status;
    };
    const std::set<std::string, std::less<>> stop_words_; 
    std::map<int,std::map<std::string_view, double>> document_to_word_freqs_; //замена
    std::map<std::string_view, std::map<int, double>> word_to_document_freqs_;
    std::map<int, DocumentData> documents_;
    std::set<int> document_ids_;
    std::deque<std::string> storage;

    const std::map<std::string, DocumentStatus> str_to_status = {{"ACTUAL"s, DocumentStatus::ACTUAL},
                                                                {"IRRELEVANT"s,DocumentStatus::IRRELEVANT},
                                                                {"BANNED"s,DocumentStatus::BANNED},
                                                                 {"REMOVED"s,DocumentStatus::REMOVED}};
    

    bool IsStopWord(const std::string_view& word) const ;

    static bool IsValidWord(const std::string_view& word) ;

    std::vector<std::string_view> SplitIntoWordsNoStop(const std::string_view& text) const ;

    static int ComputeAverageRating(const std::vector<int>& ratings) ;

    struct QueryWord {
        std::string_view data;
        bool is_minus;
        bool is_stop;
    };

    QueryWord ParseQueryWord(const std::string_view& text) const ;

    struct Query {
        std::vector<std::string_view> plus_words;
        std::vector<std::string_view> minus_words;
    };

    Query ParseQuery(const std::string_view& text, const bool is_sort) const ;

    // Existence required
    double ComputeWordInverseDocumentFreq(const std::string_view& word) const ;

    template <typename DocumentPredicate>
    std::vector<Document> FindAllDocuments(const std::execution::sequenced_policy&, const Query& query,
        DocumentPredicate document_predicate) const;
    
    template <typename DocumentPredicate>
    std::vector<Document> FindAllDocuments(const std::execution::parallel_policy&, const Query& query,
        DocumentPredicate document_predicate) const;
};

template <typename DocumentPredicate> 
std::vector<Document> SearchServer::FindTopDocuments(const std::string_view& raw_query,
    DocumentPredicate document_predicate) const {
    return SearchServer::FindTopDocuments(std::execution::seq, raw_query, document_predicate);
}

template <typename ExecutionPolicy>
std::vector<Document> SearchServer::FindTopDocuments(ExecutionPolicy&& policy, const std::string_view& raw_query,  DocumentStatus status) const {
        return FindTopDocuments(policy,
            raw_query, [status]([[maybe_unused]]int document_id, DocumentStatus document_status, [[maybe_unused]]int rating) {
                return document_status == status;
            });
    }       

template <typename ExecutionPolicy, typename DocumentPredicate>
std::vector<Document> SearchServer::FindTopDocuments(ExecutionPolicy&& policy, const std::string_view& raw_query,
    DocumentPredicate document_predicate) const {
    const auto query = ParseQuery(raw_query, true);

    auto matched_documents = FindAllDocuments(policy, query, document_predicate);

    sort(matched_documents.begin(), matched_documents.end(),
        [](const Document& lhs, const Document& rhs) {
            if (std::abs(lhs.relevance - rhs.relevance) < std::numeric_limits<double>::epsilon()) {
                return lhs.rating > rhs.rating;
            }
            else {
                return lhs.relevance > rhs.relevance;
            }
        });
    if (matched_documents.size() > MAX_RESULT_DOCUMENT_COUNT) {
        matched_documents.resize(MAX_RESULT_DOCUMENT_COUNT);
    }

    return matched_documents;
}

template <typename DocumentPredicate>
    std::vector<Document> SearchServer::FindAllDocuments(const std::execution::sequenced_policy&, const Query& query,
                                      DocumentPredicate document_predicate) const {
        std::map<int, double> document_to_relevance;
        for (const std::string_view& word : query.plus_words) {
            if (word_to_document_freqs_.count(word) == 0) {
                continue;
            }
            const double inverse_document_freq = ComputeWordInverseDocumentFreq(word);
            for (const auto [document_id, term_freq] : word_to_document_freqs_.at(word)) {
                const auto& document_data = documents_.at(document_id);
                if (document_predicate(document_id, document_data.status, document_data.rating)) {
                    document_to_relevance[document_id] += term_freq * inverse_document_freq;
                }
            }
        }
         
        for (const std::string_view& word : query.minus_words) {
            if (word_to_document_freqs_.count(word) == 0) {
                continue;
            }
            for (const auto [document_id, _] : word_to_document_freqs_.at(word)) {
                document_to_relevance.erase(document_id);
            }
        }
    
        std::vector<Document> matched_documents;
        for (const auto [document_id, relevance] : document_to_relevance) {
            matched_documents.push_back(
                {document_id, relevance, documents_.at(document_id).rating});
        }
        return matched_documents;
    }

template <typename DocumentPredicate>
    std::vector<Document> SearchServer::FindAllDocuments(const std::execution::parallel_policy&, const Query& query,
                                      DocumentPredicate document_predicate) const {
        //std::map<int, double> document_to_relevance;
        ConcurrentMap<int, double> document_to_relevance(15);
        /*for (const std::string_view& word : query.plus_words) {
            if (word_to_document_freqs_.count(word) == 0) {
                continue;
            }
            const double inverse_document_freq = ComputeWordInverseDocumentFreq(word);
            for (const auto [document_id, term_freq] : word_to_document_freqs_.at(word)) {
                const auto& document_data = documents_.at(document_id);
                if (document_predicate(document_id, document_data.status, document_data.rating)) {
                    document_to_relevance[document_id] += term_freq * inverse_document_freq;
                }
            }
        }*/
    
        for_each(std::execution::par, query.plus_words.begin(), query.plus_words.end(),[this, &document_predicate, &document_to_relevance](auto& word){
            if (word_to_document_freqs_.count(word)) {
                const double inverse_document_freq = ComputeWordInverseDocumentFreq(word);
                const auto& freqs_ = word_to_document_freqs_.at(word);
                for_each(std::execution::par, freqs_.begin(), freqs_.end(), [&](const auto id_fr){
                    const auto& document_data = documents_.at(id_fr.first);
                    if (document_predicate(id_fr.first, document_data.status, document_data.rating)) {
                        document_to_relevance[id_fr.first].ref_to_value += id_fr.second * inverse_document_freq;
                        
                    }        
                });
            }
        });
        
        /*for (const std::string_view& word : query.minus_words) {
            if (word_to_document_freqs_.count(word) == 0) {
                continue;
            }
            for (const auto [document_id, _] : word_to_document_freqs_.at(word)) {
                document_to_relevance.erase(document_id);
            }
        }*/
    
        for_each(std::execution::par, query.minus_words.begin(), query.minus_words.end(), [this, &document_to_relevance](const auto word){
            if (word_to_document_freqs_.count(word)) {
                const auto freqs_ = word_to_document_freqs_.at(word);
                for_each(std::execution::par, freqs_.begin(), freqs_.end(), [&](const auto id_fr){
                    document_to_relevance.Erase(id_fr.first);
                });
            }
        });
        std::map<int, double> document_to_relevance_full = document_to_relevance.BuildOrdinaryMap();
        std::vector<Document> matched_documents;
        for (const auto [document_id, relevance] : document_to_relevance_full) {
            matched_documents.push_back(
                {document_id, relevance, documents_.at(document_id).rating});
        }
        return matched_documents;
    }

void AddDocument(SearchServer& search_server, int document_id, 
                     const std::string_view& document, DocumentStatus status, const std::vector<int>& ratings);
