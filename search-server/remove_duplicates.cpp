#include "remove_duplicates.h"

using namespace std;
 
void RemoveDuplicates(SearchServer& search_server) {
    set<vector<string_view>> unique_setwords;
    vector<int> duplicates;
     
    for (const int document_id : search_server) {
        if(!search_server.GetWordFrequencies(document_id).empty()){
            const auto& freqs = search_server.GetWordFrequencies(document_id);
            vector<string_view> words;
            words.reserve(freqs.size());
            for (const auto& [word, _] : freqs) {
                words.push_back(word);
            }   
            if (unique_setwords.count(words) > 0) {
                duplicates.push_back(document_id);
            } else {
                unique_setwords.insert(words);
            }
        }
    }
 
    for (auto it = duplicates.begin(); it != duplicates.end(); ++it) {
        search_server.RemoveDocument(*it);
        std::cout << "Found duplicate document id " << *it << endl;
    }
}