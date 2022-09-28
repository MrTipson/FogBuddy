#pragma once
#include <vector>
#include <opencv2/opencv.hpp>

class CVController
{
private:
	int perkWidth;
	void calibrate(cv::Mat screen);
	void findPages(cv::Mat screen);
	void showMatch(std::string title,  cv::Mat screen, cv::Point, double val);
public:
	std::vector<cv::Point> pages;
	CVController();
	bool findPerk(cv::Mat screen, std::string perkPath, cv::Point& perk);
	static cv::Mat processedScreenshot();
	static int width, height;
};
