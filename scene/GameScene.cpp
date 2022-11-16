#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>

#include "PrimitiveDrawer.h"

#include <random>
#include"Affin.h"
#define PI 3.14

float GameScene::Angle(float angle)
{
	return angle * (float)PI / 180.0f;
}


float Clamp(float min, float max, float num) {
	if (min > num) {
		return min;
	}
	else if (max < num) {
		return max;
	}
	return num;
}

GameScene::GameScene() {
	popTime_ = 0;
	coolTime_ = 0;
	killCounter_ = 0;
}

GameScene::~GameScene() {
	delete title_;
	delete tutoliar_;
	delete gameWin_;
	delete gameOver_;
	delete model_;

	delete model_enemy;
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	debugText_ = DebugText::GetInstance();

	//ファイル名を指定してテクスチャを入れ込む
	textureHandle_[0] = TextureManager::Load("gameover.png");
	textureHandle_[1] = TextureManager::Load("floor.png");
	textureHandle_[2] = TextureManager::Load("png.png");
	textureHandle_[3] = TextureManager::Load("inu.png");
	textureHandle_[4] = TextureManager::Load("ret.png");
	textureHandle_[5] = TextureManager::Load("Bullet.png");
	textureHandle_[6] = TextureManager::Load("Enemy.png");
	textureHandle_[7] = TextureManager::Load("Title.png");
	textureHandle_[8] = TextureManager::Load("manual.png");
	textureHandle_[9] = TextureManager::Load("end.png");
	textureHandle_[10] = TextureManager::Load("Player.png");  //追加

	//スプライトの生成
	title_ = Sprite::Create(textureHandle_[7], { 0,0 });
	tutoliar_ = Sprite::Create(textureHandle_[8], { 0,0 });
	gameWin_ = Sprite::Create(textureHandle_[9], { 0,0 });
	gameOver_ = Sprite::Create(textureHandle_[0], { 0,0 });
	Player = Sprite::Create(textureHandle_[10], { 0,0 });
	//3Dモデルの生成
	model_ = Model::Create();
	model_enemy = Model::Create();
	// 変数初期化

	//ワールドトランスフォームの初期化
	// 中心OBJ
	objHome_.Initialize();
	objHome_.scale_ = { 3,3,3 };
	objHome_.translation_ = { 0,1,0 };
	// 床OBJ
	floor_.Initialize();
	floor_.translation_ = { 0,-1,0 };
	floor_.scale_ = { 150,0.1f,150 };

	//ワールドトランスフォームの初期化(カメラ)
	//カーソル
	worldTransforms_[0].Initialize();
	worldTransforms_[0].scale_ = { 3,3,3 };
	worldTransforms_[0].translation_ = { 0,15,15 };

	//カメラ側
	worldTransforms_[1].Initialize();
	worldTransforms_[1].translation_ = { 0,10,10 };
	worldTransforms_[1].parent_ = &worldTransforms_[0];

	model_enemy = Model::CreateFromOBJ("test");

	for (int i = 2; i < 10; i++) {
		worldTransforms_[i].Initialize();
	}

	worldTransform3DReticle_.Initialize();


	//ビュープロジェクションの初期化
	viewProjection_.Initialize();
	viewProjection_.target = Affin::GetWorldTrans(worldTransforms_[0].matWorld_);
	viewProjection_.eye = Affin::GetWorldTrans(worldTransforms_[1].matWorld_);
	viewProjection_.UpdateMatrix();

	//敵キャラに自キャラのアドレスを渡す
	enemy_.Initialize(model_enemy);

	scene_ = 0;
}

