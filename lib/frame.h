#ifndef FRAME_H
#define FRAME_H 1

#include "textbox.h"
#include "config.h"
#include "colour.h"
#include <vector>

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



class Frame
{
    private:

    TextDetectionModel_EAST* detector;
    TextRecognitionModel* recogniser;
    Config* config;
#ifdef USE_FREETYPE_FONT
    Ptr<freetype::FreeType2> font;
#endif

    public:
    Mat image;

#ifdef USE_FREETYPE_FONT
    Frame(Mat image, TextDetectionModel_EAST* detector, TextRecognitionModel* recogniser, Ptr<freetype::FreeType2> font, Config* config);
#else
    Frame(Mat image, TextDetectionModel_EAST* detector, TextRecognitionModel* recogniser, Config* config);
#endif
    Frame(){};

    void replace_text();
};

void fourPointsTransform(const Mat& frame, const Point2f vertices[], Mat& result);

/**
 * Modified to do other fonts, and make it compatible with my stuff
 * @see https://stackoverflow.com/questions/50353884/calculate-text-size
*/
#ifdef USE_FREETYPE_FONT
void drawtorect(Mat & mat, Rect target, int center_y, Ptr<freetype::FreeType2> font, Scalar colour, const std::string & str);
#else
void drawtorect(Mat & mat, Rect target, int center_y, int thickness, Scalar colour, const std::string & str);
#endif

#endif