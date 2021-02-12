#pragma once
#include <cinttypes>
#include <map>

namespace config {
	extern int32_t current_combo_skin_index;
	extern int32_t current_combo_ward_index;
	extern int32_t current_combo_minion_index;

	extern int32_t current_ward_skin_index;
	extern int32_t current_minion_skin_index;

	extern std::map<uint32_t, int32_t> current_combo_ally_skin_index;
	extern std::map<uint32_t, int32_t> current_combo_enemy_skin_index;
	extern std::map<uint32_t, int32_t> current_combo_jungle_mob_skin_index;

	extern std::map<uint32_t, int32_t> current_summoner_emotes;

	void save( );
	void load( );
	void reset( );
};

namespace menu {
	void draw( );
};

