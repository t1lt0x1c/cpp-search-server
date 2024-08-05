#ifndef JSON_READER_H
#define JSON_READER_H

#include "search-server/search_server.h"
#include "json_builder.h"
#include "log_libs/log_duration.h"
#include <QMessageBox>

#include <QString>

#include <istream>

class Json_reader {
public:
    Json_reader(std::shared_ptr<SearchServer> server_, std::istream& input);
    void FillServer();
private:
    std::shared_ptr<SearchServer> server;
    const json::Document data;
};

#endif // JSON_READER_H
