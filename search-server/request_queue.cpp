#include "request_queue.h"

RequestQueue::RequestQueue(const SearchServer& search_server) 
    :server_(search_server)
    {
    }

void RequestQueue::NewRequest(const std::string& raw_query,const  std::string& filter,
                            std::vector<Document>& list_document){
    QueryResult result;
    bool empty = false;
    if (list_document.empty()){
              empty = true;  
              empty_reqest_++;   
        }
    result = SetQuery(empty, raw_query, filter);
    UpdateQuery(result);
}

std::vector<Document> RequestQueue::AddFindRequest(const std::string& raw_query, DocumentStatus status) {
        std::vector<Document> list_document = server_.FindTopDocuments(raw_query, status);
        std::string filter = "status";
        NewRequest(raw_query, filter, list_document);
        return list_document;
    }

std::vector<Document> RequestQueue::AddFindRequest(const std::string& raw_query) {            
        std::vector<Document> list_document=server_.FindTopDocuments(raw_query);
        std::string filter = "status";
        NewRequest(raw_query, filter, list_document);
        return list_document;
    }

int RequestQueue::GetNoResultRequests() const {
       return empty_reqest_;
    }

  RequestQueue::QueryResult RequestQueue::SetQuery(bool empty, std::string raw_query, std::string filte){
        QueryResult ret = {empty, raw_query, filte};
        return ret;
    }

void RequestQueue::UpdateQuery(QueryResult& elem){
        requests_.push_back(elem);
        if (requests_.size() > min_in_day_){
            if (requests_.front().empty || empty_reqest_ > 0){
                empty_reqest_--;
            }
            requests_.pop_front();
        }
    }