#pragma once

#include <vector>
#include <deque>
#include <string>

#include "document.h"
#include "search_server.h"

class RequestQueue {
public:
    explicit RequestQueue( const SearchServer& search_server);
    void NewRequest(const std::string& raw_query,const  std::string& filter,
                     std::vector<Document>& list_document);
    
    template <typename DocumentPredicate>
    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentPredicate document_predicate);

    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentStatus status);

    std::vector<Document> AddFindRequest(const std::string& raw_query);

    int GetNoResultRequests() const;
      
private:
    struct QueryResult {
        bool empty;
        std::string raw_query;
        std::string filter;        
    };

    std::deque<QueryResult> requests_;
    const static int min_in_day_ = 1440;
    int empty_reqest_ = 0;
    const SearchServer &server_;

    QueryResult SetQuery(bool empty, std::string raw_query, std::string filte);

    void UpdateQuery(QueryResult& elem);
}; 


//шаблонные методы
template <typename DocumentPredicate>
    std::vector<Document> RequestQueue::AddFindRequest(const std::string& raw_query, DocumentPredicate document_predicate) {
        std::vector<Document> list_document=server_.FindTopDocuments(raw_query, document_predicate);
        std::string filter = "status";
        NewRequest(raw_query, filter, list_document);
        return list_document;
    }