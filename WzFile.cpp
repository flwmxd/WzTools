#include "WzFile.h"
#include "WzNode.h"
#include "../Others/Console.h"
#include "../Others/Timer.h"
namespace WzFile {
	WzNode base, character, effect, etc, item, map, mob, morph, npc, quest, reactor, skill, sound, string, tamingmob, ui;
	auto loadAll(std::string &path) -> void
	{
		//Console::getInstance().println("begin to read WZ files ", __func__);
		Log("Begin to read Wz Files");
		base = WzNode(std::make_shared<WzReader>(path+"Base.wz"));
		character = WzNode(std::make_shared<WzReader>(path+"Character.wz"));
		effect = WzNode(std::make_shared<WzReader>(path+"Effect.wz"));
		etc = WzNode(std::make_shared<WzReader>(path+"Etc.wz"));
		item = WzNode(std::make_shared<WzReader>(path+"Item.wz"));
		map = WzNode(std::make_shared<WzReader>(path+"Map.wz"));
		mob = WzNode(std::make_shared<WzReader>(path+"Mob.wz"));
		morph = WzNode(std::make_shared<WzReader>(path+"Morph.wz"));
		npc = WzNode(std::make_shared<WzReader>(path+"Npc.wz"));
		quest = WzNode(std::make_shared<WzReader>(path+"Quest.wz"));
		reactor = WzNode(std::make_shared<WzReader>(path+"Reactor.wz"));
		skill = WzNode(std::make_shared<WzReader>(path+"Skill.wz"));
		sound = WzNode(std::make_shared<WzReader>(path+"Sound.wz"));
		string = WzNode(std::make_shared<WzReader>(path+"String.wz"));
		tamingmob = WzNode(std::make_shared<WzReader>(path+"TamingMob.wz"));
		ui = WzNode(std::make_shared<WzReader>(path+"UI.wz"));

		ui.root();
		map.root();
		base.root();
		character.root();
		effect.root();
		etc.root();
		item.root();

		mob.root();
		morph.root();
		npc.root();
		quest.root();
		reactor.root();
		skill.root();
		sound.root();
		string.root();
		tamingmob.root();

		Log("WZ read over");

	}
	auto loadAll() -> void
	{
		
	}
}