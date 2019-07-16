#include "main/setup.hpp"
#include "menu.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "config/config.hpp"

/*
#include "../menu.hpp"
#include "../controls/tab.hpp"
#include "../controls/group.hpp"
#include "../controls/checkbox.hpp"
#include "../controls/slider.hpp"
#include "../controls/combo.hpp"
#include "../controls/multicombo.hpp"
#include "../controls/hotkey.hpp"
#include "../controls/colorpicker.hpp"
#include "../controls/button.hpp"
*/

c_menu menu;

#define UNLEN 256
IDirect3DStateBlock9* state_block;
bool reverse = false;
int offset = 0;
bool show_popup = false;
static bool save_config = false;
static bool load_config = false;
namespace ImGui {
	long get_mils() {
		auto duration = std::chrono::system_clock::now().time_since_epoch();
		return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
	}

	void begin_popup(const char* text, int onScreenMils, bool* done) {
		if (!done)
			show_popup = true;

		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();
		int width = io.DisplaySize.x;
		static long old_time = -1;
		ImGui::SetNextWindowPos(ImVec2(width - offset, 100));
		style.WindowMinSize = ImVec2(100.f, 20.f);
		ImGui::Begin("##PopUpWindow", &show_popup, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
		ImVec2 p = ImGui::GetCursorScreenPos();

		ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2(p.x - 15, p.y - 13), ImVec2(p.x + ImGui::GetWindowWidth(), p.y - 16), ImColor(167, 24, 71, 255), ImColor(58, 31, 87, 255), ImColor(58, 31, 87, 255), ImColor(167, 24, 71, 255));

		long current_time_ms = get_mils();

		ImVec2 text_size = ImGui::CalcTextSize(text);
		ImGui::SetCursorPosY(ImGui::GetWindowHeight() / 2 - text_size.y / 2);
		ImGui::Text(text);

		if (!reverse) {
			if (offset < ImGui::GetWindowWidth())
				offset += (ImGui::GetWindowWidth() + 5) * ((1000.0f / ImGui::GetIO().Framerate) / 100);

			if (offset >= ImGui::GetWindowWidth() && old_time == -1) {
				old_time = current_time_ms;
			}
		}

		if (current_time_ms - old_time >= onScreenMils && old_time != -1)
			reverse = true;

		if (reverse) {
			if (offset > 0)
				offset -= (ImGui::GetWindowWidth() + 5) * ((1000.0f / ImGui::GetIO().Framerate) / 100);
			if (offset <= 0) {
				offset = 0;
				reverse = false;
				*done = true;
				old_time = -1;
				show_popup = false;
			}
		}

		ImGui::End();
	}
}

