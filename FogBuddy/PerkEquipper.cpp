#include "PerkEquipper.h"
#include "Interact.h"
#include "Logger.h"
#include <filesystem>
#include <math.h>
#include <Windows.h>

namespace fs = std::filesystem;

bool PerkEquipper::equipPerk(std::string perk, bool isKillerPerk) {
	if (controller == nullptr)
	{
		LOG_DEBUG("[Equip Perks]: Initializing CVController\n");
		controller = new CVController();
	}
	std::vector<std::string> perks = isKillerPerk ? killerPerks : survivorPerks;
	int perkIndex = std::find(perks.begin(), perks.end(), perk) - perks.begin();
	if (perkIndex < 0 || perkIndex >= perks.size())
	{
		LOG_DEBUG("[Equip Perks]: Equip perk sanity check failed\n");
		return false;
	}
	double ratio = controller->pages.size() / ceil(1.0 * perks.size() / 15);
	int expectedPage = (int)(perkIndex * ratio / 15);

	cv::Point button = controller->pages[expectedPage];
	// Print with + 1 so it matches in-game page numbers
	LOG_INFO("[Equip Perks]: Selecting page %d\n", expectedPage + 1);
	moveAndClickDBD(button.x, button.y);
	Sleep(500);
	cv::Mat screen = CVController::processedScreenshot();
	cv::Point foundPerk;
	if (controller->findPerk(screen, perk, foundPerk))
	{
		foundPerk.y += (int)(CVController::height / 2);
		LOG_INFO("[Equip Perks]: Selecting perk '%s'\n", nameFromPathstr(perk).c_str());
		moveAndClickDBD(foundPerk.x, foundPerk.y);
		screen.release();
		return true;
	}
	else
	{
		LOG_INFO("[Equip Perks]: Perk wasn't found on expected page, attempting to recover\n");
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
		LOG_INFO("[Equip Perks]: Perk not found.");
		LOG_DEBUG(" [ %d %d]", foundBefore, foundAfter);
		LOG_INFO("\n");
		return false;
	}
	if (foundBefore)
	{
		LOG_DEBUG("[Equip Perks]: Moving to next page.\n");
		currentPage++;
	}
	if (foundAfter)
	{
		LOG_DEBUG("[Equip Perks]: Moving to previous page.\n");
		currentPage--;
	}
	cv::Point button = controller->pages[currentPage];
	LOG_INFO("[Equip Perks]: Selecting page %d\n", currentPage);
	moveAndClickDBD(button.x, button.y);
	Sleep(500);
	screen = CVController::processedScreenshot();
	if (controller->findPerk(screen, perks[perkIndex], foundPerk))
	{
		foundPerk.y += (int)(CVController::height / 2);
		LOG_INFO("[Equip Perks]: Selecting perk '%s'\n", nameFromPathstr(perks[perkIndex]).c_str());
		moveAndClickDBD(foundPerk.x, foundPerk.y);
		screen.release();
		return true;
	}
	LOG_INFO("[Equip Perks]: Could not recover.\n");
	screen.release();
	return false;
}

PerkEquipper::PerkEquipper() {
	controller = nullptr;
	// Use \\ because the iterators use them, so its uniform
	for (const fs::directory_entry& entry : fs::directory_iterator("data\\Killers"))
	{
		std::vector<std::string> perks;
		for (const fs::directory_entry& perk : fs::directory_iterator(entry.path()))
		{
			std::string s = perk.path().filename().string();
			perks.push_back(s);
			killerPerks.push_back(perk.path().string());
		}
		std::string killer = entry.path().filename().string();
		killers.emplace(killer, perks);
	}
	for (const fs::directory_entry& entry : fs::directory_iterator("data\\Survivors"))
	{
		std::vector<std::string> perks;
		for (const fs::directory_entry& perk : fs::directory_iterator(entry.path()))
		{
			std::string s = perk.path().filename().string();
			perks.push_back(s);
			survivorPerks.push_back(perk.path().string());
		}
		std::string survivor = entry.path().filename().string();
		survivors.emplace(survivor, perks);
	}
	std::sort(killerPerks.begin(), killerPerks.end(), [](std::string a, std::string b)
			  { return a.substr(a.find_last_of("\\") + 1) < b.substr(b.find_last_of("\\") + 1); });
	std::sort(survivorPerks.begin(), survivorPerks.end(), [](std::string a, std::string b)
			  { return a.substr(a.find_last_of("\\") + 1) < b.substr(b.find_last_of("\\") + 1); });
}

std::string PerkEquipper::nameFromPathstr(std::string perk) {

	std::string perkName = perk.substr(perk.find_last_of("\\") + 1);
	return perkName.substr(0, perkName.find_last_of("."));
}

void PerkEquipper::recalibrate() {
	LOG_DEBUG("[Equip Perks]: Recalibrating CVController\n");
	if (controller != nullptr)
		delete controller;
	controller = new CVController();
}

PerkEquipper::~PerkEquipper() {
	delete controller;
}