#include "../../utilities/includes.h"

#include "../../features/Events/bullet_impact.hpp"

float __fastcall CHookManager::aspectRatio( void* ecx, void* edx, int32_t width, int32_t height )
{
	return (config_system.item.misc.aspect_ratio_w / config_system.item.misc.aspect_ratio_h);
}

void __fastcall CHookManager::beginFrame( void* thisptr, void* ptr, float ft )
{
	auto originalFn = g_Hooks->materialHook->getOriginalFunction<decltype( &beginFrame )>( 42 );

	bullet_impact::get( ).paint( );

	if ( !g_Interfaces->gameEngine->connected( ) || !g_Interfaces->gameEngine->inGame( ) )
	{
		Globals::localPlayer = nullptr;
	}
	originalFn( thisptr, ptr, ft );
}