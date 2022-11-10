#include "Enemy.h"
#include "Affin.h"
#include <cassert>

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

void Enemy::Initialize(Model* model)
{
	assert(model);
	model_ = model;
}


void Enemy::Update(Vector3 obj) {

	//デスフラグの立った
	bullets_.remove_if([](std::unique_ptr<EnemyBullet>& bullet)
		{
			return bullet->IsDead();
		});

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

	fileTimer--;
	//指定時間に達した
	if (fileTimer <= 0)
	{
		//弾の発射
		Fire();
		//発射タイマーを初期化
		fileTimer = 10;
	}

	//弾更新
	for (std::unique_ptr<EnemyBullet>& bullet : bullets_)
	{
		bullet->Update();
	}

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

void Enemy::Draw(ViewProjection view, int texHandle)
{
	for (std::unique_ptr<EnemyBullet>& bullet : bullets_)
	{
		bullet->Draw(view);
	}

	if (isDead == false)
	{
		model_->Draw(worldTransForm, view, texHandle);
	}
}


void Enemy::Hit() {
	//if (worldTransForm.translation_.x < 0.5 && worldTransForm.translation_.x > -0.5) {
	//	if (worldTransForm.translation_.z < 0.5 && worldTransForm.translation_.z > -0.5) {
	//		if (isDead == false) {
	//			isDead = true;
	//		}
	//	}
	//}
}

void Enemy::Fire()
{
	//敵の座標コピー
	Vector3 position = worldTransForm.translation_;
	//弾の速度
	const float kBulletSpeed_z = 0.1f;
	const float kBulletSpeed_y = -0.1f;
	Vector3 velocity(0, kBulletSpeed_y, kBulletSpeed_z);
	//速度ベクトルを自機の向きに合わせて回転させる
	velocity = Affin::VecMat3D(velocity, worldTransForm.matWorld_);
	//弾を生成し、初期化
	std::unique_ptr<EnemyBullet>newBullet = std::make_unique<EnemyBullet>();
	newBullet->Initialize(model_, position, velocity);

	//弾を登録する
	bullets_.push_back(std::move(newBullet));
}

void Enemy::OnColision() {
	isDead = true;
}