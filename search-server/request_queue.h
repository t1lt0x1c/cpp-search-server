#pragma once
#include <iostream>
#include <deque>
#include <vector>
#include "search_server.h"
#include "document.h"


class RequestQueue {
public:
    explicit RequestQueue(const SearchServer& search_server);

    template <typename DocumentPredicate>
    std::vector<Document> AddFindRequest(const std::string& raw_query,
        DocumentPredicate document_predicate);
    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentStatus status);
    std::vector<Document> AddFindRequest(const std::string& raw_query);  
    int GetNoResultRequests() const;
private:
    struct QueryResult {
        // время прибытия, флаг пустого запроса
        int arrival_time;
        bool empty_query = false;
    };
    std::deque<QueryResult> requests_;
    const static int min_in_day_ = 1440;  //полночь/кол-во тиков в цикле
    const SearchServer& server;
    uint64_t time;   //текущее время
    int empty_querys;  //кол-во пустых запросов
       
    void Rec(int size_result);
};

template <typename DocumentPredicate>
std::vector<Document> RequestQueue::AddFindRequest(const std::string& raw_query,
        DocumentPredicate document_predicate) {
    auto doc = server.FindTopDocuments(raw_query, document_predicate);
    Rec(doc.size());
    return doc;
}