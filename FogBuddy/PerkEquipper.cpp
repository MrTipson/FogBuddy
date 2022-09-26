#include "PerkEquipper.h"
#include <iostream>
#include <filesystem>
namespace fs = std::filesystem;


bool PerkEquipper::equipPerk(std::string perk) {
	if (controller == nullptr)
	{
		std::cout << "Initializing CVController\n";
		controller = new CVController();
	}
	cv::Mat screen = CVController::processedScreenshot();
	controller->findPerk(screen, "data/Perks/" + perk);
	screen.release();
	return true;
}

PerkEquipper::PerkEquipper() {
	controller = nullptr;
	std::string path = "data/Perks";
	for (const fs::directory_entry& entry : fs::directory_iterator(path)) {
		perks.push_back(entry.path().filename().string());
	}
}

PerkEquipper::~PerkEquipper() {
	delete controller;
}