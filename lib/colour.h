#ifndef COLOUR_H
#define COLOUR_H 1

#include <string>

class Colour {
    public:
    Colour(int r, int b, int g, int a);
    Colour(int r, int b, int g);
    Colour(int L);
    Colour(int L, int a);
    Colour(){}

    void greyscale();

    int getr();
    int getb();
    int getg();

    std::string to_string();

    int r;
    int g;
    int b;
    int a;
};
#endif