void c_menu::run() {
	static int page = 0;

	if (opened) {
		ImGui::GetStyle().Colors[ImGuiCol_CheckMark] = ImVec4(167 / 255.f, 24 / 255.f, 71 / 255.f, 1.f);
		ImGui::GetStyle().Colors[ImGuiCol_SliderGrab] = ImVec4(167 / 255.f, 24 / 255.f, 71 / 255.f, 1.f);
		ImGui::GetStyle().Colors[ImGuiCol_SliderGrabActive] = ImVec4(167 / 255.f, 24 / 71, 247 / 255.f, 1.f);

		ImGui::SetNextWindowSize(ImVec2(600, 600), ImGuiSetCond_FirstUseEver);
		ImGui::Begin("dopamine", &opened, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_ShowBorders | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar); {
			ImVec2 p = ImGui::GetCursorScreenPos();
			ImColor c = ImColor(32, 114, 247);

			ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(p.x, p.y + 30), ImVec2(p.x + ImGui::GetWindowWidth(), p.y - 3), ImColor(30, 30, 39));
			ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2(p.x, p.y + 32), ImVec2(p.x + ImGui::GetWindowWidth(), p.y + +30), ImColor(167, 24, 71, 255), ImColor(58, 31, 87, 255), ImColor(58, 31, 87, 255), ImColor(167, 24, 71, 255));
			ImGui::PushFont(font_menu);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 7);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 7);
			ImGui::Text("dopamine");
			ImGui::SameLine();

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 9);
			if (ImGui::ButtonT("rage", ImVec2(40, 30), page, 0, false, false)) page = 0; ImGui::SameLine(0, 0);
			if (ImGui::ButtonT("antiaim", ImVec2(40, 30), page, 1, false, false)) page = 1; ImGui::SameLine(0, 0);
			if (ImGui::ButtonT("visuals", ImVec2(40, 30), page, 2, false, false)) page = 2; ImGui::SameLine(0, 0);
			if (ImGui::ButtonT("misc", ImVec2(40, 30), page, 3, false, false)) page = 3; ImGui::SameLine(0, 0);
			if (ImGui::ButtonT("config", ImVec2(40, 30), page, 4, false, false)) page = 5; ImGui::SameLine(0, 0);
			ImGui::PopFont();

			ImGui::PushFont(font_menu);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 45);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 222);

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));
			ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, ImVec4(30 / 255.f, 30 / 255.f, 39 / 255.f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0 / 255.f, 0 / 255.f, 0 / 255.f, 0.1f));

			if (config_system.item.visuals.visuals_preview) {
				run_visuals_preview();
			}

			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImGui::PopStyleVar();

			static int test = 0;
			switch (page) {
			case 0:
			{
				ImGui::Columns(2, NULL, false);
				ImGui::Dummy(ImVec2(0, -2)); ImGui::SameLine();
				ImGui::Dummy(ImVec2(0, 0)); ImGui::SameLine();
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));
				ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, ImVec4(30 / 255.f, 30 / 255.f, 39 / 255.f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0 / 255.f, 0 / 255.f, 0 / 255.f, 0.1f));

				ImGui::BeginChild("aimbot", ImVec2(279, 268), true); {
					ImGui::Checkbox("enable ragebot", &config_system.item.ragebot.enableaimbot);
					ImGui::Checkbox("automatic fire", &config_system.item.ragebot.automaticfire);

					if (ImGui::BeginCombo("hitbox", "...", ImVec2(0, 105))) {
						ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 8);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(("head"), &config_system.item.ragebot.hitbox_head, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(("chest"), &config_system.item.ragebot.hitbox_chest, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(("stomach"), &config_system.item.ragebot.hitbox_stomach, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(("arms"), &config_system.item.ragebot.hitbox_arms, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(("legs"), &config_system.item.ragebot.hitbox_legs, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(("feet"), &config_system.item.ragebot.hitbox_feet, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::EndCombo();
					}

					ImGui::Checkbox("avoid limbs if moving", &config_system.item.ragebot.avoid_limbs_moving);
					ImGui::Checkbox("avoid head if jumping", &config_system.item.ragebot.avoid_head_jumping);

					if (ImGui::BeginCombo("multi-point", "...", ImVec2(0, 105))) {
						ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 8);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(("head"), &config_system.item.ragebot.multipoint_head, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(("body"), &config_system.item.ragebot.multipoint_body, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::EndCombo();
					}

					if (config_system.item.ragebot.multipoint_head)
						ImGui::SliderFloat("head scale", &config_system.item.ragebot.multipoint_head_scale, 0.0f, 100.0f, "%.1f");
					if (config_system.item.ragebot.multipoint_body)
						ImGui::SliderFloat("body scale", &config_system.item.ragebot.multipoint_body_scale, 0.0f, 100.0f, "%.1f");

					ImGui::Checkbox("hitchance", &config_system.item.ragebot.hitchance_enable);
					ImGui::SliderInt("amount##hitchance", &config_system.item.ragebot.hitchance_amount, 0, 100);

					ImGui::SliderInt("minimum damage", &config_system.item.ragebot.minimum_damage, 0, 100);

					ImGui::PopStyleColor();

				} ImGui::EndChild(true);

				ImGui::PopStyleVar();
				ImGui::Dummy(ImVec2(0, -2)); ImGui::SameLine();
				ImGui::Dummy(ImVec2(0, 0)); ImGui::SameLine();
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));

				ImGui::BeginChild("other", ImVec2(279, 268), true); {
					ImGui::Checkbox("force bodyaim", &config_system.item.ragebot.force_bodyaim);
					ImGui::Checkbox("backtrack", &config_system.item.ragebot.backtrack);
					if (config_system.item.ragebot.backtrack)
						ImGui::Combo("type##backtrack", &config_system.item.ragebot.backtrack_type, "last\0all");

					ImGui::Checkbox("bodyaim if lethal", &config_system.item.ragebot.bodyaim_if_lethal);
					ImGui::Checkbox("bodyaim if in air", &config_system.item.ragebot.bodyaim_if_inair);
					ImGui::Checkbox("bodyaim if missed shots", &config_system.item.ragebot.bodyaim_if_missedshots);

					if (config_system.item.ragebot.bodyaim_if_missedshots)
						ImGui::SliderInt("shots##baimmissedshots", &config_system.item.ragebot.bodyaim_missedshots, 1, 10);

					ImGui::Checkbox("antiaim correction", &config_system.item.ragebot.antiaim_correction);
					ImGui::Checkbox("automatic scope", &config_system.item.ragebot.automatic_scope);
					ImGui::Checkbox("automatic stop", &config_system.item.ragebot.automatic_stop);
					ImGui::Combo("type##automaticstop", &config_system.item.ragebot.automatic_stop_type, "minimum\0maximum");

					ImGui::PopStyleColor();

				} ImGui::EndChild(true);

				ImGui::NextColumn();

				ImGui::BeginChild("fake-lag", ImVec2(279, 267), true); {
					ImGui::Checkbox("enable fakelag", &config_system.item.ragebot.fakelag);
					if (config_system.item.ragebot.fakelag)
					{
						if (ImGui::BeginCombo("activation", "...", ImVec2(0, 105))) {
							ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 8);
							ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
							ImGui::Selectable(("standing"), &config_system.item.ragebot.fakelag_standing, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
							ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
							ImGui::Selectable(("moving"), &config_system.item.ragebot.fakelag_moving, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
							ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
							ImGui::Selectable(("in air"), &config_system.item.ragebot.fakelag_inair, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
							ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
							ImGui::Selectable(("shooting"), &config_system.item.ragebot.fakelag_shooting, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
							ImGui::EndCombo();
						}
						ImGui::Combo("type##fakelag", &config_system.item.ragebot.fakelag_type, "factor\0switch\0adaptive\0random");
						ImGui::SliderInt("ticks##fakelag", &config_system.item.ragebot.fakelag_ticks, 1, 14);
						ImGui::Checkbox("fakeduck", &config_system.item.ragebot.fakeduck);
					}
					ImGui::PopStyleColor();
				}
				ImGui::EndChild(true);
			}
			break;
			case 1:
			{
				ImGui::Columns(2, NULL, false);
				ImGui::Dummy(ImVec2(0, -2)); ImGui::SameLine();
				ImGui::Dummy(ImVec2(0, 0)); ImGui::SameLine();
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));
				ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, ImVec4(30 / 255.f, 30 / 255.f, 39 / 255.f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0 / 255.f, 0 / 255.f, 0 / 255.f, 0.1f));

				ImGui::BeginChild("settings", ImVec2(279, 268), true); {
					ImGui::Checkbox("enable antiaim", &config_system.item.antiaim.enable_antiaim);
					ImGui::Combo("desync type", &config_system.item.antiaim.desync_type, "none\0 1 cmd\0 2 cmd\0 1 ocmd \0 2 ocmd");
					ImGui::Checkbox("slowmotion", &config_system.item.antiaim.slowmotion);
				}
				ImGui::EndChild(true);

				ImGui::PopStyleVar();
				ImGui::Dummy(ImVec2(0, -2)); ImGui::SameLine();
				ImGui::Dummy(ImVec2(0, 0)); ImGui::SameLine();
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));

				ImGui::BeginChild("standing##standing", ImVec2(279, 267), true); {
					ImGui::Combo("pitch##standing", &config_system.item.antiaim.standing_pitch, "none\0down\0up\0zero\0jitter");
					ImGui::Combo("yaw##standing", &config_system.item.antiaim.standing_yaw, "none\0backwards\0sideways");
					ImGui::Combo("jitter type##standing", &config_system.item.antiaim.standing_jitter_type, "none\0offset\0random");
					if (config_system.item.antiaim.standing_jitter_type == 1)
						ImGui::SliderInt("range##standing_jitter", &config_system.item.antiaim.standing_jitter_range, 0, 180);

					ImGui::Checkbox("auto direction##standing", &config_system.item.antiaim.standing_autodirection);
					ImGui::PopStyleColor();
				}
				ImGui::EndChild(true);

				ImGui::NextColumn();

				ImGui::BeginChild("moving##moving", ImVec2(279, 268), true); {
					ImGui::Combo("pitch##moving", &config_system.item.antiaim.moving_pitch, "none\0down\0up\0zero\0jitter");
					ImGui::Combo("yaw##moving", &config_system.item.antiaim.moving_yaw, "none\0backwards\0sideways");
					ImGui::Combo("jitter type##moving", &config_system.item.antiaim.moving_jitter_type, "none\0offset\0random");
					if (config_system.item.antiaim.moving_jitter_type == 1)
						ImGui::SliderInt("range##moving_jitter", &config_system.item.antiaim.moving_jitter_range, 0, 180);

					ImGui::Checkbox("auto direction##moving", &config_system.item.antiaim.moving_autodirection);
				}
				ImGui::EndChild(true);

				ImGui::Spacing();

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 8);

				ImGui::BeginChild("in-air", ImVec2(279, 267), true); {
					ImGui::Combo("pitch##inair", &config_system.item.antiaim.inair_pitch, "none\0down\0up\0zero\0jitter");
					ImGui::Combo("yaw##inair", &config_system.item.antiaim.inair_yaw, "none\0backwards\0sideways");
					ImGui::Combo("jitter type##inair", &config_system.item.antiaim.inair_jitter_type, "none\0offset\0random");
					if (config_system.item.antiaim.inair_jitter_type == 1)
						ImGui::SliderInt("range##inair_jitter", &config_system.item.antiaim.inair_jitter_range, 0, 180);

					ImGui::Checkbox("auto direction##inair", &config_system.item.antiaim.inair_autodirection);
				}
				ImGui::EndChild(true);

				ImGui::PopStyleVar();
				ImGui::PopStyleColor();
				ImGui::PopStyleColor();
				ImGui::Columns();
			}
			break;
			case 2:
			{
				ImGui::Columns(2, NULL, false);
				ImGui::Dummy(ImVec2(0, -2)); ImGui::SameLine();
				ImGui::Dummy(ImVec2(0, 0)); ImGui::SameLine();
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));
				ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, ImVec4(30 / 255.f, 30 / 255.f, 39 / 255.f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0 / 255.f, 0 / 255.f, 0 / 255.f, 0.1f));

				ImGui::BeginChild("player", ImVec2(279, 268), true); {
					ImGui::Checkbox("enable visuals", &config_system.item.visuals.enable_visuals);
					if (config_system.item.visuals.enable_visuals) {
						ImGui::Combo("type##visuals", &config_system.item.visuals.visuals_type, "default\0alternative");
						ImGui::Checkbox("teammate", &config_system.item.visuals.team_check);
						ImGui::Checkbox("dormant", &config_system.item.visuals.dormant_check);
					}
					ImGui::Checkbox("bounding box", &config_system.item.visuals.player_box);
					ImGui::ColorEdit4("box color", config_system.item.visuals.clr_box, ImGuiColorEditFlags_NoInputs);
					ImGui::Checkbox("health bar", &config_system.item.visuals.player_health);
					if (config_system.item.visuals.player_health)
						ImGui::Checkbox("health text", &config_system.item.visuals.player_health_text);
					ImGui::Checkbox("name", &config_system.item.visuals.player_name);
					ImGui::ColorEdit4("name color", config_system.item.visuals.clr_name, ImGuiColorEditFlags_NoInputs);

					ImGui::PushStyleColor(ImGuiCol_Text, config_system.item.visuals.enable_visuals ? ImVec4(1.f, 1.f, 1.f, 1) : ImVec4(.6f, .6f, .6f, 1));
					if (ImGui::BeginCombo("flags", "...", ImVec2(0, 105))) {
						ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 8);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(("money"), &config_system.item.visuals.player_flags_money, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(("armor"), &config_system.item.visuals.player_flags_armor, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(("scoped"), &config_system.item.visuals.player_flags_scoped, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(("defusing"), &config_system.item.visuals.player_flags_defusing, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(("rescuing"), &config_system.item.visuals.player_flags_rescuing, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(("reloading"), &config_system.item.visuals.player_flags_reloading, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(("bomb"), &config_system.item.visuals.player_flags_c4, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(("flashed"), &config_system.item.visuals.player_flags_flashed, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(("fake-ducking"), &config_system.item.visuals.player_flags_fakeducking, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);

						ImGui::EndCombo();
					}
					ImGui::PopStyleColor();

					ImGui::Checkbox("weapon text", &config_system.item.visuals.player_weapon_text);
					ImGui::Checkbox("weapon icon", &config_system.item.visuals.player_weapon_icon);
					ImGui::ColorEdit4("weapon icon color", config_system.item.visuals.clr_weapon_icon, ImGuiColorEditFlags_NoInputs);

					ImGui::Checkbox("weapon ammo", &config_system.item.visuals.player_ammo);
					ImGui::ColorEdit4("weapon ammo color", config_system.item.visuals.clr_ammo, ImGuiColorEditFlags_NoInputs);

					ImGui::Checkbox("ammo bar", &config_system.item.visuals.player_ammo_bar);
					ImGui::ColorEdit4("ammo bar color", config_system.item.visuals.clr_ammo_bar, ImGuiColorEditFlags_NoInputs);

					//ImGui::Checkbox("footstep", &config_system.item.visuals.sound_footstep);
					//ImGui::ColorEdit4("footstep color", config_system.item.visuals.clr_footstep, ImGuiColorEditFlags_NoInputs);
					ImGui::Checkbox("skeleton", &config_system.item.visuals.skeleton);
					ImGui::ColorEdit4("skeleton color", config_system.item.visuals.clr_skeleton, ImGuiColorEditFlags_NoInputs);
					ImGui::Checkbox("backtrack skeleton", &config_system.item.visuals.backtrack_skeleton);
					ImGui::ColorEdit4("skeleton backtrack color", config_system.item.visuals.clr_backtrack_skeleton, ImGuiColorEditFlags_NoInputs);

					ImGui::Checkbox("out of fov arrows", &config_system.item.visuals.out_of_fov_arrow);
					if (config_system.item.visuals.out_of_fov_arrow)
					{
						ImGui::ColorEdit4("out of fov color", config_system.item.visuals.clr_out_of_fov, ImGuiColorEditFlags_NoInputs);

						ImGui::SliderInt("arrow size", &config_system.item.visuals.out_of_fov_arrow_size, 5, 25);
						ImGui::SliderInt("arrow radius", &config_system.item.visuals.out_of_fov_arrow_radius, 10, 100);
					}

					ImGui::Checkbox("preview visuals", &config_system.item.visuals.visuals_preview);
				}
				ImGui::EndChild(true);

				ImGui::PopStyleVar();
				ImGui::Dummy(ImVec2(0, -2)); ImGui::SameLine();
				ImGui::Dummy(ImVec2(0, 0)); ImGui::SameLine();
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));

				ImGui::BeginChild("effects", ImVec2(279, 267), true); {
					ImGui::PushStyleColor(ImGuiCol_Text, config_system.item.visuals.enable_visuals ? ImVec4(1.f, 1.f, 1.f, 1) : ImVec4(.6f, .6f, .6f, 1));
					ImGui::SliderInt("override fov", &config_system.item.visuals.override_fov, 30, 120);
					ImGui::Checkbox("force crosshair", &config_system.item.visuals.force_crosshair);
					ImGui::Checkbox("recoil crosshair", &config_system.item.visuals.recoil_crosshair);
					ImGui::Checkbox("autowall indicator", &config_system.item.visuals.autowall_indicator);
					ImGui::Checkbox("damage indicator", &config_system.item.visuals.damage_indicator);

					ImGui::Checkbox("inaccuracy overlay", &config_system.item.visuals.inaccuracy_overlay);
					if (config_system.item.visuals.inaccuracy_overlay)
					{
						ImGui::ColorEdit4("overlay color##inaccuracy", config_system.item.visuals.clr_inaccuracy_overlay, ImGuiColorEditFlags_NoInputs);
						ImGui::SliderInt("size##inaccuracy", &config_system.item.visuals.inaccuracy_overlay_size, 1, 100);
					}

					ImGui::Checkbox("grenade prediction", &config_system.item.visuals.grenade_prediction);
					ImGui::ColorEdit4("grenade prediction color", config_system.item.visuals.clr_grenade_prediction, ImGuiColorEditFlags_NoInputs);

					ImGui::PopStyleColor();

					ImGui::PushStyleColor(ImGuiCol_Text, config_system.item.visuals.enable_visuals ? ImVec4(1.f, 1.f, 1.f, 1) : ImVec4(.6f, .6f, .6f, 1));
					if (ImGui::BeginCombo("modulation", "...", ImVec2(0, 105))) {
						ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 8);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(("night"), &config_system.item.visuals.modulation_night, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(("fullbright"), &config_system.item.visuals.modulation_fullbright, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::EndCombo();
					}

					if (ImGui::BeginCombo("dropped weapons", "...", ImVec2(0, 105))) {
						ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 8);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(("name"), &config_system.item.visuals.droppedweapons_name, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(("ammo"), &config_system.item.visuals.droppedweapons_ammo, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::EndCombo();
					}

					if (config_system.item.visuals.modulation_night || config_system.item.visuals.modulation_fullbright) {
						ImGui::SliderInt("amount##modulation", &config_system.item.visuals.modulation_amount, 0, 100);
					}

					if (ImGui::BeginCombo("removals", "...", ImVec2(0, 105))) {
						ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 8);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(("smoke"), &config_system.item.visuals.removals_smoke, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(("recoil"), &config_system.item.visuals.removals_recoil, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(("flash"), &config_system.item.visuals.removals_flash, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(("sleeves"), &config_system.item.visuals.removals_sleeves, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(("hands"), &config_system.item.visuals.removals_hands, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(("scope overlay"), &config_system.item.visuals.removals_scope, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(("scope zoom"), &config_system.item.visuals.removals_scope, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::EndCombo();
					}

					ImGui::Checkbox("hit marker", &config_system.item.visuals.hitmarker);

					if (config_system.item.visuals.hitmarker)
						ImGui::Combo("hit sound", &config_system.item.visuals.hitmarker_sound, "none\0switch press\0bubble\0cod");

					ImGui::Checkbox("thirdperson", &config_system.item.visuals.thirdperson);
					ImGui::Checkbox("spectators list", &config_system.item.visuals.spectators_list);

					ImGui::Checkbox("bullet tracer", &config_system.item.visuals.bullettracer_tracer);
					ImGui::ColorEdit4("bullet tracer color", config_system.item.visuals.clr_bullettracer_tracer, ImGuiColorEditFlags_NoInputs);
					ImGui::Checkbox("bullet impact", &config_system.item.visuals.bullettracer_impact);
					ImGui::ColorEdit4("bullet impact color", config_system.item.visuals.clr_bullettracer_impact, ImGuiColorEditFlags_NoInputs);

					ImGui::PopStyleColor();
				}
				ImGui::EndChild(true);

				ImGui::NextColumn();

				ImGui::BeginChild("glow", ImVec2(279, 268), true); {
					ImGui::Checkbox("enable glow", &config_system.item.visuals.enable_glow);
					ImGui::Checkbox("enemy##glow", &config_system.item.visuals.glow_enemy);
					ImGui::ColorEdit4("glow color enemy", config_system.item.visuals.clr_glow, ImGuiColorEditFlags_NoInputs);
					ImGui::Checkbox("teammate##glow", &config_system.item.visuals.glow_team);
					ImGui::ColorEdit4("glow color team", config_system.item.visuals.clr_glow_team, ImGuiColorEditFlags_NoInputs);
					ImGui::Checkbox("local##glow", &config_system.item.visuals.glow_local);
					ImGui::ColorEdit4("glow color local", config_system.item.visuals.clr_glow_local, ImGuiColorEditFlags_NoInputs);
					ImGui::Checkbox("planted bomb##glow", &config_system.item.visuals.glow_planted);
					ImGui::ColorEdit4("glow color planted", config_system.item.visuals.clr_glow_planted, ImGuiColorEditFlags_NoInputs);
					ImGui::Checkbox("dropped weapons##glow", &config_system.item.visuals.glow_dropped_weapons);
					ImGui::ColorEdit4("glow color weapons", config_system.item.visuals.clr_glow_dropped_weapons, ImGuiColorEditFlags_NoInputs);
				}
				ImGui::EndChild(true);

				ImGui::Spacing();

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 8);

				ImGui::BeginChild("chams", ImVec2(279, 267), true); {
					ImGui::PushStyleColor(ImGuiCol_Text, config_system.item.visuals.enable_visuals ? ImVec4(1.f, 1.f, 1.f, 1) : ImVec4(.6f, .6f, .6f, 1));
					ImGui::Combo("type##chams", &config_system.item.visuals.chams_type, "textured\0flat\0metallic\0pulsating");
					ImGui::PopStyleColor();
					ImGui::Checkbox("enemy##chams", &config_system.item.visuals.chams_enemy_vis);
					ImGui::ColorEdit4("enemy color##chams", config_system.item.visuals.clr_chams_enemy_vis, ImGuiColorEditFlags_NoInputs);
					ImGui::Checkbox("enemy (behind wall)##chams", &config_system.item.visuals.chams_enemy_invis);
					ImGui::ColorEdit4("enemy (behind wall) color##chams", config_system.item.visuals.clr_chams_enemy_invis, ImGuiColorEditFlags_NoInputs);

					ImGui::Checkbox("teammate##chams", &config_system.item.visuals.chams_team_vis);
					ImGui::ColorEdit4("teammate color##chams", config_system.item.visuals.clr_chams_team_vis, ImGuiColorEditFlags_NoInputs);
					ImGui::Checkbox("teammate (behind wall)##chams", &config_system.item.visuals.chams_team_invis);
					ImGui::ColorEdit4("teammate (behind wall) color##chams", config_system.item.visuals.clr_chams_team_invis, ImGuiColorEditFlags_NoInputs);

					ImGui::Checkbox("local##chams", &config_system.item.visuals.chams_local);
					ImGui::ColorEdit4("local color##chams", config_system.item.visuals.clr_chams_local, ImGuiColorEditFlags_NoInputs);
					ImGui::Checkbox("desync##chams", &config_system.item.visuals.chams_local_desync);
					ImGui::ColorEdit4("desync color##chams", config_system.item.visuals.clr_chams_local_desync, ImGuiColorEditFlags_NoInputs);
					ImGui::Checkbox("backtrack##chams", &config_system.item.visuals.chams_backtrack_visualize);
					ImGui::ColorEdit4("backtrack color##chams", config_system.item.visuals.clr_chams_backtrack_visualize, ImGuiColorEditFlags_NoInputs);
					ImGui::Checkbox("smoke check##chams", &config_system.item.visuals.chams_smoke_check);

					ImGui::Checkbox("hands##chams", &config_system.item.visuals.chams_hands);
					ImGui::ColorEdit4("hands color##chams", config_system.item.visuals.clr_chams_hands, ImGuiColorEditFlags_NoInputs);
					ImGui::Checkbox("weapons##chams", &config_system.item.visuals.chams_weapons);
					ImGui::ColorEdit4("weapons color##chams", config_system.item.visuals.clr_chams_weapons, ImGuiColorEditFlags_NoInputs);
				}
				ImGui::EndChild(true);

				ImGui::PopStyleVar();
				ImGui::PopStyleColor();
				ImGui::PopStyleColor();
				ImGui::Columns();
			}
			break;
			case 3:
			{
				ImGui::Columns(2, NULL, false);
				ImGui::Dummy(ImVec2(0, -2)); ImGui::SameLine();
				ImGui::Dummy(ImVec2(0, 0)); ImGui::SameLine();
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));
				ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, ImVec4(30 / 255.f, 30 / 255.f, 39 / 255.f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0 / 255.f, 0 / 255.f, 0 / 255.f, 0.1f));

				ImGui::Checkbox("anti-untrusted", &config_system.item.misc.anti_untrusted);

				ImGui::BeginChild("misc", ImVec2(279, 543), true); {
					ImGui::Checkbox("enable misc", &config_system.item.misc.enable_misc);
					ImGui::Checkbox("clantag spammer", &config_system.item.misc.clantag_spammer);

					ImGui::PushStyleColor(ImGuiCol_Text, config_system.item.misc.enable_misc ? ImVec4(1.f, 1.f, 1.f, 1) : ImVec4(.6f, .6f, .6f, 1));
					if (ImGui::BeginCombo("logs", "...", ImVec2(0, 65))) {
						ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 8);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(("player hurt"), &config_system.item.misc.logs_player_hurt, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(("player purchase"), &config_system.item.misc.logs_player_bought, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::EndCombo();
					}
					ImGui::PopStyleColor();

					ImGui::Checkbox("spectators", &config_system.item.misc.spectators_list);
					ImGui::Checkbox("watermark", &config_system.item.misc.watermark);

					ImGui::SliderInt("aspect ratio w", &config_system.item.misc.aspect_ratio_w, 1, 3840);
					ImGui::SliderInt("aspect ratio h", &config_system.item.misc.aspect_ratio_h, 1, 2160);

					ImGui::Checkbox("viewmodel offset", &config_system.item.misc.viewmodel_offset);
					if (config_system.item.misc.viewmodel_offset) {
						ImGui::SliderInt("viewmodel x", &config_system.item.misc.viewmodel_x, -10, 10);
						ImGui::SliderInt("viewmodel y", &config_system.item.misc.viewmodel_y, -10, 10);
						ImGui::SliderInt("viewmodel z", &config_system.item.misc.viewmodel_z, -10, 10);

					}

					ImGui::PushStyleColor(ImGuiCol_Text, config_system.item.misc.enable_misc ? ImVec4(1.f, 1.f, 1.f, 1) : ImVec4(.6f, .6f, .6f, 1));
					ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 18);
					if (ImGui::Button("dump steam id", ImVec2(84, 18))) {
						//utilities::dump_steam_id();
					}

					ImGui::SameLine();
					if (ImGui::Button("hide name", ImVec2(84, 18))) {
						//utilities::change_name("\n\xAD\xAD\xAD");
					}
					ImGui::PopStyleColor();

				}
				ImGui::EndChild(true);
				ImGui::NextColumn();

				ImGui::BeginChild("movement", ImVec2(279, 543), true); {
					ImGui::Checkbox("bunny hop", &config_system.item.misc.bunny_hop);
					if (config_system.item.misc.bunny_hop) {
						ImGui::SliderInt("hitchance", &config_system.item.misc.bunny_hop_hitchance, 0, 100);
						ImGui::SliderInt("minimum hops", &config_system.item.misc.bunny_hop_minimum_value, 0, 20);
						ImGui::SliderInt("maximum hops", &config_system.item.misc.bunny_hop_maximum_value, 0, 20);
					}

					ImGui::Checkbox("remove duck delay", &config_system.item.misc.remove_duck_delay);

					ImGui::Checkbox("auto strafe", &config_system.item.misc.auto_strafe);
					if (config_system.item.misc.auto_strafe)
					{
						ImGui::SliderInt("speed##autostrafe", &config_system.item.misc.auto_strafe_speed, 0, 15);
						ImGui::Combo("type##autostrafe", &config_system.item.misc.auto_strafe_wasd, "default\0wasd");
					}

					ImGui::Checkbox("edge jump", &config_system.item.misc.edge_jump);
					if (config_system.item.misc.edge_jump) {
						ImGui::Checkbox("duck in air", &config_system.item.misc.edge_jump_duck_in_air);
					}
				}
				ImGui::EndChild(true);
				ImGui::PopStyleVar();
				ImGui::PopStyleColor();
				ImGui::PopStyleColor();
				ImGui::Columns();
			}
			break;
			case 4:
			{
				ImGui::Columns(2, NULL, false);
				ImGui::Dummy(ImVec2(0, -2)); ImGui::SameLine();
				ImGui::Dummy(ImVec2(0, 0)); ImGui::SameLine();
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));
				ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, ImVec4(30 / 255.f, 30 / 255.f, 39 / 255.f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0 / 255.f, 0 / 255.f, 0 / 255.f, 0.1f));
				ImGui::BeginChild("knife", ImVec2(279, 268), true); {
					/*ImGui::Checkbox("enable", &config_system.item.skinchanger_enable);
					ImGui::PushStyleColor(ImGuiCol_Text, config_system.item.skinchanger_enable ? ImVec4(1.f, 1.f, 1.f, 1) : ImVec4(.6f, .6f, .6f, 1));
					ImGui::Combo("knife", &config_system.item.knife_model, "default\0bayonet\0m9\0karambit\0bowie\0butterfly\0falchion\0flip\0gut\0huntsman\0shaddow daggers\0navaja\0stiletto\0talon\0ursus");
					ImGui::Combo("condition", &config_system.item.knife_wear, "factory new\0minimal wear\0field-tested\0well-worn\0battle-scarred");

					ImGui::Combo(("skin"), &config_system.item.paint_kit_vector_index_knife, [](void* data, int idx, const char** out_text) {
						*out_text = parser_skins[idx].name.c_str();
						return true;
						}, nullptr, parser_skins.size(), 10);
					config_system.item.paint_kit_index_knife = parser_skins[config_system.item.paint_kit_vector_index_knife].id;*/
					ImGui::PopStyleColor();

				}
				ImGui::EndChild(true);

				ImGui::PopStyleVar();
				ImGui::Dummy(ImVec2(0, -2)); ImGui::SameLine();
				ImGui::Dummy(ImVec2(0, 0)); ImGui::SameLine();
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));

				ImGui::BeginChild("settings", ImVec2(279, 267), true); {
					ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 18);
					if (ImGui::Button("force update", ImVec2(84, 18))) {
						//utilities::force_update();
					}

				}
				ImGui::EndChild(true);

				ImGui::NextColumn();

				ImGui::BeginChild("weapons", ImVec2(279, 543), true); {

					ImGui::PopStyleColor();
				}
				ImGui::EndChild(true);
				ImGui::PopStyleVar();
				ImGui::PopStyleColor();
				ImGui::PopStyleColor();
				ImGui::Columns();
			}
			break;
			case 5:
			{
				ImGui::Columns(2, NULL, false);
				ImGui::Dummy(ImVec2(0, -2)); ImGui::SameLine();
				ImGui::Dummy(ImVec2(0, 0)); ImGui::SameLine();
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));
				ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, ImVec4(30 / 255.f, 30 / 255.f, 39 / 255.f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0 / 255.f, 0 / 255.f, 0 / 255.f, 0.1f));
				ImGui::BeginChild("config", ImVec2(279, 268), true); {
					constexpr auto& config_items = config_system.get_configs();
					static int current_config = -1;

					if (static_cast<size_t>(current_config) >= config_items.size())
						current_config = -1;

					static char buffer[16];

					if (ImGui::ListBox("", &current_config, [](void* data, int idx, const char** out_text) {
						auto& vector = *static_cast<std::vector<std::string>*>(data);
						*out_text = vector[idx].c_str();
						return true;
						}, &config_items, config_items.size(), 5) && current_config != -1)
						strcpy(buffer, config_items[current_config].c_str());

						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 18);
						ImGui::PushID(0);
						ImGui::PushItemWidth(178);
						if (ImGui::InputText("", buffer, IM_ARRAYSIZE(buffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
							if (current_config != -1)
								config_system.rename(current_config, buffer);
						}
						ImGui::PopID();
						ImGui::NextColumn();

						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 18);
						if (ImGui::Button(("create"), ImVec2(85, 20))) {
							config_system.add(buffer);

							//if (config_system.item.logs_config_system) 
							{
								//utilities::console_warning("[config system] ");
								g_Interfaces->cvar->ConsolePrintf(config_items[current_config].c_str());
								g_Interfaces->cvar->ConsolePrintf(" created. \n");
								std::stringstream ss;
								ss << config_items[current_config].c_str() << "  created.";
								//event_logs.add(ss.str(), Color(167, 255, 255, 255));
							}

						}

						ImGui::SameLine();

						if (ImGui::Button(("reset"), ImVec2(85, 20))) {
							config_system.reset();

							//if (config_system.item.logs_config_system) 
							{
								//utilities::console_warning("[config system] ");
								g_Interfaces->cvar->ConsolePrintf(config_items[current_config].c_str());
								g_Interfaces->cvar->ConsolePrintf(" restored to default values. \n");
								std::stringstream ss;
								ss << config_items[current_config].c_str() << "  restored to default values.";
								//event_logs.add(ss.str(), Color(167, 255, 255, 255));
							}
						}

						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 18);
						if (current_config != -1) {
							if (ImGui::Button(("load"), ImVec2(85, 20))) {
								config_system.load(current_config);

								load_config = true;

								//if (config_system.item.logs_config_system) 
								{
									//utilities::console_warning("[config system] ");
									g_Interfaces->cvar->ConsolePrintf(config_items[current_config].c_str());
									g_Interfaces->cvar->ConsolePrintf(" loaded. \n");
									std::stringstream ss;
									ss << config_items[current_config].c_str() << " loaded.";
									//event_logs.add(ss.str(), Color(167, 255, 255, 255));
								}

							}

							ImGui::SameLine();

							if (ImGui::Button(("save"), ImVec2(85, 20))) {
								config_system.save(current_config);

								save_config = true;

								//if (config_system.item.logs_config_system) 
								{
									//utilities::console_warning("[config system] ");
									g_Interfaces->cvar->ConsolePrintf(config_items[current_config].c_str());
									g_Interfaces->cvar->ConsolePrintf(" saved. \n");
									std::stringstream ss;
									ss << config_items[current_config].c_str() << " saved.";
									//event_logs.add(ss.str(), Color(167, 255, 255, 255));
								}

							}

							ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 18);
							if (ImGui::Button(("remove"), ImVec2(85, 20))) {
								config_system.remove(current_config);

								//if (config_system.item.logs_config_system) 
								{
									//utilities::console_warning("[config system] ");
									g_Interfaces->cvar->ConsolePrintf(config_items[current_config].c_str());
									g_Interfaces->cvar->ConsolePrintf(" removed. \n");
									std::stringstream ss;
									ss << config_items[current_config].c_str() << " removed.";
									//event_logs.add(ss.str(), Color(167, 255, 255, 255));
								}
							}

							if (ImGui::Button(("unload cheat"), ImVec2(85, 20))) {
								config_system.item.config.unload_cheat = true;
							}
						}
				}
				ImGui::EndChild(true);

				ImGui::PopStyleVar();
				ImGui::Dummy(ImVec2(0, -2)); ImGui::SameLine();
				ImGui::Dummy(ImVec2(0, 0)); ImGui::SameLine();
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));

				ImGui::BeginChild("settings", ImVec2(279, 267), true); {
					ImGui::Combo("keybinds", &config_system.item.config.keybinds_selection, "force bodyaim\0fakeduck\0slowmotion\0antiaim flip\0thirdperson");

					if (config_system.item.config.keybinds_selection == 0) {
						ImGui::Hotkey("##force baim key", &config_system.item.config.bind_bodyaim_key, ImVec2(100, 20));
					}
					else if (config_system.item.config.keybinds_selection == 1) {
						ImGui::Hotkey("##fakeduck key", &config_system.item.config.bind_fakeduck_key, ImVec2(100, 20));
					}
					else if (config_system.item.config.keybinds_selection == 2) {
						ImGui::Hotkey("##slowmotion key", &config_system.item.config.bind_slowmotion_key, ImVec2(100, 20));
					}
					else if (config_system.item.config.keybinds_selection == 3) {
						ImGui::Hotkey("##antiaim flip key", &config_system.item.config.bind_antiaimflip_key, ImVec2(100, 20));
					}
					else if (config_system.item.config.keybinds_selection == 4) {
						ImGui::Hotkey("##thirdperson key", &config_system.item.config.bind_thirdperson_key, ImVec2(100, 20));
					}
					else if (config_system.item.config.keybinds_selection == 5) {
						ImGui::Hotkey("##edgejump key", &config_system.item.config.bind_edgejump_key, ImVec2(100, 20));
					}
				}
				ImGui::EndChild(true);

				ImGui::NextColumn();

				ImGui::BeginChild("info", ImVec2(279, 543), true); {
					char buffer[UNLEN + 1];
					DWORD size;
					size = sizeof(buffer);
					GetUserName(buffer, &size);
					char title[UNLEN];
					char ch1[25] = "welcome, ";
					char* ch = strcat(ch1, buffer);

					ImGui::Text(ch);
					ImGui::Text("build: " __DATE__ " / " __TIME__);
					if (std::strstr(GetCommandLineA(), "-insecure")) {
						ImGui::Text("insecure mode!");
					}
				}
				ImGui::EndChild(true);
				ImGui::PopStyleVar();
				ImGui::PopStyleColor();
				ImGui::PopStyleColor();
				ImGui::Columns();
			}
			break;
			}
		}

		ImGui::PopFont();

		ImGui::End();
	}
}

