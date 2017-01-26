#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using namespace cv;
using namespace std;


/// Function header
Mat pixNoirs(Mat image, int bas, int haut);
Mat doubleSobel(Mat image, Size blurSize);
Mat supressionLignes(Mat image, int bas, int haut);
Mat supressionColonnes(Mat image, int seuil);


int main()
{
  Mat src, src_gray, blured1, blured2, mask, res, sortie;
  string filename;

  // Definition of structuring elements for dilatation and erosion
  Mat elementDilatation = getStructuringElement(MORPH_RECT, Size(4, 3), Point(0, 0));
  Mat elementErosion = getStructuringElement(MORPH_RECT, Size(2, 2), Point(0, 0));


  cout << "Entrez le nom de l'image a traiter sans extension" << endl;
  cin >> filename;


  // Image loading
  src = imread((filename+".jpg").c_str());

  // If the image can't be found
  while (src.data == NULL)
  {
	  cout << "l'image ne peut pas etre chargee" << endl;
	  cout << "Saisissez un nouveau nom" << endl;

	  cin >> filename;

	  src = imread((filename + ".jpg").c_str());
  }

  // Display of the source image
  imshow("Image Source", src);

  // Gray image conversion and blur
  cvtColor(src, src_gray, CV_BGR2GRAY);
  blur(src_gray, blured1, Size(1,1));
  medianBlur(src_gray, blured2, 7);

  // Display of the blured image
  imshow("blured", blured2.clone());


  // Black and white image according to a boundary
  // The mask created allows us to delete the lines that we don't want to recognize
  mask = pixNoirs(blured2.clone(), 0, 50);
  sortie = pixNoirs(src_gray.clone(), 0, 50);


  // color swapping of the black and white image to create the mask
  for (int i = 0; i < mask.rows; i++)
  {
	  for (int j = 0; j < mask.cols; j++)
	  {
		  mask.at<uchar>(i, j) = abs(mask.at<uchar>(i, j) - 255);
	  }
  }

  // mask applied
  sortie.copyTo(res, mask);
  imwrite("Photo1_mask.jpg", mask);

  imshow("sortie", sortie);

  // Delete columns of white piwels that we don't want to recognize
  sortie = supressionColonnes(res, 5);


  // erosions + dilatations
  for (int i = 0; i < 1; i++)
  {
	  erode(sortie.clone(), sortie, elementErosion, Point(-1, -1));
	  dilate(sortie.clone(), sortie, elementDilatation, Point(-1, -1));
  }

  // Display of the post treated image
  imshow("Post traitement", sortie.clone());
  imwrite((filename+"_PostTraitement.jpg").c_str(), sortie.clone());

  waitKey(0);
  return(0);
}


// Thresholding with a low bound and a high bound
Mat pixNoirs(Mat image, int bas, int haut)
{
	Mat sortie(image.rows, image.cols, CV_8UC1);

	for (int i = 0; i < image.rows; i++)
	{
		for (int j = 0; j < image.cols; j++)
		{
			if (image.at<uchar>(i, j) > bas && image.at<uchar>(i, j) < haut)
			{
				sortie.at<uchar>(i, j) = 255;
			}

			else sortie.at<uchar>(i, j) = 0;
		}
	}

	return sortie;
}

// Function that applies a blur and double Sobel filter to an image
Mat doubleSobel(Mat image, Size blurSize)
{
	blur(image, image, blurSize);

	Mat sortieHoriz(image.rows, image.cols, CV_8UC1);
	Mat sortieVerti(image.rows, image.cols, CV_8UC1);
	Mat sortie(image.rows, image.cols, CV_8UC1);
	Mat kernelHoriz, kernelVerti;

	kernelHoriz = (Mat_<double>(3, 3) << -1, 0, 1, -2, 0, 2, -1, 0, 1);
	kernelVerti = (Mat_<double>(3, 3) << -1, -2, -1, 0, 0, 0, 1, 2, 1);

	filter2D(image, sortieHoriz, -1, kernelHoriz);
	filter2D(image, sortieVerti, -1, kernelVerti);

	sortie = (sortieHoriz + sortieVerti)/2;

	return sortie;

}

// Function that "deletes" horizontal lines of white pixels
Mat supressionLignes(Mat image, int bas, int haut)
{

	for (int i = 0; i < image.rows; i++){


		for (int j = 0; j < image.cols; j++)
		{
			int compteur = 0;

			while (j + compteur < image.cols - 1 && image.at<uchar>(i,j + compteur) == 255)
			{
				compteur++;
			}

			if (compteur >= bas && compteur <= haut)
			{
				for (int k = 0; k < compteur; k++)
				{
					image.at<uchar>(i, j + k) = 0;
				}
			}

		}
	}

	return image;
}

// Function that "deletes" vertical lines of white pixels
Mat supressionColonnes(Mat image, int seuil)
{

	for (int i = 0; i < image.cols; i++){


		for (int j = 0; j < image.rows; j++)
		{
			int compteur = 0;

			while (j + compteur < image.rows - 1 && image.at<uchar>(j + compteur, i) == 255)
			{
				compteur++;
			}

			if (compteur >= seuil)
			{
				for (int k = 0; k < compteur; k++)
				{
					image.at<uchar>(j + k, i) = 0;
				}
			}

		}
	}

	return image;
}
