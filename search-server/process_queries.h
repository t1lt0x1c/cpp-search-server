#pragma once

#include "document.h"
#include "search_server.h"
#include <list>

#include <execution>

std::vector<std::vector<Document>> ProcessQueries(
    const SearchServer& search_server,
    const std::vector<std::string>& queries);


std::list<Document> ProcessQueriesJoined(
    const SearchServer& search_server,
    const std::vector<std::string>& queries);