void c_menu::run_popup() {
	ImGui::PushFont(font_menu);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));
	ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, ImVec4(30 / 255.f, 30 / 255.f, 39 / 255.f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0 / 255.f, 0 / 255.f, 0 / 255.f, 0.1f));

	if (save_config) {
		bool done = false;
		ImGui::begin_popup("config saved.", 2000, &done);
		if (done)
			save_config = false;
	}

	if (load_config) {
		bool done = false;
		ImGui::begin_popup("config loaded.", 2000, &done);
		if (done)
			load_config = false;
	}

	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar();
	ImGui::PopFont();
}

void c_menu::run_visuals_preview() {
	static std::vector<esp_info_s> info;
	static bool enabled = true;

	ImGui::SetNextWindowSize(ImVec2(235, 400));

	ImGui::Begin("ESP Preview", &enabled, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_ShowBorders | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar); {
		const auto cur_window = ImGui::GetCurrentWindow();
		const ImVec2 w_pos = cur_window->Pos;

		ImVec2 p = ImGui::GetCursorScreenPos();
		ImColor c = ImColor(32, 114, 247);

		//title bar
		ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(p.x - 20, p.y + 15), ImVec2(p.x + ImGui::GetWindowWidth(), p.y - 40), ImColor(30, 30, 39));

		//draw gradient bellow title bar
		ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2(p.x - 20, p.y + 14), ImVec2(p.x + ImGui::GetWindowWidth(), p.y + 16), ImColor(167, 24, 71, 255), ImColor(58, 31, 87, 255), ImColor(58, 31, 87, 255), ImColor(167, 24, 71, 255));

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 9); //gora, dol
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 8); //lewo prawo
		//render cheat name
		ImGui::Text("visuals preview");

		if (config_system.item.visuals.player_box) {
			cur_window->DrawList->AddRect(ImVec2(w_pos.x + 40, w_pos.y + 60), ImVec2(w_pos.x + 200, w_pos.y + 360), ImGui::GetColorU32(ImGuiCol_Text));
		}

		if (config_system.item.visuals.player_health)
			cur_window->DrawList->AddRectFilled(ImVec2(w_pos.x + 34, w_pos.y + 60), ImVec2(w_pos.x + 36, w_pos.y + 360), ImGui::GetColorU32(ImVec4(83 / 255.f, 200 / 255.f, 84 / 255.f, 255 / 255.f)));

		if (config_system.item.visuals.player_name)
			info.emplace_back(esp_info_s("name", Color(255, 255, 255, 255), CENTER_UP));

		if (config_system.item.visuals.player_weapon_text)
			info.emplace_back(esp_info_s("awp", Color(255, 255, 255, 255), CENTER_DOWN));

		if (config_system.item.visuals.player_flags_armor)
			info.emplace_back(esp_info_s("hk", Color(255, 255, 255, 255), RIGHT));


		for (auto render : info) {
			const auto text_size = ImGui::CalcTextSize(render.f_name.c_str());

			auto pos = ImVec2(w_pos.x + 205, w_pos.y + 60);

			if (render.f_position == CENTER_DOWN) {
				pos = ImVec2(w_pos.x + (240 / 2) - text_size.x / 2, pos.y + 315 - text_size.y);
			}
			else if (render.f_position == CENTER_UP) {
				pos = ImVec2(w_pos.x + (240 / 2) - text_size.x / 2, pos.y - 5 - text_size.y);
			}

			cur_window->DrawList->AddText(pos, ImGui::GetColorU32(ImVec4(255 / 255.f, 255 / 255.f, 255 / 255.f, 255 / 255.f)), render.f_name.c_str());
		}
	}
	ImGui::End();

	/*config->set("clr_menu", Color(225, 0, 255));
	config->set("colorGlow", Color(200, 80, 140)); // pink
	config->set("colorChamsEnemy", Color(130, 220, 90)); // green
	config->set("colorChamsEnemyXQZ", Color(35, 100, 160)); // aqua / light blue
	config->set("menu_bind", 45); // 45 = INSERT
	config->set("thirdpersonKey", 84); // 84 = T
	config->set("miscFoV", 90);
	config->set("miscAspectRatioW", 1920);
	config->set("miscAspectRatioH", 1080);

	config->set("miscAntiUT", true);
	config->set("espWatermark", true);

	menu->set_position(Vector2(100, 100));
	menu->set_size(Vector2(500, 513));*/
	/*
	Size(x,y): if Menu size is 500 x 513 then a full group-box should be 223 x 436 | half group-box should be 223 x 221

	pCalculations: 500 - 223 = 277 | 513 - 436 = 77
	Size X - 277 for half menu (right to left)
	Size Y - 77 for tabs' spacing (bottom to top)

	if Menu size is 478 x 491 then a full group-box should be 212 x 414

	pCalculations: 478 - 212 = 266 | 491 - 414 = 77
	Size X - 266 for half menu (right to left)
	Size Y - 77 for tabs' spacing (bottom to top)

	Size X Alternative: 414 - 199 = 215, 436 - 215 = 221

	Tabs: Dont forget to change tabs' width and height at menu.cpp
	*/

	/*c_tab* legit_tab = new c_tab("Legit-bot");
	{
		menu->add_tab(legit_tab);

		c_group* main_group = new c_group("Main", Vector2(223, 436));
		{
			main_group->add_child(new c_checkbox("Enable legit-bot", "legitAimEnable"));
			main_group->add_child(new c_checkbox("Backtrack", "legitBacktrack"));
			main_group->add_child(new c_slider("Field of view", "legitAimFOV", 1, 30, true));
			main_group->add_child(new c_combo("Priority hitbox", "legitPriorityHitbox", { "Head", "Chest", "Stomach", "Arms", "Legs", "Feet" }));
			main_group->add_child(new c_slider("Linear progression threshold", "legitLinearProgressionThreshold", 1, 100, true));
			main_group->add_child(new c_checkbox("Enable RCS", "legitEnableRCS"));
			main_group->add_child(new c_slider("RCS X", "legitRCSX", 1, 100, true));
			main_group->add_child(new c_slider("RCS Y", "legitRCSY", 1, 100, true));
		}
		legit_tab->add_child(main_group);
	}*/

	/*c_tab *rage_tab = new c_tab("Rage-bot");
	{
		menu->add_tab(rage_tab);

		c_group *general_group = new c_group("Aimbot", Vector2(223, 436));
		{
			general_group->add_child(new c_checkbox("Enable rage-bot", "rageEnable"));
			general_group->add_child(new c_checkbox("Automatic fire", "rageAutoFire"));
			general_group->add_child(new c_multicombo("Hitboxes", "rageHitboxes", { "Head", "Chest", "Stomach", "Arms", "Legs", "Feet" }));
			general_group->add_child(new c_checkbox("Avoid limbs if moving", "rageAvoidLimbsMoving"));
			general_group->add_child(new c_checkbox("Avoid head if jumping", "rageAvoidHeadJumping"));
			general_group->add_child(new c_multicombo("Multi-point", "rageMultipoints", { "Head", "Body" }));

			general_group->add_child(new c_slider("Head scale", "rageHeadScale", 1, 100, true, "%.0f%%"));
			general_group->add_child(new c_slider("Point scale", "ragePointScale", 1, 100, true, L"%.0f%%"));
			general_group->add_child(new c_checkbox("Hit chance", "rageHitchance"));
			general_group->add_child(new c_slider("Amount", "rageHitchanceValue", 0, 100, true, "%.0f%%"));
			general_group->add_child(new c_slider("Minimum damage", "rageMinDamageValue", 1, 100.f, true, "%.0fHP"));
		}
		rage_tab->add_child(general_group);

		c_group *other_group = new c_group("Other", Vector2(223, 210));
		{
			other_group->add_child(new c_hotkey("Body-aim key", "rageBodyAimKey", true));
			other_group->add_child(new c_checkbox("Force body-aim", "rageBodyAimOnKey"));

			other_group->add_child(new c_checkbox("Backtracking", "rageBacktrack"));
			other_group->add_child(new c_combo("Accuracy boost type", "rageLagCompType", { "Last", "All" }));

			//other_group->add_child(new c_checkbox("Delay shot", "rageDelayShot"));
			other_group->add_child(new c_checkbox("Body-aim if lethal", "rageBodyAimLethal"));
			other_group->add_child(new c_checkbox("Body-aim if in air", "rageBodyAimAir"));
			other_group->add_child(new c_checkbox("Body-aim if missed shots", "rageBodyAimShots"));
			other_group->add_child(new c_slider("X missed shots", "rageBodyAimShotsAmount", 1, 10, true, L"%.0f%%"));
			other_group->add_child(new c_checkbox("Anti-aim correction", "rageAntiAimCorrection"));
			other_group->add_child(new c_checkbox("Automatic scope", "rageAutoScope"));
			other_group->add_child(new c_checkbox("Automatic stop", "rageAutoStop"));
			other_group->add_child(new c_combo("Type", "rageAutoStopType", { "Minimum speed", "Full stop" }));
		}
		rage_tab->add_child(other_group);

		c_group* fakelag_group = new c_group("Fake-lag", Vector2(223, 210));
		{
			fakelag_group->add_child(new c_checkbox("Enable fake-lag", "rageChokeEnable"));
			fakelag_group->add_child(new c_multicombo("Activation", "rageChokeActivation", { "Standing", "Moving", "In air", "Shooting" }));
			fakelag_group->add_child(new c_combo("Type", "rageChokeType", { "Factor", "Switch", "Adaptive", "Random" }));
			fakelag_group->add_child(new c_slider("Ticks", "rageChokeAmount", 1, 14, true, "%.0f"));

			fakelag_group->add_child(new c_hotkey("Fake duck key", "miscfakeDuckKey", true));
			fakelag_group->add_child(new c_checkbox("Fake duck", "rageFakeDuck"));
		}
		rage_tab->add_child(fakelag_group);

	}
	c_tab* antiaim_tab = new c_tab("Anti-aim");
	{
		menu->add_tab(antiaim_tab);

		c_group* antiaim_settings_group = new c_group("Settings", Vector2(223, 210));
		{
			antiaim_settings_group->add_child(new c_checkbox("Enable anti-aim", "aaEnable"));

			antiaim_settings_group->add_child(new c_combo("Desync", "aaDesync", { "None", "1 cmd", "2 cmd", "1 ocmd", "2 ocmd" }));

			antiaim_settings_group->add_child(new c_hotkey("Slow motion key", "aaSlowWalkKey", true));
			antiaim_settings_group->add_child(new c_checkbox("Slow motion", "aaSlowWalk"));

			antiaim_settings_group->add_child(new c_hotkey("Flip key", "aaFlipKey", false));
			//antiaim_settings_group->add_child(new c_checkbox("Flip", "aaFlip"));
		}
		antiaim_tab->add_child(antiaim_settings_group);

		c_group* standing_aa_group = new c_group("Standing", Vector2(223, 210));
		{
			standing_aa_group->add_child(new c_combo("Pitch", "aaStandingPitch", { "None", "Down", "Up", "Zero", "Jitter" }));
			standing_aa_group->add_child(new c_combo("Yaw", "aaStandingYaw", { "None", "Backwards", "Sideways" }));
			standing_aa_group->add_child(new c_combo("Jitter type", "aaStandingJitterType", { "None", "Offset", "Random" }));
			standing_aa_group->add_child(new c_slider("Jitter range", "aaStandingJitterRange", 1, 180, true, L"%.0f�"));

			standing_aa_group->add_child(new c_checkbox("Auto direction", "aaStandingAutoDir"));
		}
		antiaim_tab->add_child(standing_aa_group);

		c_group* moving_aa_group = new c_group("Moving", Vector2(223, 210));
		{
			moving_aa_group->add_child(new c_combo("Pitch", "aaMovingPitch", { "None", "Down", "Up", "Zero", "Jitter" }));
			moving_aa_group->add_child(new c_combo("Yaw", "aaMovingYaw", { "None", "Backwards", "Sideways" }));
			moving_aa_group->add_child(new c_combo("Jitter type", "aaMovingJitterType", { "None", "Offset", "Random" }));
			moving_aa_group->add_child(new c_slider("Jitter range", "aaMovingJitterRange", 1, 180, true, L"%.0f�"));

			moving_aa_group->add_child(new c_checkbox("Auto direction", "aaMovingAutoDir"));
		}
		antiaim_tab->add_child(moving_aa_group);

		c_group* inair_aa_group = new c_group("In-air", Vector2(223, 210));
		{
			inair_aa_group->add_child(new c_combo("Pitch", "aaInAirPitch", { "None", "Down", "Up", "Zero", "Jitter" }));
			inair_aa_group->add_child(new c_combo("Yaw", "aaInAirYaw", { "None", "Backwards", "Sideways" }));
			inair_aa_group->add_child(new c_combo("Jitter type", "aaInAirJitterType", { "None", "Offset", "Random" }));
			inair_aa_group->add_child(new c_slider("Jitter range", "aaInAirJitterRange", 1, 180, true, L"%.0f�"));

			inair_aa_group->add_child(new c_checkbox("Auto direction", "aaInAirAutoDir"));
		}
		antiaim_tab->add_child(inair_aa_group);
	}
	c_tab *visuals_tab = new c_tab("Visuals");
	{
		menu->add_tab(visuals_tab);

		c_group *player_group = new c_group("Player ESP", Vector2(223, 230));
		{
			visuals_tab->add_child(player_group);
			player_group->add_child(new c_combo("Visuals type", "espType", { "Default", "Alternate" }));

			player_group->add_child(new c_checkbox("Enable visuals", "espEnable"));
			player_group->add_child(new c_checkbox("Teammates", "espTeammates"));
			player_group->add_child(new c_checkbox("Dormant", "espDormant"));

			player_group->add_child(new c_colorpicker("Box color", "colorBox", true, true));
			player_group->add_child(new c_checkbox("Bounding box", "espBox"));

			player_group->add_child(new c_checkbox("Health bar", "espHealth"));
			player_group->add_child(new c_checkbox("Health text", "espHealthText"));

			player_group->add_child(new c_colorpicker("nameclr", "colorName", true, true));
			player_group->add_child(new c_checkbox("Name", "espName"));

			//player_group->add_child(new c_checkbox("Flags", "espFlags"));
			player_group->add_child(new c_multicombo("Flags", "espFlags", { "Has armour", "Is scoped", "Is defusing", "Has hostage", "Is reloading", "Is fake-duck" }));

			player_group->add_child(new c_checkbox("Weapon text", "espWeapon"));
			player_group->add_child(new c_colorpicker("wpnclr", "colorWeapon", true, true));
			player_group->add_child(new c_checkbox("Weapon icon", "espWeaponIcon"));

			player_group->add_child(new c_colorpicker("ammoclr", "colorAmmoBar", true, true));
			player_group->add_child(new c_checkbox("Ammo", "espWeaponAmmoBar"));

			player_group->add_child(new c_checkbox("Distance", "espDistance"));

			player_group->add_child(new c_colorpicker("glowclr", "colorGlow", true, true));
			player_group->add_child(new c_multicombo("Glow", "espGlow", { "Enemy", "Team", "Hostage", "Local" }));

			//player_group->add_child(new c_colorpicker("glowclr", "colorGlow", true, true));
			//player_group->add_child(new c_checkbox("Glow", "espGlow"));

			//player_group->add_child(new c_colorpicker("glowclr", "colorGlowLocal", true, true));
			//player_group->add_child(new c_checkbox("Local glow", "espGlowLocal"));

			//player_group->add_child(new c_colorpicker("skeletonclr", "colorSkeleton", true, false));
			//player_group->add_child(new c_checkbox("Skeleton", "espSkeleton"));

			player_group->add_child(new c_colorpicker("skeletonbtclr", "colorSkeletonBT", true, false));
			player_group->add_child(new c_checkbox("History Skeleton", "espSkeletonBT"));

			player_group->add_child(new c_colorpicker("Out of FOV color", "colorOffscreenEsp", true, true));
			player_group->add_child(new c_checkbox("Out of FOV arrow", "espOffscreen"));

			player_group->add_child(new c_slider("Arrow size", "espOffscreenSize", 5.f, 25.f, "%.0fpx"));
			player_group->add_child(new c_slider("Arrow radius", "espOffscreenRadius", 10.f, 100.f, "%.0f%%"));

			player_group->add_child(new c_checkbox("Hit marker", "espHitmarker"));
			player_group->add_child(new c_combo("Hit marker sound", "espHitSound", { "Off", "Arena switch press 02", "Bubble", "Cod" }));

			player_group->add_child(new c_checkbox("Money", "espMoney"));
		}

		c_group *models_group = new c_group("Coloured models", Vector2(223, 190));
		{
			models_group->add_child(new c_combo("Model material", "chamsMaterial", { "Off", "Default", "Solid", "Shaded", "Metallic", "Glow", "Bubble" }));

			models_group->add_child(new c_colorpicker("colorEnemy", "colorChamsEnemy", true, true));
			models_group->add_child(new c_checkbox("Player", "chamsEnemy"));
			
			models_group->add_child(new c_colorpicker("xqzchamsclr", "colorChamsEnemyXQZ", true, true));
			models_group->add_child(new c_checkbox("Player (behind wall)", "chamsEnemyXQZ"));

			models_group->add_child(new c_colorpicker("teammateschamsclr", "colorChamsTeammates", true, true));
			models_group->add_child(new c_checkbox("Show teammates", "chamsTeammates"));

			models_group->add_child(new c_colorpicker("teammateschamsclrxqz", "colorChamsTeammatesXQZ", true, true));
			models_group->add_child(new c_checkbox("Show teammates (behind wall)", "chamsTeammatesXQZ"));

			models_group->add_child(new c_colorpicker("handschamsclr", "colorChamsHands", true, true));
			models_group->add_child(new c_checkbox("Hands", "chamsHands"));

			models_group->add_child(new c_colorpicker("weaponchamsclr", "colorChamsWeapon", true, true));
			models_group->add_child(new c_checkbox("Weapon", "chamsWeapon"));

			models_group->add_child(new c_colorpicker("shadowchamsclr", "colorChamsShadow", true, true));
			models_group->add_child(new c_checkbox("History chams", "chamsShadow"));

			models_group->add_child(new c_colorpicker("local", "colorChamsLocal", true, true));
			models_group->add_child(new c_checkbox("Local", "chamsLocal"));

			models_group->add_child(new c_colorpicker("clrDesync", "colorDesyncModel", true, true));
			models_group->add_child(new c_checkbox("Fake local shadow", "chamsLocalDesync"));
		}
		visuals_tab->add_child(models_group);


		c_group *other_group = new c_group("Other ESP", Vector2(223, 210));
		{
			other_group->add_child(new c_colorpicker("droppedWeaponCol", "colorWorldWeapon", true, true));
			other_group->add_child(new c_multicombo("Dropped weapons", "worldWeapon", { "Name", "Ammo", "Chams" }));

			other_group->add_child(new c_checkbox("Spectator list", "miscSpectators"));
			other_group->add_child(new c_checkbox("Sniper crosshair", "miscCrosshair"));
			other_group->add_child(new c_colorpicker("clrOutside", "colorInnacuracyOutside", true, true));
			other_group->add_child(new c_checkbox("Inaccuracy overlay", "miscInaccuracyOverlay"));
			other_group->add_child(new c_slider("Overlay size", "miscInaccuracySize", 0, 100, true, "%.0f%%"));

			other_group->add_child(new c_colorpicker("Grenade grenadenfsjhgbb", "colorGrenadePred", true, true));
			other_group->add_child(new c_checkbox("Grenade trajectory", "espGrenadePred"));

			other_group->add_child(new c_checkbox("Damage indicator", "espDamageIndicator"));
			other_group->add_child(new c_checkbox("Autowall indicator", "espAutowallIndicator"));

			other_group->add_child(new c_checkbox("Watermark", "espWatermark"));
		}
		visuals_tab->add_child(other_group);

		c_group *effects_group = new c_group("Effects", Vector2(223, 210));
		{
			effects_group->add_child(new c_checkbox("Remove flashbang effects", "removalsFlash"));
			effects_group->add_child(new c_checkbox("Remove smoke grenades", "removalsSmoke"));
			effects_group->add_child(new c_checkbox("Remove scope overlay", "removalsScope"));
			effects_group->add_child(new c_checkbox("Remove zoom", "removalsZoom"));
			effects_group->add_child(new c_checkbox("Remove visual recoil", "removalsRecoil"));

			effects_group->add_child(new c_multicombo("World modulation", "worldModulation", { "Night", "Fullbright" }));
			effects_group->add_child(new c_slider("Brightness", "nightModeDarkness", 0, 100, true, "%.0f%%"));

			effects_group->add_child(new c_hotkey("Thirdperson key", "thirdpersonKey", true));
			effects_group->add_child(new c_checkbox("Thirdperson", "espThirdperson"));
		}
		visuals_tab->add_child(effects_group);

	}
	c_tab *misc_tab = new c_tab("Miscellaneous");
	{
		menu->add_tab(misc_tab);

		c_group *misc_group = new c_group("Miscellaneous", Vector2(223, 436));
		{
			misc_group->add_child(new c_slider("Override fov", "miscFoV", 30, 120, true, L"%.0f�"));
			misc_group->add_child(new c_checkbox("Bunnyhop", "miscBhop"));
			misc_group->add_child(new c_checkbox("Air strafe", "miscStrafe"));

			misc_group->add_child(new c_checkbox("Movement key strafer", "miscKeyStrafe"));
			misc_group->add_child(new c_slider("Strafer speed", "miscStrafeSpeed", 0, 15, true, "%.0f%%"));

			misc_group->add_child(new c_checkbox("Air duck", "miscDuckInAir"));

			misc_group->add_child(new c_checkbox("Clan tag spammer", "miscClanTag"));

			misc_group->add_child(new c_checkbox("Log weapon purchases", "miscPurchasesLog"));
			misc_group->add_child(new c_checkbox("Log damage dealt", "miscDamageLog"));
			misc_group->add_child(new c_checkbox("Remove duck delay", "miscUnlimitedDuck"));

			misc_group->add_child(new c_slider("Aspect ratio W", "miscAspectRatioW", 1, 3840, true, L"%.0fpx"));
			misc_group->add_child(new c_slider("Aspect ratio H", "miscAspectRatioH", 1, 2160, true, L"%.0fpx"));

			misc_group->add_child(new c_colorpicker("TracerColor", "colorBulletTracer", true, true));
			misc_group->add_child(new c_checkbox("Bullet tracer", "miscBulletTracer"));

			misc_group->add_child(new c_colorpicker("ImpactColor", "colorBulletImpact", true, true));
			misc_group->add_child(new c_checkbox("Bullet impact", "miscBulletImpact"));
		}
		misc_tab->add_child(misc_group);

		c_group *settings_group = new c_group("Settings", Vector2(223, 155));
		{
			settings_group->add_child(new c_hotkey("Menu key", "menu_bind", false));
			settings_group->add_child(new c_colorpicker("Menu color", "clr_menu", false, true));

			settings_group->add_child(new c_checkbox("Anti-untrusted", "miscAntiUT"));
		}
		misc_tab->add_child(settings_group);

		c_group* other_group = new c_group("Other", Vector2(223, 265));
		{
			other_group->add_child(new c_combo("Presets", "cfgPreset", { "Legit", "Rage", "HvH", "Secret", "Headshot", "Alpha", "Bravo", "Charlie", "Delta", "Echo", "Foxtrot", "Golf", "Hotel", "India", "Juliet", "Kilo" }));

			other_group->add_child(new c_button("Load config", []() {
				std::string load_name = std::string("ConfigId").append(std::to_string(config->get_int("cfgPreset") + 1)).append(".cfg");

				config->load(load_name);
				}));

			other_group->add_child(new c_button("Save config", []() {
				std::string save_name = std::string("ConfigId").append(std::to_string(config->get_int("cfgPreset") + 1)).append(".cfg");

				config->save(save_name);
				}));

			other_group->add_child(new c_button("Reset config", []() {
				config->load("reset.cfg");
				}));

			other_group->add_child(new c_button("Unload", []() {
				config->set("unload", true);
				}));
		}
		misc_tab->add_child(other_group);
	}
	c_tab* skinschanger_tab = new c_tab("Skin changer");
	{
		menu->add_tab(skinschanger_tab);

		c_group* knifeoptions_group = new c_group("Knife options", Vector2(223, 125));
		{
			knifeoptions_group->add_child(new c_checkbox("Override knife", "skinsOverrideKnife"));

			//if (config->get_bool("skinsOverrideKnife"))
			knifeoptions_group->add_child(new c_combo("", "skinsKnifeModel", { "Bayonet", "Flip", "Gut", "Karambit", "M9 Bayonet", "Tactical", "Butterfly", "Falchion", "Shadow Daggers", "Survival Bowie", "Ursus", "Navaja", "Stiletto", "Talon" }));
		}
		skinschanger_tab->add_child(knifeoptions_group);

		c_group* gloveoptions_group = new c_group("Glove options", Vector2(223, 295));
		{

		}
		skinschanger_tab->add_child(gloveoptions_group);

		c_group* weaponskin_group = new c_group("Weapon skin", Vector2(223, 436));
		{

		}
		skinschanger_tab->add_child(weaponskin_group);
	}


	menu->set_active_tab(rage_tab); // active tab*/
}



