#pragma once
#include <vector>
#include <exception>


template <typename Iterator>
class Paginator {
public:
    Paginator(Iterator begin, Iterator end, int page_size)
        :begin_(begin), end_(end), pages(page_size)
    {
       SetStep();
       FullList();    
    }

    std::vector<std::vector<typename Iterator::value_type>> ReturnList(){
        return list;
    }

private:
    void FullList(){
        for(auto iter = begin_; iter != end_; iter += step){
            Iterator start = iter;
            Iterator last;
            if ((end_ - start) >= step){
                last = start + step;
                list.push_back({start, last});
            } else {
                last = end_;
                list.push_back({start, last});
                break;
            } 
        }
    }

    void SetStep(){
        if (pages <= 0){
            throw std::domain_error("Вы ввели ноль страниц некуда вывести информацию!");
        } else if (pages <= end_ - begin_){
            if ((end_ - begin_) % pages != 0){
                step = (end_ - begin_) / pages + 1;
            } else {
                step = (end_ - begin_) / pages;
            }
        } else if (pages > end_ - begin_ && end_ != begin_){
                step = 1;
        } else {
            step = 0;
        }
    }

    Iterator begin_;
    Iterator end_;
    int pages;
    int step;
    std::vector<std::vector<typename Iterator::value_type>> list;   
}; 