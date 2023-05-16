// -------- Начало модульных тестов поисковой системы ----------

// Тест проверяет, что поисковая система исключает стоп-слова при добавлении документов
void TestExcludeStopWordsFromAddedDocumentContent() {
    const int doc_id = 42;
    const string content = "cat in the city"s;
    const vector<int> ratings = { 1, 2, 3 };
    {
        SearchServer server;
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        const auto found_docs = server.FindTopDocuments("in"s);
        ASSERT_EQUAL(found_docs.size(), 1u);
        const Document& doc0 = found_docs[0];
        ASSERT_EQUAL(doc0.id, doc_id);
    }

    {
        SearchServer server;
        server.SetStopWords("in the"s);
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        ASSERT_HINT(server.FindTopDocuments("in"s).empty(),
            "Stop words must be excluded from documents"s);
    }
}

/*
Разместите код остальных тестов здесь
*/
void MinusWordTest() {
    const vector<int> ratings = { 1, 2, 3 };
    SearchServer server;
    server.AddDocument(1, "I love c++"s, DocumentStatus::ACTUAL, ratings);
    server.AddDocument(2, "All love cats"s, DocumentStatus::ACTUAL, ratings);
    server.AddDocument(3, "Vera have 5 cats"s, DocumentStatus::ACTUAL, ratings);
    const auto doc__with_minus_word = server.FindTopDocuments("-love cats");
    ASSERT_EQUAL_HINT(doc__with_minus_word.size(), 1, "The number of documents found is not equivalent to the query");
}

void MatchTest() {
    const vector<int> ratings = { 1, 2, 3 };
    SearchServer server;
    string query = "have cats"s;
    string text = "Vera have 5 cats"s;
    server.AddDocument(0, text, DocumentStatus::ACTUAL, ratings);
    const auto& matchdoc = server.MatchDocument(query, 0);
    vector<string> doc = { "cats"s, "have"s };
    ASSERT_EQUAL_HINT(get<vector<string>>(matchdoc), doc, "Not found all query words from document"s);
    query = "have -cats"s;
    const auto& matchdoc_ = server.MatchDocument(query, 0);
    ASSERT_HINT(get<vector<string>>(matchdoc_).empty(), "List words not empty with minus-word"s);
}

void SortRelevanceTest() {
    const vector<int> ratings = { 1, 2, 3 };
    SearchServer server;
    server.SetStopWords("a an on the in is has been are with for from have be was"s);
    server.AddDocument(0, "a small curly guinea pig with grey hair has been found"s, DocumentStatus::ACTUAL, ratings);
    server.AddDocument(1, "a young 50 year old crocodile wants to make friends"s, DocumentStatus::ACTUAL, ratings);
    server.AddDocument(2, "a strange brown creature was seen in the box of oranges"s, DocumentStatus::ACTUAL, ratings);
    server.AddDocument(3, "a strange animal with big ears is building a house for its friends"s, DocumentStatus::ACTUAL, ratings);
    const auto found_docs = server.FindTopDocuments("cheburashka with big ears likes oranges"s);
    ASSERT_HINT((found_docs[0].id == 3 && found_docs[1].id == 2), "Not correct sorting relevance"s);
}

void CalculateRatingTest() {
    SearchServer server;
    server.SetStopWords("a an on the in is has been are with for from have be was"s);
    server.AddDocument(0, "a small curly guinea pig with grey hair has been found"s, DocumentStatus::ACTUAL, { 4, 3, 3, 8, 2, 7 });
    server.AddDocument(1, "a young 50 year old crocodile wants to make friends"s, DocumentStatus::ACTUAL, { 3, 9, 6, 4, 10, 3 });
    server.AddDocument(2, "a strange brown creature was seen in the box of oranges"s, DocumentStatus::ACTUAL, { 6, 9, 9, 3, 4, 6 });
    server.AddDocument(3, "a strange animal with big ears is building a house for its friends"s, DocumentStatus::ACTUAL, { 3, 4, 1, 1, 6, 4 });
    const auto found_docs = server.FindTopDocuments("cheburashka with big ears likes oranges"s);
    ASSERT_HINT((found_docs[0].rating == 3 && found_docs[1].rating == 6), "Not correct calculate rating"s);
}

void FilterPredicateTest() {
    SearchServer server;
    server.SetStopWords("a an on the in is has been are with for from have be was"s);
    server.AddDocument(0, "a small curly guinea pig with grey hair has been found"s, DocumentStatus::ACTUAL, { 4, 3, 3, 8, 2, 7 });
    server.AddDocument(1, "a young 50 year old crocodile wants to make friends"s, DocumentStatus::ACTUAL, { 3, 9, 6, 4, 10, 3 });
    server.AddDocument(2, "a strange brown creature was seen in the box of oranges"s, DocumentStatus::BANNED, { 6, 9, 9, 3, 4, 6 });
    server.AddDocument(3, "a strange animal with big ears is building a house for its friends"s, DocumentStatus::ACTUAL, { 3, 4, 1, 1, 6, 4 });
    const auto found_docs = server.FindTopDocuments("cheburashka with big ears likes oranges"s, [](int document_id, [[maybe_unused]] DocumentStatus status, [[maybe_unused]] int rating) { return (document_id % 2 != 0); });
    ASSERT_EQUAL_HINT(found_docs[0].id, 3, "Not correct search with predicate"s);
    const auto found_docs__ = server.FindTopDocuments("cheburashka with big ears likes oranges"s, DocumentStatus::BANNED);
    ASSERT_EQUAL_HINT(found_docs__[0].id, 2, "Not correct search with status"s);
}

void CalculateTrueRelevanceTest() {
    SearchServer server;
    server.SetStopWords("a an on the in is has been are with for from have be was"s);
    server.AddDocument(0, "a small curly guinea pig with grey hair has been found"s, DocumentStatus::ACTUAL, { 4, 3, 3, 8, 2, 7 });
    server.AddDocument(1, "a young 50 year old crocodile wants to make friends"s, DocumentStatus::ACTUAL, { 3, 9, 6, 4, 10, 3 });
    server.AddDocument(2, "a strange brown creature was seen in the box of oranges"s, DocumentStatus::ACTUAL, { 6, 9, 9, 3, 4, 6 });
    server.AddDocument(3, "a strange animal with big ears is building a house for its friends"s, DocumentStatus::ACTUAL, { 3, 4, 1, 1, 6, 4 });
    const auto found_docs = server.FindTopDocuments("small brown"s);
    ASSERT_EQUAL_HINT(found_docs[0].relevance, found_docs[1].relevance, "Relevances not equal. Not correct caltulated."s);
}
// Функция TestSearchServer является точкой входа для запуска тестов
void TestSearchServer() {
    RUN_TEST(TestExcludeStopWordsFromAddedDocumentContent);
    RUN_TEST(MinusWordTest);
    RUN_TEST(MatchTest);
    RUN_TEST(SortRelevanceTest);
    RUN_TEST(CalculateRatingTest);
    RUN_TEST(FilterPredicateTest);
    RUN_TEST(CalculateTrueRelevanceTest);
    // Не забудьте вызывать остальные тесты здесь
}

// --------- Окончание модульных тестов поисковой системы -----------