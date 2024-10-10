#pragma once

#include <vector>
#include "Camera/Camera.h"
#include "Models/Cube.h"
#include "Skybox/Skybox.h"
#include "Player/Player.h"
#include "../CoreApp/EventManager.h"

class Game
{
public:
	Game(XMFLOAT2 Size,XMMATRIX* Projection);
	~Game();
	void Render();
	void Update(float dt);

	void onWindowSizeChangedEvent(int width, int height);
private:
	std::vector<std::unique_ptr<Object>> objects;
	Player* player = nullptr;
	std::unique_ptr<Skybox> skybox;
	EventManager* EventHandler;
};

