#pragma once

#include <filesystem>

class c_config final {
public:
	void run(const char*) noexcept;
	void load(size_t) noexcept;
	void save(size_t) const noexcept;
	void add(const char*) noexcept;
	void remove(size_t) noexcept;
	void rename(size_t, const char*) noexcept;
	void reset() noexcept;

	constexpr auto& get_configs() noexcept {
		return configs;
	}

	struct menu_items {
		struct ragebot_items{
			//aimbot groupbox
			bool enableaimbot{ false };
			bool automaticfire{ false };

			bool hitbox_head{ false };
			bool hitbox_chest{ false };
			bool hitbox_stomach{ false };
			bool hitbox_arms{ false };
			bool hitbox_legs{ false };
			bool hitbox_feet{ false };

			bool avoid_limbs_moving{ false };
			bool avoid_head_jumping{ false };

			bool multipoint_head{ false };
			bool multipoint_body{ false };
			float multipoint_head_scale{ 50 };
			float multipoint_body_scale{ 50 };

			bool hitchance_enable{ false };
			int hitchance_amount{ 50 };
			int minimum_damage{ 25 };

			//other groupbox
			bool force_bodyaim{ false };
			bool backtrack{ false };
			int backtrack_type{ 0 };
			bool bodyaim_if_lethal{ false };
			bool bodyaim_if_inair{ false };
			bool bodyaim_if_missedshots{ false };
			int bodyaim_missedshots{ 0 };

			bool antiaim_correction{ false };

			bool automatic_scope{ false };
			bool automatic_stop{ false };
			int automatic_stop_type{ 0 };

			//fake-lag groupbox
			bool fakelag{ false };
			bool fakelag_standing{ false };
			bool fakelag_moving{ false };
			bool fakelag_inair{ false };
			bool fakelag_shooting{ false };
			int fakelag_type{ 0 };
			int fakelag_ticks{ 0 };
			bool fakeduck{ false };
		} ragebot;

		struct antiaim_items {
			//settings groupbox
			bool enable_antiaim{ false };
			int desync_type{ 0 };
			bool slowmotion{ false };

			//standing groupbox
			int standing_pitch{ 0 };
			int standing_yaw{ 0 };
			int standing_jitter_type{ 0 };
			int standing_jitter_range{ 35 };
			bool standing_autodirection{ false };
			
			//moving groupbox
			int moving_pitch{ 0 };
			int moving_yaw{ 0 };
			int moving_jitter_type{ 0 };
			int moving_jitter_range{ 35 };
			bool moving_autodirection{ false };
			
			//in air groupbox
			int inair_pitch{ 0 };
			int inair_yaw{ 0 };
			int inair_jitter_type{ 0 };
			int inair_jitter_range{ 35 };
			bool inair_autodirection{ false };
		} antiaim;

		struct visuals_items {
			//player groupbox
			bool enable_visuals{ false };
			int visuals_type{ 0 };
			bool team_check{ false };
			bool dormant_check{ false };
			bool player_name{ false };
			float clr_name[4]{ 1.0f,1.0f,1.0f,1.0f };
			bool player_box{ false };
			float clr_box[4]{ 1.0f,1.0f,1.0f,1.0f };
			bool player_health{ false };
			bool player_health_text{ false };
			bool player_weapon_text{ false };
			bool player_weapon_icon{ false };
			float clr_weapon_icon[4]{ 1.0f,1.0f,1.0f,1.0f };
			bool player_ammo{ false };
			float clr_ammo[4]{ 1.0f,1.0f,1.0f,1.0f };
			bool player_ammo_bar{ false };
			float clr_ammo_bar[4]{ 1.0f,1.0f,1.0f,1.0f };

			bool player_flags_money{ false };
			bool player_flags_armor{ false };
			bool player_flags_scoped{ false };
			bool player_flags_defusing{ false };
			bool player_flags_rescuing{ false };
			bool player_flags_reloading{ false };
			bool player_flags_c4{ false };
			bool player_flags_flashed{ false };
			bool player_flags_fakeducking{ false };

			bool skeleton{ false };
			float clr_skeleton[4]{ 1.0f,1.0f,1.0f,1.0f };
			bool backtrack_skeleton{ false };
			float clr_backtrack_skeleton[4]{ 1.0f,1.0f,1.0f,1.0f };

			bool out_of_fov_arrow{ false };
			float clr_out_of_fov[4]{ 1.0f,1.0f,1.0f,1.0f };
			int out_of_fov_arrow_size{ 5 };
			int out_of_fov_arrow_radius{ 10 };

			bool hitmarker{ false };
			int hitmarker_sound{ 0 };

			bool thirdperson{ false };
			bool spectators_list{ false };

