#include "../managers/hookManager.hpp"
#include "../../features/Aimbot/LagCompensation.hpp"

using drawModelExecuteFn = void( __thiscall* )( IVModelRender*, void*, void*, const ModelRenderInfo_t&, matrix3x4_t* );
void __stdcall  CHookManager::drawModelExecute( void* context, void* state, const ModelRenderInfo_t& info, matrix3x4_t* matrix ) {

	auto originalFn = g_Hooks->modelRenderHook->getOriginalFunction<drawModelExecuteFn>( 21 );

	if ( g_Interfaces->modelRender->IsForcedMaterialOverride( ) ) {
		return originalFn( g_Interfaces->modelRender, context, state, info, matrix );
	}

	static bool initialized = false;
	static IMaterial* material = nullptr;
	static IMaterial* lcMat = nullptr;
	static IMaterial* textured = nullptr;
	static IMaterial* metallic = nullptr;
	static IMaterial* dogtag = nullptr;
	static IMaterial* flat = nullptr;
	if ( !initialized ) {
		initialized = true;

		textured = g_Interfaces->materialSystem->FindMaterial("dopamine_material", TEXTURE_GROUP_MODEL, true, nullptr);
		metallic = g_Interfaces->materialSystem->FindMaterial("dopamine_reflective", TEXTURE_GROUP_MODEL, true, nullptr);
		dogtag = g_Interfaces->materialSystem->FindMaterial("models/inventory_items/dogtags/dogtags_outline", TEXTURE_GROUP_MODEL, true, nullptr);
		flat = g_Interfaces->materialSystem->FindMaterial("debug/debugdrawflat", TEXTURE_GROUP_MODEL, true, nullptr);
		lcMat = flat;
	}
	textured->IncrementReferenceCount();
	metallic->IncrementReferenceCount();
	dogtag->IncrementReferenceCount();
	flat->IncrementReferenceCount();
	lcMat->IncrementReferenceCount();

	switch (config_system.item.visuals.chams_type) {
	case 0:
		material = textured;
		break;
	case 1:
		material = flat;
		break;
	case 2:
		material = metallic;
		break;
	case 3:
		material = dogtag;
		break;
	}

	CBaseEntity* entity = reinterpret_cast< CBaseEntity* >( g_Interfaces->clientEntity->GetClientEntity( info.entity_index ) );
	std::string model_name = g_Interfaces->modelInfo->getModelName( ( model_t* )info.pModel );

	if ( g_Interfaces->gameEngine->inGame( ) ) {
		if ( Globals::localPlayer )
		{
			if ( config_system.item.visuals.enable_visuals && config_system.item.visuals.chams_type != 0 )
			{
				if ( model_name.find( "models/player" ) != std::string::npos )
				{
					//if ( material && materialIgnoreZ && materialDefault && materialDefaultIgnoreZ && materialFlat && materialFlatIgnoreZ && materialShaded && materialShadedIgnoreZ && materialMetallic && materialMetallicIgnoreZ && materialGlow && materialGlowIgnoreZ && materialBubble && materialBubbleIgnoreZ && lcMat )
					if ( material && textured && metallic && dogtag && flat && lcMat )
					{
						if ( entity )
						{
							if ( entity->team( ) != Globals::localPlayer->team( ) ) // enemy
							{
								// man we're so fucking smart, we better hook drawmodel instead of drawmodelexecute, because POLAK DOES IT. right? ~alpha
								if ( entity->health( ) > 0 )
								{
									auto& records = g_LagCompensation.PlayerRecord[ entity->EntIndex( ) ].records;
									if ( !records.empty( ) ) {
										matrix3x4_t matrix[ 128 ];
										std::memcpy( matrix, records.front( ).boneMatrix, sizeof( records.front( ).boneMatrix ) );

										// lag comp 
										if ( Globals::localPlayer->alive( ) && lcMat && config_system.item.visuals.chams_backtrack_visualize && !entity->immunity( ) )
										{
											auto red = config_system.item.visuals.clr_chams_backtrack_visualize[0] * 255;
											auto green = config_system.item.visuals.clr_chams_backtrack_visualize[1] * 255;
											auto blue = config_system.item.visuals.clr_chams_backtrack_visualize[2] * 255;
											auto alpha = config_system.item.visuals.clr_chams_backtrack_visualize[3] * 255;
											lcMat->AlphaModulate( alpha );
											lcMat->ColorModulate( red, green, blue );
											lcMat->SetMaterialVarFlag( MATERIAL_VAR_IGNOREZ, true );
											g_Interfaces->modelRender->ForcedMaterialOverride( lcMat );
											originalFn( g_Interfaces->modelRender, context, state, info, matrix );
										}

									}

									// visible 
									if (config_system.item.visuals.chams_enemy_vis) // player
									{
										auto red = config_system.item.visuals.clr_chams_enemy_vis[0] * 255;
										auto green = config_system.item.visuals.clr_chams_enemy_vis[1] * 255;
										auto blue = config_system.item.visuals.clr_chams_enemy_vis[2] * 255;
										auto alpha = config_system.item.visuals.clr_chams_enemy_vis[3] * 255;
										material->AlphaModulate(alpha);
										material->ColorModulate(red, green, blue);
										material->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);

										g_Interfaces->modelRender->ForcedMaterialOverride(material);
										originalFn(g_Interfaces->modelRender, context, state, info, matrix);
									}

									//invisible
									if (config_system.item.visuals.chams_enemy_invis) // player (behind wall)
									{
										auto red = config_system.item.visuals.clr_chams_enemy_invis[0] * 255;
										auto green = config_system.item.visuals.clr_chams_enemy_invis[1] * 255;
										auto blue = config_system.item.visuals.clr_chams_enemy_invis[2] * 255;
										auto alpha = config_system.item.visuals.clr_chams_enemy_invis[3] * 255;
										material->AlphaModulate(alpha);
										material->ColorModulate(red, green, blue);
										material->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);

										g_Interfaces->modelRender->ForcedMaterialOverride(material);
										originalFn(g_Interfaces->modelRender, context, state, info, matrix);
									}
								}
							}
							if (entity->team() == Globals::localPlayer->team()) // teammate
							{
								// visible 
								if (config_system.item.visuals.chams_team_vis) // teammate
								{
									auto red = config_system.item.visuals.clr_chams_team_vis[0] * 255;
									auto green = config_system.item.visuals.clr_chams_team_vis[1] * 255;
									auto blue = config_system.item.visuals.clr_chams_team_vis[2] * 255;
									auto alpha = config_system.item.visuals.clr_chams_team_vis[3] * 255;
									material->AlphaModulate(alpha);
									material->ColorModulate(red, green, blue);
									material->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);

									g_Interfaces->modelRender->ForcedMaterialOverride(material);
									originalFn(g_Interfaces->modelRender, context, state, info, matrix);
								}
								//invisible
								if (config_system.item.visuals.chams_team_invis) // teammate (behind wall)
								{
									auto red = config_system.item.visuals.clr_chams_team_invis[0] * 255;
									auto green = config_system.item.visuals.clr_chams_team_invis[1] * 255;
									auto blue = config_system.item.visuals.clr_chams_team_invis[2] * 255;
									auto alpha = config_system.item.visuals.clr_chams_team_invis[3] * 255;
									material->AlphaModulate(alpha);
									material->ColorModulate(red, green, blue);
									material->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);

									g_Interfaces->modelRender->ForcedMaterialOverride(material);
									originalFn(g_Interfaces->modelRender, context, state, info, matrix);
								}
							}
							if ( entity == Globals::localPlayer && Globals::inThirdperson && Globals::localPlayer->alive( ) )
							{
								// i'm sorry 
								bool shouldBlend = Globals::localPlayer->scoped( );
								float blendAmt = 0.25f;

								if ( Globals::localPlayer->activeWeapon( ) )
								{
									auto alpha = config_system.item.visuals.clr_chams_local[3] * 255;
									if ( Globals::localPlayer->activeWeapon( )->zoomLevel( ) == 1 )
										blendAmt = ( alpha ) * 0.1f;
									else if ( Globals::localPlayer->activeWeapon( )->zoomLevel( ) == 2 )
										blendAmt = ( alpha ) * 0.025f;
									else
										blendAmt = 1.0f;
								}

								// Fake angle chams
						
								if ( config_system.item.visuals.chams_local_desync )
								{
									Vector3 bonePos;
									Vector3 outPos;
									matrix3x4_t BoneMatrix[ 128 ];
									for ( int i = 0; i < 128; i++ )
									{
										g_Math.angleMatrix( Globals::fakeAngle, BoneMatrix[ i ] );
										g_Math.matrixMultiply( BoneMatrix[ i ], matrix[ i ] );
										bonePos = Vector3( matrix[ i ][ 0 ][ 3 ], matrix[ i ][ 1 ][ 3 ], matrix[ i ][ 2 ][ 3 ] ) - info.origin;
										g_Math.vectorRotate( bonePos, Globals::fakeAngle, outPos );
										outPos += info.origin;
										BoneMatrix[ i ][ 0 ][ 3 ] = outPos.x;
										BoneMatrix[ i ][ 1 ][ 3 ] = outPos.y;
										BoneMatrix[ i ][ 2 ][ 3 ] = outPos.z;
									}
									
									
									auto red = config_system.item.visuals.clr_chams_local_desync[0] * 255;
									auto green = config_system.item.visuals.clr_chams_local_desync[1] * 255;
									auto blue = config_system.item.visuals.clr_chams_local_desync[2] * 255;
									auto alpha = config_system.item.visuals.clr_chams_local_desync[3] * 255;
									material->AlphaModulate(alpha);
									material->ColorModulate(red, green, blue);

									material->SetMaterialVarFlag( MATERIAL_VAR_IGNOREZ, false );
									//Globals::localPlayer->setAbsOrigin(Globals::fakePosition);
									g_Interfaces->modelRender->ForcedMaterialOverride( material );
									originalFn( g_Interfaces->modelRender, context, state, info, BoneMatrix );
									//Globals::localPlayer->setAbsOrigin(Globals::localPlayer->GetAbsOrigin());
								}

								// Regular chams
								if ( config_system.item.visuals.chams_local ) {
									auto red = config_system.item.visuals.clr_chams_local[0] * 255;
									auto green = config_system.item.visuals.clr_chams_local[1] * 255;
									auto blue = config_system.item.visuals.clr_chams_local[2] * 255;
									auto alpha = config_system.item.visuals.clr_chams_local[3] * 255;
									material->AlphaModulate( shouldBlend ? blendAmt : alpha );

									material->AlphaModulate(alpha);
									material->ColorModulate(red, green, blue);

									material->SetMaterialVarFlag( MATERIAL_VAR_IGNOREZ, false );
									g_Interfaces->modelRender->ForcedMaterialOverride( material );
									originalFn( g_Interfaces->modelRender, context, state, info, matrix );
								}
							}
						}
						if (model_name.find("arms") != std::string::npos)
						{
							// arms
							if (config_system.item.visuals.chams_hands)
							{
								auto red = config_system.item.visuals.clr_chams_hands[0] * 255;
								auto green = config_system.item.visuals.clr_chams_hands[1] * 255;
								auto blue = config_system.item.visuals.clr_chams_hands[2] * 255;
								auto alpha = config_system.item.visuals.clr_chams_hands[3] * 255;
								material->AlphaModulate(alpha);
								material->ColorModulate(red, green, blue);

								material->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);

								g_Interfaces->modelRender->ForcedMaterialOverride(material);
								originalFn(g_Interfaces->modelRender, context, state, info, matrix);
							}
						}
						else if (model_name.find("weapon") != std::string::npos)
						{
							if (!(model_name.find("arms") != std::string::npos))
							{
								// arms
								if (config_system.item.visuals.chams_weapons)
								{
									auto red = config_system.item.visuals.clr_chams_weapons[0] * 255;
									auto green = config_system.item.visuals.clr_chams_weapons[1] * 255;
									auto blue = config_system.item.visuals.clr_chams_weapons[2] * 255;
									auto alpha = config_system.item.visuals.clr_chams_weapons[3] * 255;
									material->AlphaModulate(alpha);
									material->ColorModulate(red, green, blue);

									material->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);

									g_Interfaces->modelRender->ForcedMaterialOverride(material);
									originalFn(g_Interfaces->modelRender, context, state, info, matrix);
								}
							}
						}
					}
				}
			}
		}
	}

	originalFn( g_Interfaces->modelRender, context, state, info, matrix );
	g_Interfaces->modelRender->ForcedMaterialOverride( nullptr );
}