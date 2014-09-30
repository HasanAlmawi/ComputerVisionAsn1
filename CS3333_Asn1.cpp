/* Name: Hasan Almawi
 * Assignment: 1
 * Date: September 30th, 2014
 * Course: CS3333A - Introduction to Computer Vision
 * Western University
 * Professor Steven S. Beauchemin 
 */

// Compile with  g++ CS3333_Asn1.cpp `pkg-config --libs opencv`

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv/cvaux.h>

/*  GaussianFourX
 *  Author: Hasan Almawi
 *  Date of Creation: 29th September, 2014
 *  Purpose: Returns a 4 level high Gaussian pyramid for any image inputed
 *  Input Parameters: 
 *      - *in: an Image file to be read
 *      - filter: The type of Gaussian mask to be used
 *  Output Parameters:
 *      - out3: The 4 level high Gaussian pyramid for the inputed image
*/
IplImage* GaussianFourX(IplImage* in, int filter = IPL_GAUSSIAN_5x5){
    //Checking if width and height divisible by 2
    assert(in->width%2 == 0 && in->height%2 == 0);
    //Creating the image to store the 2nd level high Gaussian pyramid
    IplImage* out1 = cvCreateImage(cvSize(in->width/2, in->height/2), in->depth, in->nChannels);
    //Getting the 2nd level high Gaussian pyramid
    cvPyrDown(in, out1);
    //Creating the image to store the 3rd level high Gaussian pyramid
    IplImage* out2 = cvCreateImage(cvSize(out1->width/2, out1->height/2), out1->depth, out1->nChannels);
    //Getting the 3rd level high Gaussian pyramid
    cvPyrDown(out1, out2);
    //Creating the image to store the 4th level high Gaussian pyramid
    IplImage* out3 = cvCreateImage(cvSize(out2->width/2, out2->height/2), out2->depth, out2->nChannels);
    //Getting the 4th level high Gaussian pyramid
    cvPyrDown(out2, out3);
    //Releasing the temp image since we're done with it
    cvReleaseImage(&out1);
    //Releasing the temp image
    cvReleaseImage(&out2);
    //Returning the image
    return(out3);
};

/*  ConvertBackX
 *  Author: Hasan Almawi
 *  Date of Creation: 29th September, 2014
 *  Purpose: Returns the image size converted back to the resolution of the input images using nearest neighbor as an interpolation method
 *  Input Parameters: 
 *      - *in: an Image file to be read
 *  Output Parameters:
 *      - out3: The image converted back to the resolution of the input image.
*/
IplImage* ConvertBackX(IplImage* in){
    //Creating the image to store the image with the size of the 3rd level high Gaussian pyramid
    IplImage* out1 = cvCreateImage(cvSize(in->width*2, in->height*2), in->depth, in->nChannels);
    //Doing the resize with nearest neighbor method
    cvResize(in, out1, CV_INTER_NN);
    //Creating the image to store the image with the size of the 2nd level high Gaussian pyramid
    IplImage* out2 = cvCreateImage(cvSize(out1->width*2, out1->height*2), out1->depth, out1->nChannels);
    //Doing the resize with nearest neighbor method
    cvResize(out1, out2, CV_INTER_NN);
    //Creating the image to store the image with the size of the original image
    IplImage* out3 = cvCreateImage(cvSize(out2->width*2, out2->height*2), out2->depth, out2->nChannels);
    //Doing the resize with nearest neighbor method
    cvResize(out2, out3, CV_INTER_NN);
    //Releasing the temp image
    cvReleaseImage(&out1);
    //Releasing the temp image
    cvReleaseImage(&out2);
    //Returning the image
    return(out3);
};

/*  Main Method
 *  Author: Hasan Almawi
 *  Date of Creation: 13th September, 2014
 *  Purpose: Calls upon the other functions to draw the lines with the specified point of origin and point of termination
 *  Input Parameters: None
 *  Output Parameters: A 512x512 white window with lines drawn on it.
*/

int main(int argc, char** argv )
{
    //The first image loaded from the command line argument
    IplImage* img = cvLoadImage(argv[1]);
    //The second image loaded from the command line argument
    IplImage* img2 = cvLoadImage(argv[2]);
    //Creating the final image that will be used for the final result
    IplImage* imgFinal = cvCreateImage(cvSize(img->width,img->height), img->depth, img->nChannels);
    //Creating a window for the final result image
    cvNamedWindow("Final", CV_WINDOW_AUTOSIZE );
    
    /****** 4 Level High Gaussian Pyramid Section ******/

    //Creating an image to store the 4 level high Gaussian image of the first image
    IplImage* first= cvCreateImage(cvSize(img->width/8,img->height/8), img->depth, img->nChannels);
    //Creating an image to store the 4 level high Gaussian image of the second image
    IplImage* second= cvCreateImage(cvSize(img->width/8,img->height/8), img->depth, img->nChannels);
    //Producing the 4 level high Gaussian image of the first image
    first = GaussianFourX(img);
    //Producing the 4 level high Gaussian image of the second image
    second = GaussianFourX(img2);
    
    /****** Gray Scale Section ******/

    //Creating an image to store the grayscale of the 4 level high Gaussian image of the first image
    IplImage *imGray1 = cvCreateImage(cvGetSize(first),IPL_DEPTH_8U,1);
    //Creating an image to store the grayscale of the 4 level high Gaussian image of the second image
    IplImage *imGray2 = cvCreateImage(cvGetSize(second),IPL_DEPTH_8U,1);
    //Getting the gray scale of the first image
    cvCvtColor(first,imGray1,CV_RGB2GRAY);
    //Getting the gray scale of the second image
    cvCvtColor(second,imGray2,CV_RGB2GRAY);
    //Creating an image to store the absolute difference of the two gray scaled images
    IplImage* result = cvCreateImage(cvSize(imGray2->width,imGray2->height), imGray2->depth, imGray2->nChannels);
    //Getting the absolute difference between the gray scaled
    cvAbsDiff(imGray1, imGray2, result);

    /****** Threshold Section ******/

    //Creating an image to store the threshold image calculated
    IplImage* threshold = cvCreateImage(cvSize(result->width,result->height), result->depth, result->nChannels);
    //Getting the binary threshold of the absolute difference image
    cvThreshold(result, threshold, 75, 255, CV_THRESH_BINARY);

    /****** Resizing Image Section ******/

    //Creating an image to store the resized image
    IplImage* resizeImage = cvCreateImage(cvSize(img->width,img->height), img->depth, img->nChannels);
    //Getting the resized image
    resizeImage = ConvertBackX(threshold);

    /****** Dilate Section ******/

    //Creating an image to store the dilated image
    IplImage* dilate = cvCreateImage(cvSize(resizeImage->width,resizeImage->height), resizeImage->depth, resizeImage->nChannels);
    //Getting the dilated image with 25 iterations
    cvDilate(resizeImage, dilate, NULL, 25);

    /****** Logical And Section ******/

    //Getting the logical and of the first image and the final image using the dilate mask.
    cvAnd(img, img, imgFinal, dilate);

    //Putting the image onto the window
    cvShowImage("Final", imgFinal);
    cvWaitKey(0);
    //Releasing all the images used in the main method, since they are no longer needed
    cvReleaseImage(&img);
    cvReleaseImage(&img2);
    cvReleaseImage(&imGray1);
    cvReleaseImage(&imGray2);
    cvReleaseImage(&first);
    cvReleaseImage(&second);
    cvReleaseImage(&result);
    cvReleaseImage(&threshold);
    cvReleaseImage(&imgFinal);
    //Destroying the window that is no longer needed
    cvDestroyWindow("Final");
}