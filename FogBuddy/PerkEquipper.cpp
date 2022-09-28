#include "PerkEquipper.h"
#include "Interact.h"
#include <iostream>
#include <filesystem>
#include <math.h>
#include <Windows.h>

namespace fs = std::filesystem;


bool PerkEquipper::equipPerk(std::string perk, bool isKillerPerk) {
	if (controller == nullptr)
	{
		std::cout << "Initializing CVController\n";
		controller = new CVController();
	}
	std::vector<std::string> perks = isKillerPerk ? killerPerks : survivorPerks;
	int perkIndex = std::find(perks.begin(), perks.end(), perk) - perks.begin();
	if (perkIndex < 0 || perkIndex >= perks.size())
	{
		std::cout << "Equip perk sanity check failed\n";
		return false;
	}
	double ratio = controller->pages.size() / ceil(1.0 * perks.size() / 15);
	int expectedPage = (int)(perkIndex * ratio / 15);
	// Print with + 1 so it matches in-game page numbers
	std::cout << "Expected page: " << expectedPage + 1 << std::endl;

	cv::Point button = controller->pages[expectedPage];
	moveAndClickDBD(button.x, button.y);
	Sleep(500);
	cv::Mat screen = CVController::processedScreenshot();
	cv::Point foundPerk;
	if (controller->findPerk(screen, perk, foundPerk))
	{
		foundPerk.y += (int)(CVController::height / 2);
		std::cout << "Sending click	[ x: " << foundPerk.x << ", y: " << foundPerk.y << " ]\n";
		moveAndClickDBD(foundPerk.x, foundPerk.y);
		screen.release();
		return true;
	}
	else
	{
		std::cout << "Perk wasn't found on expected page, attempting to recover\n";
		screen.release();
		equipPerkAdjust(expectedPage, perkIndex, isKillerPerk);
		return false;
	}
}

bool PerkEquipper::equipPerkAdjust(int currentPage, int perkIndex, bool isKillerPerk) {
	bool foundBefore = false, foundAfter = false;

	std::vector<std::string> perks = isKillerPerk ? killerPerks : survivorPerks;
	cv::Point foundPerk;

	cv::Mat screen = CVController::processedScreenshot();
	for (int i = perkIndex - 1; i >= 0 && i >= perkIndex - 5; i--)
	{
		if (controller->findPerk(screen, perks[i], foundPerk))
		{
			foundBefore = true;
			break;
		}
	}
	for (int i = perkIndex + 1; perkIndex < perks.size() && i <= perkIndex + 5; i++)
	{
		if (controller->findPerk(screen, perks[i], foundPerk))
		{
			foundAfter = true;
			break;
		}
	}
	if (foundBefore == foundAfter)
	{
		std::cout << "Perk not found [ " << foundBefore << " " << foundAfter << " ].\n";
		return false;
	}
	if (foundBefore)
	{
		std::cout << "Moving to next page.\n";
		currentPage++;
	}
	if (foundAfter)
	{
		std::cout << "Moving to previous page.\n";
		currentPage--;
	}
	cv::Point button = controller->pages[currentPage];
	moveAndClickDBD(button.x, button.y);
	Sleep(500);
	screen = CVController::processedScreenshot();
	if (controller->findPerk(screen, perks[perkIndex], foundPerk))
	{
		foundPerk.y += (int)(CVController::height / 2);
		std::cout << "Sending click	[ x: " << foundPerk.x << ", y: " << foundPerk.y << " ]\n";
		moveAndClickDBD(foundPerk.x, foundPerk.y);
		screen.release();
		return true;
	}
	std::cout << "Could not recover.\n";
	screen.release();
	return false;
}

PerkEquipper::PerkEquipper() {
	controller = nullptr;
	// Use \\ because the iterators use them, so its uniform
	for (const fs::directory_entry& entry : fs::directory_iterator("data\\Killers")) {
		std::vector<std::string> perks;
		for (const fs::directory_entry& perk : fs::directory_iterator(entry.path())) {
			std::string s = perk.path().filename().string();
			perks.push_back(s);
			killerPerks.push_back(perk.path().string());
		}
		std::string killer = entry.path().filename().string();
		killers.emplace(killer, perks);
	}
	for (const fs::directory_entry& entry : fs::directory_iterator("data\\Survivors")) {
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