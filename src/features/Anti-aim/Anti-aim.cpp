#include "Anti-aim.hpp"

#include "../../menu/menu.hpp"
#include "../../menu/menu.hpp"
bool Switch = false;

CAntiAim g_AntiAim;

bool IsStandingCheck(CUserCmd* pCmd)
{
	return ((Globals::localPlayer->velocity().Length2D() < 36) && (Globals::localPlayer->flags() & FL_ONGROUND) /*&& && !Globals::isSlowWalking*/);
}

bool IsMovingCheck(CUserCmd* pCmd)
{
	return ((Globals::localPlayer->velocity().Length2D() > 36) && (Globals::localPlayer->flags() & FL_ONGROUND) && !Globals::isSlowWalking);
}

bool IsInAirCheck(CUserCmd* pCmd)
{
	//return (((pCmd->buttons & IN_JUMP) && (!(Globals::localPlayer->flags() & FL_ONGROUND))) || (!(Globals::localPlayer->flags() & FL_ONGROUND) && !Globals::isSlowWalking));
	return (!(Globals::localPlayer->flags() & FL_ONGROUND) && !Globals::isSlowWalking);
}

bool IsSlowWalkingCheck(CUserCmd* pCmd)
{
	return ((Globals::isSlowWalking) && (Globals::localPlayer->flags() & FL_ONGROUND));
}

void CAntiAim::onMove(CUserCmd* userCmd)
{
	if (!g_Interfaces->gameEngine->inGame() && !g_Interfaces->gameEngine->connected()) {
		Globals::desyncEnabledSpecialChecks = false;
		return;
	}

	if (!config->get_bool("aaEnable")) {
		Globals::desyncEnabledSpecialChecks = false;
		return;
	}

	if (Globals::disableAA) {
		Globals::desyncEnabledSpecialChecks = false;
		return;
	}

	if (!Globals::localPlayer->alive()) {
		Globals::desyncEnabledSpecialChecks = false;
		return;
	}

	if (Globals::oCmd->buttons & IN_USE) {
		Globals::desyncEnabledSpecialChecks = false;
		return;
	}

	if (!Globals::localPlayer->activeWeapon() || Globals::localPlayer->isNade()) {
		Globals::desyncEnabledSpecialChecks = false;
		return;
	}

	if (Globals::localPlayer->moveType() == movetype_ladder /*|| Globals::localPlayer->moveType( ) == movetype_noclip  */) {
		Globals::desyncEnabledSpecialChecks = false;
		return;
	}

	if (Globals::localPlayer->activeWeapon()->itemDefenitionIndex() == weapon_revolver)
	{
		if (userCmd->buttons & IN_ATTACK2) {
			Globals::desyncEnabledSpecialChecks = false;
			return;
		}

		if (Globals::localPlayer->activeWeapon()->canFirePostpone() && (userCmd->buttons & IN_ATTACK)) {
			Globals::desyncEnabledSpecialChecks = false;
			return;
		}
	}

	float serverTime = Globals::localPlayer->tickBase() * g_Interfaces->globalVars->intervalPerTick;
	float canShoot = Globals::localPlayer->activeWeapon()->nextPrimaryAttack() <= serverTime;

	if (canShoot && (Globals::oCmd->buttons & IN_ATTACK)) {
		Globals::desyncEnabledSpecialChecks = false;
		return;
	}

	/* dont blame me, this shit is so we can feel the 180z experience(lol) even when freestanding :sunglasses: */

	if (Globals::bSendPacket)
	{
		Switch = !Switch;
	}

	pitch(userCmd);

	//yaw(userCmd);

	if (config->get_int("aaDesync"))
		desync_initialize(userCmd);
	else
		yaw(userCmd);
}

