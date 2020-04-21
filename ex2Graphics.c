#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <iostream>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

Mat image;
Mat blurredOutput;
const int blur_slider_max = 100;
int blur_slider;
double blurAmount;
Mat difference;

void on_trackbar(int, void*) {
    if (blur_slider % 2 == 0){blur_slider += 1;};   
   GaussianBlur( image, blurredOutput, Size(blur_slider, blur_slider), 0);
   imshow( "Display window", blurredOutput );
   imwrite( "/Outputimages/blurredOutput.jpg", blurredOutput );
}

Mat drawHist (Mat hist, int scale){
    double mx = 0;
    minMaxLoc(hist, 0, &mx, 0, 0);

    Mat result = Mat::zeros(256 * scale, 256 * scale, CV_8UC1);


    for (int i = 0; i < 255; i++){
        // Get the histogram values
        float histValue = hist.at<float>(i, 0);
        float nextValue = hist.at<float>(i+1, 0);

        // create 4 points for the polys
        Point p1 = Point(i * scale, 256 * scale);
        Point p2 = Point(i * scale + scale, 256 * scale);
        Point p3 = Point(i * scale + scale, (256 -nextValue*256/mx) * scale);
        Point p4 = Point(i * scale, (256 - nextValue *256/mx) * scale);
        // Draw the poly (Ending in p1)
        int numPoints = 5;
        Point points[] = {p1, p2, p3, p4, p1};
        fillConvexPoly(result, points, numPoints, Scalar::all(256), 0, 0);
    }
    return result;
}

int main (int argc, char ** argv){
    // load the image in and make sure it is greyscale.
    image = imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE);

    // Images are greyscale when the R == G == B

    
    if (! image.data){
        cout << "Check its valid pls" << endl ;    
        return -1;
    }

    // create window and keep ratio of whatever is in window
    namedWindow("Display window", CV_WINDOW_KEEPRATIO);
    // give the slider a name
    char TrackbarName[50];
    sprintf( TrackbarName, "blur x %d", blur_slider_max );
    // create the slider and the method which it calls
    createTrackbar( TrackbarName, "Display window", &blur_slider, blur_slider_max, on_trackbar);
    on_trackbar(blur_slider, 0);

    // finding the difference between the two images
    
    waitKey(0);
    difference = image - blurredOutput + 128;
   // subtract(blurredOutput, image, difference);
    imshow("Difference between the images", difference);
    imwrite("OutputImages/differenceInImages.jpg", difference);
    waitKey(0);
    // make the histogram 
    Mat hist;
    // set up the channels
    int channels[] = {0};
    // the size of the histogram
    int histSize[] = {256};
    // range of values
    float range[] = {0, 256};
    const float* ranges[] = {range};
    // calculate the histogram
    calcHist(&image, 1, channels, Mat(), hist, 1, histSize, ranges, true, false);
    // display histogram using code given
    Mat histimg = drawHist(hist, 3);
  //  imshow("histogram", );
    // calculate mean and standard deviation
    Scalar mean, stddev;
    meanStdDev(image, mean, stddev);
    char stddevdisplay[200];
    sprintf(stddevdisplay, "The SD is: %f", stddev[0]); 
    putText(histimg, stddevdisplay, Point(1, hist.rows / 9), FONT_HERSHEY_COMPLEX_SMALL, 1.0, CV_RGB(120, 255, 255), 1);
    imshow("histogram", histimg);
    imwrite("Histogram.jpg", histimg);
    waitKey(0);

    return(0);

}

