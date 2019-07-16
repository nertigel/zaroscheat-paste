#pragma once
#include "../utilities/includes.h"
#include "sprites/sprite.hpp"
#include "../utilities/renderer_d3d.hpp"

#include <d3d9.h>
#include <d3dx9.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#include "imgui/imgui.h"
#include "imgui/dx9/imgui_impl_dx9.h"
/*
#include "utilities/singleton.hpp"
#include "utilities/fnv.hpp"
#include "utilities/utilities.hpp"
#include "utilities/render.hpp"
#include "utilities/hook.hpp"
#include "interfaces/interfaces.hpp"
#include "../source-sdk/sdk.hpp"
#include "../core/hooks/hooks.hpp"
#include "../core/menu/config/config.hpp"
#include "../dependencies/math/math.hpp"
*/

extern IDirect3DStateBlock9* state_block;

enum esp_info_position {
	RIGHT,
	CENTER_DOWN,
	CENTER_UP
};

struct esp_info_s {
	esp_info_s(const std::string name, const Color colors, const int position = RIGHT) {
		f_name = name; f_color = colors; f_position = position;
	}

	std::string f_name;
	int f_position;
	Color f_color;
};

class c_menu {
public:
	void run_popup();
	void run();
	void run_visuals_preview();

	void __stdcall create_objects(IDirect3DDevice9* device) {
		if (Globals::csgoWindow)
			ImGui_ImplDX9_CreateDeviceObjects();
	}

	void __stdcall invalidate_objects() {
		ImGui_ImplDX9_InvalidateDeviceObjects();
	}

	void __stdcall setup_resent(IDirect3DDevice9* device) {
		ImGui_ImplDX9_Init(Globals::csgoWindow, device);

		ImGuiStyle& style = ImGui::GetStyle();
		style.Alpha = 1.0f;
		style.WindowPadding = ImVec2(0, 0);
		style.WindowMinSize = ImVec2(32, 32);
		style.WindowRounding = 0.0f;
		style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
		style.ChildWindowRounding = 0.0f;
		style.FramePadding = ImVec2(4, 3);
		style.FrameRounding = 0.0f;
		style.ItemSpacing = ImVec2(8, 8);
		style.ItemInnerSpacing = ImVec2(8, 8);
		style.TouchExtraPadding = ImVec2(0, 0);
		style.IndentSpacing = 21.0f;
		style.ColumnsMinSpacing = 0.0f;
		style.ScrollbarSize = 6.0f;
		style.ScrollbarRounding = 0.0f;
		style.GrabMinSize = 5.0f;
		style.GrabRounding = 0.0f;
		style.ButtonTextAlign = ImVec2(0.0f, 0.5f);
		style.DisplayWindowPadding = ImVec2(22, 22);
		style.DisplaySafeAreaPadding = ImVec2(4, 4);
		style.AntiAliasedLines = true;
		style.AntiAliasedShapes = false;
		style.CurveTessellationTol = 1.f;

		ImVec4* colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(33 / 255.f, 35 / 255.f, 47 / 255.f, 1.0f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
		colors[ImGuiCol_Border] = ImVec4(30 / 255.f, 30 / 255.f, 41 / 255.f, 1.0f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_ChildWindowBg] = ImVec4(33 / 255.f, 35 / 255.f, 47 / 255.f, 1.0f);
		colors[ImGuiCol_FrameBg] = ImVec4(33 / 255.f, 35 / 255.f, 47 / 255.f, 1.0f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(35 / 255.f, 35 / 255.f, 35 / 255.f, 1.0f);
		colors[ImGuiCol_TitleBg] = ImVec4(35 / 255.f, 35 / 255.f, 35 / 255.f, 1.0f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(35 / 255.f, 35 / 255.f, 35 / 255.f, 1.0f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.17f, 0.17f, 0.17f, 1.00f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
		colors[ImGuiCol_Button] = ImVec4(33 / 255.f, 35 / 255.f, 47 / 255.f, 1.0f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(35 / 255.f, 35 / 255.f, 35 / 255.f, 1.0f); //
		colors[ImGuiCol_ButtonActive] = ImVec4(135 / 255.f, 135 / 255.f, 135 / 255.f, 1.0f); //
		colors[ImGuiCol_Header] = ImVec4(167 / 255.f, 24 / 255.f, 71 / 255.f, 1.0f); //multicombo, combo selected item color.
		colors[ImGuiCol_HeaderHovered] = ImVec4(35 / 255.f, 35 / 255.f, 35 / 255.f, 1.0f);
		colors[ImGuiCol_HeaderActive] = ImVec4(35 / 255.f, 35 / 255.f, 35 / 255.f, 1.0f);
		colors[ImGuiCol_Separator] = ImVec4(0, 0, 0, 1);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0, 0, 0, 1);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0, 0, 0, 1);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
		colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
		colors[ImGuiCol_CloseButton] = ImVec4(0, 0, 0, 0);
		colors[ImGuiCol_CloseButtonHovered] = ImVec4(0, 0, 0, 0);
		colors[ImGuiCol_HotkeyOutline] = ImVec4(0, 0, 0, 0);

		create_objects(device);
	}

	void apply_fonts() {
		ImGui::CreateContext();

		font_main = ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Tahoma.ttf", 18);
		font_menu = ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Verdana.ttf", 12);
	}

	void __stdcall end_present(IDirect3DDevice9* device) {
		if (opened) {
			if (ImGui::GetStyle().Alpha > 1.f)
				ImGui::GetStyle().Alpha = 1.f;
			else if (ImGui::GetStyle().Alpha != 1.f)
				ImGui::GetStyle().Alpha += 0.05f;
		}

		ImGui::Render();

		state_block->Apply();
		state_block->Release();
	}

	void __stdcall pre_render(IDirect3DDevice9 * device) {
		D3DVIEWPORT9 d3d_viewport;
		device->GetViewport(&d3d_viewport);

		device->CreateStateBlock(D3DSBT_ALL, &state_block);
		state_block->Capture();

		device->SetVertexShader(nullptr);
		device->SetPixelShader(nullptr);
		device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);

		device->SetRenderState(D3DRS_LIGHTING, FALSE);
		device->SetRenderState(D3DRS_FOGENABLE, FALSE);
		device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

		device->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
		device->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);
		device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		device->SetRenderState(D3DRS_STENCILENABLE, FALSE);

		device->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
		device->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, TRUE);

		device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		device->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, TRUE);
		device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		device->SetRenderState(D3DRS_SRCBLENDALPHA, D3DBLEND_INVDESTALPHA);
		device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		device->SetRenderState(D3DRS_DESTBLENDALPHA, D3DBLEND_ONE);

