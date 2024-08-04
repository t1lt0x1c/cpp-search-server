#include "json_reader.h"

Json_reader::Json_reader(std::shared_ptr<SearchServer> server_, std::istream& input)
    : server(server_),
    data(json::Load(input))
{}

void Json_reader::FillServer() {
    auto& docs = data.GetRoot().AsMap().at("docs").AsArray();
    for (const auto& doc : docs) {
        auto& doc_map = doc.AsMap();
        const int id = doc_map.at("id").AsInt();
        std::string text = doc_map.at("document").AsString();
        DocumentStatus status = server.get()->StrToStatus(doc_map.at("status").AsString());

        std::vector<int> ratings;
        auto& ratings_array = doc_map.at("raiting").AsArray();
        for(auto& value: ratings_array) {
            ratings.push_back(value.AsInt());
        }
        try {
            server.get()->AddDocument(id, text, status, ratings);
        } catch (...) {
            //сообщать о невозможности добавлять документ с определенным id
            /*QString s = "Документ с id ";
            s += QString::fromStdString(std::to_string(id));
            s += " уже существует";
            QMessageBox* mess = new QMessageBox("Не добавлен документ", s);*/
        }
    }
}
