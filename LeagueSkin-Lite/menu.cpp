#include "menu.hpp"
#include "skin_database.hpp"
#include "fnv_hash.hpp"
#include "offsets.hpp"
#include "game_classes.hpp"
#include "../imgui/imgui.h"
#include "json.hpp"
#include "Tools.h"

#include <Windows.h>
#include <fstream>

using json = nlohmann::json;

int32_t config::current_combo_skin_index = 0;
int32_t config::current_combo_ward_index = 0;
int32_t config::current_combo_minion_index = 0;

int32_t config::current_ward_skin_index = -1;
int32_t config::current_minion_skin_index = -1;

std::map<uint32_t, int32_t> config::current_combo_jungle_mob_skin_index;

std::map<uint32_t, int32_t> config::current_combo_ally_skin_index;
std::map<uint32_t, int32_t> config::current_combo_enemy_skin_index;

std::map<uint32_t, int32_t> config::current_summoner_emotes;

auto config_json = json( );
Tools* tools = new Tools;

std::vector<std::pair<SummonerEmoteSlot, std::string>> available_emote_slots;
std::int32_t current_emote_slot = 0;
std::int32_t current_emote = 0;

void config::save( )
{
	auto player = *reinterpret_cast< AIBaseCommon** >( std::uintptr_t( GetModuleHandle( nullptr ) ) + offsets::global::Player );
	if ( player )
		config_json[ std::string( player->get_character_data_stack( )->base_skin.model.str ) + ".current_combo_skin_index" ] = current_combo_skin_index;

	config_json[ "current_combo_ward_index" ] = current_combo_ward_index;
	config_json[ "current_combo_minion_index" ] = current_combo_minion_index;
	config_json[ "current_ward_skin_index" ] = current_ward_skin_index;
	config_json[ "current_minion_skin_index" ] = current_minion_skin_index;

	for ( auto& it : config::current_combo_ally_skin_index )
		config_json[ "current_combo_ally_skin_index" ][ std::to_string( it.first ) ] = it.second;

	for ( auto& it : config::current_combo_enemy_skin_index )
		config_json[ "current_combo_enemy_skin_index" ][ std::to_string( it.first ) ] = it.second;

	for ( auto& it : config::current_combo_jungle_mob_skin_index )
		config_json[ "current_combo_jungle_mob_skin_index" ][ std::to_string( it.first ) ] = it.second;

	for ( auto& it : config::current_summoner_emotes )
		config_json[ "current_summoner_emotes" ][ std::to_string( it.first ) ] = it.second;

	auto out = std::ofstream( L"league_changer.json" );
	out << config_json.dump( );
	out.close( );
}

void config::load( )
{
	auto out = std::ifstream( L"league_changer.json" );
	if ( !out.good( ) )
		return;

	config_json = json::parse( out );

	auto player = *reinterpret_cast< AIBaseCommon** >( std::uintptr_t( GetModuleHandle( nullptr ) ) + offsets::global::Player );
	if ( player )
		current_combo_skin_index = config_json.value( std::string( player->get_character_data_stack( )->base_skin.model.str ) + ".current_combo_skin_index", 0 );

	current_combo_ward_index = config_json.value( "current_combo_ward_index", 0 );
	current_combo_minion_index = config_json.value( "current_combo_minion_index", 0 );
	current_ward_skin_index = config_json.value( "current_ward_skin_index", -1 );
	current_minion_skin_index = config_json.value( "current_minion_skin_index", -1 );

	auto ally_skins = config_json.find( "current_combo_ally_skin_index" );
	if ( ally_skins != config_json.end( ) )
		for ( auto& it : ally_skins.value( ).items( ) )
			current_combo_ally_skin_index[ std::stoul( it.key( ) ) ] = it.value( ).get<int32_t>( );

	auto enemy_skins = config_json.find( "current_combo_enemy_skin_index" );
	if ( enemy_skins != config_json.end( ) )
		for ( auto& it : enemy_skins.value( ).items( ) )
			current_combo_enemy_skin_index[ std::stoul( it.key( ) ) ] = it.value( ).get<int32_t>( );

	auto jungle_mobs_skins = config_json.find( "current_combo_jungle_mob_skin_index" );
	if ( jungle_mobs_skins != config_json.end( ) )
		for ( auto& it : jungle_mobs_skins.value( ).items( ) )
			current_combo_jungle_mob_skin_index[ std::stoul( it.key( ) ) ] = it.value( ).get<int32_t>( );

	auto summoner_emotes = config_json.find( "current_summoner_emotes" );
	if ( summoner_emotes != config_json.end( ) )
		for ( auto& it : summoner_emotes.value( ).items( ) )
			current_summoner_emotes[ std::stoul( it.key( ) ) ] = it.value( ).get<int32_t>( );

	out.close( );
}

