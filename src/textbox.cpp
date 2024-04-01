#include "textbox.h"
//#include "coordinate.h"
#include "colour.h"
#include <string>
#include <cmath>

#include <opencv2/core/types.hpp>
#include <vector>

using namespace cv;


Textbox::Textbox(std::vector<Point> points, std::string text)
{
    // extract the points out
    this->top_left = points[TOP_LEFT_INDEX];
    this->top_right = points[TOP_RIGHT_INDEX];
    this->bottom_left = points[BOTTOM_LEFT_INDEX];
    this->bottom_right = points[BOTTOM_RIGHT_INDEX];

    // text
    this->text = text;

    // calculate some midpoints
    this->mid_left = midpoint(this->top_left, this->bottom_left);
    this->mid_right = midpoint(this->top_right, this->bottom_right);

    // calculate center of box
    this->center = midpoint(this->mid_left, this->mid_right);

    // calculate some more stuff
    this->box_width = abs(this->mid_left.x - this->mid_right.x);
    this->box_height = abs(this->top_left.y - this->bottom_left.y);

    // get line thickness
    this->box_thickness = round(std::sqrt(SQUARE(top_left.x - bottom_left.x) + SQUARE(top_left.y - bottom_left.y)));
}


// midpoint for points instead of coordinates
Point Textbox::midpoint(Point a, Point b)
{
    return Point(round((a.x + b.x)/2), round((a.y + b.y)/2));
}

Colour Textbox::calc_text_colour(Colour background_colour)
{
    // convert to greyscale
    background_colour.greyscale();

    // get lightness (using r channel)
    int lightness = background_colour.getr();

    // choose appropriate colour 
    if(lightness < 128)
    {
        return Colour(255);
    }
    else
    {
        return Colour(0);
    }
}

void Textbox::scale(double scale_factor)
{
    this->top_left = this->top_left * scale_factor;
    this->top_right = this->top_right * scale_factor;
    this->bottom_left = this->bottom_left * scale_factor;
    this->bottom_right = this->bottom_right * scale_factor;
}

/////////////////////////// OLD CONSTRUCTORS ///////////////////////////////////
/*
Textbox::Textbox(Coordinate topleft, Coordinate topright, Coordinate bottomleft, Coordinate bottomright)
{
    this->topleft = topleft;
    this->topright = topright;
    this->bottomleft = bottomleft;
    this->bottomright = bottomright;
}

Textbox::Textbox(Coordinate topleft, Coordinate topright, Coordinate bottomleft, Coordinate bottomright, std::string text)
{
    this->topleft = topleft;
    this->topright = topright;
    this->bottomleft = bottomleft;
    this->bottomright = bottomright;

    this->text = text;
}
Textbox::Textbox(Coordinate topleft, Coordinate topright, Coordinate bottomleft, Coordinate bottomright, std::string text, Colour colour)
{
    this->topleft = topleft;
    this->topright = topright;
    this->bottomleft = bottomleft;
    this->bottomright = bottomright;

    this->text = text;
    this->colour = colour; // maybe convert to greyscale?
}


///////////////////// OLD GETTERS ///////////////////////////////

Coordinate Textbox::get_top_left()
{
    return this->topleft;
}
Coordinate Textbox::get_top_right()
{
    return this->topright;
}
Coordinate Textbox::get_bottom_left()
{
    return this->bottomleft;
}
Coordinate Textbox::get_bottom_right()
{
    return this->bottomright;
}

std::string Textbox::get_text()
{
    return this->text;
}

Colour Textbox::get_colour()
{
    return this->colour;
}*/