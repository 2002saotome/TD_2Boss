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
	model_ = model;
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

	//���ʂ𔽉f
	worldTransForm.TransferMatrix();

	fileTimer--;
	//�w�莞�ԂɒB����
	if (fileTimer <= 0)
	{
		//�e�̔���
		Fire();
		//���˃^�C�}�[��������
		fileTimer = 10;
	}

	//�e�X�V
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
//		//�����������u
//		std::random_device seed_gen;
//		std::mt19937_64 engine(seed_gen());
//		std::uniform_real_distribution<float>dist(20.0f, 50.0f);
//		std::uniform_real_distribution<float>dist2(-1.0f, 1.0f);
//		
//		//����
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
	//�G�̍��W�R�s�[
	Vector3 position = worldTransForm.translation_;
	//�e�̑��x
	const float kBulletSpeed_z = 0.1f;
	const float kBulletSpeed_y = -0.1f;
	Vector3 velocity(0, kBulletSpeed_y, kBulletSpeed_z);
	//���x�x�N�g�������@�̌����ɍ��킹�ĉ�]������
	velocity = Affin::VecMat3D(velocity, worldTransForm.matWorld_);
	//�e�𐶐����A������
	std::unique_ptr<EnemyBullet>newBullet = std::make_unique<EnemyBullet>();
	newBullet->Initialize(model_, position, velocity);

	//�e��o�^����
	bullets_.push_back(std::move(newBullet));
}

void Enemy::OnColision() {
	isDead = true;
}