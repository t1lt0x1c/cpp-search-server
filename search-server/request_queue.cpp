//Вставьте сюда своё решение из урока «Очередь запросов» темы «Стек, очередь, дек».‎
#include "request_queue.h"


using namespace std;

RequestQueue::RequestQueue(const SearchServer& search_server)
    :server(search_server),
    time(0),
    empty_querys(0)
    {
    }
vector<Document> RequestQueue::AddFindRequest(const string& raw_query, DocumentStatus status) {
        auto doc = server.FindTopDocuments(raw_query, status);
        Rec(doc.size());
        return doc;
    }
vector<Document> RequestQueue::AddFindRequest(const string& raw_query) {
        auto doc = server.FindTopDocuments(raw_query);
        Rec(doc.size());
        return doc;
    }
int RequestQueue::GetNoResultRequests() const {
        return empty_querys;
    }
void RequestQueue::Rec(int size_result) {  //кол-во результатов запроса
        ++time;
        if (time == min_in_day_) { //обнуляем время если оно достигает "полночи"
            time = 0;
        }
        if (!requests_.empty() && static_cast<uint64_t>(requests_.at(0).arrival_time) == time) { //если время прибытия первого на выход запроса равно текущему времени =>
        //этот запрос находится здесь "сутки", следовательно его нужно удалять
            if (requests_.at(0).empty_query) {  //проверка на пустоту, счетчик уменьшает общее кол-во пустых запросов
                empty_querys--;
            }
            requests_.pop_front();
        }
        QueryResult result;
        result.arrival_time = time; //обозначаем время прибытия
        if (size_result == 0) {  //проверка на пустой запрос
            result.empty_query = true; 
            empty_querys++;
        }
        requests_.push_back(result);
    }
