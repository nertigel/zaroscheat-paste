#include "Visuals.hpp"
#include "../Misc/Dormancy.hpp"

#include <algorithm>
#include "../../sdk/vector.hpp"
#include "../Aimbot/Aimbot.hpp"
#include <locale> // include this for std::wstring_convert, needed for wpn esp
#include <codecvt> // include this for std::codecvt_utf8, also needed for wpn esp

#include "../../utilities/renderer_d3d.hpp"

CVisuals g_Visuals;
int alpha[ 65 ];
int arrow_alpha[ 65 ];
bool last_dormant[ 65 ];
float increment = 0.f;
bool shouldIncrement;

void CVisuals::onMove( float sampleTime, CUserCmd* userCmd )
{
	if ( !g_Interfaces->gameEngine->inGame( ) && !g_Interfaces->gameEngine->connected( ) )
		return;

	if ( !Globals::localPlayer )
		return;

	if ( !Globals::localPlayer->alive( ) )
		return;
}

void CVisuals::onFrameStage( clientFrameStage_t frameStage )
{

}

void CVisuals::onPaint( unsigned int panel )
{
	if ( config_system.item.visuals.modulation_night )
	{
		bNightModulationRemoveComplete = false;
		nightModulation( );
	}
	else
	{
		bNightModulationComplete = false;
		removeNight( );
	}

	if ( config_system.item.visuals.modulation_fullbright )
	{
		bFullBrightRemoveComplete = false;
		applyFullBright( );
	}
	else
	{
		bFullBrightComplete = false;
		removeFullBright( );
	}


	if ( !Globals::localPlayer || !g_Interfaces->gameEngine->inGame( ) )
		return;

	//localViewModelSpoof( );


	if ( config_system.item.visuals.removals_scope && Globals::localPlayer->scoped( ) )
		noScopeLines( );

	if ( !config_system.item.visuals.enable_visuals )
		return;

	const float arrowFade = 1.1f * g_Interfaces->globalVars->frametime;

	Globals::shouldDormant = true;

	increment += ( shouldIncrement ? arrowFade : -arrowFade );

	if ( increment > 1.f )
	{
		shouldIncrement = false;
		increment = 1.f;
	}
	else if ( increment <= 0.f )
	{
		shouldIncrement = true;
		increment = 0.f;
	}

	for ( int i = 1; i <= g_Interfaces->clientEntity->GetHighestEntityIndex( ); ++i )
	{
		CBaseEntity* pEntity = g_Interfaces->clientEntity->GetClientEntity( i );

		if ( !pEntity )
			continue;

		if ( !pEntity->IsDormant( ) )
			worldWeapon( pEntity );

		if ( !pEntity->alive( ) )
			continue;

		if ( pEntity == Globals::localPlayer || pEntity->team( ) == Globals::localPlayer->team( ) )
			continue;

		if ( boundingBox( pEntity ) )
		{
			int fade = int( ( ( 6.667f * g_Interfaces->globalVars->frametime ) * 255.f ) );
			alpha[ i ] += pEntity->IsDormant( ) ? -fade : fade;

			// now this is some epic code here
			if ( alpha[ i ] > ( pEntity->immunity( ) ? 130 : 240 ) )
				alpha[ i ] = ( pEntity->immunity( ) ? 130 : 240 );

			if ( alpha[ i ] < ( ( config_system.item.visuals.dormant_check && Globals::shouldDormant ) ? 50 : 0 ) )
				alpha[ i ] = ( config_system.item.visuals.dormant_check && Globals::shouldDormant ) ? 50 : 0;

			if ( config_system.item.visuals.player_box )
				playerBox( pEntity );


			if ( config_system.item.visuals.player_health )
				playerHealth( pEntity );


			if ( config_system.item.visuals.player_name )
				playerName( pEntity, i );


			if ( config_system.item.visuals.player_ammo )
				playerAmmo( pEntity );


			//if (config->get_int("espFlags") & (1 << 1))

			playerFlags( pEntity );

			playerWeapon( pEntity );
		}


		if ( config_system.item.visuals.out_of_fov_arrow )
			outOfPovArrows( pEntity );

		if ( !pEntity->IsDormant( ) )
		{
			if ( config_system.item.visuals.skeleton )
				playerBones(pEntity);

			if ( config_system.item.visuals.backtrack_skeleton && Globals::localPlayer->alive( ) )
				playerHistoryBones( pEntity );
		}
	}
}

void CVisuals::onEvent( IGameEvent* pEvent )
{

}

bool CVisuals::boundingBox( CBaseEntity* pEntity, Vector3 offset )
{
	Vector3 min, max;
	pEntity->GetRenderBounds( min, max );

	Vector3 pos3D, top3D;
	Vector2 pos, top;
	pos3D = pEntity->GetAbsOrigin( ) - Vector3( 0, 0, 10 );
	top3D = pos3D + Vector3( 0, 0, max.z + 15 );

	if ( g_Utils->WorldToScreen( pos3D, pos ) && g_Utils->WorldToScreen( top3D, top ) )
	{
		int height = ( pos.y - top.y );
		Box.h = height; //h
		int width = Box.h / 2;
		Box.w = width; //w

		Box.x = pos.x - width / 2; //x
		Box.y = top.y; //y

		return true;
	}

	return false;
}