void CAntiAim::pitch(CUserCmd* pCmd)
{
	if (config->get_bool("miscAntiUT")) // anti-untrusted
	{
		if (IsStandingCheck(pCmd))
		{ // standing
			switch (config->get_int("aaStandingPitch"))
			{
			case 1: pitch_down(pCmd); break;
			case 2: pitch_up(pCmd); break;
			case 3: pitch_zero(pCmd); break;
			case 4: pitch_jitter(pCmd); break;
			}
		}
		else if (IsMovingCheck(pCmd))
		{ // moving
			switch (config->get_int("aaMovingPitch"))
			{
			case 1: pitch_down(pCmd); break;
			case 2: pitch_up(pCmd); break;
			case 3: pitch_zero(pCmd); break;
			case 4: pitch_jitter(pCmd); break;
			}
		}
		else if (IsInAirCheck(pCmd))
		{ // in-air
			switch (config->get_int("aaInAirPitch"))
			{
			case 1: pitch_down(pCmd); break;
			case 2: pitch_up(pCmd); break;
			case 3: pitch_zero(pCmd); break;
			case 4: pitch_jitter(pCmd); break;
			}
		}
		/*else
		{ // is slow walking
			switch (config->get_int("aaSlowWalkPitch"))
			{
			case 1: pitch_down(pCmd); break;
			case 2: pitch_up(pCmd); break;
			case 3: pitch_zero(pCmd); break;
			case 4: pitch_jitter(pCmd); break;
			}
		}*/
	}
	else
	{
		if (IsStandingCheck(pCmd))
		{ // standing
			switch (config->get_int("aaStandingPitch"))
			{
			case 1: pitch_fake_down(pCmd); break;
			case 2: pitch_fake_up(pCmd); break;
			case 3: pitch_fake_zero(pCmd); break;
			case 4: pitch_fake_jitter(pCmd); break;
			}
		}
		else if (IsMovingCheck(pCmd))
		{ // moving
			switch (config->get_int("aaMovingPitch"))
			{
			case 1: pitch_fake_down(pCmd); break;
			case 2: pitch_fake_up(pCmd); break;
			case 3: pitch_fake_zero(pCmd); break;
			case 4: pitch_fake_jitter(pCmd); break;
			}
		}
		else if (IsInAirCheck(pCmd))
		{ // in-air
			switch (config->get_int("aaInAirPitch"))
			{
			case 1: pitch_fake_down(pCmd); break;
			case 2: pitch_fake_up(pCmd); break;
			case 3: pitch_fake_zero(pCmd); break;
			case 4: pitch_fake_jitter(pCmd); break;
			}
		}
		/*else
		{ // is slow walking
			switch (config->get_int("aaSlowWalkPitch"))
			{
			case 1: pitch_fake_down(pCmd); break;
			case 2: pitch_fake_up(pCmd); break;
			case 3: pitch_fake_zero(pCmd); break;
			case 4: pitch_fake_jitter(pCmd); break;
			}
		}*/
	}
}

