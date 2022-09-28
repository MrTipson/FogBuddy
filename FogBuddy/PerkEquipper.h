#pragma once
#include <string>
#include <vector>
#include "CVController.h"

class PerkEquipper
{
private:
	CVController* controller;
	bool equipPerkAdjust(int currentPage, int perkIndex, bool isKillerPerk);
public:
	std::map<std::string, std::vector<std::string>> killers;
	std::vector<std::string> killerPerks;
	std::map<std::string, std::vector<std::string>> survivors;
	std::vector<std::string> survivorPerks;

	PerkEquipper();
	~PerkEquipper();
	
	bool equipPerk(std::string perk, bool isKillerPerk);
};

