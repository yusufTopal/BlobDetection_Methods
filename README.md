# Coin-Detection-using-OpenCV
The code snippet illustrates SimpleBlobDetection, ConnectedComponentAnalysis and Contour Analysis.The main objective here is to get practical experience with the methods mentinoed.

Simple flow can be explained as follows:

* Read the source image
* Convert source image to grayscale
* Split grayscale image into 3 channels (B,G,R)
* Choose the best fit for binary image processing
* Perform thresholding and some morphological operations
* Perform simple blob detection using `cv::SimpleBlobDetector`
* Perform connected component analysis and use color mapping
* Perform contour analysis


                        Contour A

![Contour Analysis](/Results/contourAnalysis.png "Contour Analysis")