void CVisuals::playerBox( CBaseEntity* pEntity )
{
	int red = config_system.item.visuals.clr_box[0] * 255;
	int green = config_system.item.visuals.clr_box[1] * 255;
	int blue = config_system.item.visuals.clr_box[2] * 255;
	Color CasualCol = Color(
		red,
		green,
		blue,
		alpha[ pEntity->EntIndex( ) ] );

	g_Renderer->rectangle( Box.x, Box.y, Box.w, Box.h, CasualCol );
	g_Renderer->rectangle( Box.x + 1, Box.y + 1, Box.w - 2, Box.h - 2, Color( 0, 0, 0, alpha[ pEntity->EntIndex( ) ] ) );
	g_Renderer->rectangle( Box.x - 1, Box.y - 1, Box.w + 2, Box.h + 2, Color( 0, 0, 0, alpha[ pEntity->EntIndex( ) ] ) );
}

void CVisuals::playerHealth( CBaseEntity* pEntity )
{
	int health = pEntity->health( ) > 100 ? 100 : pEntity->health( );

	if ( health <= 0 )
		return;

	int barHeight = health * Box.h / 100.f, offset = Box.h - barHeight;

	g_Renderer->rectangle( Box.x - 7, Box.y - 1, 4, Box.h + 2, Color( 0, 0, 0, alpha[ pEntity->EntIndex( ) ] ) );

	g_Renderer->fillRectangle( Box.x - 6, Box.y, 2, Box.h, Color( 0, 0, 0, alpha[ pEntity->EntIndex( ) ] * 0.85 ) );
	g_Renderer->fillRectangle( Box.x - 6, Box.y + offset, 2, barHeight, Color( 120, min( 170, pEntity->health( ) * 95 / 50 ), 0, alpha[ pEntity->EntIndex( ) ] ) );


	if ( health <= 95 && config_system.item.visuals.player_health_text )
	{
		std::string strHealth( std::to_string( health ) );

		std::transform( strHealth.begin( ), strHealth.end( ), strHealth.begin( ), toupper );

		g_Renderer->text( Box.x - 6, ( Box.y + offset ) - 4, getFont( g_Fonts->visualsFlags ), strHealth.c_str( ), Color( 255, 255, 255, alpha[ pEntity->EntIndex( ) ] ), true );
	}
}

void CVisuals::playerName( CBaseEntity* pEntity, int index )
{
	playerinfo_t playerInfo;
	g_Interfaces->gameEngine->getPlayerInfo( index, &playerInfo );

	auto strPlayerName = playerInfo.name;
	int red = config_system.item.visuals.clr_name[0] * 255;
	int green = config_system.item.visuals.clr_name[1] * 255;
	int blue = config_system.item.visuals.clr_name[2] * 255;
	Color CasualCol = Color(
		red,
		green,
		blue,
		alpha[ pEntity->EntIndex( ) ] );

	g_Renderer->text( Box.x + ( Box.w / 2 ), Box.y - 13, getFont( g_Fonts->visuals ), strPlayerName, CasualCol, true );
}

void CVisuals::playerWeapon( CBaseEntity* pEntity )
{
	auto weapon = pEntity->activeWeapon( );

	if ( !weapon )
		return;

	const char *name = weapon->weaponData( )->hudName;
	wchar_t* localised_name = g_Interfaces->localize->Find( name );

	std::string strWeaponName( std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>( ).to_bytes( localised_name ) );
	std::transform( strWeaponName.begin( ), strWeaponName.end( ), strWeaponName.begin( ), ::toupper );


	if ( config_system.item.visuals.player_ammo && ( !weapon->isKnife( ) || !weapon->isKnifeorNade( ) ) )
		strWeaponName.append( " (" ).append( std::to_string( weapon->ammo( ) ) ).append( " / " + std::to_string( weapon->weaponData( )->maxClip ) + ")" );

	int offset;

	if ( config_system.item.visuals.player_ammo_bar )
		offset = 9;
	else
		offset = 1;

	int red = config_system.item.visuals.clr_weapon_icon[0] * 255;
	int green = config_system.item.visuals.clr_weapon_icon[1] * 255;
	int blue = config_system.item.visuals.clr_weapon_icon[2] * 255;
	Color CasualCol = Color(
		red,
		green,
		blue,
		alpha[pEntity->EntIndex()]);

	unsigned int font = getFont( g_Fonts->visualsWeapon );


	if ( config_system.item.visuals.player_weapon_text )
		g_Renderer->text( Box.x + ( Box.w / 2 ), Box.y + Box.h + offset - ( font == g_Fonts->visualsAlternate ? 3 : 0 ), getFont( g_Fonts->visualsWeapon ), strWeaponName.c_str( ), Color( 255, 255, 255, alpha[ pEntity->EntIndex( ) ] ), true );


	if ( config_system.item.visuals.player_weapon_icon )
	{
		g_Renderer->text( Box.x + ( Box.w / 2 ), Box.y + Box.h + ( config_system.item.visuals.player_weapon_text ? offset + 4 : offset - 6 ), g_Fonts->visualsWeaponIcon, weaponToIcon( weapon->itemDefenitionIndex( ) ), CasualCol, true );

		if ( weaponToIcon( weapon->itemDefenitionIndex( ) ) == "" && !config_system.item.visuals.player_weapon_text )
		{
			g_Renderer->text( Box.x + ( Box.w / 2 ), Box.y + Box.h + offset - ( font == g_Fonts->visualsAlternate ? 3 : 0 ), getFont( g_Fonts->visualsWeapon ), strWeaponName.c_str( ), Color( 255, 255, 255, alpha[ pEntity->EntIndex( ) ] ), true );
		}
	}

}

