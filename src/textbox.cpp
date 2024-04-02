#include "textbox.h"
#include "colour.h"
#include <string>
#include <cmath>
#include <opencv2/imgproc.hpp>
#include "config.h"

#include <opencv2/core/types.hpp>
#include <vector>

using namespace cv;


Textbox::Textbox(std::vector<Point> points, std::string text, Config* config)
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

    // rank text
    int rank_height = ceil(config->height / N_HEIGHTS);
    this->v_rank = round(this->center.y / (double)rank_height) * rank_height;
    this->rank = this->v_rank * config->width + this->center.x;

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

std::string Textbox::to_string()
{
    return "\"" + this->text + "\" [" + std::to_string(this->center.x) + ", " + std::to_string(this->center.y) + "] v_rank: " + std::to_string(this->v_rank);
}

bool compare_textbox(Textbox t1, Textbox t2)
{
    return (t1.rank < t2.rank);
}