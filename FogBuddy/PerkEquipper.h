#pragma once
#include <string>
#include <vector>
#include "CVController.h"

class PerkEquipper
{
private:
	CVController* controller;
public:
	std::vector<std::string> perks;

	PerkEquipper();
	~PerkEquipper();
	
	bool equipPerk(std::string perk);
};

