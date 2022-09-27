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
	perkWidth = widths.at((int)(widths.size() / 2));
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

bool CVController::findPerk(cv::Mat screen, std::string perkPath, cv::Point& foundPerk) {
	int offsets[] = { 0,1,2,3 };

	cv::Mat screenThresh, template_, thresholded, resized, resizedMask;
	cv::threshold(screen, screenThresh, 100, 255, cv::THRESH_TOZERO);

	template_ = cv::imread(perkPath, cv::IMREAD_GRAYSCALE);
	cv::Mat mask = cv::imread("data/mask.png", cv::IMREAD_GRAYSCALE);

	cv::threshold(template_, thresholded, 100, 255, cv::THRESH_TOZERO);
	template_.release();
	double opt_val = 1;
	cv::Point opt_loc;
	for (int i = 0; i < 4; i++) {
		cv::Size newsize(perkWidth+offsets[i], perkWidth + offsets[i]);
		cv::resize(thresholded, resized, newsize);
		cv::resize(mask, resizedMask, newsize);

		cv::Mat match;
		cv::matchTemplate(screenThresh, resized, match, cv::TM_SQDIFF_NORMED, resizedMask);

		double min_val, max_val;
		cv::Point min_loc, max_loc;
		cv::minMaxLoc(match, &min_val, &max_val, &min_loc, &max_loc);

		if (min_val < opt_val)
		{
			opt_val = min_val;
			opt_loc = min_loc;
		}
	}
	screenThresh.release();
	resized.release();
	resizedMask.release();
	thresholded.release();
	mask.release();

	
	cv::Rect rect(opt_loc.x, opt_loc.y, perkWidth, perkWidth);
	cv::Mat screen2 = screen.clone();
	cv::rectangle(screen2, rect, cv::Scalar(255, 255, 255));
	cv::namedWindow("Perk " + std::to_string(opt_val));
	cv::imshow("Perk " + std::to_string(opt_val), screen2);
	cv::waitKey(0);

	std::cout << opt_val << std::endl;
	if (opt_val < 0.5)
	{
		std::cout << "Found perk match (with error " << opt_val << ")\n";
		opt_loc.x += (int)(perkWidth / 2);
		opt_loc.y += (int)(perkWidth / 2);
		foundPerk = opt_loc;
		return true;
	}
	return false;
}

cv::Mat CVController::processedScreenshot(int* origWidth, int* origHeight) {
	cv::Mat gray, cropped;
	cv::Mat screen = captureScreenshot();
	int width = screen.cols;
	int height = screen.rows;
	if (origWidth != nullptr) *origWidth = width;
	if (origHeight != nullptr) *origHeight = height;

	cv::cvtColor(screen, gray, cv::COLOR_BGR2GRAY);
	screen.release();

	int startX = 0, startY = (int)(height / 2), rectW = (int)(0.55 * width), rectH = (int)(height / 2);
	cv::Rect window(startX, startY, rectW, rectH);

	cv::Mat ROI(gray, window);
	cv::Mat croppedImage;
	ROI.copyTo(croppedImage);
	gray.release();

	return croppedImage;
}