#pragma once
#include "Model.h"
#include "Vector3.h"
#include "WorldTransform.h"
#include"Input.h"
#include"DebugText.h"
#include "ViewProjection.h"
#include <cassert>

#include "Affin.h"
class EnemyBullet
{
public:
    ///<summary>
   ///初期化
   /// </summary>
   /// <param name="model">モデル</param>
    void Initialize(Model* model, const Vector3& position, const Vector3& velocity);

    ///<summary>
    ///更新
    /// </summary>
    void Update();

    ///<summary>
    ///描画
    /// </summary>
    void Draw(const ViewProjection& viewProjection);

    bool IsDead()const { return isDead_; }
private:
    //ワールド変換データ
    WorldTransform worldTransform_;
    //モデル
    Model* model_ = nullptr;
    //テクスチャハンドル
    uint32_t textureHandle_ = 0u;
    //速度
    Vector3 velocity_;
    //寿命
    static const int32_t kLineTime = 60 * 5;
    //デスタイマー
    int32_t deathTimer_ = kLineTime;
    //デスフラグ
    bool isDead_ = false;
};

