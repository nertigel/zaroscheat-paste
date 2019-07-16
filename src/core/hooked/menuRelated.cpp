//#include "../../utilities/includes.h"
#include "../../menu/menu.hpp"

using lockCursorFn = void( __thiscall* )( void* );

void __stdcall CHookManager::lockCursor( )
{
	static auto originalFn = g_Hooks->surfaceHook->getOriginalFunction<lockCursorFn>( 67 );
	
	g_Interfaces->inputSystem->enableInput( !menu.opened );
	if ( menu.opened )
	{
		g_Interfaces->surface->unlockCursor( );
		return;
	}

	originalFn( g_Interfaces->surface );
}
/*
static bool initialized = false;
long __stdcall CHookManager::present(IDirect3DDevice9* device, RECT* source_rect, RECT* dest_rect, HWND dest_window_override, RGNDATA* dirty_region) {
	if (!initialized) {
		menu.apply_fonts();
		menu.setup_resent(device);
		initialized = true;
	}
	if (initialized) {
		menu.pre_render(device);
		menu.post_render();

		menu.run_popup();
		menu.run();
		menu.end_present(device);
	}

	return original_present(device, source_rect, dest_rect, dest_window_override, dirty_region);
}

long __stdcall CHookManager::reset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* present_parameters) {
	if (!initialized)
		original_reset(device, present_parameters);

	menu.invalidate_objects();
	long hr = original_reset(device, present_parameters);
	menu.create_objects(device);

	return hr;
}*/