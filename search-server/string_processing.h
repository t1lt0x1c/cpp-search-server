#pragma once
#include <string>
#include <vector>
#include <set>
#include <iostream>

#include <QString>


std::string ReadLine();
int ReadLineWithNumber();
std::vector<std::string> SplitIntoWords(const std::string& text);
template <typename StringContainer>
std::set<std::string, std::less<>> MakeUniqueNonEmptyStrings(const StringContainer& strings) {
    std::set<std::string, std::less<>> non_empty_strings;
    for (const std::string& str : strings) {
        if (!str.empty()) {
            non_empty_strings.insert(str);
        }
    }
    return non_empty_strings;
}
std::vector<std::string_view> SplitIntoWords(const std::string_view& str);
std::vector<int> SplitRating(QString str);