		device->SetRenderState(D3DRS_SRGBWRITEENABLE, FALSE);
		device->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA);
	}

	void __stdcall post_render() {
		ImGui_ImplDX9_NewFrame();
	}
	ImFont* font_main;
	ImFont* font_menu;
	ImFont* font_main_caps;
	bool opened = false;
private:
	ImDrawData _drawData;
	DWORD dwOld_D3DRS_COLORWRITEENABLE;
};

extern c_menu menu;

/*
class c_element {
protected:
	Vector2 position;
	Vector2 size;

	std::string label;
	std::string var_name;

	std::vector< c_element* > children;
	c_element *parent;

public:
	virtual void think( ) = 0;
	virtual void draw( ) = 0;

	virtual c_element *get_parent( ) {
		return this->parent;
	}
	virtual void set_parent( c_element *parent ) {
		this->parent = parent;
	}

	virtual bool is_child( c_element *child ) {
		for ( c_element *elem : this->children )
			if ( elem == child )
				return true;

		return false;
	}
	virtual void add_child( c_element *child ) {
		this->children.push_back( child );
		child->set_parent( this );
	}

	virtual void set_label( const std::string &label ) {
		this->label = std::move( label );
	}
	virtual std::string get_label( ) {
		return this->label;
	}

	virtual void set_var_name( const std::string &var_name ) {
		this->var_name = std::move( var_name );
	}
	virtual std::string get_var_name( ) {
		return this->var_name;
	}

	virtual void set_size( const Vector2 &size ) {
		this->size = size;
	}
	virtual Vector2 get_size( ) {
		return this->size;
	}

	virtual void set_position( const Vector2 &position ) {
		this->position = position;
	}
	virtual Vector2 get_position( ) {
		return this->position;
	}

	virtual Vector2 get_child_draw_position( ) {
		if ( this->parent == nullptr )
			return this->position;

		return this->parent->get_child_draw_position( ) + this->position;
	}

};

class c_tab;

class c_menu : public c_element {
private:
	bool initialized = false;
	bool dragging = false;
	int menu_alpha = 0;

	Vector2 previous_mouse_pos;
	Vector2 mouse_pos;

	std::unordered_map<std::string, bool> hotkey_states;
	bool previous_key_state[ 256 ];
	bool key_state[ 256 ];
	int mouse_wheel;

	std::vector< c_tab* > tabs;
	c_tab *active_tab;

	c_element *blocking = nullptr;
	c_sprite *menu_background;

	IDirect3DTexture9 *menu_texture;
	LPD3DXSPRITE menu_texture_sprite;

public:
	void think( ) override;
	void draw( ) override;

	Vector2 get_child_draw_position( ) override;

	void poll_input( );
	bool is_key_down( int key );
	bool is_key_pressed( int key );
	bool is_key_released( int key );

	void set_mouse_wheel( int mouse_wheel );
	int get_mouse_wheel( );

	Vector2 get_mouse_pos( );
	bool in_bounds( Vector2 pos, Vector2 size );
	bool is_hovered( Vector2 min, Vector2 max );

	void add_tab( c_tab *tab );

	bool is_blocking( c_element *elem = nullptr );
	void block( c_element *elem );
	c_element *get_blocking( );

	c_tab *get_active_tab( );
	void set_active_tab( c_tab *tab );

	bool get_hotkey( const std::string &value );

	bool opened = false;
	Vector2 fixed_position;

};
extern c_menu *menu;
*/
