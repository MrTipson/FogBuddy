#pragma once
#include <vector>
#include <opencv2/opencv.hpp>

class CVController
{
private:
	int perkWidth;
	std::vector<cv::Point> pages;
	void calibrate(cv::Mat screen);
	void findPages(cv::Mat screen);
public:
	CVController();
	cv::Point findPerk(cv::Mat screen, std::string perkPath);
	static cv::Mat processedScreenshot();
};
