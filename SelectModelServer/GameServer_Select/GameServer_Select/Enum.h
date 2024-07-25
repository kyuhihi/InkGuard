#pragma once

enum PACKET_TYPE {
	PACKET_TRANSFORM,
	PACKET_INPUT,
	PACKET_NONE,
};

enum GAME_PLAY {
	GAME_RED_TEAM,
	GAME_END,
	GAME_BLUE_TEAM
};

enum PLAYER_INPUT {
	INPUT_F,
	INPUT_ATTACK,
	INPUT_DODGE,
	INPUT_SPRINT,
	INPUT_CROUCH,
	INPUT_QSKILL,
	INPUT_ESKILL,
	INPUT_END,
};

enum INTERACTION_TYPE {
	INTERACTION_DORERAE_A,	
	INTERACTION_DORERAE_B,
	INTERACTION_DORERAE_C,
	INTERACTION_VAULT,
	INTERACTION_CLIMB,
	INTERACTION_DAEPO_RED,
	INTERACTION_DAEPO_BLUE,
	INTERACTION_BALISTA,
	INTERACTION_END,
};

enum CLIENT_STATE { STATE_READY, STATE_TRANSFORM, STATE_INPUT, STATE_ADDITIONAL, STATE_END };