void CAntiAim::yaw(CUserCmd* pCmd)
{
	if (IsStandingCheck(pCmd))
	{ // standing
		if (config->get_bool("aaStandingAutoDir"))
			yaw_autoDirection(pCmd);
		else
		{
			switch (config->get_int("aaStandingYaw"))
			{
			case 0: { } break;
			case 1: { yaw_backwards(pCmd); } break;
			case 2: { yaw_sideways(pCmd); } break;
			}
		}

		if (config->get_int("aaStandingJitterType") == 1)
			pCmd->viewAngles.y += rand() % (int)config->get_int("aaStandingJitterRange"); // jitter
		else if (config->get_int("aaStandingJitterType") == 2)
			pCmd->viewAngles.y += rand() % 179 + 1; // jitter
	}
	if (IsMovingCheck(pCmd))
	{ // moving
		if (config->get_bool("aaMovingAutoDir"))
			yaw_autoDirection(pCmd);
		else
		{
			switch (config->get_int("aaMovingYaw"))
			{
			case 0: { } break;
			case 1: { yaw_backwards(pCmd); } break;
			case 2: { yaw_sideways(pCmd); } break;
			}
		}
		if (config->get_int("aaMovingJitterType") == 1)
			pCmd->viewAngles.y += rand() % (int)config->get_int("aaMovingJitterRange"); // jitter
		else if (config->get_int("aaMovingJitterType") == 2)
			pCmd->viewAngles.y += rand() % 179 + 1; // jitter
	}
	if (IsInAirCheck(pCmd))
	{ // in-air
		if (config->get_bool("aaInAirAutoDir"))
			yaw_autoDirection(pCmd);
		else
		{
			switch (config->get_int("aaInAirYaw"))
			{
			case 0: { } break;
			case 1: { yaw_backwards(pCmd); } break;
			case 2: { yaw_sideways(pCmd); } break;
			}
		}

		if (config->get_int("aaInAirJitterType") == 1)
			pCmd->viewAngles.y += rand() % (int)config->get_int("aaInAirJitterRange"); // jitter
		else if (config->get_int("aaInAirJitterType") == 2)
			pCmd->viewAngles.y += rand() % 179 + 1; // jitter
	}
	/*else
	{ // is slow walking
		if (config->get_bool("aaSlowWalkAutoDir"))
			1;
		else
		{
			switch (config->get_int("aaSlowWalkYaw"))
			{
			case 0: { } break;
			case 1: { yaw_backwards(pCmd); } break;
			case 2: { yaw_sideways(pCmd); } break;
			}

			if (config->get_int("aaSlowWalkJitterType") > 0 && config->get_int("aaSlowWalkJitterRange") > 0) // should jitter?
				pCmd->viewAngles += rand() % (int)config->get_int("aaSlowWalkJitterRange"); // jitter
		}
	}*/
}

float get_curtime(CUserCmd* ucmd) {
	auto local_player = Globals::localPlayer;

	if (!local_player)
		return 0;

	int g_tick = 0;
	CUserCmd* g_pLastCmd = nullptr;
	if (!g_pLastCmd || g_pLastCmd->hasbeenPredicted) {
		g_tick = (float)local_player->tickBase();
	}
	else {
		++g_tick;
	}
	g_pLastCmd = ucmd;
	float curtime = g_tick * g_Interfaces->globalVars->intervalPerTick;
	return curtime;
}
bool next_lby_update(const float yaw_to_break, CUserCmd* cmd)
{
	auto local_player = Globals::localPlayer;

	if (!local_player)
		return false;

	static float next_lby_update_time = 0;
	float curtime = get_curtime(cmd);

	auto animstate = local_player->animState();
	if (!animstate)
		return false;

	if (!(local_player->flags() & FL_ONGROUND))
		return false;

	if (animstate->speed_2d > 0.1)
		next_lby_update_time = curtime + 0.22f;

	if (next_lby_update_time < curtime)
	{
		next_lby_update_time = curtime + 1.1f;
		return true;
	}

	return false;
}

bool next_lby_update2(CUserCmd* cmd)
{
	auto local_player = Globals::localPlayer;

	if (!local_player)
		return false;

	static float next_lby_update_time = 0;
	float curtime = get_curtime(cmd);

	auto animstate = local_player->animState();
	if (!animstate)
		return false;

	if (!(local_player->flags() & FL_ONGROUND))
		return false;

	if (animstate->speed_2d > 0.1)
		next_lby_update_time = curtime + 0.22f;

	if (next_lby_update_time < curtime)
	{
		next_lby_update_time = curtime + 1.1f;
		return true;
	}

	return false;
}

//        _  _          _     
// _ __  (_)| |_   ___ | |__  
//| '_ \ | || __| / __|| '_ \ 
//| |_) || || |_ | (__ | | | |
//| .__/ |_| \__| \___||_| |_|
//|_|                         

void CAntiAim::pitch_down(CUserCmd* cmd)
{
	cmd->viewAngles.x = 89.f;
}

