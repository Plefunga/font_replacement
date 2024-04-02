#ifndef CONFIG_H
#define CONFIG_H 1

#include <string>
#include <opencv2/core/types.hpp>

#define USE_FREETYPE_FONT

#define N_HEIGHTS 40

//#define DEBUG

// amount to subtract from box width to create a space because sometimes those are missing
#define BOX_WIDTH_OFFSET 6

// no text replacement, only text detection
//#define NO_TEXT_REPLACEMENT

// the detected text boxes are always rotated, so apply correction to the data itself
#define CORRECT_FOR_THAT_STRANGE_ROTATION

// indexes of detResults
#define TOP_LEFT_INDEX 1
#define TOP_RIGHT_INDEX 2
#define BOTTOM_LEFT_INDEX 0
#define BOTTOM_RIGHT_INDEX 3

using namespace cv;

class Config
{
    public:
    std::string detector_model_path;
    std::string recogniser_model_path;
    std::string vocab_path;
    std::string font_path;

    int imread_RGB;
    double confidence_threshold;
    double non_max_suppression_threshold;

    int width;
    int height;
    double downsample_scale;

    double rec_scale;
    Scalar rec_mean;
    Size rec_input_size;

    double det_scale;
    Size det_input_size;
    Scalar det_mean;
    
    bool swap_RB;

    std::string kWinName;
    
    
    std::string input;


    Config(){}
    Config(std::string config_file_path);

    void set_size(int width, int height);
};


#endif