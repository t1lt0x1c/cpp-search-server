//Вставьте сюда своё решение из урока «Очередь запросов» темы «Стек, очередь, дек».‎
#include "read_input_functions.h"


using namespace std;

ostream& operator<<(ostream& out, const Document& doc) {
    out << "{ "s << "document_id = "s << doc.id << ", relevance = "s << doc.relevance << ", rating = "s << doc.rating << " }"s;
    return out;
}