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
	//�E�x�N�g��
	Vector3 enemyRight;
	//���x�N�g��
	Vector3 enemyLeft;
	//���ʃx�N�g��
	Vector3 enemyFront;
	//����
	float speed;
	//time
	int time;

	int r = 2;

	class Player;
	Player* player_ = nullptr;
	void SetPlayer(Player* player) { player_ = player; }

	//�e
	std::list<std::unique_ptr<EnemyBullet>>bullets_;


	Model* model_enemy = nullptr;
	//���˃^�C�}�[
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