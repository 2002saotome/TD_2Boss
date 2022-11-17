#include "EnemyBullet.h"

void EnemyBullet::Initialize(Model* model, const Vector3& position, const Vector3& velocity)
{
    //NULLポインタチェック
     assert(model);

    //引数で受け取った速度をメンバ変数に代入
    model_ = model;
    velocity_ = velocity;
    //テクスチャ読み込み
    textureHandle_ = TextureManager::Load("mario.png");
    //引数で受け取った初期座標をリセット
    worldTransform_.translation_ = position;

    worldTransform_.Initialize();
   
}

void EnemyBullet::Update()
{
    //座標を移動させる
    worldTransform_.translation_ -= velocity_;

    worldTransform_.matWorld_ *= Affin::matUnit();
    worldTransform_.matWorld_ *= Affin::matWorld(
        worldTransform_.translation_
        , worldTransform_.rotation_,
        worldTransform_.scale_);

    //行列の再計算
    worldTransform_.TransferMatrix();

    //時間経過でデス
    if (--deathTimer_<= 0)
    {
        isDead_= true;
    }
}

void EnemyBullet::Draw(const ViewProjection& viewProjection)
{
    //モデルの描画
    model_->Draw(worldTransform_, viewProjection, textureHandle_);
}