static bool reload = false; // meh
void CVisuals::playerAmmo( CBaseEntity* pEntity )
{
	auto weapon = pEntity->activeWeapon( );

	if ( !weapon )
		return;

	int ammo = weapon->ammo( );

	float width = ammo * Box.w / weapon->weaponData( )->maxClip;

	CAnimationLayer animationLayer = pEntity->animOverlayStructByIndex( 1 );

	if ( animationLayer.m_pOwner )
	{
		auto activity = pEntity->sequenceActivity( animationLayer.m_nSequence );

		bool reloading = ( activity == 967 && animationLayer.m_flWeight != 0.0f );
		reload = reloading;

		if ( reloading && animationLayer.m_flCycle < 0.99 )
			width = ( animationLayer.m_flCycle * Box.w ) / 1.f;
	}

	int red = config_system.item.visuals.clr_ammo[0] * 255;
	int green = config_system.item.visuals.clr_ammo[1] * 255;
	int blue = config_system.item.visuals.clr_ammo[2] * 255;
	Color CasualCol = Color(
		red,
		green,
		blue,
		alpha[pEntity->EntIndex()]);

	g_Renderer->rectangle( Box.x - 1, Box.y + Box.h + 3, Box.w + 2, 4, Color( 0, 0, 0, alpha[ pEntity->EntIndex( ) ] ) );
	g_Renderer->fillRectangle( Box.x, Box.y + Box.h + 4, Box.w, 2, Color( 0, 0, 0, alpha[ pEntity->EntIndex( ) ] * 0.85 ) );
	g_Renderer->fillRectangle( Box.x, Box.y + Box.h + 4, width, 2, CasualCol );

	if ( weapon->ammo( ) < weapon->weaponData( )->maxClip )
	{
		g_Renderer->text( Box.x + width, Box.y + Box.h + 1, getFont( g_Fonts->visualsAmmoText ), std::to_string( ammo ), Color( 255, 255, 255, alpha[ pEntity->EntIndex( ) ] ) );
	}
}

void CVisuals::playerFlags( CBaseEntity* pEntity )
{
	int x = Box.x + Box.w + 2, y = Box.y - 1;

	auto drawFlag = [ & ]( const std::string& str, Color color, ... ) -> void
	{
		g_Renderer->text( x, y, getFont( g_Fonts->visualsFlags ), str, color, false );
		y += 10;
	};

	auto fakeDuck = [ & ]( ) -> bool
	{
		static float storedTick;
		static float crouchedTicks[ 65 ];

		if ( !pEntity->animState( ) )
			return false;

		if ( !pEntity->alive( ) )
			return false;

		float m_flDuckAmount = pEntity->animState( )->m_fDuckAmount;
		float m_flDuckSpeed = pEntity->m_fDuckSpeed( );
		float m_fFlags = pEntity->flags( );

		if ( m_flDuckSpeed != 0.f && m_flDuckAmount != 0.f )
		{
			if ( m_flDuckSpeed == 8.f && m_flDuckAmount <= 0.9f && m_flDuckAmount > 0.01f )
			{
				if ( storedTick != g_Interfaces->globalVars->tickcount )
				{
					crouchedTicks[ pEntity->EntIndex( ) ] = crouchedTicks[ pEntity->EntIndex( ) ] + 1;
					storedTick = g_Interfaces->globalVars->tickcount;
				}
				return ( crouchedTicks[ pEntity->EntIndex( ) ] >= 5 );
			}
			else
				crouchedTicks[ pEntity->EntIndex( ) ] = 0;
		}

		return false;
	};

	bool kevlar = pEntity->armorValue( ) > 0, helmet = pEntity->hasHelmet( );

	if (config_system.item.visuals.player_flags_money)
		drawFlag("$" + std::to_string(pEntity->m_iAccount()), Color(25, 255, 25, alpha[pEntity->EntIndex()]));

	if (config_system.item.visuals.player_flags_armor && helmet || kevlar)
		drawFlag( pEntity->armorName(), Color( 255, 255, 255, alpha[ pEntity->EntIndex( ) ] ) );
		//drawFlag( armor, Color( 255, 255, 255, alpha[ pEntity->EntIndex( ) ] ) );

	if (config_system.item.visuals.player_flags_scoped & ( 1 << 2 ) && pEntity->scoped( ))
		drawFlag( "zoom", Color( 150, 150, 220, alpha[ pEntity->EntIndex( ) ] ) );

	if (config_system.item.visuals.player_flags_defusing && pEntity->defusing())
		drawFlag( "defusing", Color(255, 255, 255, alpha[pEntity->EntIndex()]));

	if (config_system.item.visuals.player_flags_rescuing && pEntity->rescuing( ))
		drawFlag( "rescuing", Color( 255, 255, 255, alpha[ pEntity->EntIndex( ) ] ) );

	if (config_system.item.visuals.player_flags_reloading && reload)
		drawFlag( "reload", Color( 255, 255, 255, alpha[ pEntity->EntIndex( ) ] ) );

	if (config_system.item.visuals.player_flags_c4)
	{

	}

	if (config_system.item.visuals.player_flags_flashed)
	{

	}

	if (config_system.item.visuals.player_flags_fakeducking && fakeDuck( ) && pEntity->flags( ) & FL_ONGROUND)
		drawFlag( "fake-duck", Color( 255, 255, 255, alpha[ pEntity->EntIndex( ) ] ) );

	//drawFlag( Globals::bruteStage[ pEntity->EntIndex( ) ], Color( 255, 255, 255, alpha[ pEntity->EntIndex( ) ] ) );

	//if ( Globals::aimbotTarget )
	//	drawFlag( std::to_string( Globals::MissedShots[ Globals::aimbotTarget->EntIndex( ) ] ), Color( 255, 200, 0, alpha[ pEntity->EntIndex( ) ] ) );
}

