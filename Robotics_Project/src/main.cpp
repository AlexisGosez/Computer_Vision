/*  Gosez Alexis - Rolin Aubin - Gavel Florian
 *  ECN Robotics Project
 *
 *  Automatic number recognition
 *  2016-2017
 *
 */


#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <cv.h>
#include <highgui.h>
#include "opencv2/highgui/highgui.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <vector>

using namespace cv;
using namespace std;

//Header
void convertAllFiles();
void drawRectangle(string name);
double MatchingMethod(Mat img, Mat templ, int &coordX, int &coordY);
void SearchTemplates(Mat image, int &number, double &maxTemplate, int &coordX, int &coordY, int &height, int &width, string type);
vector<int> VectorSort(vector<int> nombre, vector<int> positions);
void SearchData(Mat img, double thresh, string type);
string showResult(vector<int> final, string type);

Mat img, result;

int main(int argc, char** argv)
{
    //Images conversion (gray) and rotation
    //convertAllFiles();

    //drawRectangle("gray/c3_0_gray.jpg");

    //Picture to analyze
    img = imread("gray/c3_02620_gray.jpg",1);
    cvtColor(img, img, CV_BGR2GRAY);

    //Two rectangles to select the ROI for the detection
    Rect roi_conso = Rect(570, 370, 151, 81);
    Rect roi_gps = Rect(310, 345, 96, 56);

    Mat image_GPS = img(roi_gps);
    Mat image_conso = img(roi_conso);

    //Thresholding for an easier recognition of the templates
    adaptiveThreshold( image_GPS, image_GPS, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 11, -3);
    adaptiveThreshold( image_conso, image_conso, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 13, -9);

    waitKey(0);

    double thresholdGPS = 0.75;
    double thresholdCompteur = 0.80;

    //Searching
    SearchData(image_GPS, thresholdGPS, "gps");
    SearchData(image_conso, thresholdCompteur, "compteur");


    return 0;
}


//Sorting the numbers vector to recover the original number detected
vector<int> VectorSort(vector<int> nombre, vector<int> positions)
{
    int mem, mini;
    vector<int> final;

    if (positions.size()==1)
    {
        final=nombre;
    }

    if (positions.size() == 2)
    {
        if (positions[0]<positions[1])
        {
            final=nombre;
        }

        else
        {
            final.push_back(nombre[1]);
            final.push_back(nombre[0]);
        }
    }

    if (positions.size()==3)
    {
        vector<int> positions_trie = positions;
        sort (positions_trie.begin(), positions_trie.end());

        for (int i =0; i<3 ; i++)
        {
            int temp = positions_trie[i];

            if (positions[0] == temp)
                final.push_back(nombre[0]);

            else if (positions[1] == temp)
                final.push_back(nombre[1]);

            else
                final.push_back(nombre[2]);
        }

    }

    return final;
}

//Function to return a string containing the detected number
string showResult(vector<int> final, string type)
{
    string res="";

    if (type == "gps")
    {
        for (int i=0 ; i<final.size() ; i++)
        {
            res+=to_string(final[i]);
        }
    }

    else
    {
        if (final.size() != 0)
        {
            for (int i=0 ; i<final.size()-1 ; i++)
            {
                res+=to_string(final[i]);
            }

            res += "."+to_string(final[final.size()-1]);
        }

    }

    return res;
}


//Searching the templates in the image
void SearchData(Mat img, double thresh, string type)
{
    double maxTemplate = 0;
    int number, coordX, coordY, height, width = 0;
    vector<int> positions;
    vector<int> nombre;

    imshow(type, img);

    SearchTemplates(img, number, maxTemplate, coordX, coordY, height, width, type);

    //While a template is found we search for another one
    while (maxTemplate > thresh)
    {
        nombre.push_back(number);
        positions.push_back(coordX);

        //Drawing a white rectangle to avoid detecting the same number twice
        rectangle(img, Point(coordX+width/5,coordY), Point(coordX + 4*width/5, coordY + 4*height/2), Scalar(255,255,255), -1, 4, 0 );
        imshow(type, img);
        number = coordX = coordY = height = width = 0;
        maxTemplate = 0;

        //Search for another template
        SearchTemplates(img, number, maxTemplate, coordX, coordY, height, width, type);
    }

    //We determine the detected number and print it
    nombre = VectorSort(nombre, positions);
    string res = showResult(nombre, type);

    cout << "res " << res << endl;

}



