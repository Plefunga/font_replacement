#include "config.h"
#include <opencv2/core/types.hpp>

#include <string>
#include <iostream>

#include "iniparser.hpp"
#include <cmath>

using namespace cv;

Config::Config(std::string config_file_path)
{
    INI::File ft;
    ft.Load(config_file_path);

    // Window Settings
    this->kWinName = ft.GetSection("Window Settings")->GetValue("window_name","Text replacement").AsString();

    // Downsample Settings
    this->downsample_scale = ft.GetSection("Downsample Settings")->GetValue("downsample_scale", 1).AsDouble();

    // Input Settings
    // calculate the width and height, rounding to 32
    this->set_size(ft.GetSection("Input Settings")->GetValue("width", 1920).AsInt(), ft.GetSection("Input Settings")->GetValue("height", 1200).AsInt());
    //this->width = 32 * floor((double)ft.GetSection("Input Settings")->GetValue("width", 1920).AsInt() / (this->downsample_scale * 32.0));
    //this->height = 32 * floor((double)ft.GetSection("Input Settings")->GetValue("height", 1200).AsInt() / (this->downsample_scale * 32.0));
    
    this->imread_RGB = ft.GetSection("Input Settings")->GetValue("imread_RGB", 1).AsInt();

    // Paths
    this->detector_model_path = ft.GetSection("Paths")->GetValue("detector_model_path","resources/frozen_east_text_detection.pb").AsString();
    this->recogniser_model_path = ft.GetSection("Paths")->GetValue("recogniser_model_path","resources/crnn_cs.onnx").AsString();
    this->vocab_path = ft.GetSection("Paths")->GetValue("vocab_path","resources/alphabet_94.txt").AsString();
    this->font_path = ft.GetSection("Paths")->GetValue("font_path","resources/OpenDyslexic-Regular.otf").AsString();

    // Model Settings
    this->confidence_threshold = ft.GetSection("Model Settings")->GetValue("confidence_threshold",0.5).AsDouble();
    this->non_max_suppression_threshold = ft.GetSection("Model Settings")->GetValue("non_max_suppression_threshold",0.4).AsDouble();

    this->rec_scale = ft.GetSection("Model Settings")->GetValue("rec_scale", 1.0 / 127.5).AsDouble();

    double rec_mean_0 = ft.GetSection("Model Settings")->GetValue("rec_mean_0",127.5).AsDouble();
    double rec_mean_1 = ft.GetSection("Model Settings")->GetValue("rec_mean_1",127.5).AsDouble();
    double rec_mean_2 = ft.GetSection("Model Settings")->GetValue("rec_mean_2",127.5).AsDouble();

    int rec_input_size_0 = ft.GetSection("Model Settings")->GetValue("rec_input_size_0", 100).AsInt();
    int rec_input_size_1 = ft.GetSection("Model Settings")->GetValue("rec_input_size_1", 32).AsInt();

    this->rec_mean = Scalar(rec_mean_0, rec_mean_1, rec_mean_2);
    this->rec_input_size = Size(rec_input_size_0, rec_input_size_1);

    this->det_scale = ft.GetSection("Model Settings")->GetValue("det_scale",1.0).AsDouble();
    this->det_input_size = Size(this->width, this->height);

    double det_mean_0 = ft.GetSection("Model Settings")->GetValue("det_mean_0",123.68).AsDouble();
    double det_mean_1 = ft.GetSection("Model Settings")->GetValue("det_mean_1",116.78).AsDouble();
    double det_mean_2 = ft.GetSection("Model Settings")->GetValue("det_mean_2",103.94).AsDouble();
    this->det_mean = Scalar(det_mean_0, det_mean_1, det_mean_2);

    this->swap_RB =  ft.GetSection("Model Settings")->GetValue("swap_RB", 1).AsInt() == 1 ? true : false;
}

void Config::set_size(int width, int height)
{
    this->width = 32 * floor((double)width / (downsample_scale * 32.0));
    this->height = 32 * floor((double)height / (downsample_scale * 32.0));
    this->det_input_size = Size(this->width, this->height);

#ifdef DEBUG
    std::cout << width << " x " << height << "\n";
#endif
}