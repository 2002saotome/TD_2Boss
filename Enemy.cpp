#include "Enemy.h"
#include "Affin.h"
#include <cassert>

Enemy::Enemy() {
	worldTransForm.Initialize();
	worldTransForm.translation_ = { 0,0,0 };
	isDead = false;
	YTmp = { 0,1,0 };

}

Enemy::~Enemy() {
}

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
	model_enemy = model;

	worldTransForm.scale_ = { 10.0f,10.0f,10.0f};
	//スケーリング行列
	worldTransForm.matWorld_ = Affin::matScale(worldTransForm.scale_);
	//行列の転送
	worldTransForm.TransferMatrix();
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

	//結果を反映
	worldTransForm.TransferMatrix();

	fileTimer--;
	//指定時間に達した
	if (fileTimer <= 0)
	{
		//弾の発射
		Fire();
		//発射タイマーを初期化
		fileTimer = 50;
	}

	//弾更新
	for (std::unique_ptr<EnemyBullet>& bullet : bullets_)
	{
		bullet->Update();
	}

}

void Enemy::Draw(ViewProjection view)
{
	for (std::unique_ptr<EnemyBullet>& bullet : bullets_)
	{
		bullet->Draw(view);
	}

	if (isDead == false)
	{
		model_enemy->Draw(worldTransForm, view);
	}
}


//void Enemy::Hit() {
//	if (worldTransForm.translation_.x < 0.5 && worldTransForm.translation_.x > -0.5) {
//		if (worldTransForm.translation_.z < 0.5 && worldTransForm.translation_.z > -0.5) {
//			if (isDead == false) {
//				isDead = true;
//			}
//		}
//	}
//}

void Enemy::Fire()
{
	//敵の座標コピー
	Vector3 position = worldTransForm.translation_;
	//弾の速度
	const float kBulletSpeed_z = 0.01f;
	const float kBulletSpeed_y = -0.01f;
	Vector3 velocity(0, kBulletSpeed_y, kBulletSpeed_z);
	//速度ベクトルを自機の向きに合わせて回転させる
	velocity = Affin::VecMat3D(velocity, worldTransForm.matWorld_);
	//弾を生成し、初期化
	std::unique_ptr<EnemyBullet>newBullet = std::make_unique<EnemyBullet>();
	newBullet->Initialize(model_enemy, position, velocity);

	//弾を登録する
	bullets_.push_back(std::move(newBullet));
}

int Enemy::GetHp(){return Hp_;}

int32_t Enemy::GetTimer() { return fileTimer; }

void Enemy::OnColision() {

	Hp_--;

	if (Hp_ <= 0)
	{
		isDead = true;
	}
}