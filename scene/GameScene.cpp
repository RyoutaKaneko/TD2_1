#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>

#include "AxisIndicator.h"
#include "PrimitiveDrawer.h"

#include <random>
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

	worldTransforms_[1].Initialize();
	worldTransforms_[1].translation_ = { 0,4.5f,0 };
	worldTransforms_[1].parent_ = &worldTransforms_[0];
	//ビュープロジェクションの初期化
	viewProjection_.Initialize();
	viewProjection_.UpdateMatrix();


}

void GameScene::Update() {

	//視点の移動速さ
	const float KEyeSpeed = 0.01f;
	const float kCharacterSpeed = 0.2f;

	Vector3 move = { 0,0,0 };

	{
		if (input_->PushKey(DIK_RIGHT)) {
			move = { kCharacterSpeed,0,0 };
		}
		else if (input_->PushKey(DIK_LEFT)) {
			move = { -kCharacterSpeed,0,0 };
		}
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
	model_ ->Draw(worldTransforms_[0], viewProjection_, textureHandle_);
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
