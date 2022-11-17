#include "EnemyBullet.h"

void EnemyBullet::Initialize(Model* model, const Vector3& position, const Vector3& velocity)
{
    //NULL�|�C���^�`�F�b�N
     assert(model);

    //�����Ŏ󂯎�������x�������o�ϐ��ɑ��
    model_ = model;
    velocity_ = velocity;
    //�e�N�X�`���ǂݍ���
    textureHandle_ = TextureManager::Load("mario.png");
    //�����Ŏ󂯎�����������W�����Z�b�g
    worldTransform_.translation_ = position;

    worldTransform_.Initialize();
   
}

void EnemyBullet::Update()
{
    //���W���ړ�������
    worldTransform_.translation_ -= velocity_;

    worldTransform_.matWorld_ *= Affin::matUnit();
    worldTransform_.matWorld_ *= Affin::matWorld(
        worldTransform_.translation_
        , worldTransform_.rotation_,
        worldTransform_.scale_);

    //�s��̍Čv�Z
    worldTransform_.TransferMatrix();

    //���Ԍo�߂Ńf�X
    if (--deathTimer_<= 0)
    {
        isDead_= true;
    }
}

void EnemyBullet::Draw(const ViewProjection& viewProjection)
{
    //���f���̕`��
    model_->Draw(worldTransform_, viewProjection, textureHandle_);
}

