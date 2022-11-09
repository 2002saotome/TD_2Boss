#pragma once

#include "Audio.h"
#include "DirectXCommon.h"
#include "DebugText.h"
#include "Input.h"
#include "Model.h"
#include "SafeDelete.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "DebugCamera.h"

#include "Bullet.h"
#include "Enemy.h"

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene {

public: // メンバ関数
  /// <summary>
  /// コンストクラタ
  /// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	void Attack();

	float Angle(float angle);

	void Reticle3D();

	void HomeOnColision();

	int CheckAlive(Enemy enemys[]);

private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;
	DebugText* debugText_ = nullptr;

	/// <summary>
	/// ゲームシーン用
	/// </summary>
	/// 
	/// 
	uint32_t textureHandle_[10];

	Sprite* title = nullptr;
	Sprite* tutoliar = nullptr;
	Sprite* gameWin = nullptr;
	Sprite* gameOver = nullptr;
	Model* model_ = nullptr;

	WorldTransform objHome_;
	int objHomeR = 4;			// 中心ブロック半径
	int homeLife = 20;
	int isDamage = 0;
	int damTimer = 0;

	WorldTransform floor_;
	WorldTransform worldTransforms_[10];
	ViewProjection viewProjection_;

	DebugCamera* debugCamera_ = nullptr;

	//カメラ上方向の角度
	float viewAngle = 0.0f;

	//弾
	//Bullet* bullet_[50];
	//弾
	std::list<std::unique_ptr<Bullet>> bullets_;
	Vector3 pos;
	Vector3 moveBul;
	Vector3 velo;
	Vector3 ai;

	float KEyeSpeed = 0.1f;
	float addspeed = 0.0f;


	// 3Dレティクル用ワールドトランスフォーム
	WorldTransform worldTransform3DReticle_;
	float kDistancePlayerTo3DReticle = 15.0f;
	Vector3 ret3DPos;
	Vector3 myPos;
	Vector3 resultRet;
	// 敵
	Enemy enemys[50];
	int popTime;
	int coolTime;
	int killCounter;
	int popCount = 0;
	int wave = 0;
	int waitTimer = 250;

	// シーン
	int scene = 0;

	//yの仮ベクトル
	Vector3 yTmpVec;
	//正面仮ベクトル
	Vector3 frontTmp;
	//右ベクトル
	Vector3 rightVec;
	//左ベクトル
	Vector3 leftVec;
	//正面ベクトル
	Vector3 frontVec;
	//背面ベクトル
	Vector3 behindVec;

	int a;
	float kCharacterSpeed = 0.1f;
	Vector3 posA, posB;
	Vector3 move = { 0,0,0 };
};