void GameScene::Update() {

	switch (scene_)
	{
	case 0:
#pragma region TITLE
		if (input_->TriggerKey(DIK_SPACE)) {
			scene_ = 4;
		}

		break;

#pragma endregion

#pragma region GAME SCENE1
	case 1:

#pragma region 

		if (isDamage_ == true) {
			damTimer_++;
			if (damTimer_ == 30) {
				isDamage_ = false;
				damTimer_ = 0;
			}
		}

		//デスフラグの立った弾を削除
		bullets_.remove_if([](std::unique_ptr<Bullet>& bullet) { return bullet->IsDead(); });

		ai_ = Affin::GetWorldTrans(worldTransforms_[1].matWorld_);
		viewProjection_.eye = { ai_.x,ai_.y,ai_.z };
		viewProjection_.UpdateMatrix();

		//yの仮ベクトル
		yTmpVec = { 0, 1, 0 };
		yTmpVec.normalize();
		//正面仮ベクトル
		frontTmp = viewProjection_.target - viewProjection_.eye;
		frontTmp.normalize();
		//右ベクトル
		rightVec = yTmpVec.cross(frontTmp);
		rightVec.normalize();
		//左ベクトル
		leftVec = frontTmp.cross(yTmpVec);
		leftVec.normalize();
		//正面ベクトル
		frontVec = rightVec.cross(yTmpVec);
		frontVec.normalize();
		//背面ベクトル
		behindVec = frontVec * -1;

		//視点の移動速さ

		kCharacterSpeed = 0.1f;

		{	// 中心オブジェクト
			objHome_.matWorld_ = Affin::matUnit();
			objHome_.matWorld_ = Affin::matWorld(objHome_.translation_, objHome_.rotation_, objHome_.scale_);
			objHome_.TransferMatrix();
		}

		{
			addspeed_ = 0;
			// 回転処理
			if (input_->PushKey(DIK_RIGHT)) {

				if (KEyeSpeed_ > 0.0f) {
					KEyeSpeed_ *= -1;
				}
				else {
					addspeed_ -= 0.2f;
				}
			}
			else if (input_->PushKey(DIK_LEFT)) {
				if (KEyeSpeed_ < 0.0f) {
					KEyeSpeed_ *= -1;
				}
				else {
					addspeed_ += 0.2f;
				}
			}
			// 親オブジェクト
			worldTransforms_[0].rotation_.y += KEyeSpeed_ + addspeed_;
		}


		for (int i = 0; i < _countof(worldTransforms_); i++) {

			worldTransforms_[i].matWorld_ = Affin::matUnit();
			worldTransforms_[i].matWorld_ = Affin::matWorld(
				worldTransforms_[i].translation_,
				worldTransforms_[i].rotation_,
				worldTransforms_[i].scale_);

			if (worldTransforms_[i].parent_ != nullptr) {
				worldTransforms_[i].matWorld_ *= worldTransforms_[i].parent_->matWorld_;
			}

			worldTransforms_[i].TransferMatrix();

		}

		{	// 床
			floor_.matWorld_ = Affin::matUnit();
			floor_.matWorld_ = Affin::matWorld(floor_.translation_, floor_.rotation_, floor_.scale_);
			floor_.TransferMatrix();
		}

		//自機のワールド座標から3Dレティクルのワールド座標を計算
		//自機から3Dレティクルへの距離	

		if (input_->PushKey(DIK_DOWN) && kDistancePlayerTo3DReticle_ < 25) {
			kDistancePlayerTo3DReticle_ += 0.1f;
			/*if (-9 < kDistancePlayerTo3DReticle && kDistancePlayerTo3DReticle < 5) {
				kDistancePlayerTo3DReticle = 5;
			}*/
		}
		else if (input_->PushKey(DIK_UP) && kDistancePlayerTo3DReticle_ > -1) {
			kDistancePlayerTo3DReticle_ -= 0.1f;
			/*if (kDistancePlayerTo3DReticle < 5) {
				kDistancePlayerTo3DReticle = -10;
			}*/
		}

		//カメラアップ
		if (input_->TriggerKey(DIK_U))
		{
			CameraUpFlag = 1;
			CameraBackFlag = 0;
		}
		if (CameraUpFlag == 1)
		{
			worldTransforms_[1].translation_.y -= 0.5;
			worldTransforms_[1].translation_.z -= 0.5;
		}

		if (worldTransforms_[1].translation_.y < -5 && worldTransforms_[1].translation_.z < -5)
		{
			worldTransforms_[1].translation_.y = -5;
			worldTransforms_[1].translation_.z = -5;
		}

		
		//カメラバック
		if (input_->TriggerKey(DIK_B))
		{
			CameraBackFlag = 1;
			CameraUpFlag = 0;
		}

		if (CameraBackFlag == 1)
		{
			worldTransforms_[1].translation_.y += 0.5;
			worldTransforms_[1].translation_.z += 0.5;
		}

		if (worldTransforms_[1].translation_.y > 15 && worldTransforms_[1].translation_.z > 15)
		{
			worldTransforms_[1].translation_.y = 15;
			worldTransforms_[1].translation_.z = 15;
		}


		/*else {
			kDistancePlayerTo3DReticle = 15;
		}*/
		/*DebugText::GetInstance()->SetPos(20, 200);
		DebugText::GetInstance()->Printf(
			"distance:(%f,", kDistancePlayerTo3DReticle);*/
		DebugText::GetInstance()->SetPos(30, 180);
		DebugText::GetInstance()->Printf(
			"Kill : %d", killCounter_);
		DebugText::GetInstance()->SetPos(30, 60);
		DebugText::GetInstance()->Printf(
			"homeLife : %d", homeLife_);
		DebugText::GetInstance()->SetPos(30, 40);
		DebugText::GetInstance()->Printf(
			"wave : %d", wave_);

		DebugText::GetInstance()->SetPos(30, 240);
			DebugText::GetInstance()->Printf(
				" enemyHP : %d", enemy_.GetHp());

			DebugText::GetInstance()->SetPos(30, 200);
			DebugText::GetInstance()->Printf("worldTransforms_[1].translation_.y: %f", worldTransforms_[1].translation_.y);

			DebugText::GetInstance()->SetPos(30, 220);
			DebugText::GetInstance()->Printf("worldTransforms_[1].translation_.z: %f", worldTransforms_[1].translation_.z);

		Reticle3D();

		Attack();
		/*	for (int i = 0; i < _countof(bullet_);) {
				if (bullet_[i])
				{
					bullet_[i]->Update(resultRet);
				}
			}*/
		for (std::unique_ptr<Bullet>& bullet : bullets_) {
			bullet->Update(resultRet_);
		}

		//敵更新
		//for (int i = 0; i < _countof(enemys); i++) {

		//}

		enemy_.Update(objHome_.translation_);
		/// <summary>
		/// 弾と敵の当たり判定
		/// </summary>
		for (std::unique_ptr<Bullet>& bullet : bullets_) {
			posA = bullet->GetWorldPosition();
			//敵更新
			/*for (int i = 0; i < _countof(enemys); i++) {

			}*/
			posB = enemy_.GetWorldPosition();

			dist_ = std::pow(posB.x - posA.x, 2.0f) + std::pow(posB.y - posA.y, 2.0f) +
				std::pow(posB.z - posA.z, 2.0f);
			lenR_ = std::powf((float)(enemy_.GetRadius() + bullet->r), 2.0f);

			// 球と球の交差判定
			if (enemy_.IsDead() == false) {
				if (dist_ <= lenR_) {
					// 自キャラの衝突時コールバックを呼び出す
					bullet->OnColision();
					// 敵弾の衝突時コールバックを呼び出す
					enemy_.OnColision();
					killCounter_++;
				}
			}
			if (posA.y < -10) {
				bullet->OnColision();
			}
		}

		posA = Affin::GetWorldTrans(objHome_.matWorld_);

		posB = enemy_.GetWorldPosition();
		dist_ = std::pow(posB.x - posA.x, 2.0f) + std::pow(posB.y - posA.y, 2.0f) +
			std::pow(posB.z - posA.z, 2.0f);
		lenR_ = std::powf((float)(enemy_.GetRadius() + objHomeR_), 2.0f);

		// 球と球の交差判定
		if (dist_ <= lenR_) {

			if (enemy_.IsDead() == false) {
				/*HomeOnColision();*/
			}
			// 敵弾の衝突時コールバックを呼び出す
			/*enemy_.OnColision();*/
		}
		if (homeLife_ == 0) {
			scene_ = 3;
		}

		break;

#pragma endregion

#pragma endregion

	case 2:// victory

		if (input_->TriggerKey(DIK_SPACE)) {
			scene_ = 0;
		}

		break;
	case 3:// game over

		if (input_->TriggerKey(DIK_SPACE)) {
			scene_ = 0;
			/*for (int i = 0; i < _countof(enemys); i++) {

			}*/
			if (enemy_.IsDead() == false) {
				enemy_.SetDeadFlag(true);
			}
		}
		break;

	case 4://操作説明
		if (input_->TriggerKey(DIK_SPACE)) {
			homeLife_ = 15;
			popCount_ = 0;
			isDamage_ = false;
			damTimer_ = 0;
			killCounter_ = 0;
			scene_ = 1;
			wave_ = 0;
			waitTimer_ = 250;
			//textureHandle_[2] = TextureManager::Load("png.png");
		}
		break;
	}

}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる

	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	if (scene_ == 1) {
		//model_->Draw(objHome_, viewProjection_, textureHandle_[2]);
		model_->Draw(worldTransforms_[1], viewProjection_, textureHandle_[5]);
		model_->Draw(floor_, viewProjection_, textureHandle_[1]);

		model_->Draw(worldTransform3DReticle_, viewProjection_, textureHandle_[4]);
		//for (int i = 0; i < _countof(enemys); i++) {


		//}

		enemy_.Draw(viewProjection_);

		//弾描画
		for (std::unique_ptr<Bullet>& bullet : bullets_) {
			bullet->Draw(viewProjection_, textureHandle_[5]);
		}
	}
	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>
	switch (scene_) {
	case 0:
		title_->Draw();
		break;
	case 1:
		Player->Draw();
		break;
	case 2:
		gameWin_->Draw();
		break;
	case 3:
		gameOver_->Draw();
		break;
	case 4:
		tutoliar_->Draw();
		break;
	}

	// デバッグテキストの描画
	debugText_->DrawAll(commandList);
	//
	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

