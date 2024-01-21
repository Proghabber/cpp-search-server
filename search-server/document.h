#pragma once
#include <iostream>

struct Document {

    Document() 
    :id(0), relevance(0.0), rating(0)
     {
     }

     Document(int i, double r, int rat) 
     :id(i), relevance(r), rating(rat)
     {
     }

    int id;
    double relevance;
    int rating;
};

std::ostream& operator<<(std::ostream& out, const Document& document);