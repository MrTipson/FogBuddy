#include "PerkEquipper.h"
#include "Interact.h"
#include <iostream>
#include <filesystem>
namespace fs = std::filesystem;


bool PerkEquipper::equipPerk(std::string perk) {
	if (controller == nullptr)
	{
		std::cout << "Initializing CVController\n";
		controller = new CVController();
	}
	int width, height;
	cv::Mat screen = CVController::processedScreenshot(&width, &height);
	cv::Point foundPerk;
	if (controller->findPerk(screen, perk, foundPerk))
	{
		foundPerk.y += (int)(height / 2);
		std::cout << "Sending click	[ x: " << foundPerk.x << ", y: " << foundPerk.y << " ]\n";
		moveAndClickDBD(foundPerk.x, foundPerk.y);
		screen.release();
		return false;
	}
	else
	{
		std::cout << "O no, couldnt find perk\n";
		screen.release();
		return true;
	}
}

PerkEquipper::PerkEquipper() {
	controller = nullptr;
	for (const fs::directory_entry& entry : fs::directory_iterator("data/Killers")) {
		std::vector<std::string> perks;
		for (const fs::directory_entry& perk : fs::directory_iterator(entry.path())) {
			std::string s = perk.path().filename().string();
			perks.push_back(s);
			killerPerks.push_back(perk.path().string());
		}
		std::string killer = entry.path().filename().string();
		killers.emplace(killer, perks);
	}
	for (const fs::directory_entry& entry : fs::directory_iterator("data/Survivors")) {
		std::vector<std::string> perks;
		for (const fs::directory_entry& perk : fs::directory_iterator(entry.path())) {
			std::string s = perk.path().filename().string();
			perks.push_back(s);
			survivorPerks.push_back(perk.path().string());
		}
		std::string survivor = entry.path().filename().string();
		survivors.emplace(survivor, perks);
	}
	std::sort(killerPerks.begin(), killerPerks.end(), [](std::string a, std::string b) {
		return a.substr(a.find_last_of("\\") + 1) < b.substr(b.find_last_of("\\") + 1);
		});
	std::sort(survivorPerks.begin(), survivorPerks.end(), [](std::string a, std::string b) {
		return a.substr(a.find_last_of("\\") + 1) < b.substr(b.find_last_of("\\") + 1);
		});
}

PerkEquipper::~PerkEquipper() {
	delete controller;
}