//////////////////////////////////////////////////////////////////////////////
// This file is part of the PharaohStroy MMORPG client                      // 
// Copyright ?2016-2017 Prime Zeng                                          // 
//                                                                          // 
// This program is free software: you can redistribute it and/or modify     // 
// it under the terms of the GNU Affero General Public License as           // 
// published by the Free Software Foundation, either version 3 of the       // 
// License, or (at your option) any later version.                          // 
//                                                                          // 
// This program is distributed in the hope that it will be useful,          // 
// but WITHOUT ANY WARRANTY; without even the implied warranty of           // 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            // 
// GNU Affero General Public License for more details.                      // 
//                                                                          // 
// You should have received a copy of the GNU Affero General Public License // 
// along with this program.  If not, see <http://www.gnu.org/licenses/>.    // 
////////////////////////////////////////////////////////////////////////////// 

#include "WzFile.h"
#include "WzNode.h"
namespace WzFile {
	WzNode base, character,  effect, etc, item, map, mob, morph, npc, quest, reactor, skill, sound, string, tamingmob, ui;
	auto loadAll(const std::string &path) -> void
	{
		base.load(std::make_shared<WzReader>(path+"Base.wz"));
		character.load(std::make_shared<WzReader>(path+"Character.wz"));
		effect.load(std::make_shared<WzReader>(path+"Effect.wz"));
		etc.load(std::make_shared<WzReader>(path+"Etc.wz"));
		item.load(std::make_shared<WzReader>(path+"Item.wz"));
		map.load(std::make_shared<WzReader>(path+"Map.wz"));
		mob.load(std::make_shared<WzReader>(path+"Mob.wz"));
		morph.load(std::make_shared<WzReader>(path+"Morph.wz"));
		npc.load(std::make_shared<WzReader>(path+"Npc.wz"));
		quest.load(std::make_shared<WzReader>(path+"Quest.wz"));
		reactor.load(std::make_shared<WzReader>(path+"Reactor.wz"));
		skill.load(std::make_shared<WzReader>(path+"Skill.wz"));
		sound.load(std::make_shared<WzReader>(path+"Sound.wz"));
		string.load(std::make_shared<WzReader>(path+"String.wz"));
		tamingmob.load(std::make_shared<WzReader>(path+"TamingMob.wz"));
		ui.load(std::make_shared<WzReader>(path+"UI.wz"));
	}
}