void CAntiAim::pitch_fake_down(CUserCmd* cmd)
{
	cmd->viewAngles.x = -991;
}

void CAntiAim::pitch_up(CUserCmd* cmd)
{
	cmd->viewAngles.x = -89.f;
}

void CAntiAim::pitch_fake_up(CUserCmd* cmd)
{
	cmd->viewAngles.x = 991;
}

void CAntiAim::pitch_zero(CUserCmd* cmd)
{
	cmd->viewAngles.x = 0.f;
}
void CAntiAim::pitch_fake_zero(CUserCmd* cmd)
{
	cmd->viewAngles.x = 1080.f;
}

void CAntiAim::pitch_jitter(CUserCmd* cmd)
{
	bool jitter = false;
	if (jitter)
		jitter != jitter;
	else
		jitter != jitter;

	cmd->viewAngles.x = jitter ? -89.0 : 89.0;
}

void CAntiAim::pitch_fake_jitter(CUserCmd* cmd)
{
	bool jitter = false;
	if (jitter)
		jitter != jitter;
	else
		jitter != jitter;

	cmd->viewAngles.x = jitter ? 991.0 : -991.0;
}

                        
// _   _   __ _ __      __
//| | | | / _` |\ \ /\ / /
//| |_| || (_| | \ V  V / 
// \__, | \__,_|  \_/\_/  
// |___/                  

void CAntiAim::yaw_backwards(CUserCmd* cmd)
{
	cmd->viewAngles.y += 180;
}

void CAntiAim::yaw_sideways(CUserCmd* cmd)
{
	if (menu->get_hotkey("aaFlipKey"))
		cmd->viewAngles.y += 90;
	else
		cmd->viewAngles.y -=  90;
}

void CAntiAim::yaw_autoDirection( CUserCmd* cmd )
{
	static float FinalAngle;
	bool bside1 = false;
	bool bside2 = false;
	bool autowalld = false;
	for (int i = 0; i <= g_Interfaces->globalVars->maxClients; ++i)
	{
		CBaseEntity* pPlayerEntity = g_Interfaces->clientEntity->GetClientEntity(i);

		if (!pPlayerEntity
			|| !pPlayerEntity->alive()
			|| pPlayerEntity->IsDormant()
			|| pPlayerEntity == Globals::localPlayer
			|| pPlayerEntity->team() == Globals::localPlayer->team())
			continue;

		float angToLocal = g_Math.calcAngle(Globals::localPlayer->GetOrigin(), pPlayerEntity->GetOrigin()).y;
		Vector3 ViewPoint = pPlayerEntity->GetOrigin() + Vector3(0, 0, 90);

		Vector2 Side1 = { (45 * sin(g_Math.GRD_TO_BOG(angToLocal))),(45 * cos(g_Math.GRD_TO_BOG(angToLocal))) };
		Vector2 Side2 = { (45 * sin(g_Math.GRD_TO_BOG(angToLocal + 180))) ,(45 * cos(g_Math.GRD_TO_BOG(angToLocal + 180))) };

		Vector2 Side3 = { (50 * sin(g_Math.GRD_TO_BOG(angToLocal))),(50 * cos(g_Math.GRD_TO_BOG(angToLocal))) };
		Vector2 Side4 = { (50 * sin(g_Math.GRD_TO_BOG(angToLocal + 180))) ,(50 * cos(g_Math.GRD_TO_BOG(angToLocal + 180))) };

		Vector3 Origin = Globals::localPlayer->GetOrigin();

		Vector2 OriginLeftRight[] = { Vector2(Side1.x, Side1.y), Vector2(Side2.x, Side2.y) };

		Vector2 OriginLeftRightLocal[] = { Vector2(Side3.x, Side3.y), Vector2(Side4.x, Side4.y) };

		for (int side = 0; side < 2; side++)
		{
			Vector3 OriginAutowall = { Origin.x + OriginLeftRight[side].x,  Origin.y - OriginLeftRight[side].y , Origin.z + 80 };
			Vector3 OriginAutowall2 = { ViewPoint.x + OriginLeftRightLocal[side].x,  ViewPoint.y - OriginLeftRightLocal[side].y , ViewPoint.z };

			if (g_Autowall.CanHitFloatingPoint(OriginAutowall, ViewPoint))
			{
				if (side == 0)
				{
					bside1 = true;
					FinalAngle = angToLocal + 90;
				}
				else if (side == 1)
				{
					bside2 = true;
					FinalAngle = angToLocal - 90;
				}
				autowalld = true;
			}
			else
			{
				for (int side222 = 0; side222 < 2; side222++)
				{
					Vector3 OriginAutowall222 = { Origin.x + OriginLeftRight[side222].x,  Origin.y - OriginLeftRight[side222].y , Origin.z + 80 };

					if (g_Autowall.CanHitFloatingPoint(OriginAutowall222, OriginAutowall2))
					{
						if (side222 == 0)
						{
							bside1 = true;
							FinalAngle = angToLocal + 90;
						}
						else if (side222 == 1)
						{
							bside2 = true;
							FinalAngle = angToLocal - 90;
						}
						autowalld = true;
					}
				}
			}
		}
	}
	if (!autowalld || (bside1 && bside2))
		cmd->viewAngles.y += 180;
	else
		cmd->viewAngles.y += FinalAngle;
}

