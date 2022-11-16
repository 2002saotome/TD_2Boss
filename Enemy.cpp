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
	//���ʉ��x�N�g��
	enemyTmp = obj - worldTransForm.translation_;
	enemyTmp.normalize();
	//�E�x�N�g��
	enemyRight = YTmp.cross(enemyTmp);
	enemyRight.normalize();
	//���ʃx�N�g��
	enemyFront = enemyRight.cross(YTmp);
	enemyFront.normalize();
}

void Enemy::Initialize(Model* model)
{
	assert(model);
	model_enemy = model;

	worldTransForm.scale_ = { 10.0f,10.0f,10.0f};
	//�X�P�[�����O�s��
	worldTransForm.matWorld_ = Affin::matScale(worldTransForm.scale_);
	//�s��̓]��
	worldTransForm.TransferMatrix();
}


void Enemy::Update(Vector3 obj) {

	//�f�X�t���O�̗�����
	bullets_.remove_if([](std::unique_ptr<EnemyBullet>& bullet)
		{
			return bullet->IsDead();
		});

	//�x�N�g���v�Z
	CalcVec(obj);

	//�s��v�Z
	worldTransForm.matWorld_ = Affin::matUnit();
	worldTransForm.matWorld_ = Affin::matWorld(
		worldTransForm.translation_,
		worldTransForm.rotation_,
		worldTransForm.scale_);

	//���ʂ𔽉f
	worldTransForm.TransferMatrix();

	fileTimer--;
	//�w�莞�ԂɒB����
	if (fileTimer <= 0)
	{
		//�e�̔���
		Fire();
		//���˃^�C�}�[��������
		fileTimer = 50;
	}

	//�e�X�V
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
	//�G�̍��W�R�s�[
	Vector3 position = worldTransForm.translation_;
	//�e�̑��x
	const float kBulletSpeed_z = 0.01f;
	const float kBulletSpeed_y = -0.01f;
	Vector3 velocity(0, kBulletSpeed_y, kBulletSpeed_z);
	//���x�x�N�g�������@�̌����ɍ��킹�ĉ�]������
	velocity = Affin::VecMat3D(velocity, worldTransForm.matWorld_);
	//�e�𐶐����A������
	std::unique_ptr<EnemyBullet>newBullet = std::make_unique<EnemyBullet>();
	newBullet->Initialize(model_enemy, position, velocity);

	//�e��o�^����
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