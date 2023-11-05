#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>



using namespace std;

const int MAX_RESULT_DOCUMENT_COUNT = 5;

string ReadLine() {
    string s;
    getline(cin, s);
    return s;
}

int ReadLineWithNumber() {
    int result = 0;
    cin >> result;
    ReadLine();
    return result;
}

vector<string> SplitIntoWords(const string& text) {
    vector<string> words;
    string word;
    for (const char c : text) {
        if (c == ' ') {
            if (!word.empty()) {
                words.push_back(word);
                word.clear();
            }
        }
        else {
            word += c;
        }
    }
    if (!word.empty()) {
        words.push_back(word);
    }

    return words;
}

struct Document {
    int id;
    double relevance;
};


class SearchServer {
public:
    void SetStopWords(const string& text) {
        for (const string& word : SplitIntoWords(text)) {
            stop_words_.insert(word);
        }
    }

    void AddDocument(int document_id, const string& document) {
        document_count_++;
        set<string> words_doc;
        const vector<string> words = SplitIntoWordsNoStop(document);
        id_count_word[document_id] = words.size();
        for (const string word : words) {
            words_doc.insert(word);
            index_[word][document_id] += double(1.0 / words.size());
        }
        for (string word : words_doc) {
            word_count[word]++;
        }

    }

    vector<Document> FindTopDocuments(const string& raw_query) const {
        const set<string> query_words = ParseQuery(raw_query);
        auto matched_documents = FindAllDocuments(query_words);

        sort(matched_documents.begin(), matched_documents.end(),
            [](const Document& lhs, const Document& rhs) {
                return lhs.relevance > rhs.relevance;
            });
        if (matched_documents.size() > MAX_RESULT_DOCUMENT_COUNT) {
            matched_documents.resize(MAX_RESULT_DOCUMENT_COUNT);
        }
        return matched_documents;
    }

private:
    map<string, map<int, double>> index_;// name{id,tf}
    map<string, double>words_count_;// сколько этого слова в тексте
    map<int, int> id_count_words_;//сколько слов в докменте
    set<string> stop_words_;
    int document_count_ = 0;

    bool IsStopWord(const string& word) const {
        return stop_words_.count(word) > 0;
    }

    vector<string> SplitIntoWordsNoStop(const string& text) const {
        vector<string> words;
        for (const string& word : SplitIntoWords(text)) {
            if (!IsStopWord(word)) {
                words.push_back(word);
            }
        }
        return words;
    }

    set<string> ParseQuery(const string& text) const {
        set<string> query_words;
        for (const string& word : SplitIntoWordsNoStop(text)) {
            query_words.insert(word);
        }
        return query_words;
    }

    vector<Document> FindAllDocuments(const set<string>& query_words) const {
        vector<Document> matched_documents;
        map<int, double> id_tf_ide;
        /*Отменил проверку на минус слова так как в тексте оно не попадется , а также установил проверку наличия поиск-слова
        в ключах index_  чтобы не вызывать ошибку при отсутствии слова в самом тексте у метода .at()*/
        for (string word : query_words)if (!stop_words_.count(word) && index_.count(word)) {
            for (auto [id, tf] : index_.at(word)) {
                id_tf_ide[id] += tf * log(documents_count_ / words_count.at(word));
            }
        }

        for (auto [id, relevance] : id_tf_ide) {
            matched_documents.push_back({ id, relevance });
        }

        return matched_documents;
    }


};

SearchServer CreateSearchServer() {
    SearchServer search_server;
    search_server.SetStopWords(ReadLine());

    const int document_count = ReadLineWithNumber();
    for (int document_id = 0; document_id < document_count; ++document_id) {
        search_server.AddDocument(document_id, ReadLine());
    }

    return search_server;
}

int main() {
    const SearchServer search_server = CreateSearchServer();

    const string query = ReadLine();
    for (const auto& [document_id, relevance] : search_server.FindTopDocuments(query)) {
        cout << "{ document_id = "s << document_id << ", "
            << "relevance = "s << relevance << " }"s << endl;
    }
    return 0;
}