void CAntiAim::breakLby( ) //polak
{
	// anti-paste
}

float dget_fixed_feet_yaw() {

	float current_feet_yaw = *(float*)(Globals::localPlayer->animState() + 0x80);

	if (current_feet_yaw >= -360)
		current_feet_yaw = min(current_feet_yaw, 360.0);

	return current_feet_yaw;
}
float dmax_desync_angle() {
	auto local_player = Globals::localPlayer;
	auto animstate = uintptr_t(local_player->animState());

	float duckammount = *(float*)(animstate + 0xA4);
	float speedfraction = max(0, min(*reinterpret_cast<float*>(animstate + 0xF8), 1));

	float speedfactor = max(0, min(1, *reinterpret_cast<float*> (animstate + 0xFC)));

	float unk1 = ((*reinterpret_cast<float*> (animstate + 0x11C) * -0.30000001) - 0.19999999) * speedfraction;
	float unk2 = unk1 + 1.f;
	float unk3;

	if (duckammount > 0)
		unk2 += ((duckammount * speedfactor) * (0.5f - unk2));

	unk3 = *(float*)(animstate + 0x334) * unk2;

	return unk3;
}
float dquick_normalize(float degree, const float min, const float max) {
	while (degree < min)
		degree += max - min;
	while (degree > max)
		degree -= max - min;

	return degree;
}
float BalanceDumpedFromPolak(CUserCmd* cmd, int type) {
	auto net_channel = *reinterpret_cast<NetChannel * *>(reinterpret_cast<std::uintptr_t>(g_Interfaces->clientState) + 0x9C);

	float desync = dmax_desync_angle();

	float balance = 1.0f;
	if (type == 2)
		balance = -1.0f;

	if (g_Interfaces->globalVars->curtime <= next_lby_update2(cmd)) {
		if (net_channel->m_nChokedPackets >= 2)
			return dquick_normalize(cmd->viewAngles.y, -180.f, 180.f);

		if (type == 1)
			return -100.0f;
		else
			return +(balance * 120.0f);
	}
	else if (type != 1) {
		return -((desync + 30.0f) * balance);
	}
}

int m_iJitter = 0;
void CAntiAim::desync_initialize(CUserCmd* cmd)
{
	// if ( Globals::localPlayer->animState( ) && Globals::bSendPacket && g_Interfaces->clientState->ChokedCommands )
	if (config->get_bool("aaDesync")) {
		// removed cause anti-paste(y'all niggas are gonna go to commits anyways) - nertigel
	}
}