void CVisuals::playerHistoryBones( CBaseEntity* pEntity )
{
	int index = pEntity->EntIndex( );
	studiohdr_t* pStudioModel = g_Interfaces->modelInfo->getStudiomodel( ( model_t* ) pEntity->GetModel( ) );

	if ( !pStudioModel )
		return;

	auto& records = g_LagCompensation.PlayerRecord[ index ].records;

	if ( records.empty( ) )
		return;

	for ( int i = 1; i < records.size( ); ++i )
	{
		auto& record = records.at( 0 );

		for ( int i = 0; i < pStudioModel->numbones; ++i )
		{
			mstudiobone_t* pBone = pStudioModel->getBone( i );

			if ( !pBone || !( pBone->flags & 256 ) || pBone->parent == -1 )
				continue;

			Vector3 bonePos1 = g_Utils->bonePosition( pEntity, i, record.boneMatrix ), vBonePos1;

			if ( bonePos1 == Vector3( 0, 0, 0 ) )
				continue;

			if ( !g_Interfaces->debugOverlay->WorldToScreen( bonePos1, vBonePos1 ) )
				continue;

			Vector3 bonePos2 = g_Utils->bonePosition( pEntity, pBone->parent, record.boneMatrix ), vBonePos2;

			if ( bonePos2 == Vector3( 0, 0, 0 ) )
				continue;

			if ( !g_Interfaces->debugOverlay->WorldToScreen( bonePos2, vBonePos2 ) )
				continue;

			int red = config_system.item.visuals.clr_backtrack_skeleton[0] * 255;
			int green = config_system.item.visuals.clr_backtrack_skeleton[1] * 255;
			int blue = config_system.item.visuals.clr_backtrack_skeleton[2] * 255;

			g_Renderer->line( ( int ) vBonePos1.x, ( int ) vBonePos1.y, ( int ) vBonePos2.x, ( int ) vBonePos2.y, Color( red, green, blue, 255 ) );
		}
	}
}

void CVisuals::playerBones(CBaseEntity* pEntity)
{
	int index = pEntity->EntIndex();
	studiohdr_t* pStudioModel = g_Interfaces->modelInfo->getStudiomodel((model_t*)pEntity->GetModel());

	if (!pStudioModel)
		return;

	for (int i = 0; i < pStudioModel->numbones; ++i)
	{
		mstudiobone_t* pBone = pStudioModel->getBone(i);

		if (!pBone || !(pBone->flags & 256) || pBone->parent == -1)
			continue;

		Vector3 vChild = pEntity->GetBonePosition(i);
		Vector3 vParent = pEntity->GetBonePosition(pBone->parent);

		int iChestBone = 6;  // Parameter of relevant Bone number
		Vector3 vBreastBone; // New reference Point for connecting many bones
		Vector3 vUpperDirection = pEntity->GetBonePosition(iChestBone + 1) - pEntity->GetBonePosition(iChestBone); // direction vector from chest to neck
		vBreastBone = pEntity->GetBonePosition(iChestBone) + vUpperDirection / 2;
		Vector3 vDeltaChild = vChild - vBreastBone; // Used to determine close bones to the reference point
		Vector3 vDeltaParent = vParent - vBreastBone;

		// Eliminating / Converting all disturbing bone positions in three steps.
		if ((vDeltaParent.Length() < 9 && vDeltaChild.Length() < 9))
			vParent = vBreastBone;

		if (i == iChestBone - 1)
			vChild = vBreastBone;

		if (abs(vDeltaChild.z) < 5 && (vDeltaParent.Length() < 5 && vDeltaChild.Length() < 5) || i == iChestBone)
			continue;

		float sParent = 0;
		float sChild = 0;

		//g_Interfaces->debugOverlay->screenPosition(vParent, sParent);
		//Interfaces.g_pDebugOverlay->ScreenPosition(vParent, sParent);
		//Interfaces.g_pDebugOverlay->ScreenPosition(vChild, sChild);

		//if (!g_Interfaces->debugOverlay->WorldToScreen(bonePos2, vBonePos2))
			//continue;

		//g_Renderer->line(sParent[0], sParent[1], sChild[0], sChild[1], SkellyCol));
	}
}

