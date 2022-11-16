#pragma once

#include "WorldTransform.h"
#include "Bullet.h"
#include "EnemyBullet.h"

#include <random>

class Enemy {
	private:
	WorldTransform worldTransForm;
	int isDead;
	Vector3 enemyTmp;
	Vector3 YTmp;
	//右ベクトル
	Vector3 enemyRight;
	//左ベクトル
	Vector3 enemyLeft;
	//正面ベクトル
	Vector3 enemyFront;
	//加速
	float speed;
	//time
	int time;

	int r = 2;

	class Player;
	Player* player_ = nullptr;
	void SetPlayer(Player* player) { player_ = player; }

	//弾
	std::list<std::unique_ptr<EnemyBullet>>bullets_;


	Model* model_enemy = nullptr;
	//発射タイマー
	int32_t fileTimer = 50;

public:
	Enemy();
	~Enemy();
	void CalcVec(Vector3 view);
	void Initialize(Model* model);
	void Update(Vector3 obj);
	void Draw(ViewProjection view);
	Vector3 GetWorldPosition() { return Affin::GetWorldTrans(worldTransForm.matWorld_); };
	void OnColision();
	void Fire();
	
	int GetRadius() { return r; }
	bool IsDead() { return isDead; }
	void SetDeadFlag(bool flag) { isDead = flag; }
};