// Searching for the templates
void SearchTemplates(Mat image, int &number, double &maxTemplate, int &coordX, int &coordY,int &height, int &width, string type)
{
    double value;
    int tempX, tempY;
    string file;
    value = 0;

    for (int i=0 ; i<10 ; i++)
    {
        file = "templatesBW/" + to_string(i) + "_" + type + ".jpg";
        Mat templ = imread(file,1);
        cvtColor(templ, templ, CV_BGR2GRAY);
        value = MatchingMethod(image,templ, tempX, tempY);

        cout << value << endl;

        if (value > maxTemplate)
        {
            maxTemplate = value;
            number = i;
            height = templ.rows;
            width = templ.cols;
            coordX = tempX;
            coordY = tempY;
        }

    }

    waitKey(0);
    cout << "Meilleur résultat pour le " << number << " avec resultat " << maxTemplate << endl;
}


//Matching method from OpenCV algorithm
double MatchingMethod( Mat img, Mat templ, int &coordX, int &coordY)
{
      /// Create the result matrix
      int result_cols =  img.cols - templ.cols + 1;
      int result_rows = img.rows - templ.rows + 1;

      result.create( result_rows, result_cols, CV_32FC1 );

      /// Do the Matching and Normalize
      matchTemplate( img, templ, result, CV_TM_CCOEFF_NORMED);

      /// Localizing the best match with minMaxLoc
      double minVal; double maxVal; Point minLoc; Point maxLoc;
      Point matchLoc;

      minMaxLoc( result, &minVal, &maxVal, &minLoc, &maxLoc, Mat() );

      /// For SQDIFF and SQDIFF_NORMED, the best matches are lower values. For all the other methods, the higher the better
      matchLoc = maxLoc;
      coordX = matchLoc.x;
      coordY = matchLoc.y;

      return maxVal;
}


//Converting all files in gray with the rotation
void convertAllFiles()
{
    Mat image, gray;
    Mat rot_mat(2,3,CV_32FC1);
    string file, write;
    double angle = -7.0;

    image = imread("conso/c3_00002.jpg", 1);
    Point center = Point(image.cols/2, image.rows/2);
    rot_mat = getRotationMatrix2D(center, angle, 1);

    for (int i=2 ; i<9455 ; i++)
    {
        if (i<10)
        {
            file = "conso/c3_0000"+to_string(i)+".jpg";
            write = "gray/c3_0000"+to_string(i)+"_gray.jpg";
        }
        else if (i<100)
        {
            file = "conso/c3_000"+to_string(i)+".jpg";
            write = "gray/c3_000"+to_string(i)+"_gray.jpg";
        }
        else if (i<1000)
        {
            file = "conso/c3_00"+to_string(i)+".jpg";
            write = "gray/c3_00"+to_string(i)+"_gray.jpg";
        }
        else if (i<10000)
        {
            file = "conso/c3_0"+to_string(i)+".jpg";
            write = "gray/c3_0"+to_string(i)+"_gray.jpg";
        }

        image = imread(file, 1);
        cvtColor(image, gray, CV_BGR2GRAY);

        warpAffine(gray, gray, rot_mat, gray.size());

        imwrite (write, gray);
    }
}

//A simple function to draw the rectangles on the ROI
void drawRectangle(string name)
{
    Mat img;
    img=imread(name,1);
    Point upleft = Point (580,380);
    Point downright = Point(710,450);
    Point up2 = Point(310,345);
    Point down2 = Point (395,390);
    rectangle(img,upleft,downright,Scalar(0,255,0));
    rectangle(img,up2,down2,Scalar(0,0,255));
    imshow("Rectangle", img);
    waitKey(0);
}