void CVisuals::worldWeapon( CBaseEntity * pEntity )
{
	CClientClass* clientClass = pEntity->GetClientClass( );

	if ( !clientClass )
		return;

	if ( !boundingBox( pEntity ) )
		return;

	auto owner = g_Interfaces->clientEntity->GetClientEntityFromHandle( pEntity->owner( ) );

	if ( !owner && ( clientClass->iClassID != CBaseWeaponWorldModel && ( std::strstr( clientClass->NetworkName, "Weapon" ) || clientClass->iClassID == CDEagle || clientClass->iClassID == CAK47 ) ) )
	{
		auto weapon = static_cast< CBaseCombatWeapon* >( pEntity );

		if ( !weapon )
			return;

		if ( !weapon->itemDefenitionIndex( ) )
			return;

		const auto info = g_Interfaces->weaponSystem->GetWpnData( weapon->itemDefenitionIndex( ) );

		if ( !info )
			return;

		int red = config_system.item.visuals.clr_ammo[0] * 255;
		int green = config_system.item.visuals.clr_ammo[1] * 255;
		int blue = config_system.item.visuals.clr_ammo[2] * 255;
		Color WeaponsCol = Color(red, green, blue, 255);

		if ( config_system.item.visuals.droppedweapons_name )
		{
			const char *name = info->hudName;
			wchar_t* localised_name = g_Interfaces->localize->Find( name );

			std::string strWeaponName( std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>( ).to_bytes( localised_name ) );
			std::transform( strWeaponName.begin( ), strWeaponName.end( ), strWeaponName.begin( ), ::toupper );

			unsigned int font = getFont( g_Fonts->visualsAmmoText );
			g_Renderer->text( Box.x + ( Box.w / 2 ), Box.y + Box.h - ( font == g_Fonts->visualsAmmoText ? 9 : 11 ), getFont( g_Fonts->visualsAmmoText ), strWeaponName, Color( 255, 255, 255 ), true );
		}

		if ( config_system.item.visuals.droppedweapons_ammo )
		{
			const auto ammo = weapon->ammo( );
			const auto max_ammo = info->maxClip;
			auto width = Box.w + 14;

			width *= ammo;
			width /= max_ammo;


			g_Renderer->fillRectangle( Box.x - 7, Box.y + Box.h, Box.w + 16, 4, WeaponsCol );
			g_Renderer->fillRectangle( Box.x - 6, Box.y + Box.h + 1, width, 2, WeaponsCol );
		}
	}
}

void CVisuals::localViewModelSpoof( )
{
	//n o
}

void CVisuals::localViewModelCM( )
{
	//n o
}

void CVisuals::noScopeLines( )
{
	if ( !Globals::localPlayer->alive( ) )
		return;

	int Height, Width;
	g_Interfaces->gameEngine->getScreenSize( Width, Height );

	float x = Width / 2;
	float y = Height / 2;

	if ( Globals::localPlayer->activeWeapon( )->hasSniperScope( ) )
	{
		g_Renderer->line( 0, y, Width, y, Color( 0, 0, 0, 255 ) );
		g_Renderer->line( x, 0, x, Height, Color( 0, 0, 0, 255 ) );
	}
}

void CVisuals::inaccuracyOverlay( )
{
	if ( !config_system.item.visuals.inaccuracy_overlay )
		return;

	if ( !g_Interfaces->gameEngine->connected( ) && !g_Interfaces->gameEngine->inGame( ) )
		return;

	if ( !Globals::localPlayer )
		return;

	if ( !Globals::localPlayer->alive( ) )
		return;

	auto weapon = Globals::localPlayer->activeWeapon( );

	if ( !weapon )
		return;

	if ( Globals::localPlayer->isKnifeorNade( ) )
		return;

	int width, height;
	g_Interfaces->gameEngine->getScreenSize( width, height );

	double spreadDistance = ( ( weapon->inaccuracy( ) + weapon->spread( ) ) * 320 ) * ( config_system.item.visuals.inaccuracy_overlay_size / 5.f ); // b1g maths lmao
	double spreadSize = ( height * 0.002083 ) * ( spreadDistance / 10 );

	int red = config_system.item.visuals.clr_inaccuracy_overlay[0] * 255;
	int green = config_system.item.visuals.clr_inaccuracy_overlay[1] * 255;
	int blue = config_system.item.visuals.clr_inaccuracy_overlay[2] * 255;
	int alpha1 = config_system.item.visuals.clr_inaccuracy_overlay[3] * 255;

	g_renderer_d3d->clircleFilledDualColour( width / 2, height / 2, spreadSize, 0.0f, 1, 100, D3DCOLOR_ARGB( alpha1, red, green, blue ), D3DCOLOR_ARGB( 0, 0, 0, 0 ), g_Interfaces->dx9Device );
}

