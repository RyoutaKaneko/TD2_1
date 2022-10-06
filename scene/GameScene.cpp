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
	textureHandle_ = TextureManager::Load("mario.jpg");

	//スプライトの生成
	sprite_ = Sprite::Create(textureHandle_, { 100,50 });
	//3Dモデルの生成
	model_ = Model::Create();
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


	viewProjection_.eye = Affin::GetWorldTrans(worldTransforms_[1].matWorld_);
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
		if (input_->PushKey(DIK_R)) {

			if (KEyeSpeed < 0.0f) {
				KEyeSpeed *= -1;
			}
		}
		else if (input_->PushKey(DIK_T)) {
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
		if (input_->PushKey(DIK_RIGHT)) {
			viewProjection_.target.x += kCharacterSpeed;
		}
		else if (input_->PushKey(DIK_LEFT)) {
			viewProjection_.target.x -= kCharacterSpeed;
		}
		viewProjection_.UpdateMatrix();

		worldTransforms_[1].matWorld_ = Affin::matUnit();
		worldTransforms_[1].matWorld_ = Affin::matWorld(worldTransforms_[1].translation_, worldTransforms_[1].rotation_, worldTransforms_[1].scale_);

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

		//3つ目
		//worldTransforms_[2].matWorld_ *= worldTransforms_[2].parent_->matWorld_;

		worldTransforms_[3].TransferMatrix();
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
	model_->Draw(worldTransforms_[0], viewProjection_, textureHandle_);
	model_->Draw(worldTransforms_[1], viewProjection_, textureHandle_);
	model_->Draw(worldTransforms_[2], viewProjection_, textureHandle_);
	model_->Draw(worldTransforms_[3], viewProjection_, textureHandle_);
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
