//Вставьте сюда своё решение из урока «Очередь запросов» темы «Стек, очередь, дек».‎
#include "read_input_functions.h"


using namespace std;

ostream& operator<<(ostream& out, const Document& doc) {
    out << "{ "s << "document_id = "s << doc.id << ", relevance = "s << doc.relevance << ", rating = "s << doc.rating << " }"s;
    return out;
}

QString ToQStr(const Document& doc) {
    QString str = "{ document_id = ";
    str += QString::number(doc.id);
    str += ", relevance = ";
    str += QString::number(doc.relevance);
    str += ", rating = ";
    str += QString::number(doc.rating);
    str += " }";

    return str;
}
