#pragma once
#include "model.h"
#include "WorldTransform.h"
#include "input.h"
#include "DebugText.h"
#include "ViewProjection.h"
#include <cassert>

#include "Affin.h"

class Bullet
{
public:

	void Initialize(Model* model, const Vector3& position);

	void Update(Vector3 trans);

	void Draw(const ViewProjection& viewProjection, uint32_t textureHandle);

	void OnColision();

	Vector3 GetWorldPosition() { return Affin::GetWorldTrans(worldTransform_.matWorld_); };

	bool IsDead() const { return isDead; }

public:
	const float speed = 100;
	const int r = 2;
private:
	WorldTransform worldTransform_;
	Model* model_ = nullptr;


	bool isDead = false;


};