void CVisuals::glow( )
{
	if ( !Globals::localPlayer )
		return;

	if ( !config_system.item.visuals.enable_glow )// && !config->get_bool( "espGlowLocal" ) )
		return;

	for ( auto i = 0; i < g_Interfaces->glow->size; i++ )
	{
		auto glowObject = &g_Interfaces->glow->m_GlowObjectDefinitions[ i ];
		CBaseEntity* pEntity = glowObject->m_pEntity;

		if ( !glowObject->m_pEntity || glowObject->IsUnused( ) )
			continue;

		if ( pEntity->GetClientClass( )->iClassID == 40)
		{
			if ( pEntity->team( ) != Globals::localPlayer->team( ) && config_system.item.visuals.glow_enemy) // enemy
			{
				int red = config_system.item.visuals.clr_glow[0] * 255;
				int green = config_system.item.visuals.clr_glow[1] * 255;
				int blue = config_system.item.visuals.clr_glow[2] * 255;
				int alpha1 = config_system.item.visuals.clr_glow[3] * 255;

				glowObject->m_flRed = red;
				glowObject->m_flGreen = green;
				glowObject->m_flBlue = blue;
				glowObject->m_flGlowAlpha = alpha1;

				glowObject->m_bRenderWhenOccluded = true;
				glowObject->m_bRenderWhenUnoccluded = false;
			}
			else if ( pEntity->team( ) == Globals::localPlayer->team( ) && config_system.item.visuals.glow_team) // team
			{
				int red = config_system.item.visuals.clr_glow_team[0] * 255;
				int green = config_system.item.visuals.clr_glow_team[1] * 255;
				int blue = config_system.item.visuals.clr_glow_team[2] * 255;
				int alpha1 = config_system.item.visuals.clr_glow_team[3] * 255;

				glowObject->m_flRed = red;
				glowObject->m_flGreen = green;
				glowObject->m_flBlue = blue;
				glowObject->m_flGlowAlpha = alpha1;

				glowObject->m_bRenderWhenOccluded = true;
				glowObject->m_bRenderWhenUnoccluded = false;
			}
			else if ( pEntity == Globals::localPlayer && config_system.item.visuals.glow_local && Globals::inThirdperson && Globals::localPlayer->alive( ) ) // local
			{
				int red = config_system.item.visuals.clr_glow_local[0] * 255;
				int green = config_system.item.visuals.clr_glow_local[1] * 255;
				int blue = config_system.item.visuals.clr_glow_local[2] * 255;
				int alpha1 = config_system.item.visuals.clr_glow_local[3] * 255;

				glowObject->m_flRed = red;
				glowObject->m_flGreen = green;
				glowObject->m_flBlue = blue;
				glowObject->m_flGlowAlpha = alpha1;

				glowObject->m_bRenderWhenOccluded = true;
				glowObject->m_bRenderWhenUnoccluded = false;

			}
		}
	}
}

void CVisuals::historyHitbox( int entity )
{
	if ( entity == g_Interfaces->gameEngine->getLocalPlayer( ) )
		return;

	CBaseEntity* pEntity = g_Interfaces->clientEntity->GetClientEntity( entity );

	if ( !pEntity )
		return;

}

static ConVar* oldSkyName;

void CVisuals::nightModulation( )
{
	if ( !Globals::localPlayer )
		return;

	static auto r_drawspecificstaticprop = g_Interfaces->cvar->FindVar( "r_DrawSpecificStaticProp" );
	r_drawspecificstaticprop->SetValue( 0 );

	if ( !g_Interfaces->gameEngine->inGame( ) )
	{
		bNightModulationComplete = false;
		return;
	}

	static float oldBrightness;

	oldSkyName = g_Interfaces->cvar->FindVar( "sv_skyname" );


	float brightness = config_system.item.visuals.modulation_amount / 100.f;

	if ( !bNightModulationComplete || oldBrightness != brightness )
	{
		for ( MaterialHandle_t i = g_Interfaces->materialSystem->FirstMaterial( ); i != g_Interfaces->materialSystem->InvalidMaterial( ); i = g_Interfaces->materialSystem->NextMaterial( i ) )
		{
			IMaterial *pMaterial = g_Interfaces->materialSystem->GetMaterial( i );

			if ( !pMaterial )
				continue;

			if ( strstr( pMaterial->GetTextureGroupName( ), "World" ) )
			{
				pMaterial->ColorModulate( brightness - 0.1f, brightness - 0.1f, brightness - 0.1f );
			}
			else if ( strstr( pMaterial->GetTextureGroupName( ), "StaticProp" ) )
			{
				pMaterial->ColorModulate( brightness + 0.05f, brightness + 0.05f, brightness + 0.05f );
			}
			if ( strstr( pMaterial->GetTextureGroupName( ), ( "SkyBox" ) ) )
			{
				//pMaterial->ColorModulate( config->get_color( "colorSkyBox" )[ 0 ] / 255.f, config->get_color( "colorSkyBox" )[ 1 ] / 255.f, config->get_color( "colorSkyBox" )[ 2 ] / 255.f );
			}
		}

		g_Utils->loadNamedSky( "sky_csgo_night02" );

		bNightModulationComplete = true;
		oldBrightness = brightness;
	}
}

