#include "search_server.h"
 
void SearchServer::SetStopWords(const std::string& text) {
        std::set <std::string> set_words;
        for (const std::string& word : SplitIntoWords(text)){
            set_words.insert(word);
        }
        SetStopWords(set_words);
    }
   
void SearchServer::AddDocument(int document_id, const std::string& document, DocumentStatus status,
                                const std::vector<int>& ratings) {
        if (document_id < 0){
            throw std::invalid_argument("ID документа меньше 0");
        } else if (documents_.count(document_id)){
            throw std::invalid_argument("Документ с таким id уже есть в базе");
        }

        const std::vector<std::string> words = SplitIntoWordsNoStop(document);
        for (const std::string& word : words){
            if (!NotSpecSymbol(word)){
                throw std::invalid_argument("Документ имеет запрещенные символы");
            }
        }

        set_id_.push_back(document_id);
        const double inv_word_count = 1.0 / static_cast<int>(words.size());
        for (const std::string& word : words){
            word_to_document_freqs_[word][document_id] += inv_word_count;
        }
        documents_.emplace(document_id, DocumentData{ComputeAverageRating(ratings), status});
    }
    
    std::vector<Document> SearchServer::FindTopDocuments(const std::string& raw_query, DocumentStatus match_status) const {
        auto funk_filter = [match_status](int , DocumentStatus status, int ){return match_status == status;};
        return  FindTopDocuments(raw_query, funk_filter);
    }

    std::vector<Document> SearchServer::FindTopDocuments(const std::string& raw_query) const { 
       return FindTopDocuments(raw_query, DocumentStatus::ACTUAL);
    }

    int SearchServer::GetDocumentCount() const {
        int rez=static_cast<int>(documents_.size());
        return rez;
    }

    std::tuple<std::vector<std::string>, DocumentStatus> SearchServer::MatchDocument(const std::string& raw_query,
                                                                                    int document_id)  const{   
        const Query query = ParseQuery(raw_query);
        
        std::vector<std::string> matched_words;
        for (const std::string& word : query.plus_words){
            if (word_to_document_freqs_.count(word) == 0){
                continue;
            }
            if (word_to_document_freqs_.at(word).count(document_id)){
                matched_words.push_back(word);
            }
        }
        
        for (const std::string& word : query.minus_words){
            if (word_to_document_freqs_.count(word) == 0){
                continue;
            }
            if (word_to_document_freqs_.at(word).count(document_id)){
                matched_words.clear();
                break;
            }
        }
        std::tuple<std::vector<std::string>, DocumentStatus> result = {matched_words, documents_.at(document_id).status};
        return result;
    }

    int SearchServer::GetDocumentId(int index) const {
        try
        {
            return set_id_.at(index);
        }
        catch (std::out_of_range const& exc)
        {
            throw std::out_of_range("index за границами размера списка документов ");
            return INVALID_DOCUMENT_ID;
        }
        return 0; 
    }


    bool SearchServer::IsStopWord(const std::string& word) const {
        return stop_words_.count(word) > 0;
    }

    bool SearchServer::NotSpecSymbol(const std::string& word){  
        // A valid word must not contain special characters
        return none_of(word.begin(), word.end(), [](char c) {
            return c >= '\0' && c < ' ';
        });
    }
        
    std::vector<std::string> SearchServer::SplitIntoWordsNoStop(const std::string& text) const {
        std::vector<std::string> words;
        for (const std::string& word : SplitIntoWords(text)){
            if (!IsStopWord(word)){
                words.push_back(word);
            }
        }
        return words;
    }

   int SearchServer::ComputeAverageRating(const std::vector<int>& ratings) {
        if (ratings.empty()){
            return 0;
        }
        int size=static_cast<int>(ratings.size());
        return accumulate(begin(ratings), end(ratings), 0, std::plus<int>()) / size;       
    }

    SearchServer::QueryWord SearchServer::ParseQueryWord(std::string text) const {
        bool is_minus = false;
        std::string mistake;
        // Word shouldn't be empty
        if (text[0] == '-'){
            is_minus = true;
            text = text.substr(1);
        }
        if (text.empty()){         
            throw std::invalid_argument( "index за границами размера списка документов ");
        }
        if (text[0] == '-'){
            throw std::invalid_argument( "два минуса у минус слова ");
        } 
        if (!NotSpecSymbol(text)){
            throw std::invalid_argument( "минус слово имеет недопустимые символы");
        } 
        return {text, is_minus, IsStopWord(text)};
    }

    SearchServer::Query SearchServer::ParseQuery(const std::string& text) const {
        Query query;
        for (const std::string& word : SplitIntoWords(text)){
            const QueryWord query_word = ParseQueryWord(word);
            if (!query_word.is_stop) {
                if (query_word.is_minus){
                    query.minus_words.insert(query_word.data);
                } else {
                    query.plus_words.insert(query_word.data);
                }
            }
        }
        return query;
    }

    // Existence required
    double SearchServer::ComputeWordInverseDocumentFreq(const std::string& word) const {
        return log(GetDocumentCount() * 1.0 / static_cast<int>(word_to_document_freqs_.at(word).size()));
    }
    
    



    