/*
#include "menu.hpp"
#include "controls/tab.hpp"
#include "sprites/texture.hpp"

#define DopamineTabSize 462

void __stdcall d3dx_color_fill( D3DXVECTOR4 *out, const D3DXVECTOR2 *texture_coordinates, const D3DXVECTOR2 *texel_size, LPVOID data ) {
	D3DCOLORVALUE *color = ( D3DCOLORVALUE* )data;
	*out = D3DXVECTOR4( color->r, color->g, color->b, color->a );
}

void c_menu::poll_input( ) {
	this->previous_mouse_pos = this->mouse_pos;

	for ( int i = 0; i < 256; i++ ) {
		this->previous_key_state[ i ] = this->key_state[ i ];
		this->key_state[ i ] = GetAsyncKeyState( i );
	}

	POINT mouse_point;

	GetCursorPos( &mouse_point );
	ScreenToClient( FindWindow( "Valve001", "Counter-Strike: Global Offensive" ), &mouse_point );

	this->mouse_pos = Vector2( mouse_point.x, mouse_point.y );
}

bool c_menu::is_key_down( int key ) {
	return this->key_state[ key ] == true;
}

bool c_menu::is_key_pressed( int key ) {
	return this->previous_key_state[ key ] == false && this->key_state[ key ] == true;
}

bool c_menu::is_key_released( int key ) {
	return this->previous_key_state[ key ] == true && this->key_state[ key ] == false;
}

void c_menu::set_mouse_wheel( int mouse_wheel ) {
	this->mouse_wheel = mouse_wheel;
}

int c_menu::get_mouse_wheel( ) {
	return this->mouse_wheel;
}

Vector2 c_menu::get_mouse_pos( ) {
	return this->mouse_pos;
}

bool c_menu::in_bounds( Vector2 pos, Vector2 size ) {
	// grab cursor
	auto cursor = get_mouse_pos( );

	// math for inbounds
	return ( cursor.x > pos.x &&
		cursor.y > pos.y &&
		cursor.x < pos.x + size.x &&
		cursor.y < pos.y + size.y );
}


bool c_menu::is_hovered( Vector2 min, Vector2 max ) {
	return this->mouse_pos.x >= min.x && this->mouse_pos.y >= min.y &&
		this->mouse_pos.x <= max.x && this->mouse_pos.y <= max.y;
}

void c_menu::add_tab( c_tab *tab ) {
	tab->set_parent( this );

	this->tabs.push_back( tab );
	this->active_tab = tab;
}

bool c_menu::is_blocking( c_element *elem ) {
	if ( elem == nullptr )
		return this->blocking != nullptr;

	return this->blocking == elem;
}

void c_menu::block( c_element *elem ) {
	this->blocking = elem;
}

c_element *c_menu::get_blocking( ) {
	return this->blocking;
}

c_tab *c_menu::get_active_tab( ) {
	return this->active_tab;
}

void c_menu::set_active_tab( c_tab *tab ) {
	this->active_tab = tab;
}

bool c_menu::get_hotkey( const std::string &value ) {
	int key = config->get_int( value );
	int activation_type = config->get_int( value + "_type", 1 );

	switch ( activation_type ) {
	case 0:
		return true;
		break;
	case 1:
		return menu->is_key_down( key );
		break;
	case 2:
		if ( hotkey_states.find( value ) == hotkey_states.end( ) ) hotkey_states[ value ] = menu->is_key_down( key );
		else if ( menu->is_key_pressed( key ) ) hotkey_states[ value ] = !hotkey_states[ value ];
		return hotkey_states.at( value );
		break;
	case 3:
		return !menu->is_key_down( key );
		break;
	}
}

void c_menu::think( ) {
	if ( !initialized )
		return;

	this->poll_input( );

	if ( this->is_hovered( fixed_position - Vector2( 6, 6 ), fixed_position + Vector2( size.x + 6, 6 ) ) ) {
		if ( this->is_key_pressed( VK_LBUTTON ) && !this->dragging )
			this->dragging = true;
	}
	else if ( !this->is_blocking( ) ) {
		for ( int i = 0; i < ( int )tabs.size( ); i++ ) {
			c_tab *tab = tabs[ i ];

			int tab_text_width, tab_text_height;

			Vector2 text_size = g_renderer_d3d->menu->measure_size( tab->get_label( ) );

			const int tab_width = (DopamineTabSize / ( int )tabs.size( ) );

			Vector2 tab_hover_min( fixed_position.x + 19 + ( tab_width * i ), fixed_position.y + ( this->size.y - 40 ) );
			Vector2 tab_hover_max = tab_hover_min + Vector2( tab_width, 25 );

			if ( this->is_hovered( tab_hover_min, tab_hover_max ) && this->is_key_pressed( VK_LBUTTON ) )
				this->active_tab = tab;
		}
	}

	if ( this->is_key_down( VK_LBUTTON ) && this->dragging ) {
		Vector2 mouse_delta = this->previous_mouse_pos - this->mouse_pos;
		this->position -= mouse_delta;
	}

	if ( !this->is_key_down( VK_LBUTTON ) && this->dragging )
		this->dragging = false;

	this->active_tab->think( );
}

void c_menu::draw( ) {
	if ( this->menu_background == nullptr ) {
		this->menu_background = new c_sprite( );
		this->menu_background->setup( g_renderer_d3d->device, background_texture_bytes, sizeof( background_texture_bytes ), 1000, 1000 );

		D3DXCreateSprite( g_renderer_d3d->device, &menu_texture_sprite );

		int screen_w, screen_h;
		g_Interfaces->gameEngine->getScreenSize( screen_w, screen_h );

		g_renderer_d3d->device->CreateTexture( screen_w, screen_h, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &menu_texture, nullptr );
		initialized = true;
	}

	constexpr float fade_factor = 6.666667f;
	float fade_increment = ( fade_factor * g_Interfaces->globalVars->frametime ) * 255.0f;

	if ( menu->opened )
		menu_alpha += ( int )fade_increment;
	else
		menu_alpha -= ( int )fade_increment;

	menu_alpha = max( 0, min( 255, menu_alpha ) );

	if ( menu_alpha == 0 )
		return;

	IDirect3DSurface9 *back_buffer_surface;
	IDirect3DSurface9 *menu_render_surface;

	menu_texture->GetSurfaceLevel( 0, &menu_render_surface );

	g_renderer_d3d->device->GetRenderTarget( 0, &back_buffer_surface );
	g_renderer_d3d->device->SetRenderTarget( 0, menu_render_surface );

	D3DVIEWPORT9 old_viewport = g_renderer_d3d->get_viewport( );

	fixed_position.x = max( 6, min( position.x, old_viewport.Width - size.x - 6 ) );
	fixed_position.y = max( 6, min( position.y, old_viewport.Height - size.y - 6 ) );

	g_renderer_d3d->set_viewport( { ( DWORD )fixed_position.x, ( DWORD )fixed_position.y, ( DWORD )size.x, ( DWORD )size.y, 0.f, 1.f } );

	this->menu_background->sprite->Begin( D3DXSPRITE_ALPHABLEND );
	this->menu_background->draw( fixed_position );
	this->menu_background->sprite->End( );

	g_renderer_d3d->set_viewport( old_viewport );

	g_renderer_d3d->rect( fixed_position.x - 1, fixed_position.y - 1, size.x + 2, size.y + 2, Color( 60, 60, 60 ) );
	g_renderer_d3d->rect( fixed_position.x - 2, fixed_position.y - 2, size.x + 4, size.y + 4, Color( 40, 40, 40 ) );
	g_renderer_d3d->rect( fixed_position.x - 3, fixed_position.y - 3, size.x + 6, size.y + 6, Color( 40, 40, 40 ) );
	g_renderer_d3d->rect( fixed_position.x - 4, fixed_position.y - 4, size.x + 8, size.y + 8, Color( 40, 40, 40 ) );
	g_renderer_d3d->rect( fixed_position.x - 5, fixed_position.y - 5, size.x + 10, size.y + 10, Color( 60, 60, 60 ) );
	g_renderer_d3d->rect( fixed_position.x - 6, fixed_position.y - 6, size.x + 12, size.y + 12, Color( 31, 31, 31 ) );

	g_renderer_d3d->rect_fill( fixed_position.x + 1, fixed_position.y + 1, size.x - 2, 2, config->get_color( "clr_menu" ) );
	g_renderer_d3d->line( fixed_position.x + 1, fixed_position.y + 2, fixed_position.x + size.x - 1, fixed_position.y + 2,
		Color( 0, 0, 0, ( int )( 80.0f * ( ( float )config->get_color( "clr_menu" ).a( ) / 255.0f ) ) ) );

	g_renderer_d3d->rect_fill( fixed_position.x + 19, fixed_position.y + this->size.y - 40, DopamineTabSize, 25, Color( 24, 24, 24 ) );
	g_renderer_d3d->rect( fixed_position.x + 18, fixed_position.y + this->size.y - 41, DopamineTabSize + 2, 27, Color( 44, 44, 44 ) );

	for ( int i = 0; i < ( int )tabs.size( ); i++ ) {
		c_tab *tab = tabs[ i ];

		auto tab_text_color = tab == active_tab ? config->get_color( "clr_menu" ) : Color( 210, 210, 210 );
		auto tab_text_font = tab == active_tab ? g_renderer_d3d->menu_bold : g_renderer_d3d->menu;

		const int tab_width = (DopamineTabSize / ( int )tabs.size( ) );

		int tab_center_x = fixed_position.x + 19 + ( tab_width * i ) + ( tab_width / 2 );
		int tab_center_y = fixed_position.y + ( this->size.y - 40 ) + ( 25 / 2 );

		Vector2 tab_text_size = tab_text_font->measure_size( tab->get_label( ) );

		tab_text_font->draw_text( tab->get_label( ), { tab_center_x - ( tab_text_size.x / 2 ), tab_center_y - ( tab_text_size.y / 2 ) },
			tab_text_color, font_flags_d3d::font_dropshadow );

		if ( i != tabs.size( ) - 1 ) {
			g_renderer_d3d->line( { float( tab_center_x + ( tab_width / 2 ) ), float( tab_center_y - 4 ) },
				{ float( tab_center_x + ( tab_width / 2 ) ), float( tab_center_y + 5 ) }, { 44, 44, 44 } );
		}
	}

	this->active_tab->draw( );

	if ( this->is_blocking( ) )
		this->get_blocking( )->draw( );

	g_renderer_d3d->device->SetRenderTarget( 0, back_buffer_surface );

	menu_texture_sprite->Begin( D3DXSPRITE_ALPHABLEND );

	D3DXVECTOR3 position = { 0.0f, 0.0f, 0.01f };
	D3DXVECTOR3 scaling( 1.f, 1.f, 1.f );
	D3DXVECTOR3 spriteCentre( 100, 100, 0 );

	D3DXMATRIX matrix;
	D3DXMatrixTransformation( &matrix, NULL, 0, &scaling, &spriteCentre, NULL, &position );

	menu_texture_sprite->SetTransform( &matrix );
	menu_texture_sprite->Draw( menu_texture, NULL, NULL, NULL, D3DCOLOR_RGBA( 255, 255, 255, menu_alpha ) );
	menu_texture_sprite->End( );

	g_renderer_d3d->device->SetRenderTarget( 0, menu_render_surface );
	g_renderer_d3d->device->Clear( 0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_RGBA( 255, 255, 255, 0 ), 0.01f, 0 );
	g_renderer_d3d->device->SetRenderTarget( 0, back_buffer_surface );
}

Vector2 c_menu::get_child_draw_position( ) {
	return fixed_position + Vector2( 19, 22 );
}

c_menu *menu = new c_menu( );
*/