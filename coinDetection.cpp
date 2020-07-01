#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <vector>

using namespace std;
using namespace cv;

Mat displayConnectedComponents(Mat &im);
int getLargestContour(vector<vector<Point>> contours);

int main()
{

    Mat image = imread("../CoinsA.png", IMREAD_COLOR);
    if (image.empty())
    {
        cout << "Error while reading image..." << endl;
        return -1;
    }
    Mat imageCopy;
    imageCopy = image.clone();

    Mat imageGray;
    cvtColor(image, imageGray, COLOR_BGR2GRAY);

    Mat imageChannels[3];
    split(imageCopy, imageChannels);

    Mat imageThresh;
    imageThresh = imageChannels[1].clone();

    threshold(imageThresh, imageThresh, 17, 255, THRESH_BINARY_INV);

    Mat kernelDilation = getStructuringElement(MORPH_RECT, Size(3, 3), Point(-1, -1));

    Mat dilatedImage;
    Mat erodedImage;

    dilate(imageThresh, dilatedImage, kernelDilation, Point(-1, -1), 5);
    imshow("Dilated Image", dilatedImage);

    Mat kernelErosion = getStructuringElement(MORPH_RECT, Size(2, 2), Point(-1, -1));
    erode(dilatedImage, erodedImage, kernelErosion, Point(-1, -1), 5);
    imshow("Eroded Image", erodedImage);

    // Blob detection
    SimpleBlobDetector::Params params;

    params.blobColor = 0; // 0 for black, 1 for white

    params.minDistBetweenBlobs = 2;

    params.filterByArea = false;

    params.filterByCircularity = true;
    params.minCircularity = 0.8;

    params.filterByConvexity = true;
    params.minConvexity = 0.8;

    params.filterByInertia = true;
    params.minInertiaRatio = 0.8;

    Ptr<SimpleBlobDetector> detector = SimpleBlobDetector::create(params);
    vector<KeyPoint> keyPoints;
    detector->detect(erodedImage, keyPoints);
    cout << "Number of detected coins is " << keyPoints.size() << endl;
    int x, y;
    int radius;
    double diameter;
    for (int i = 0; i < keyPoints.size(); i++)
    {
        KeyPoint k = keyPoints[i];
        Point keyPt;
        keyPt = k.pt;
        x = (int)keyPt.x;
        y = (int)keyPt.y;
        circle(imageCopy, Point(x, y), 5, Scalar(0, 0, 0), 2);
        diameter = k.size;
        radius = (int)diameter / 2.0;
        circle(imageCopy, Point(x, y), radius, Scalar(0, 255, 0), 4);
    }

      imshow("Simple Blob Detection", imageCopy);
      imshow("Thresholded Image", imageThresh);

    /// Connected component analysis and color map application

    Mat imLabels;
    Mat invertedImage;
    bitwise_not(erodedImage.clone(), invertedImage);
    int nComponents = connectedComponents(invertedImage, imLabels);
    imshow("Color Map", displayConnectedComponents(imLabels));
    cout << "Number of connected components " << nComponents << endl;

    //Contour analysis

    Mat conIm;
    conIm = image.clone();
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(erodedImage, contours, hierarchy, RETR_LIST, CHAIN_APPROX_SIMPLE);
    drawContours(conIm, contours, -1, Scalar(0, 0, 0), 4);
    cout << "Number of found contours is " << contours.size() << endl;
    imshow("Contours", conIm);

    for (int counter = 0; counter < contours.size(); counter++)
    {
        cout << "Area of the contour #" << counter + 1 << " " << contourArea(contours[counter], false) << " and perimeter is " << arcLength(contours[counter], true) << endl;
    }

    //Outer Contour Check
    Mat largestContour = image.clone();
    // We could perform another findContours with RETR_EXTERNAL
    /*
    contours.clear();
    hierarchy.clear();
    findContours(erodedImage,contours,hierarchy,RETR_EXTERNAL,CHAIN_APPROX_SIMPLE);
    */
    drawContours(largestContour, contours, 9, Scalar(0, 255, 0), 4);
      imshow("Outer Contour", largestContour);

    int largestContourIndex = getLargestContour(contours);
    cout << "Largest contour areas index " << largestContourIndex << " and the largest areas is " << contourArea(contours[largestContourIndex], false) << endl;
    contours.erase(contours.begin() + largestContourIndex);

    Mat removedOuterContour = image.clone();
    drawContours(removedOuterContour, contours, -1, Scalar(0, 0, 0), 4);
    imshow("Removed outercontour",removedOuterContour);

    // Finding center of mass and fitting circles around
    Mat circleFittedImage = image.clone();
    Moments M;
    Point2f center;
    int cMx, cMy;
    float enclosingCircleradius;
    for (int i = 0; i < contours.size(); i++)
    {
        M = moments(contours[i]);
        cMx = int(M.m10 / double(M.m00));
        cMy = int(M.m01) / double(M.m00);
        circle(circleFittedImage, Point(cMx, cMy), 10, Scalar(255, 0, 0), -1);
        minEnclosingCircle(contours[i], center, enclosingCircleradius);
        circle(circleFittedImage, center, enclosingCircleradius, Scalar(0, 255, 0), 2);
    }
        imshow("CoM and Enclosing Circle",circleFittedImage);
    waitKey(0);
    destroyAllWindows();
    return 0;
}
Mat displayConnectedComponents(Mat &im)
{

    Mat imLabels = im.clone();
    Point minLoc, maxLoc;
    double min, max;

    minMaxLoc(imLabels, &min, &max, &minLoc, &maxLoc); //find min max pixel values and their loc.

    imLabels = 255 * (imLabels - min) / (max - min);

    imLabels.convertTo(imLabels, CV_8U);

    Mat imColorMap;
    applyColorMap(imLabels, imColorMap, COLORMAP_JET);

    return imColorMap;
}
int getLargestContour(vector<vector<Point>> contours)
{

    int largestContourId = 0;
    double largestContourArea = 0;
    for (int i = 0; i < contours.size(); i++)
    {
        double tempArea = contourArea(contours[i]);
        if (tempArea > largestContourArea)
        {
            largestContourArea = tempArea;
            largestContourId = i;
        }
    }
    return largestContourId;
}
