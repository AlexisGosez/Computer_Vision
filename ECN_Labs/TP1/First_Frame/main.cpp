/*  Gosez Alexis - Rolin Aubin
    ECN Lab 1

    Keypoints detection and tracking
    The first frame of the video is the reference
*/

#include <stdio.h>
#include <iostream>
#include <opencv2/opencv.hpp>
# include "opencv2/core/core.hpp"
# include "opencv2/features2d/features2d.hpp"
# include "opencv2/highgui/highgui.hpp"
# include "opencv2/nonfree/features2d.hpp"

using namespace cv;
using namespace std;

int main(int argc, char** argv )
{
    VideoCapture cap("video1.mp4");
    if(!cap.isOpened())
        return -1;

    Mat gray, first;
    Mat descriptors_1, descriptors_2;
    SurfDescriptorExtractor extractor;
    FlannBasedMatcher matcher;
    vector< DMatch > matches;
    double max_dist = 0; double min_dist = 100;

    namedWindow("Gray",1);


    cap >> first;
    cvtColor(first, first, CV_BGR2GRAY);

    cout << "Width = " << first.cols << "\n" <<"Height = " << first.rows << endl;

    //Detect the keypoints using SURF Detector
    int minHessian = 400;

    SurfFeatureDetector detector( minHessian );

    vector<KeyPoint> keypoints_1, keypoints_2, keypoints_filtered;

    detector.detect( first, keypoints_1 );

    //We only keep the wanted keypoints
    for (int i = 0 ; i < keypoints_1.size() ; i++)
    {
        if (norm(keypoints_1[i].pt - Point2f(110, 127)) < 84)
        {
            keypoints_filtered.push_back(keypoints_1[i]);
        }
    }

    keypoints_1 = keypoints_filtered;
    keypoints_filtered.clear();

    extractor.compute( first, keypoints_1, descriptors_1 );

    //Draw a rectangle on the considered area
    rectangle(first, Point2f(24, 44), Point2f(196, 210), Scalar(0,0,255));

    for(;;)
    {
        Mat frame;
        cap >> frame; // get a new frame from camera
        cvtColor(frame, gray, CV_BGR2GRAY);

        detector.detect( gray, keypoints_2 );

        //Calculate descriptors (feature vectors)
        extractor.compute( gray, keypoints_2, descriptors_2 );

        //Matching descriptor vectors using FLANN matcher
        matcher.match( descriptors_1, descriptors_2, matches );

        //Draw onlyScalar()  "good" matches (i.e. whose distance is less than 2*min_dist,
        //or a small arbitary value ( 0.02 ) in the event that min_dist is very small)

        std::vector< DMatch > good_matches;

        for( int i = 0; i < descriptors_1.rows; i++ )
        {
            if( matches[i].distance <= 0.18 )
            {
                good_matches.push_back( matches[i]);
            }
        }

        //Find Homography
        vector<Point2f> obj;
        vector<Point2f> scene;
        vector<Point2f> inputArray;
        vector<Point2f> outputArray;


        //The points to draw the new rectangle
        inputArray.push_back(Point2f(24, 44));
        inputArray.push_back(Point2f(24, 210));
        inputArray.push_back(Point2f(196, 210));
        inputArray.push_back(Point2f(196, 44));

        for( int i = 0; i < good_matches.size(); i++ )
        {
            //Get the keypoints from the good matches
            obj.push_back( keypoints_1[ good_matches[i].queryIdx ].pt );
            scene.push_back( keypoints_2[ good_matches[i].trainIdx ].pt );
        }

        Mat H = findHomography( obj, scene, CV_RANSAC );

        perspectiveTransform(inputArray, outputArray, H);

        //The new rectangle
        line(gray,outputArray[0], outputArray[1], Scalar(0,0,255));
        line(gray,outputArray[1], outputArray[2], Scalar(0,0,255));
        line(gray,outputArray[2], outputArray[3], Scalar(0,0,255));
        line(gray,outputArray[3], outputArray[0], Scalar(0,0,255));



        //Draw only "good" matches
        Mat img_matches;
        drawMatches( first, keypoints_1, gray, keypoints_2,
                   good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
                   vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );

        //Show detected matches
        imshow( "Good Matches", img_matches );

        //waitKey(0);

        imshow("Gray", gray);
        if(waitKey(30) >= 0) break;
    }

    return 0;
}
