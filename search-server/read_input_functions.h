#pragma once
#include "document.h"
#include "paginator.h"
#include <iostream>

#include <QString>


std::ostream& operator<<(std::ostream& out, const Document& doc);

template <typename Iterator>
std::ostream& operator<<(std::ostream& out, IteratorRange<Iterator> page) {
    for (auto doc = page.begin(); doc != page.end(); ++doc) {
        out << *doc;
    };
    return out;
}

QString ToQStr(const Document& doc);
