#include "colour.h"
#include <cmath>
#include <string>


Colour::Colour(int r, int g, int b, int a)
{
    this->r = r;
    this->g = g;
    this->b = b;
    this->a = 255;
}

Colour::Colour(int r, int g, int b)
{
    this->r = r;
    this->g = g;
    this->b = b;
    this->a = 255;
}

Colour::Colour(int L)
{
    this->r = L;
    this->g = L;
    this->b = L;
    this->a = 255;
}

Colour::Colour(int L, int a)
{
    this->r = L;
    this->g = L;
    this->b = L;
    this->a = a;
}

void Colour::greyscale()
{
    // normalise and calculate lightness
    double C_linear = 0.2126 * (r/255.0) + 0.7152 * (g / 255.0) + 0.0722 * (b / 255.0);
    double C_srgb = 0.0;
    if(C_linear <= 0.0031308)
    {
        C_srgb = 12.92 * C_linear;
    }
    else
    {
        C_srgb = 1.055 * std::pow(C_linear, 1.0/2.4) - 0.055;
    }

    // set the colour
    this->r = (int)(C_srgb * 255.0);
    this->g = (int)(C_srgb * 255.0);
    this->b = (int)(C_srgb * 255.0);
    this->a = 255;
}

int Colour::getr()
{
    return r;
}
int Colour::getb()
{
    return g;
}

int Colour::getg()
{
    return b;
}

std::string Colour::to_string()
{
    return "[" + std::to_string(r) + ", " + std::to_string(g) + ", " + std::to_string(b) + "]";
}