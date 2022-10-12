#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>

#include "AxisIndicator.h"
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
GameScene::GameScene() {}

GameScene::~GameScene() {
	delete sprite_;
	delete model_;
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	debugText_ = DebugText::GetInstance();

	//ファイル名を指定してテクスチャを入れ込む
	textureHandle_[0] = TextureManager::Load("mario.jpg");
	textureHandle_[1] = TextureManager::Load("dog.png");
	textureHandle_[2] = TextureManager::Load("Ibox.png");

	//スプライトの生成
	sprite_ = Sprite::Create(textureHandle_[0], {100,50});
	//3Dモデルの生成
	model_ = Model::Create();
	//ワールドトランスフォームの初期化
	objHome_.Initialize();
	objHome_.scale_ = { 3,3,3 };
	objHome_.translation_ = { 0,1,0 };
	//ワールドトランスフォームの初期化
	worldTransforms_[0].Initialize();
	worldTransforms_[0].scale_ = { 3,3,3 };
	worldTransforms_[0].translation_ = { 0,1,0};

	worldTransforms_[1].Initialize();
	worldTransforms_[1].translation_ = { 0,15,15 };
	worldTransforms_[1].parent_ = &worldTransforms_[0];
	worldTransforms_[2].Initialize();
	worldTransforms_[2].translation_ = { 10,0,0 };	
	worldTransforms_[3].Initialize();
	worldTransforms_[3].translation_ = { -10,0,0 };

	worldTransforms_[4].Initialize();
	worldTransforms_[4].translation_ = { 0,10,10 };
	worldTransforms_[4].parent_ = &worldTransforms_[1];

	worldTransforms_[5].Initialize();
	worldTransforms_[5].translation_ = { 0,-1,0 };
	worldTransforms_[5].scale_ = { 150,0.1f,150  };

	//ビュープロジェクションの初期化
	viewProjection_.Initialize();
	//viewProjection_.target = { 0,0,0 };
	viewProjection_.target = Affin::GetWorldTrans(worldTransforms_[0].matWorld_);
	//viewProjection_.eye = { 0,0,-15 };
	viewProjection_.eye = Affin::GetWorldTrans(worldTransforms_[1].matWorld_);
	//viewProjection_.eye = worldTransforms_[1].translation_;

	//viewProjection_.
	viewProjection_.UpdateMatrix();



}

