/* This file is part of LeagueSkinChanger by b3akers, licensed under the MIT license:
*
* MIT License
*
* Copyright (c) b3akers 2020
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/
#include "skin_database.hpp"
#include "skin_changer.hpp"
#include "offsets.hpp"
#include "fnv_hash.hpp"
#include "game_classes.hpp"

#include <algorithm>

void skin_database::load( ) {
	static const auto translateString_UNSAFE_DONOTUSE = reinterpret_cast<const char* ( __cdecl* )( const char* )>( std::uintptr_t( GetModuleHandle( nullptr ) ) + offsets::functions::translateString_UNSAFE_DONOTUSE );

	auto g_championg_manager = *reinterpret_cast<ChampionManager**>( std::uintptr_t( GetModuleHandle( nullptr ) ) + offsets::global::ChampionManager );
	for ( auto& champion : g_championg_manager->champions ) {
		std::vector<int32_t> skins_ids;
		for ( auto& skin : champion->skins )
			skins_ids.push_back( skin.skin_id );
		std::sort( skins_ids.begin( ), skins_ids.end( ) );

		std::map<std::string, int32_t> temp_skin_list;
		for ( auto& i : skins_ids ) {
			auto skin_display_name = std::string( "game_character_skin_displayname_" );
			skin_display_name.append( champion->champion_name.str );
			skin_display_name.append( "_" );
			skin_display_name.append( std::to_string( i ) );

			auto skin_display_name_translated = i > 0 ? std::string( translateString_UNSAFE_DONOTUSE( skin_display_name.c_str( ) ) ) : std::string( champion->champion_name.str );

			auto it = temp_skin_list.find( skin_display_name_translated );

			if ( it == temp_skin_list.end( ) )
				temp_skin_list[ skin_display_name_translated ] = 1;
			else {
				skin_display_name_translated.append( " Chroma " );
				skin_display_name_translated.append( std::to_string( it->second ) );

				it->second = it->second + 1;
			}

			champions_skins[ fnv::hash_runtime( champion->champion_name.str ) ].push_back( skin_info { std::string( champion->champion_name.str ),skin_display_name_translated,i } );

			if ( i == 7 && fnv::hash_runtime( champion->champion_name.str ) == FNV( "Lux" ) ) {
				champions_skins[ fnv::hash_runtime( champion->champion_name.str ) ].push_back( skin_info { "LuxAir", "Elementalist Air Lux", i } );
				champions_skins[ fnv::hash_runtime( champion->champion_name.str ) ].push_back( skin_info { "LuxFire", "Elementalist Fire Lux", i } );
				champions_skins[ fnv::hash_runtime( champion->champion_name.str ) ].push_back( skin_info { "LuxIce", "Elementalist Ice Lux", i } );
				champions_skins[ fnv::hash_runtime( champion->champion_name.str ) ].push_back( skin_info { "LuxMagma", "Elementalist Magma Lux", i } );
				champions_skins[ fnv::hash_runtime( champion->champion_name.str ) ].push_back( skin_info { "LuxMystic", "Elementalist Mystic Lux", i } );
				champions_skins[ fnv::hash_runtime( champion->champion_name.str ) ].push_back( skin_info { "LuxNature", "Elementalist Nature Lux", i } );
				champions_skins[ fnv::hash_runtime( champion->champion_name.str ) ].push_back( skin_info { "LuxStorm", "Elementalist Storm Lux", i } );
				champions_skins[ fnv::hash_runtime( champion->champion_name.str ) ].push_back( skin_info { "LuxWater", "Elementalist Water Lux", i } );
			}
		}
	}

	for ( auto ward_skin_id = 1;; ward_skin_id++ ) {
		auto ward_display_name = std::string( "game_character_skin_displayname_SightWard_" + std::to_string( ward_skin_id ) );
		auto ward_display_name_translated = std::string( translateString_UNSAFE_DONOTUSE( ward_display_name.c_str( ) ) );
		if ( ward_display_name == ward_display_name_translated )
			break;

		wards_skins.push_back( { ward_skin_id, ward_display_name_translated } );
	}

	// Semi autoupdater for others skin
	//

	static const auto get_skins_len_for_model = [ ] ( std::string model ) -> uint32_t {
		static const auto CharacterData__GetCharacterPackage = reinterpret_cast<uintptr_t( __cdecl* )( std::string&, int32_t )>( std::uintptr_t( GetModuleHandle( nullptr ) ) + offsets::functions::CharacterData__GetCharacterPackage );
		auto default_skin_data = *reinterpret_cast<uintptr_t*>( CharacterData__GetCharacterPackage( model, 0 ) + 0x3C );
		for ( auto skins_len = 1;; skins_len++ ) {
			if ( *reinterpret_cast<uintptr_t*>( CharacterData__GetCharacterPackage( model, skins_len ) + 0x3C ) == default_skin_data )
				return skins_len;
		}
	};

	for ( auto& jungle_mob_info : jungle_mobs_skins ) {
		auto skins_len = get_skins_len_for_model( jungle_mob_info.model_name );
		for ( auto i = jungle_mob_info.skins.size( ); i < skins_len; i++ )
			jungle_mob_info.skins.push_back( jungle_mob_info.name + " " + std::to_string( i ) );
	}

	auto minion_skin_len = get_skins_len_for_model( "SRU_ChaosMinionMelee" ) / 2;
	for ( auto i = skin_database::minions_skins.size( ); i < minion_skin_len; i++ )
		skin_database::minions_skins.push_back( "Minion " + std::to_string( i ) );

	//Summoners Emotes, hardcoded way
	//TODO

	for ( auto i = 0u; i < 10000; i++ )
	{
		auto emote = SummonerEmoteUserComponent::get_summoner_emote_data( i );
		if ( !emote )
			continue;

		auto raw_name = std::string( emote->emote_name.str );
		auto translated = std::string( translateString_UNSAFE_DONOTUSE( raw_name.c_str() ) );

		if ( translated == raw_name )
			continue;

		summoner_emotes.push_back( { emote,  translated } );
	}
}

std::map<uint32_t, std::vector<skin_database::skin_info>> skin_database::champions_skins;
std::vector<std::pair<uint32_t, std::string>> skin_database::wards_skins;
std::vector<std::pair<SummonerEmote*, std::string>> skin_database::summoner_emotes;


std::vector<skin_database::jungle_mob_skin_info> skin_database::jungle_mobs_skins =
{
	{
		u8"纳什男爵",
		"SRU_Baron",
		{
			FNV("SRU_Baron")
		},
		{
			u8"纳什男爵",
			u8"冰雪节男爵",
			u8"锦标赛男爵",
			u8"农历狂欢男爵",
			u8"MSI男爵",
			u8"奥德赛男爵",
			u8"冠军生日男爵"
		}
	},
	{
		u8"蓝Buff",
		"SRU_Blue",
		{
			FNV("SRU_Blue")
		},
		{
			u8"蓝Buff",
			u8"深蓝蓝Buff",
			u8"泳池派对蓝Buff"
		}
	},
	{
		u8"红Buff",
		"SRU_Red",
		{
			FNV("SRU_Red")
		},
		{
			u8"红Buff",
			u8"泳池派对红Buff"
		}
	},
	{
		u8"河蟹",
		"Sru_Crab",
		{
			FNV("Sru_Crab"),
			FNV("Sru_CrabWard")
		},
		{
			u8"河蟹",
			u8"万圣节灯蟹",
			u8"万圣节黑蟹"
		}
	},
	{
		u8"七鸟",
		"SRU_Razorbeak",
		{
			FNV("SRU_Razorbeak"),
			FNV("SRU_RazorbeakMini")
		},
		{
			u8"七鸟",
			u8"白色七鸟"
		}
	},
	{
		u8"石头",
		"SRU_Krug",
		{
			FNV("SRU_Krug"),
			FNV("SRU_KrugMini")
		},
		{
			u8"石头",
			u8"黑色的石头"
		}
	}
};

std::vector<std::string> skin_database::minions_skins =
{
	u8"小兵",
	u8"夏日小兵",
	u8"铠甲小兵",
	u8"降雪小兵",
	u8"德莱文小兵",
	u8"明星守护小兵",
	u8"街机小兵",
	u8"降雪2小兵",
	u8"奥德赛小兵",
	u8"老鼠小兵"
};