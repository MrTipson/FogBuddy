#pragma once
#include <string>
#include <vector>
#include "CVController.h"

class PerkEquipper
{
private:
	CVController* controller;
public:
	std::map<std::string, std::vector<std::string>> killerPerks;
	std::map<std::string, std::vector<std::string>> survivorPerks;

	PerkEquipper();
	~PerkEquipper();
	
	bool equipPerk(std::string perk);
};

