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
	bool findPerk(cv::Mat screen, std::string perkPath, cv::Point& perk);
	static cv::Mat processedScreenshot(int* origWidth = nullptr, int* origHeight = nullptr);
};
