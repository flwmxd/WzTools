#pragma once
#include <memory>
#include "WzNode.h"
#include "../Others/Singleton.h"
#include <string>
namespace WzFile{
	extern WzNode base, character, effect, etc, item, map, mob, morph, npc, quest, reactor, skill, sound, string, tamingmob, ui;
	auto loadAll(std::string &path) -> void;
	auto loadAll() -> void;
};