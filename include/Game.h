#pragma once

#define GAME_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>


#include "GameLevel.h"
#include "PowerUp.h"

enum GameState {
	GAME_ACTIVE,
	GAME_MENU,
	GAME_WIN
};

// Represents the four possible (collision) directions
enum Direction {
	UP,
	RIGHT,
	DOWN,
	LEFT
};

// Defines a Collision typedef that represents collision data
typedef std::tuple<bool, Direction, glm::vec2> Collision; 
// <collision?, what direction?, difference vector center - closest point>


const glm::vec2 PLAYER_SIZE(100.0f, 20.0f);
const float PLAYER_VELOCITY(500.0f);

class Game {


public:
	GameState State;
	bool Keys[1024];
	unsigned int Width, Height;
	std::vector<GameLevel> Levels;
	std::vector<PowerUp> PowerUps;
	unsigned int Level;

	Game(unsigned int width, unsigned int height);
	~Game();

	void Init();

	void ProcessInput(float dt, GLFWwindow* window);
	void Update(float dt);
	void Render();
	void DoCollisions();

	void ResetLevel();
	void ResetPlayer();

	void SpawnPowerUps(GameObject& block);
	void UpdatePowerUps(float dt);

};