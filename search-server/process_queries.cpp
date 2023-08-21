#include "process_queries.h"


using namespace std;

std::vector<std::vector<Document>> ProcessQueries(
    const SearchServer& search_server,
    const std::vector<std::string>& queries) {
    std::vector<std::vector<Document>> result(queries.size());
    std::transform(std::execution::par, queries.begin(), queries.end(), result.begin(), [&search_server](std::string_view query) {
       return search_server.FindTopDocuments(query);
    });
    return result;
}


std::list<Document> ProcessQueriesJoined(
    const SearchServer& search_server,
    const std::vector<std::string>& queries) {
    list<Document> result;
    auto tmp = ProcessQueries(search_server, queries);
    for(auto i : tmp) {
        for (auto j : i) {
            result.push_back(j);
        }
    }
    return result;
}