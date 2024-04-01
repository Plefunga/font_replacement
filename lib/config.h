#ifndef CONFIG_H
#define CONFIG_H 1

#include <string>
#include <opencv2/core/types.hpp>

#define USE_FREETYPE_FONT

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
};


#endif