void GameScene::Attack()
{
	if (killCounter_ > 10) {
		if (input_->PushKey(DIK_SPACE))
		{
			if (coolTime_ == 0) {
				//弾を生成し、初期化
				std::unique_ptr<Bullet> newBullet = std::make_unique<Bullet>();

				//Bullet* newbullet = new Bullet();
				pos_ = Affin::GetWorldTrans(worldTransforms_[1].matWorld_);
				pos_.y -= 5;
				ret3DPos_ = Affin::GetWorldTrans(worldTransform3DReticle_.matWorld_);
				velo_ = ret3DPos_ - pos_;
				velo_.normalize();
				resultRet_ = velo_ * newBullet->speed;
				newBullet->Initialize(model_, pos_);

				//弾を登録
				bullets_.push_back(std::move(newBullet));

				//クールタイムをリセット
				coolTime_ = 12;
			}
			else {
				coolTime_--;
			}
		}
	}
	else {
		if (input_->TriggerKey(DIK_SPACE))
		{

			//弾を生成し、初期化
			std::unique_ptr<Bullet> newBullet = std::make_unique<Bullet>();

			//Bullet* newbullet = new Bullet();
			pos_ = Affin::GetWorldTrans(worldTransforms_[1].matWorld_);
			pos_.y -= 5;
			ret3DPos_ = Affin::GetWorldTrans(worldTransform3DReticle_.matWorld_);
			velo_ = ret3DPos_ - pos_;
			velo_.normalize();
			resultRet_ = velo_ * newBullet->speed;
			newBullet->Initialize(model_, pos_);

			//弾を登録
			bullets_.push_back(std::move(newBullet));
		}
	}
}

