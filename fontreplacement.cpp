#include <iostream>
#include <fstream>

#include "config.h"
#include "frame.h"

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core/core.hpp> 
#include <opencv2/dnn.hpp>
#include <opencv2/photo.hpp>

#ifdef USE_FREETYPE_FONT
// from opencv_contrib
#include <opencv2/freetype.hpp>
#endif

using namespace cv;
using namespace cv::dnn;

const char* keys =
"{ help  h              | | Print help message. }"
"{ input i              | | Path to input image or video file. Skip this argument to capture frames from a camera.}";

int main(int argc, char** argv)
{
    // Parse command line arguments.
    CommandLineParser parser(argc, argv, keys);
    parser.about("Changes font from video (or still) input to OpenDyslexic-Regular");
    if (argc == 1 || parser.has("help"))
    {
        parser.printMessage();
        return 0;
    }

    if (!parser.check())
    {
        parser.printErrors();
        return 1;
    }

    Config config = Config("config.ini");

#ifdef USE_FREETYPE_FONT
    // load fonts
    Ptr<freetype::FreeType2> font;
    font = freetype::createFreeType2();
    font->loadFontData(config.font_path, 0);
#endif

    // Load networks.
    TextDetectionModel_EAST detector(config.detector_model_path);
    detector.setConfidenceThreshold(config.confidence_threshold)
        .setNMSThreshold(config.non_max_suppression_threshold);

    TextRecognitionModel recogniser(config.recogniser_model_path);

    // Load vocabulary
    std::ifstream vocFile;
    vocFile.open(samples::findFile(config.vocab_path));
    CV_Assert(vocFile.is_open());
    String vocLine;
    std::vector<String> vocabulary;
    while (std::getline(vocFile, vocLine)) {
        vocabulary.push_back(vocLine);
    }
    recogniser.setVocabulary(vocabulary);
    recogniser.setDecodeType("CTC-greedy");

    // Parameters for Recognition
    recogniser.setInputParams(config.rec_scale, config.rec_input_size, config.rec_mean);

    // Parameters for Detection
    detector.setInputParams(config.det_scale, config.det_input_size, config.det_mean, config.swap_RB);

    std::string input = "";

    if (parser.has("input"))
    {
        input = parser.get<String>("input");
    }
    else
    {
        input = "0";
    }

    // temp
    config.input = input;

    // Open a video file or an image file or a camera stream.
    VideoCapture cap;
    bool openSuccess = cap.open(config.input);
    CV_Assert(openSuccess);

    Mat frame;
    while (waitKey(1) < 0)
    {
        cap >> frame;
        if (frame.empty())
        {
            waitKey();
            break;
        }

#ifdef USE_FREETYPE_FONT
        Frame f = Frame(frame, &detector, &recogniser, font, &config);
#else
        Frame f = Frame(frame, &detector, &recogniser, &config);
#endif
        f.replace_text();

        imshow(config.kWinName, f.image);
    }
    return 0;
}