void CVisuals::removeNight( )
{
	if ( !Globals::localPlayer )
		return;

	static auto r_drawspecificstaticprop = g_Interfaces->cvar->FindVar( "r_DrawSpecificStaticProp" );
	r_drawspecificstaticprop->SetValue( 1 );

	if ( !g_Interfaces->gameEngine->inGame( ) )
	{
		bNightModulationRemoveComplete = false;
		return;
	}

	if ( !bNightModulationRemoveComplete )
	{
		for ( MaterialHandle_t i = g_Interfaces->materialSystem->FirstMaterial( ); i != g_Interfaces->materialSystem->InvalidMaterial( ); i = g_Interfaces->materialSystem->NextMaterial( i ) )
		{
			IMaterial *pMaterial = g_Interfaces->materialSystem->GetMaterial( i );

			if ( !pMaterial )
				continue;

			if ( strstr( pMaterial->GetTextureGroupName( ), "World" ) )
			{
				pMaterial->ColorModulate( 1.f, 1.f, 1.f );
			}
			else if ( strstr( pMaterial->GetTextureGroupName( ), "StaticProp" ) )
			{
				pMaterial->ColorModulate( 1.f, 1.f, 1.f );
			}
			if ( strstr( pMaterial->GetTextureGroupName( ), ( "SkyBox" ) ) )
			{
				pMaterial->ColorModulate( 1.f, 0.f, 1.f );
			}
		}

		if ( oldSkyName )
			g_Utils->loadNamedSky( oldSkyName->GetString( ) );

		bNightModulationRemoveComplete = true;
	}
}

void CVisuals::applyFullBright( )
{
	if ( !Globals::localPlayer )
		return;

	static auto r_drawspecificstaticprop = g_Interfaces->cvar->FindVar( "r_DrawSpecificStaticProp" );
	r_drawspecificstaticprop->SetValue( 0 );

	if ( !g_Interfaces->gameEngine->inGame( ) )
	{
		bFullBrightComplete = false;
		return;
	}

	static int oldSetting;

	int curSettings = config_system.item.visuals.modulation_fullbright;

	if ( !bFullBrightComplete || oldSetting != curSettings )
	{
		for ( MaterialHandle_t i = g_Interfaces->materialSystem->FirstMaterial( ); i != g_Interfaces->materialSystem->InvalidMaterial( ); i = g_Interfaces->materialSystem->NextMaterial( i ) )
		{
			IMaterial *pMaterial = g_Interfaces->materialSystem->GetMaterial( i );

			if ( !pMaterial )
				continue;

			if ( strstr( pMaterial->GetTextureGroupName( ), ( "SkyBox" ) ) )
			{
				pMaterial->ColorModulate( 1.f, 0.f, 1.f );
			}
		}

		g_Interfaces->cvar->FindVar( "mat_fullbright" )->SetValue( 1 );

		bFullBrightComplete = true;
		oldSetting = curSettings;
	}
}

void CVisuals::removeFullBright( )
{
	if ( !Globals::localPlayer )
		return;

	static auto r_drawspecificstaticprop = g_Interfaces->cvar->FindVar( "r_DrawSpecificStaticProp" );
	r_drawspecificstaticprop->SetValue( 1 );

	if ( !g_Interfaces->gameEngine->inGame( ) )
	{
		bFullBrightRemoveComplete = false;
		return;
	}

	if ( !bFullBrightRemoveComplete )
	{
		g_Interfaces->cvar->FindVar( "mat_fullbright" )->SetValue( 0 );
		bFullBrightRemoveComplete = true;
	}
}