			bool bullettracer_tracer{ false };
			float clr_bullettracer_tracer[4]{ 1.0f,0.0f,0.0f,1.0f };
			bool bullettracer_impact{ false };
			float clr_bullettracer_impact[4]{ 1.0f,0.0f,0.0f,1.0f };

			bool visuals_preview{ false };

			//effects groupbox
			int override_fov{ 90 };
			bool force_crosshair{ false };
			bool recoil_crosshair{ false };
			bool autowall_indicator{ false };
			bool damage_indicator{ false };
			bool modulation_night{ false };
			bool modulation_fullbright{ false };
			int modulation_amount{ 100 };

			bool inaccuracy_overlay{ false };
			float clr_inaccuracy_overlay[4]{ 1.0f,1.0f,1.0f,1.0f };
			int inaccuracy_overlay_size{ 15 };

			bool grenade_prediction{ false };
			float clr_grenade_prediction[4]{ 1.0f,0.0f,0.0f,1.0f };

			bool droppedweapons_name{ false };
			bool droppedweapons_ammo{ false };

			bool removals_smoke{ false };
			bool removals_recoil{ false };
			bool removals_flash{ false };
			bool removals_sleeves{ false };
			bool removals_hands{ false };
			bool removals_scope{ false };
			bool removals_scope_zoom{ false };

			//glow groupbox
			bool enable_glow{ false };
			bool glow_enemy{ false };
			float clr_glow[4]{ 1.0f,1.0f,1.0f,1.0f };
			bool glow_team{ false };
			float clr_glow_team[4]{ 1.0f,1.0f,1.0f,1.0f };
			bool glow_local{ false };
			float clr_glow_local[4]{ 1.0f,1.0f,1.0f,1.0f };
			bool glow_planted{ false };
			float clr_glow_planted[4]{ 1.0f,1.0f,1.0f,1.0f };
			bool glow_dropped_weapons{ false };
			float clr_glow_dropped_weapons[4]{ 1.0f,1.0f,1.0f,1.0f };

			//chams groupbox
			int chams_type{ 0 };
			bool chams_enemy_vis{ false };
			float clr_chams_enemy_vis[4]{ 1.0f,1.0f,1.0f,1.0f };
			bool chams_enemy_invis{ false };
			float clr_chams_enemy_invis[4]{ 1.0f,1.0f,1.0f,1.0f };

			bool chams_team_vis{ false };
			float clr_chams_team_vis[4]{ 1.0f,1.0f,1.0f,1.0f };
			bool chams_team_invis{ false };
			float clr_chams_team_invis[4]{ 1.0f,1.0f,1.0f,1.0f };

			bool chams_local{ false };
			float clr_chams_local[4]{ 1.0f,1.0f,1.0f,1.0f };
			bool chams_local_desync{ false };
			float clr_chams_local_desync[4]{ 1.0f,1.0f,1.0f,1.0f };
			bool chams_backtrack_visualize{ false };
			float clr_chams_backtrack_visualize[4]{ 1.0f,1.0f,1.0f,1.0f };
			bool chams_smoke_check{ false };

			bool chams_hands{ false };
			float clr_chams_hands[4]{ 1.0f,1.0f,1.0f,1.0f };
			bool chams_weapons{ false };
			float clr_chams_weapons[4]{ 1.0f,1.0f,1.0f,1.0f };
		} visuals;

		struct misc_items {
			//misc groupbox
			bool enable_misc{ false };
			bool anti_untrusted{ true };

			bool clantag_spammer{ false };

			bool logs_player_hurt{ false };
			bool logs_player_bought{ false };

			bool spectators_list{ false };
			bool watermark{ true };

			int aspect_ratio_w{ 1920 };
			int aspect_ratio_h{ 1080 };

			bool viewmodel_offset{ false };
			int viewmodel_x{ 5 };
			int viewmodel_y{ 5 };
			int viewmodel_z{ 5 };

			//movement groupbox
			bool bunny_hop{ false };
			int bunny_hop_hitchance{ 0 };
			int bunny_hop_minimum_value{ 0 };
			int bunny_hop_maximum_value{ 0 };

			bool remove_duck_delay{ false };

			bool auto_strafe{ false };
			int auto_strafe_speed{ 7 };
			int auto_strafe_wasd{ 0 };

			bool edge_jump{ false };
			bool edge_jump_duck_in_air{ false };
		} misc;

		struct config_items {
			bool unload_cheat{ false };

			int keybinds_selection{ 0 };
			int bind_bodyaim_key{ 0 };
			int bind_fakeduck_key{ 0 };
			int bind_slowmotion_key{ 0 };
			int bind_antiaimflip_key{ 0 };
			int bind_thirdperson_key{ 0 };
			int bind_edgejump_key{ 0 };
		} config;
	} item;

private:
	std::filesystem::path path;
	std::vector<std::string> configs;
};

extern c_config config_system;