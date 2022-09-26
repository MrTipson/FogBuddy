#include "CVController.h"
#include "Interact.h"
#include <stdio.h>
#include <vector>
#include <math.h>

CVController::CVController() {
	cv::Mat croppedImage = processedScreenshot();
	calibrate(croppedImage);
	findPages(croppedImage);
	croppedImage.release();
}

void CVController::calibrate(cv::Mat screen) {
	cv::Mat thresholded, morbed;
	cv::threshold(screen, thresholded, 15, 255, cv::THRESH_BINARY_INV);

	cv::Size seSize(20,20);
	cv::morphologyEx(thresholded, morbed, cv::MORPH_CLOSE, cv::getStructuringElement(cv::MORPH_ELLIPSE, seSize));
	thresholded.release();

	cv::Mat labels, stats, centroids;
	int numLabels = cv::connectedComponentsWithStats(morbed, labels, stats, centroids, 8, CV_16U);
	morbed.release();

	std::vector<int> widths;
	for (int i = 0; i < numLabels; i++)
	{
		int area = stats.at<int>(i, cv::CC_STAT_AREA);
		if (area > 300 && area < 2000)
		{
			widths.push_back(stats.at<int>(i, cv::CC_STAT_WIDTH));
		}
	}
	std::sort(widths.begin(), widths.end());
	perkWidth = widths.at((int)(1.3 * widths.size() / 2));
	printf("[Calibration]: Perk width calculated @ %d\n", perkWidth);
}

void CVController::findPages(cv::Mat screen) {
	cv::Mat thresholded, morbed;

	cv::threshold(screen, thresholded, 130, 255, cv::THRESH_BINARY);

	cv::Size seSize(20, 20);
	cv::morphologyEx(thresholded, morbed, cv::MORPH_CROSS, cv::getStructuringElement(cv::MORPH_ELLIPSE, seSize));
	thresholded.release();

	cv::Mat labels, stats, centroids;
	int numLabels = cv::connectedComponentsWithStats(morbed, labels, stats, centroids, 8, CV_16U);
	morbed.release();

	std::vector<int> topVals;
	for (int i = 0; i < numLabels; i++)
	{
		int area = stats.at<int>(i, cv::CC_STAT_AREA);
		if (area > 350 && area < 750)
		{
			topVals.push_back(stats.at<int>(i, cv::CC_STAT_TOP));
		}
	}
	std::sort(topVals.begin(), topVals.end());
	int median = topVals.at((int)(topVals.size() / 2)); // This is not a 'real' median
	for (int i = 0; i < numLabels; i++)
	{
		int top = stats.at<int>(i, cv::CC_STAT_TOP);
		if (abs(top - median) < 20)
		{
			int x = stats.at<int>(i, cv::CC_STAT_LEFT) + stats.at<int>(i, cv::CC_STAT_WIDTH) / 2;
			int y = stats.at<int>(i, cv::CC_STAT_TOP) + stats.at<int>(i, cv::CC_STAT_HEIGHT) / 2;
			pages.push_back({ x, y });
		}
	}
	printf("[Calibration]: Found %d pages\n", pages.size());
}

cv::Point CVController::findPerk(cv::Mat screen, std::string perkPath) {
	cv::Mat screenThresh;
	cv::threshold(screen, screenThresh, 100, 255, cv::THRESH_TOZERO);

	cv::Mat perk = cv::imread(perkPath, cv::IMREAD_UNCHANGED);
	cv::Mat template_, thresholded, resized;
	cv::cvtColor(perk, template_, cv::COLOR_BGR2GRAY);
	
	std::vector<cv::Mat> channels(4);
	cv::split(perk, channels);
	template_.mul(channels[3]);
	perk.release();

	cv::threshold(template_, thresholded, 100, 255, cv::THRESH_TOZERO);
	template_.release();

	cv::Size newsize(perkWidth, perkWidth);
	cv::resize(thresholded, resized, newsize);
	thresholded.release();

	cv::Mat match, matchNMS;
	cv::matchTemplate(screenThresh, resized, match, cv::TM_SQDIFF_NORMED);

	double min_val, max_val;
	cv::Point min_loc, max_loc;
	cv::minMaxLoc(match, &min_val, &max_val, &min_loc, &max_loc);

	cv::Mat screen2 = screen.clone();
	cv::Rect rect(min_loc.x, min_loc.y, perkWidth, perkWidth);
	cv::rectangle(screen2, rect, cv::Scalar(255, 255, 255));

	cv::namedWindow("Perk");
	cv::imshow("Perk", screen2);
	return { 1, 2 };
}

cv::Mat CVController::processedScreenshot() {
	cv::Mat gray, blurred, cropped;
	cv::Mat screen = captureScreenshot();
	int width = screen.cols;
	int height = screen.rows;

	cv::cvtColor(screen, gray, cv::COLOR_BGR2GRAY);
	screen.release();

	cv::Size kernelSize = cv::Size(5, 5);
	cv::GaussianBlur(gray, blurred, kernelSize, 1);
	gray.release();

	int startX = 0, startY = (int)(height / 2), rectW = (int)(0.55 * width), rectH = (int)(height / 2);
	cv::Rect window(startX, startY, rectW, rectH);

	cv::Mat ROI(blurred, window);
	cv::Mat croppedImage;
	ROI.copyTo(croppedImage);
	blurred.release();

	return croppedImage;
}