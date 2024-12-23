#pragma once

#include <algorithm>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <exception>
#include <iostream>

#include <cmath>
#include <numeric>

#include "document.h"
#include "string_processing.h"


const int MAX_RESULT_DOCUMENT_COUNT = 5;
const double  DEVIATION = 1e-6;

class SearchServer {
public:
    inline static constexpr int INVALID_DOCUMENT_ID = -1;
    template<typename list>
    SearchServer(list setter) {
        SetStopWords(setter);
    }

    void SetStopWords(const std::string& text);


template<typename list>
    void SetStopWords(const list& set_words);

    void AddDocument(int document_id, const std::string& document, DocumentStatus status,
                     const std::vector<int>& ratings);
    
    
    template <typename compor>
    std::vector<Document> FindTopDocuments(const std::string& raw_query,
                                        compor filter_func) const;
 
    std::vector<Document> FindTopDocuments(const std::string& raw_query, DocumentStatus match_status) const;

    std::vector<Document> FindTopDocuments(const std::string& raw_query) const;

    int GetDocumentCount() const;

    std:: tuple<std::vector<std::string>, DocumentStatus> MatchDocument(const std::string& raw_query,
                                                                        int document_id)  const;

    int GetDocumentId(int index) const;

private:
    struct DocumentData {
        int rating;
        DocumentStatus status;
    };

    std::set<std::string> stop_words_;
    std::map<std::string, std::map<int, double>> word_to_document_freqs_;
    std::map<int, DocumentData> documents_;
    std::vector<int> set_id_;

    bool IsStopWord(const std::string& word) const;

    static bool NotSpecSymbol(const std::string& word); 

    std::vector<std::string> SplitIntoWordsNoStop(const std::string& text) const;

    static int ComputeAverageRating(const std::vector<int>& ratings);

    struct QueryWord {
        std::string data;
        bool is_minus;
        bool is_stop;
    };

    QueryWord ParseQueryWord(std::string text) const;

    struct Query {
        std::set<std::string> plus_words;
        std::set<std::string> minus_words;
    };

    Query ParseQuery(const std::string& text) const;

    // Existence required
    double ComputeWordInverseDocumentFreq(const std::string& word) const;
    
    template <typename compor>
    std::vector<Document> FindAllDocuments(const Query& query, compor func) const;     
};

//шаблонные методы
template<typename list>
    void SearchServer::SetStopWords(const list& set_words) {
        for (const std::string& word : set_words) {
            if (!word.empty()){
                if (!NotSpecSymbol(word)){
                    throw std::invalid_argument( "Присутствие спец символов недопустимо");
                }
                stop_words_.insert(word);
            }
        }
    }
  
template <typename compor>
    std::vector<Document> SearchServer::FindTopDocuments(const std::string& raw_query,
                                        compor filter_func) const {
        std::vector<Document> result;                                                         
        const Query query = ParseQuery(raw_query); 
        result = FindAllDocuments(query, filter_func);
         
        sort(result.begin(), result.end(),
             [](const Document& lhs, const Document& rhs){
                    if (std::abs(lhs.relevance - rhs.relevance) < DEVIATION){
                        return lhs.rating > rhs.rating;
                    } else {
                        return lhs.relevance > rhs.relevance;
                    }
                });
        if (result.size() > MAX_RESULT_DOCUMENT_COUNT){
            result.resize(MAX_RESULT_DOCUMENT_COUNT);
        }
        return result;
    }

template <typename compor>
    std::vector<Document> SearchServer::FindAllDocuments(const Query& query, compor func) const {
        std::map<int, double> document_to_relevance;
        for (const std::string& word : query.plus_words){
            if (word_to_document_freqs_.count(word) == 0){
                continue;
            }
            const double inverse_document_freq = ComputeWordInverseDocumentFreq(word);
            for (const auto [document_id, term_freq] : word_to_document_freqs_.at(word)){
                if (func(document_id, documents_.at(document_id).status, documents_.at(document_id).rating)){
                    document_to_relevance[document_id] += term_freq * inverse_document_freq;
                }
            }
        }

        for (const std::string& word : query.minus_words){
            if (word_to_document_freqs_.count(word) == 0){
                continue;
            }
            for (const auto &[document_id, _] : word_to_document_freqs_.at(word)){
                document_to_relevance.erase(document_id);
            }
        }

        std::vector<Document> matched_documents;
        for (const auto &[document_id, relevance] : document_to_relevance){
            matched_documents.push_back(
                {document_id, relevance, documents_.at(document_id).rating});
        }
        return matched_documents;
    }