void CVisuals::outOfPovArrows( CBaseEntity* pEntity ) // premium
{

	auto isOnScreen = [ ]( Vector3 origin, Vector3& screen ) -> bool
	{
		if ( !g_Interfaces->debugOverlay->WorldToScreen( origin, screen ) ) return false;
		int iScreenWidth, iScreenHeight;
		g_Interfaces->gameEngine->getScreenSize( iScreenWidth, iScreenHeight );
		bool xOk = iScreenWidth > screen.x > 0, yOk = iScreenHeight > screen.y > 0;
		return xOk && yOk;
	};

	Vector3 screenPos;
	if ( isOnScreen( pEntity->getHitbox( 2 ), screenPos ) )
		return;

	if ( pEntity->IsDormant( ) )
		return;

	auto rotateArrow = [ ]( std::array< Vector2, 3 >& points, float rotation )->void
	{
		const auto points_center = ( points.at( 0 ) + points.at( 1 ) + points.at( 2 ) ) / 3;
		for ( auto& point : points )
		{
			point -= points_center;

			const auto temp_x = point.x;
			const auto temp_y = point.y;

			const auto theta = DEG2RAD( rotation );
			const auto c = cos( theta );
			const auto s = sin( theta );

			point.x = temp_x * c - temp_y * s;
			point.y = temp_x * s + temp_y * c;

			point += points_center;
		}
	};

	const int fade = ( int ) ( ( 4 * g_Interfaces->globalVars->frametime ) * 255 );

	int new_alpha = arrow_alpha[ pEntity->index( ) ];
	new_alpha += pEntity->dormant( ) ? -fade : fade;
	new_alpha = std::clamp< int >( new_alpha, 0, 200 );
	arrow_alpha[ pEntity->index( ) ] = new_alpha;

	Vector3 viewangles;
	g_Interfaces->gameEngine->getViewAngles( viewangles );
	int width, height;

	g_Interfaces->gameEngine->getScreenSize( width, height );

	//	const auto screen_center = Vector2( width / 2.f, height / 2.f );
	//  float division is slower, do this instead:
	const auto screen_center = Vector2( width * .5f, height * .5f );

	const auto angle_yaw_rad = DEG2RAD( viewangles.y - g_Math.calcAngle( Globals::localPlayer->eyePosition( ), pEntity->getHitbox( 2 ) ).y - 90 );

	int radius = max( 10, config_system.item.visuals.out_of_fov_arrow_radius );
	int size = max( 5, config_system.item.visuals.out_of_fov_arrow_size );

	const auto new_point_x = screen_center.x + ( ( ( ( width - ( size * 3 ) ) * .5f ) * ( radius / 100.0f ) ) * cos( angle_yaw_rad ) ) + ( int ) ( 6.0f * ( ( ( float ) size - 4.f ) / 16.0f ) );
	const auto new_point_y = screen_center.y + ( ( ( ( height - ( size * 3 ) ) * .5f ) * ( radius / 100.0f ) ) * sin( angle_yaw_rad ) );

	std::array< Vector2, 3 >points { Vector2( new_point_x - size, new_point_y - size ),
		Vector2( new_point_x + size, new_point_y ),
		Vector2( new_point_x - size, new_point_y + size ) };

	int red = config_system.item.visuals.clr_out_of_fov[0] * 255;
	int green = config_system.item.visuals.clr_out_of_fov[1] * 255;
	int blue = config_system.item.visuals.clr_out_of_fov[2] * 255;
	Color CasualCol = Color(
		red,
		green,
		blue,
		new_alpha);

	rotateArrow( points, viewangles.y - g_Math.calcAngle( Globals::localPlayer->eyePosition( ), pEntity->getHitbox( 2 ) ).y - 90 );
	g_Renderer->fillTriangle( points, CasualCol );

}
/*
void CVisuals::antiaimIndicator(CBaseEntity* pEntity)
{
	//g_Renderer=Surface Drawings

	if (!Globals::localPlayer || !g_Interfaces->gameEngine->inGame())
		return;

	int x, y;
	g_Interfaces->gameEngine->getScreenSize(x, y);
	int centerX = x / 2;

	int m_Framerate = 0.9 * m_Framerate + (1.0 - 0.9) * g_Interfaces->globalVars->absoluteframetime;

	//left
	g_Renderer->gradient(centerX - 200, y - 20, centerX - 51, y, Color(255, 0, 0), Color(255, 0, 0), true);
	g_Renderer->gradient(centerX - 200, y - 20, centerX - 51, y - 19, Color(0, 255, 0), Color(0, 255, 0), true);

	//middle
	g_Renderer->fillRectangle(centerX - 50, y - 20, centerX + 50, y, Color(0, 0, 200));

	g_Renderer->fillRectangle(centerX - 50, y - 20, centerX + 50, y - 19, Color(0, 0, 255));

	//right
	g_Renderer->gradient(centerX + 50, y - 20, centerX + 200, y, Color(0, 0, 255), Color(0, 0, 255), false);
	g_Renderer->gradient(centerX + 50, y - 20, centerX + 200, y - 19, Color(0, 0, 255), Color(0, 0, 255), false);

	//fps
	g_Renderer->text(centerX - 10, y - 15, getFont(g_Fonts->visuals), std::to_string((int)(1.0f / m_Framerate)), Color(255, 255, 255), true);
	//draw.Text(centerX - 10, y - 15, get_abs_fps();

	//draw.Color(200, 255, 0, 255);
	//draw.Text(centerX + 10, y - 15, "fps");

	g_Renderer->text(centerX + 10, y - 15, getFont(g_Fonts->visuals), "fps", Color(200, 255, 0, 255), true);

	//kills
	//draw.Color(255, 255, 255, 255);
	//draw.Text(centerX - 70, y - 15, kills);

	//draw.Color(255, 100, 0, 255);
	//draw.Text(centerX - 55, y - 15, "kills");

	//deaths
	//draw.Color(255, 255, 255, 255);
	//draw.Text(centerX + 50, y - 15, deaths);

	//draw.Color(255, 50, 50, 255);
	//draw.Text(centerX + 65, y - 15, "deaths");
}*/

/*void CVisuals::drawAntiaimIndicator(bool hotkey)
{
	if (!g_Interfaces->gameEngine->inGame() || !g_Interfaces->gameEngine->connected())
		return;

	if (!Globals::localPlayer)
		return;

	if (!Globals::localPlayer->alive())
		return;

	int x, y;
	g_Interfaces->gameEngine->getScreenSize(x, y);

	if (!config->get_bool("aaEnable"))
		return;

	Color DirectionCol = Color(
		config->get_color("clr_menu").r(),
		config->get_color("clr_menu").g(),
		config->get_color("clr_menu").b(),
		config->get_color("clr_menu").a());
	if (hotkey)
	{
		g_Renderer->text((x / 2) + 40, y / 2, getFont(g_Fonts->visuals), ">", Color(255, 255, 255, config->get_color("clr_menu").a()), true);
		g_Renderer->text((x / 2) - 40, y / 2, getFont(g_Fonts->visuals), "<", DirectionCol, true);
	}
	else
	{
		g_Renderer->text((x / 2) + 40, y / 2, getFont(g_Fonts->visuals), ">", DirectionCol, true);
		g_Renderer->text((x / 2) - 40, y / 2, getFont(g_Fonts->visuals), "<", Color(255, 255, 255, config->get_color("clr_menu").a()), true);
	}
}*/