#include "bullet_impact.hpp"

void bullet_impact::fireGameEvent( IGameEvent * event )
{
	if ( !Globals::localPlayer | !event )
		return;

	
	if ( config_system.item.visuals.bullettracer_tracer || config_system.item.visuals.bullettracer_impact )
	{
		if ( g_Interfaces->gameEngine->getPlayerIndex( event->GetInt( "userid" ) ) == g_Interfaces->gameEngine->getLocalPlayer( ) && Globals::localPlayer && Globals::localPlayer->alive( ) )
		{
			float x = event->GetFloat( "x" ), y = event->GetFloat( "y" ), z = event->GetFloat( "z" );
			bulletImpactInfo.push_back( { g_Interfaces->globalVars->curtime, Vector3( x, y, z ) } );
		}
	}

	Globals::dequeBulletImpacts.push_back( Vector3( event->GetFloat( "x" ), event->GetFloat( "y" ), event->GetFloat( "z" ) ) );
}

int bullet_impact::getEventDebugID( void )
{
	return EVENT_DEBUG_ID_INIT;
}

void bullet_impact::registerSelf( )
{
	g_Interfaces->gameEvent->AddListener( this, "bullet_impact", false );
}

void bullet_impact::unRegisterSelf( )
{
	g_Interfaces->gameEvent->RemoveListener( this );
}

void bullet_impact::paint( void )
{
	if ( !g_Interfaces->gameEngine->inGame( ) || !Globals::localPlayer || !Globals::localPlayer->alive( ) )
	{
		bulletImpactInfo.clear( );
		return;
	}

	std::vector<BulletImpactInfo> &impacts = bulletImpactInfo;

	if ( impacts.empty( ) )
		return;

	int red1 = config_system.item.visuals.clr_bullettracer_tracer[0];
	int red2 = config_system.item.visuals.clr_bullettracer_impact[0];
	int green1 = config_system.item.visuals.clr_bullettracer_tracer[1];
	int green2 = config_system.item.visuals.clr_bullettracer_impact[1];
	int blue1 = config_system.item.visuals.clr_bullettracer_tracer[2];
	int blue2 = config_system.item.visuals.clr_bullettracer_impact[2];

	Color tracerColor( red1, green1, blue1 );
	Color impactColor( red2, green2, blue2 );

	if ( config_system.item.visuals.bullettracer_tracer || config_system.item.visuals.bullettracer_impact )
	{
		for ( size_t i = 0; i < impacts.size( ); ++i )
		{
			auto currentImpact = impacts.at( i );

			if ( config_system.item.visuals.bullettracer_tracer )
			{
				BeamInfo_t beamInfo;
				beamInfo.m_nType = TE_BEAMPOINTS;
				beamInfo.m_pszModelName = "sprites/purplelaser1.vmt";
				beamInfo.m_nModelIndex = -1;
				beamInfo.m_flHaloScale = 0.0f;
				beamInfo.m_flLife = 4.5f; //TODO: make this a slider, same as for impacts...
				beamInfo.m_flWidth = 3.f;
				beamInfo.m_flEndWidth = 3.f;
				beamInfo.m_flFadeLength = 0.0f;
				beamInfo.m_flAmplitude = 2.0f;
				beamInfo.m_flBrightness = 255.f;
				beamInfo.m_flSpeed = 0.2f;
				beamInfo.m_nStartFrame = 0;
				beamInfo.m_flFrameRate = 0.f;
				beamInfo.m_flRed = tracerColor.r( );
				beamInfo.m_flGreen = tracerColor.g( );
				beamInfo.m_flBlue = tracerColor.b( );
				beamInfo.m_nSegments = 2;
				beamInfo.m_bRenderable = true;
				beamInfo.m_nFlags = 0;

				beamInfo.m_vecStart = Globals::localPlayer->eyePosition( );
				beamInfo.m_vecEnd = currentImpact.m_vecHitPos;


				auto beam = g_Interfaces->renderBeams->createBeamPoints( beamInfo );

				if ( beam )
					g_Interfaces->renderBeams->drawBeam( beam );
			}

			if ( config_system.item.visuals.bullettracer_impact )
				g_Interfaces->debugOverlay->addBoxOverlay( currentImpact.m_vecHitPos, Vector3( -3, -3, -3 ), Vector3( 3, 3, 3 ), Vector3( 0, 0, 0 ), impactColor.r( ), impactColor.g( ), impactColor.b( ), impactColor.a( ), 0.8f );

			impacts.erase( impacts.begin( ) + i );
		}
	}
}