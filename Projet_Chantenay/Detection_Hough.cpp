#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>

using namespace cv;
using namespace std;


int main()
{
	string filename = "test.jpg";

	Mat src = imread(filename, 0);
	if (src.empty())
	{
		cout << "can not open " << filename << endl;
		return -1;
	}

	Mat dst, cdst;
	Canny(src, dst, 50, 200, 3);
	cvtColor(dst, cdst, CV_GRAY2BGR);

#if 0
	vector<Vec2f> lines;
	cv::HoughLines(dst, lines, 1, CV_PI / 180, 100, 0, 0);

	for (size_t i = 0; i < lines.size(); i++)
	{
		float rho = lines[i][0], theta = lines[i][1];
		Point pt1, pt2;
		double a = cos(theta), b = sin(theta);
		double x0 = a*rho, y0 = b*rho;
		pt1.x = cvRound(x0 + 1000 * (-b));
		pt1.y = cvRound(y0 + 1000 * (a));
		pt2.x = cvRound(x0 - 1000 * (-b));
		pt2.y = cvRound(y0 - 1000 * (a));
		line(cdst, pt1, pt2, Scalar(0, 0, 255), 3, CV_AA);

		float coef = (float)(pt2.y - pt1.y) / (float)(pt2.x - pt1.x);
		cout << "y = " << coef << "* x + " << pt1.y - coef*pt1.x << endl;
	}
#else
	vector<Vec4i> lines;
	HoughLinesP(dst, lines, 1, CV_PI / 180, 50, 50, 10);
	Mat source = imread("Photos/Photo1.jpg");

	for (size_t i = 0; i < lines.size(); i++)
	{
		Vec4i l = lines[i];
		line(cdst, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 3, CV_AA);

		// We determines the affine parameters of the equations of the lines detected
		float coefDirecteur = (float)(l[3] - l[1]) / (float)(l[2] - l[0]);
		float yOrigin = l[1] - coefDirecteur*l[0];
		cout << "y = " << coefDirecteur << "* x + " << yOrigin << endl;

		float yEnd = coefDirecteur * (source.cols - 1) + yOrigin;

		// We use two points to trace the lines
		int x1, x2, y1, y2;
		if (coefDirecteur >= 0)
		{
			if (yOrigin <= 0.f)
			{
				x1 = cvRound(-yOrigin / coefDirecteur);
				y1 = 0;
			}
			else
			{
				y1 = yOrigin;
				x1 = 0;
			}

			if (yEnd > source.rows)
			{
				y2 = source.rows - 1;
				x2 = cvRound((y2 - yOrigin) / coefDirecteur);
			}
			else
			{
				x2 = source.cols - 1;
				y2 = cvRound(coefDirecteur*x2 + yOrigin);
			}
		}

		else
		{
			if (yEnd <= 0.f)
			{
				x2 = cvRound(-yOrigin / coefDirecteur);
				y2 = 0;
			}
			else
			{
				x2 = source.cols - 1;
				y2 = coefDirecteur * x2 + yOrigin;
			}

			if (yOrigin > source.rows)
			{
				y1 = source.rows - 1;
				x1 = cvRound((y1 - yOrigin) / coefDirecteur);
			}
			else
			{
				x1 = 0;
				y1 = cvRound(coefDirecteur*x1 + yOrigin);
			}
		}

		// Trace it
		line(source, Point(x1, y1), Point(x2, y2), Scalar(0, 0, 255), 3, CV_AA);
	}
#endif
	imshow("source", src);
	imshow("detected lines", cdst);
	imshow("lines", source);



	waitKey();

	return 0;
}
