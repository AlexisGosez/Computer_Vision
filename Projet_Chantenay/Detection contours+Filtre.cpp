#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using namespace cv;
using namespace std;

Mat src; Mat src_gray;
Mat imageContraste;
int thresh = 200;
int max_thresh = 255;
RNG rng(12345);

/// Function header
void thresh_callback(int, void* );

/** @function main */
int main( int argc, char** argv )
{
  /// Load source image and convert it to gray
  src = imread("Photos/Photo1.JPG");
  cvtColor(src, src_gray, CV_BGR2GRAY);
  src_gray.convertTo(imageContraste, -1, 2, 10);

  // threshold
  threshold(imageContraste, imageContraste, seuil, 255, 0);

  /// Convert image to gray and blur it
  blur( src, src, Size(3,3) );

  /// Create Window
  char* source_window = "Source";
  namedWindow( source_window, CV_WINDOW_AUTOSIZE );
  imshow( source_window, src );

  // create a trackbar for canny filter settings
  createTrackbar( " Canny thresh:", "Source", &thresh, max_thresh, thresh_callback );
  thresh_callback( 0, 0 );

  waitKey(0);
  return(0);
}

/** @function thresh_callback */
void thresh_callback(int, void* )
{
  Mat canny_output;
  vector<vector<Point> > contours, contours2;
  vector<Vec4i> hierarchy;
  vector<vector<Point> > newContours;

  /// Detect edges using canny
  Canny( src, canny_output, thresh, thresh*2, 3 );
  /// Find contours
  findContours( canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

 // We only keep the longest contours
 for ( int i = 0 ; i < contours.size() ; i++)
  {
    if (contours[i].size() >= 100)
    {
      newContours.push_back(contours[i]);
    }
  }

  contours = newContours;
  newContours.clear();


  /// Draw contours
  Mat drawing = Mat::zeros( canny_output.size(), CV_8UC3 );
  for( int i = 0; i< contours.size(); i++ )
  {
     Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
     drawContours( drawing, contours, i, color, 2, 8, hierarchy, 0, Point() );
  }

  /// Show in a window
  namedWindow( "Contours", CV_WINDOW_AUTOSIZE );
  imshow( "Contours", drawing );
}
