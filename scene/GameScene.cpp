#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>

#include "PrimitiveDrawer.h"

#include <random>
#include"Affin.h"
#define PI 3.14

float GameScene::Angle(float angle)
{
	return angle * PI / 180;
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
	popTime = 0;
	coolTime = 0;
	killCounter = 0;
}

GameScene::~GameScene() {
	delete title;
	delete tutoliar;
	delete gameWin;
	delete gameOver;
	delete model_;
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

	//スプライトの生成
	title = Sprite::Create(textureHandle_[7], { 0,0 });
	tutoliar = Sprite::Create(textureHandle_[8], { 0,0 });
	gameWin = Sprite::Create(textureHandle_[9], { 0,0 });
	gameOver = Sprite::Create(textureHandle_[0], { 0,0 });
	//3Dモデルの生成
	model_ = Model::Create();

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
	worldTransforms_[0].Initialize();
	worldTransforms_[0].scale_ = { 3,3,3 };
	worldTransforms_[0].translation_ = { 0,1,0 };

	worldTransforms_[1].Initialize();
	worldTransforms_[1].translation_ = { 0,15,15 };
	worldTransforms_[1].parent_ = &worldTransforms_[0];

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
}

void GameScene::Update() {

	switch (scene)
	{
#pragma region TITLE
	case 0:
		if (input_->TriggerKey(DIK_SPACE)) {
			scene = 4;
		}

		break;

#pragma endregion

#pragma region GAME SCENE1
	case 1:
		if (isDamage == true) {
			damTimer++;
			if (damTimer == 30) {
				isDamage = false;
				textureHandle_[2] = TextureManager::Load("png.png");
				damTimer = 0;
			}
		}

		//デスフラグの立った弾を削除
		bullets_.remove_if([](std::unique_ptr<Bullet>& bullet) { return bullet->IsDead(); });

		//敵ポップ
		//if (waitTimer == 0) {
		//	if (popCount > 0) {
		//		if (popTime == 0) {
		//			for (int i = 0; i < _countof(enemys); i++) {
		//				if (enemys[i].isDead == true) {
		//					enemys[i].Pop();
		//					break;
		//				}
		//			}
		//			popCount--;
		//			popTime = 150;
		//		}
		//		else {
		//			popTime--;
		//		}
		//	}
		//	else {
		//		if (wave == 0) {
		//			wave = 1;
		//		}
		//	}
		//}
		//else {
		//	waitTimer--;
		//}
		//ウェーブ&勝利判定
		/*if (wave >= 0 && popCount == 0) {
			if (CheckAlive(enemys) == true) {
				if (wave < 3) {
					wave++;
					if (wave == 3) {
						popCount = 30;
					}
					else if (wave == 2) {
						popCount = 20;
					}
					else if (wave == 1) {
						popCount = 10;
					}
					waitTimer = 250;
				}
				else if (wave == 3) {
					scene = 2;
				}
			}
		}*/


		ai = Affin::GetWorldTrans(worldTransforms_[1].matWorld_);
		viewProjection_.eye = { ai.x,ai.y,ai.z };
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
			addspeed = 0;
			// 回転処理
			if (input_->PushKey(DIK_RIGHT)) {

				if (KEyeSpeed > 0.0f) {
					KEyeSpeed *= -1;
				}
				else {
					addspeed -= 0.2;
				}
			}
			else if (input_->PushKey(DIK_LEFT)) {
				if (KEyeSpeed < 0.0f) {
					KEyeSpeed *= -1;
				}
				else {
					addspeed += 0.2;
				}
			}
			// 親オブジェクト
			worldTransforms_[0].rotation_.y += KEyeSpeed + addspeed;
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

		if (input_->PushKey(DIK_DOWN) && kDistancePlayerTo3DReticle < 25) {
			kDistancePlayerTo3DReticle += 0.1;
			/*if (-9 < kDistancePlayerTo3DReticle && kDistancePlayerTo3DReticle < 5) {
				kDistancePlayerTo3DReticle = 5;
			}*/
		}
		else if (input_->PushKey(DIK_UP)) {
			kDistancePlayerTo3DReticle -= 0.1;
			/*if (kDistancePlayerTo3DReticle < 5) {
				kDistancePlayerTo3DReticle = -10;
			}*/
		}


		/*else {
			kDistancePlayerTo3DReticle = 15;
		}*/
		/*DebugText::GetInstance()->SetPos(20, 200);
		DebugText::GetInstance()->Printf(
			"distance:(%f,", kDistancePlayerTo3DReticle);*/
		DebugText::GetInstance()->SetPos(30, 180);
		DebugText::GetInstance()->Printf(
			"Kill : %d", killCounter);
		DebugText::GetInstance()->SetPos(30, 60);
		DebugText::GetInstance()->Printf(
			"homeLife : %d", homeLife);
		DebugText::GetInstance()->SetPos(30, 40);
		DebugText::GetInstance()->Printf(
			"wave : %d", wave);

		DebugText::GetInstance()->SetPos(30, 200);
		DebugText::GetInstance()->Printf(
			"isDead : %d", enemys->isDead);

		Reticle3D();

		Attack();
		/*for (int i = 0; i < _countof(bullet_);) {
			if (bullet_[i])
			{
				bullet_[i]->Update(resultRet);
			}
		}*/
		for (std::unique_ptr<Bullet>& bullet : bullets_) {
			bullet->Update(resultRet);
		}

		//敵更新
		for (int i = 0; i < _countof(enemys); i++) {
			enemys[i].Update(objHome_.translation_);
		}

		/// <summary>
		/// 弾と敵の当たり判定
		/// </summary>
		for (std::unique_ptr<Bullet>& bullet : bullets_) {
			posA = bullet->GetWorldPosition();
			//敵更新
			for (int i = 0; i < _countof(enemys); i++) {
				posB = enemys[i].GetWorldPosition();

				float a = std::pow(posB.x - posA.x, 2.0f) + std::pow(posB.y - posA.y, 2.0f) +
					std::pow(posB.z - posA.z, 2.0f);
				float lenR = std::pow((enemys[i].r + bullet->r), 2.0);

				// 球と球の交差判定
				if (enemys[i].isDead == false) {
					if (a <= lenR) {
						// 自キャラの衝突時コールバックを呼び出す
						bullet->OnColision();
						// 敵弾の衝突時コールバックを呼び出す
						enemys[i].OnColision();
						killCounter++;
					}
				}
			}
			if (posA.y < -10) {
				bullet->OnColision();
			}
		}

		posA = Affin::GetWorldTrans(objHome_.matWorld_);
		//弾
		for (int i = 0; i < _countof(enemys); i++) {

			posB = enemys[i].GetWorldPosition();
			float a = std::pow(posB.x - posA.x, 2.0f) + std::pow(posB.y - posA.y, 2.0f) +
				std::pow(posB.z - posA.z, 2.0f);
			float lenR = std::pow((enemys[i].r + objHomeR), 2.0);

			// 球と球の交差判定
			if (a <= lenR) {

				if (enemys[i].isDead == false) {
					/*HomeOnColision();*/
				}
				// 敵弾の衝突時コールバックを呼び出す
				/*enemys[i].OnColision();*/
			}
		}

		if (homeLife == 0) {
			scene = 3;
		}
#pragma endregion

		break;
	case 2:// victory

		if (input_->TriggerKey(DIK_SPACE)) {
			scene = 0;
		}

		break;
	case 3:// game over

		if (input_->TriggerKey(DIK_SPACE)) {
			scene = 0;
			for (int i = 0; i < _countof(enemys); i++) {
				if (enemys[i].isDead == false) {
					enemys[i].isDead = true;
				}
			}
		}
		break;

	case 4://操作説明
		if (input_->TriggerKey(DIK_SPACE)) {
			homeLife = 15;
			popCount = 0;
			isDamage = false;
			damTimer = 0;
			killCounter = 0;
			scene = 1;
			wave = 0;
			waitTimer = 250;
			textureHandle_[2] = TextureManager::Load("png.png");
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
	if (scene == 1) {
		//model_->Draw(objHome_, viewProjection_, textureHandle_[2]);
		model_->Draw(worldTransforms_[1], viewProjection_, textureHandle_[5]);
		//model_->Draw(floor_, viewProjection_, textureHandle_[1]);

		model_->Draw(worldTransform3DReticle_, viewProjection_, textureHandle_[4]);
		for (int i = 0; i < _countof(enemys); i++) {

			if (enemys[i].isDead == false) {
				model_->Draw(enemys[i].worldTransForm, viewProjection_, textureHandle_[6]);
			}
		}

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
	switch (scene) {
	case 0:
		title->Draw();
		break;
	case 1:
		break;
	case 2:
		gameWin->Draw();
		break;
	case 3:
		gameOver->Draw();
		break;
	case 4:
		tutoliar->Draw();
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
	if (killCounter > 10) {
		if (input_->PushKey(DIK_SPACE))
		{
			if (coolTime == 0) {
				//弾を生成し、初期化
				std::unique_ptr<Bullet> newBullet = std::make_unique<Bullet>();

				//Bullet* newbullet = new Bullet();
				pos = Affin::GetWorldTrans(worldTransforms_[1].matWorld_);
				pos.y -= 5;
				ret3DPos = Affin::GetWorldTrans(worldTransform3DReticle_.matWorld_);
				velo = ret3DPos - pos;
				velo.normalize();
				resultRet = velo * newBullet->speed;
				newBullet->Initialize(model_, pos);

				//弾を登録
				bullets_.push_back(std::move(newBullet));

				//クールタイムをリセット
				coolTime = 12;
			}
			else {
				coolTime--;
			}
		}
	}
	else {
		if (input_->TriggerKey(DIK_SPACE))
		{

			//弾を生成し、初期化
			std::unique_ptr<Bullet> newBullet = std::make_unique<Bullet>();

			//Bullet* newbullet = new Bullet();
			pos = Affin::GetWorldTrans(worldTransforms_[1].matWorld_);
			pos.y -= 5;
			ret3DPos = Affin::GetWorldTrans(worldTransform3DReticle_.matWorld_);
			velo = ret3DPos - pos;
			velo.normalize();
			resultRet = velo * newBullet->speed;
			newBullet->Initialize(model_, pos);

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
	offset *= kDistancePlayerTo3DReticle;
	worldTransform3DReticle_.translation_ = offset;
	worldTransform3DReticle_.scale_ = Vector3(0.5f, 0.5f, 0.5f);
	worldTransform3DReticle_.matWorld_ = Affin::matScale(worldTransform3DReticle_.scale_);
	worldTransform3DReticle_.matWorld_ = Affin::matTrans(worldTransform3DReticle_.translation_);

	worldTransform3DReticle_.TransferMatrix();

	//DebugText::GetInstance()->SetPos(20, 260);
	//DebugText::GetInstance()->Printf(
	//	"ReticleObject:(%f,%f,%f)", worldTransform3DReticle_.translation_.x,
	//	worldTransform3DReticle_.translation_.y, worldTransform3DReticle_.translation_.z);

}

void GameScene::HomeOnColision() {
	textureHandle_[2] = TextureManager::Load("red.png");
	if (isDamage == false) {
		isDamage = true;
	}
	homeLife--;
}

//int GameScene::CheckAlive(Enemy enemys[]) {
//	int aliveNum = 0;
//
//	for (int i = 0; i < 50; i++) {
//		if (enemys[i].isDead == false) {
//			aliveNum++;
//		}
//	}
//
//	if (aliveNum == 0) {
//		return true;
//	}
//	else {
//		return false;
//	}
//}
