#pragma once

#include "Entity.h"
#include <SDL3/SDL.h>
#include "Animation.h"

struct SDL_Texture;

class Item : public Entity
{
public:

	Item();
	virtual ~Item();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	bool Destroy();

	void OnCollision(PhysBody* physA, PhysBody* physB) override;
public:

	bool isPicked = false;
	Vector2D startPosition;

	bool isCoin = false;
	bool isStar = false;

	int id = -1;
private:

	SDL_Texture* texture;
	const char* texturePath;
	int texW, texH;

	//L08 TODO 4: Add a physics to an item
	PhysBody* pbody;

	AnimationSet anims;
	int texH_offset = 0; // Para compensar la altura en el renderizado.
};