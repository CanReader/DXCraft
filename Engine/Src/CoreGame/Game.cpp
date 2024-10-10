#include "Game.h"
#include <sstream>
#include "../CoreApp/Graphics.h"
#include "../CoreApp/Math/Vector.h"

Game::Game(XMFLOAT2 Size,XMMATRIX* Projection)
{
	player = new Player();
	player->GetCamera()->UpdateProj(90,Size.x,Size.y);

	ID3D11ShaderResourceView* texture;
	D3DX11CreateShaderResourceViewFromFileA(RendererHelper::GetDevice(), "mcgrass.png", NULL, NULL, &texture, NULL);

	for (int i = 0; i < 16; i++)
		for (int j = 0; j < 16; j++)
			for (int k = 0; k < 1; k++) {
				auto cube = std::make_unique<Cube>(2*j, k*2, i*2, texture);
				cube->Initialize();
				cube->PassMatrices(player->GetCamera()->GetView(), player->GetCamera()->GetProj());
				objects.push_back(std::move(cube));
	}

	skybox = std::make_unique<Skybox>(1000,1000, player->GetCamera()->GetView(), player->GetCamera()->GetProj());

	EventHandler = EventManager::getManager();

	add_event_2(onWindowSizeChanged, &Game::onWindowSizeChangedEvent,this);
}

Game::~Game()
{
	delete player;
	for (auto& obj : objects) {
		obj.release();
	}
}

void Game::Render()
{
	skybox->Render();

	for (auto& c : objects)
		c->Render();
}

void Game::Update(float DeltaTime)
{
	player->Update(DeltaTime);
	skybox->Update(DeltaTime);
}

void Game::onWindowSizeChangedEvent(int width, int height)
{
	std::stringstream infotext("The window size has been changed to: ");
	infotext << "[" << width << ', ' << height << "]";
	
	DX_INFO(infotext.str().c_str());
	player->GetCamera()->UpdateProj(90,width,height);
}