void GameScene::Update() {

	ai=Affin::GetWorldTrans(worldTransforms_[1].matWorld_);
	viewProjection_.eye = { ai.x,ai.y,ai.z};
	viewProjection_.UpdateMatrix();

	//yの仮ベクトル
	Vector3 yTmpVec(0, 1, 0);
	yTmpVec.normalize();
	//正面仮ベクトル
	Vector3 frontTmp = viewProjection_.target - viewProjection_.eye;
	frontTmp.normalize();
	//右ベクトル
	Vector3 rightVec = yTmpVec.cross(frontTmp);
	rightVec.normalize();
	//左ベクトル
	Vector3 leftVec = frontTmp.cross(yTmpVec);
	leftVec.normalize();
	//正面ベクトル
	Vector3 frontVec = rightVec.cross(yTmpVec);
	frontVec.normalize();
	//背面ベクトル
	Vector3 behindVec = frontVec * -1;

	//視点の移動速さ

	int a;
	float kCharacterSpeed = 0.1f;

	Vector3 move = { 0,0,0 };

	{
	
		objHome_.matWorld_ = Affin::matUnit();
		objHome_.matWorld_ = Affin::matWorld(objHome_.translation_, objHome_.rotation_, objHome_.scale_);

		objHome_.TransferMatrix();
	}
	{
		if (input_->PushKey(DIK_RIGHT)) {

			if (KEyeSpeed < 0.0f) {
				KEyeSpeed *= -1;
			}
		}
		else if (input_->PushKey(DIK_LEFT)) {
			if (KEyeSpeed > 0.0f) {
				KEyeSpeed *= -1;
			}
		}

		worldTransforms_[0].rotation_.y += KEyeSpeed;
		

		worldTransforms_[0].matWorld_ = Affin::matUnit();
		worldTransforms_[0].matWorld_ = Affin::matWorld(worldTransforms_[0].translation_, worldTransforms_[0].rotation_, worldTransforms_[0].scale_);

		worldTransforms_[0].TransferMatrix();
	}
	//子の更新(子は子で初期化)
	{

		worldTransforms_[1].matWorld_ = Affin::matUnit();
		worldTransforms_[1].matWorld_ = Affin::matTrans(worldTransforms_[1].translation_);

		//２つ目
		worldTransforms_[1].matWorld_ *= worldTransforms_[1].parent_->matWorld_;

		worldTransforms_[1].TransferMatrix();
	}

	{
		worldTransforms_[2].matWorld_ = Affin::matUnit();
		worldTransforms_[2].matWorld_ = Affin::matWorld(worldTransforms_[2].translation_, worldTransforms_[2].rotation_, worldTransforms_[2].scale_);

		//3つ目
		//worldTransforms_[2].matWorld_ *= worldTransforms_[2].parent_->matWorld_;

		worldTransforms_[2].TransferMatrix();
	}
	{
		worldTransforms_[3].matWorld_ = Affin::matUnit();
		worldTransforms_[3].matWorld_ = Affin::matWorld(worldTransforms_[3].translation_, worldTransforms_[3].rotation_, worldTransforms_[3].scale_);

		//4つ目
		//worldTransforms_[2].matWorld_ *= worldTransforms_[2].parent_->matWorld_;

		worldTransforms_[3].TransferMatrix();
	}
	{
		worldTransforms_[4].matWorld_ = Affin::matUnit();
		worldTransforms_[4].matWorld_ = Affin::matTrans(worldTransforms_[4].translation_);

		//5つ目
		worldTransforms_[4].matWorld_ *= worldTransforms_[4].parent_->matWorld_;

		worldTransforms_[4].TransferMatrix();
	}
	{
		worldTransforms_[5].matWorld_ = Affin::matUnit();
		worldTransforms_[5].matWorld_ = Affin::matWorld(worldTransforms_[5].translation_, worldTransforms_[5].rotation_, worldTransforms_[5].scale_);


		//6つ目
		//worldTransforms_[5].matWorld_ *= worldTransforms_[5].parent_->matWorld_;

		worldTransforms_[5].TransferMatrix();
	}

	if (input_->TriggerKey(DIK_SPACE))
	{
		//moveBul = { 0,frontVec.y,frontVec.z };
		moveBul = { frontVec.x ,-1,frontVec.z };
		//
		Bullet* newbullet = new Bullet();
		//pos = { viewProjection_.eye.x,viewProjection_.eye.y, viewProjection_.eye.z};
		pos = Affin::GetWorldTrans(worldTransforms_[1].matWorld_);
		//velo = Affin::VecMat(moveBul, worldTransforms_[1].matWorld_);
		newbullet->Initialize(model_, pos);

		bullet_ = newbullet;
	}
	
	if (bullet_)
	{
		
		bullet_->Update(moveBul);
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
	model_->Draw(objHome_, viewProjection_, textureHandle_[0]);
	//model_->Draw(worldTransforms_[0], viewProjection_, textureHandle_[0]);
	model_->Draw(worldTransforms_[1], viewProjection_, textureHandle_[0]);
	model_->Draw(worldTransforms_[2], viewProjection_, textureHandle_[0]);
	model_->Draw(worldTransforms_[3], viewProjection_, textureHandle_[0]);
	model_->Draw(worldTransforms_[4], viewProjection_, textureHandle_[0]);
	model_->Draw(worldTransforms_[5], viewProjection_, textureHandle_[1]);

	if (bullet_) {
		bullet_->Draw(viewProjection_);
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

	// デバッグテキストの描画
	debugText_->DrawAll(commandList);
	//
	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

void GameScene::Attack()
{
	if (input_->PushKey(DIK_SPACE))
	{
		//弾を生成し、初期化
		Bullet* newbullet = new Bullet();
		newbullet->Initialize(model_, viewProjection_.eye);

		//弾を登録
		bullet_ = newbullet;
	}
}