void GameScene::Reticle3D() {
	//自機から3Dレティクルへのオフセット(Z+向き)
	Vector3 offset = { 0.0f, 0, 0.3f };
	//自機のワールド行列の回転を反映
	offset = Affin::VecMat(offset, worldTransforms_[1].matWorld_);
	//ベクトルの長さを整える
	//offset.normalize();
	float len = sqrt(offset.x * offset.x + offset.y * offset.y + offset.z * offset.z);
	if (len != 0) {
		offset /= len;
	}
	offset *= kDistancePlayerTo3DReticle_;
	worldTransform3DReticle_.translation_ = offset;
	worldTransform3DReticle_.scale_ = Vector3(0.5f, 0.5f, 0.5f);
	worldTransform3DReticle_.matWorld_ = Affin::matScale(worldTransform3DReticle_.scale_);
	worldTransform3DReticle_.matWorld_ = Affin::matTrans(worldTransform3DReticle_.translation_);

	worldTransform3DReticle_.TransferMatrix();

	/*DebugText::GetInstance()->SetPos(20, 260);
	DebugText::GetInstance()->Printf(
		"ReticleObject:(%f,%f,%f)", worldTransform3DReticle_.translation_.x,
		worldTransform3DReticle_.translation_.y, worldTransform3DReticle_.translation_.z);*/

}

void GameScene::HomeOnColision() {
	textureHandle_[2] = TextureManager::Load("red.png");
	if (isDamage_ == false) {
		isDamage_ = true;
	}
	homeLife_--;
}
