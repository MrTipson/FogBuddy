#pragma once
#include <opencv2/core/mat.hpp>

void initInteract();
cv::Mat captureScreenshot();
void mouseClick();
void mouseMove(int x, int y);
void moveAndClickDBD(int x, int y);