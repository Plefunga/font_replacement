#include "frame.h"

#include "config.h"
#include "colour.h"

#include "textbox.h"
#include <vector>

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core/core.hpp> 
#include <opencv2/dnn.hpp>
#include <opencv2/photo.hpp>
//#include <omp.h>

#ifdef USE_FREETYPE_FONT
// from opencv_contrib
#include <opencv2/freetype.hpp>
#endif

using namespace cv;
using namespace cv::dnn;

#ifdef USE_FREETYPE_FONT
Frame::Frame(Mat image, TextDetectionModel_EAST* detector, TextRecognitionModel* recogniser, Ptr<freetype::FreeType2> font, Config* config)
{
    this->image = image;
    this->detector = detector;
    this->recogniser = recogniser;
    this->config = config;
    this->font = font;
}
#else
Frame::Frame(Mat image, TextDetectionModel_EAST* detector, TextRecognitionModel* recogniser, Config* config)
{
    this->image = image;
    this->detector = detector;
    this->recogniser = recogniser;
    this->config = config;
}
#endif

void Frame::replace_text()
{
    Mat inpainted;
    // detection results
    std::vector<std::vector<Point>> detResults;

    // textboxes
    std::vector<Textbox> textboxes;


    // duplicate image for processing
    Mat image2 = this->image.clone();

    // create mask
    Mat mask = Mat::zeros(Size(image.cols,image.rows),CV_8UC1);

    // detect text
    this->detector->detect(image, detResults);

    // recognise text
    if (detResults.size() > 0)
    {
        // Text Recognition
        Mat recInput;

        // do it in the right colour space
        if (!this->config->imread_RGB)
        {
            cvtColor(image, recInput, cv::COLOR_BGR2GRAY);
        }
        else
        {
            recInput = image;
        }

        // list of contours. This seems to be a copy of detResults
        std::vector<std::vector<Point>> contours;

        // for all results
        //#pragma omp parallel for
        for (uint i = 0; i < detResults.size(); i++)
        {
            // doing something that I can't figure out
            const auto& quadrangle = detResults[i];
            CV_CheckEQ(quadrangle.size(), (size_t)4, "");

            contours.emplace_back(quadrangle);

            std::vector<Point2f> quadrangle_2f;
            for (int j = 0; j < 4; j++)
            {
                quadrangle_2f.emplace_back(quadrangle[j]);
            }

            // cropping (and rotating) to pass it through to the recogniser
            Mat cropped;
            fourPointsTransform(recInput, &quadrangle_2f[0], cropped);

            // getting the text
            std::string recognitionResult = this->recogniser->recognize(cropped);

            // draw on the mask
            if(recognitionResult.length() > 0)
            {
                Textbox tbox = Textbox(contours[i], recognitionResult);
                textboxes.push_back(tbox);

                line(mask, tbox.mid_left, tbox.mid_right, Scalar(255, 255, 255), tbox.box_thickness, LINE_4);
            }
        }
    }

    // remove text
    inpaint(image2, mask, inpainted, 7, INPAINT_NS);

    // replace text
    //#pragma omp parallel for
    for(int i = 0; i < textboxes.size(); i++)
    {
        Textbox tbox = textboxes[i];

        // get colour at center
        Vec3b c = inpainted.at<Vec3b>(tbox.center);
        Colour background_colour = Colour(c[2],c[1],c[0]); // BGR to RGB

        // set the colour of the text box
        Colour text_colour = tbox.calc_text_colour(background_colour);

        //std::cout << i << ": '" << tbox.text << "'" << tbox.top_left << " " << text_colour.to_string() << std::endl;

        Rect box = Rect(tbox.bottom_left.x, tbox.bottom_left.y,
                        tbox.box_width - 2, tbox.box_height);

#ifdef USE_FREETYPE_FONT
        drawtorect(inpainted, box, this->font, 1, Scalar(text_colour.r, text_colour.g, text_colour.b), tbox.text);
#else
        drawtorect(inpainted, box, 1, Scalar(text_colour.r, text_colour.g, text_colour.b), tbox.text);
#endif
    }

    // copy data back to image
    this->image = inpainted.clone();
}

void fourPointsTransform(const Mat& frame, const Point2f vertices[], Mat& result)
{
    const Size outputSize = Size(100, 32);

    Point2f targetVertices[4] = {
        Point(0, outputSize.height - 1),
        Point(0, 0), Point(outputSize.width - 1, 0),
        Point(outputSize.width - 1, outputSize.height - 1)
    };
    Mat rotationMatrix = getPerspectiveTransform(vertices, targetVertices);

    warpPerspective(frame, result, rotationMatrix, outputSize);
}

#ifdef USE_FREETYPE_FONT
// edited to do custom font
void drawtorect(Mat & mat, Rect target, Ptr<freetype::FreeType2> font, int thickness, Scalar colour, const std::string & str)
{
    Size rect = font->getTextSize(str, 1.0, thickness, 0);
    if(rect.width < 1 || rect.height < 1)
    {
        return;
    }
    double scalex = (double)target.width / (double)rect.width;
    double scaley = (double)target.height / (double)rect.height;
    double scale = std::min(scalex, scaley) * FONT_SCALE_MODIFIER;
    //int marginx = scale == scalex ? 0 : (int)((double)target.width * (scalex - scale) / scalex * 0.5);
    //int marginy = scale == scaley ? 0 : (int)((double)target.height * (scaley - scale) / scaley * 0.5);
    font->putText(mat, str, Point(target.x, target.y), scale, colour, thickness, LINE_8, true);
}

#else
void drawtorect(Mat & mat, Rect target, int thickness, Scalar colour, const std::string & str)
{
    Size rect = getTextSize(str, FONT_HERSHEY_COMPLEX, 1.0, thickness, 0);
    if(rect.width < 1 || rect.height < 1)
    {
        return;
    }
    double scalex = (double)target.width / (double)rect.width;
    double scaley = (double)target.height / (double)rect.height;
    double scale = std::min(scalex, scaley) * FONT_SCALE_MODIFIER;
    //int marginx = scale == scalex ? 0 : (int)((double)target.width * (scalex - scale) / scalex * 0.5);
    //int marginy = scale == scaley ? 0 : (int)((double)target.height * (scaley - scale) / scaley * 0.5);
    putText(mat, str, Point(target.x, target.y), FONT_HERSHEY_COMPLEX, scale, colour, thickness, FILLED, true);
}
#endif