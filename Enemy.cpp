#include "Enemy.h"

Enemy::Enemy() {
	worldTransForm.Initialize();
	worldTransForm.translation_ = { 0,0,0 };
	isDead = false;
	YTmp = { 0,1,0 };
	//speed = 0.0004f;
}

Enemy::~Enemy() {}

void Enemy::CalcVec(Vector3 obj) 
{
	//正面仮ベクトル
	enemyTmp = obj - worldTransForm.translation_;
	enemyTmp.normalize();
	//右ベクトル
	enemyRight = YTmp.cross(enemyTmp);
	enemyRight.normalize();
	//正面ベクトル
	enemyFront = enemyRight.cross(YTmp);
	enemyFront.normalize();
}


void Enemy::Update(Vector3 obj) {
	//ベクトル計算
	CalcVec(obj);

	//行列計算
	worldTransForm.matWorld_ = Affin::matUnit();
	worldTransForm.matWorld_ = Affin::matWorld(
		worldTransForm.translation_,
		worldTransForm.rotation_,
		worldTransForm.scale_);

	/*if (isDead == false) {
		time++;
		if (time == 6) {
			speed += 0.0001f;
			time = 0;
		}
		worldTransForm.translation_ += enemyFront * speed;
	}
	else if (isDead == true) {
		speed = 0.0008f;
		time = 0;
	}*/

	//結果を反映
	worldTransForm.TransferMatrix();

	Hit();
}

//void Enemy::Pop() {
//	if (isDead == true) {
//		isDead = false;
//
//		//乱数生成装置
//		std::random_device seed_gen;
//		std::mt19937_64 engine(seed_gen());
//		std::uniform_real_distribution<float>dist(20.0f, 50.0f);
//		std::uniform_real_distribution<float>dist2(-1.0f, 1.0f);
//		
//		//乱数
//		float value = dist(engine) * dist2(engine);
//		float value2 = dist(engine) * dist2(engine);
//		//
//		worldTransForm.translation_ = { value,0,value2 };
//	}
//}

void Enemy::Hit() {
	//if (worldTransForm.translation_.x < 0.5 && worldTransForm.translation_.x > -0.5) {
	//	if (worldTransForm.translation_.z < 0.5 && worldTransForm.translation_.z > -0.5) {
	//		if (isDead == false) {
	//			isDead = true;
	//		}
	//	}
	//}
}

void Enemy::OnColision() {
	isDead = true;
}