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
	INPUT_VAULT,
	INPUT_DODGE,
	INPUT_SPRINT,
	INPUT_CROUCH,
	INPUT_CLIMBING,
	INPUT_QSKILL,
	INPUT_END,
};

enum CLIENT_STATE { STATE_READY, STATE_TRANSFORM, STATE_INPUT, STATE_ADDITIONAL, STATE_END };