void config::reset( )
{
	current_combo_skin_index = 0;
	current_combo_ward_index = 0;
	current_combo_minion_index = 0;

	current_ward_skin_index = -1;
	current_minion_skin_index = -1;

	current_combo_ally_skin_index.clear( );
	current_combo_enemy_skin_index.clear( );
	current_combo_jungle_mob_skin_index.clear( );
	current_summoner_emotes.clear( );
}

char str_buffer[ 256 ];
void menu::draw( )
{

	ImGui::Begin( u8"LeagueSkinLite www.popop.ml,    祝大家新年快乐!" );
	{
		static auto vector_getter_skin = [ ]( void* vec, int idx, const char** out_text )
		{
			auto& vector = *static_cast< std::vector<skin_database::skin_info>* >( vec );
			if ( idx < 0 || idx > static_cast< int >( vector.size( ) ) ) { return false; }
			*out_text = idx == 0 ? u8"未改变" : vector.at( idx - 1 ).skin_name.c_str( );
			return true;
		};

		static auto vector_getter_ward_skin = [ ]( void* vec, int idx, const char** out_text )
		{
			auto& vector = *static_cast< std::vector<std::pair<int32_t, std::string>>* >( vec );
			if ( idx < 0 || idx > static_cast< int >( vector.size( ) ) ) { return false; }
			*out_text = idx == 0 ? u8"未改变" : vector.at( idx - 1 ).second.c_str( );
			return true;
		};

		auto player = *reinterpret_cast< AIBaseCommon** >( std::uintptr_t( GetModuleHandle( nullptr ) ) + offsets::global::Player );
		if ( player )
		{
			auto& values = skin_database::champions_skins[ fnv::hash_runtime( player->get_character_data_stack( )->base_skin.model.str ) ];
			ImGui::Text(u8"视觉皮肤设置");

			if ( ImGui::Combo(u8"英雄皮肤", &config::current_combo_skin_index, vector_getter_skin, static_cast< void* >( &values ), values.size( ) + 1 ) )
				if ( config::current_combo_skin_index > 0 )
					player->change_skin( values[ config::current_combo_skin_index - 1 ].model_name.c_str( ), values[ config::current_combo_skin_index - 1 ].skin_id );

			if ( ImGui::Combo(u8"守卫皮肤", &config::current_combo_ward_index, vector_getter_ward_skin, static_cast< void* >( &skin_database::wards_skins ), skin_database::wards_skins.size( ) + 1 ) )
				config::current_ward_skin_index = config::current_combo_ward_index == 0 ? -1 : skin_database::wards_skins.at( config::current_combo_ward_index - 1 ).first;
		
		}

		ImGui::Text(u8"小工具设置");

		if (ImGui::Button(u8"无限视距"))
		{
			if (tools->get_Fov_status() == 1)
			{
				tools->set_Fov_status(0);
				tools->on_Fov();
			}
			else
			{
				tools->set_Fov_status(1);
				tools->off_Fov();
			}
		}

		ImGui::SameLine();

		ImGui::Text(u8"%p", tools->get_Fov_data_address());
		
		ImGui::SameLine();

		ImGui::Text(u8"%f", tools->get_Fov_data());

		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), u8"本源码将在2021/02/12 进行开源(新年礼物)");

		ImGui::Text(u8"作者：烧杯 汉化：笑笑");

		ImGui::Text(u8"部分汉化可能有误 介意勿用");

		ImGui::Text(u8"编译日期：2021/02/11/21:18分 晚上 1° 除夕");

		ImGui::Text(u8"官网：www.popop.ml 个人博客：z.smliexx.ml");

		ImGui::Text(u8"持续更新中......");

		ImGui::Separator();

		ImGui::Text(u8"打个小广告: ");

		ImGui::NewLine();

		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), u8"TiTi驱动 - titi.smliexx.ml");

	}
	ImGui::End( );
}