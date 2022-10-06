#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>

#include "AxisIndicator.h"
#include "PrimitiveDrawer.h"

#include <random>
#define PI 3.14
//単位行列
Matrix4 Identity() {
	Matrix4 matIdentity;
	matIdentity.m[0][0] = 1;
	matIdentity.m[1][1] = 1;
	matIdentity.m[2][2] = 1;
	matIdentity.m[3][3] = 1;
	return matIdentity;
}
//スケーリング行列を宣言
Matrix4 Scale(Vector3 scale_) {
	//スケーリングの設定
	Matrix4 matScale;
	matScale.m[0][0] = scale_.x;
	matScale.m[1][1] = scale_.y;
	matScale.m[2][2] = scale_.z;
	matScale.m[3][3] = 1;
	return matScale;
}
Matrix4 RotZ(Vector3 rotation_) {
	//Z軸回転行列を宣言
	Matrix4 matRotZ;
	matRotZ = Identity();
	matRotZ.m[0][0] = cos(rotation_.z);
	matRotZ.m[0][1] = sin(rotation_.z);
	matRotZ.m[1][0] = -sin(rotation_.z);
	matRotZ.m[1][1] = cos(rotation_.z);
	return matRotZ;
}
Matrix4 RotX(Vector3 rotation_) {
	//X軸回転行列を宣言
	Matrix4 matRotX;
	matRotX = Identity();
	matRotX.m[1][1] = cos(rotation_.x);
	matRotX.m[1][2] = sin(rotation_.x);
	matRotX.m[2][1] = -sin(rotation_.x);
	matRotX.m[2][2] = cos(rotation_.x);
	return matRotX;
}
Matrix4 RotY(Vector3 rotation_) {
	//Y軸回転行列を宣言
	Matrix4 matRotY;
	matRotY = Identity();
	matRotY.m[0][0] = cos(rotation_.y);
	matRotY.m[0][2] = -sin(rotation_.y);
	matRotY.m[2][0] = sin(rotation_.y);
	matRotY.m[2][2] = cos(rotation_.y);
	return matRotY;
}
//平行移動行列を宣言
Matrix4 Trans(Vector3 translation_) {
	Matrix4 matTrans;
	matTrans = Identity();
	matTrans.m[3][0] = translation_.x;
	matTrans.m[3][1] = translation_.y;
	matTrans.m[3][2] = translation_.z;
	return matTrans;
}



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
	delete debugCamera_;
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

	//デバックカメラの生成
	debugCamera_ = new DebugCamera(1280, 720);


}

void GameScene::Update() {

	//視点の移動速さ
	const float KEyeSpeed = 0.01f;
	const float kCharacterSpeed = 0.2f;

	debugCamera_->Update();

	Vector3 move = { 0,0,0 };

	{
		if (input_->PushKey(DIK_RIGHT)) {
			move = { kCharacterSpeed,0,0 };
		}
		else if (input_->PushKey(DIK_LEFT)) {
			move = { -kCharacterSpeed,0,0 };
		}

		worldTransforms_[0].translation_ += move;

		worldTransforms_[0].matWorld_ = Identity();
		worldTransforms_[0].matWorld_ *= Scale(worldTransforms_[0].scale_);
		worldTransforms_[0].matWorld_ *= RotX(worldTransforms_[0].rotation_);
		worldTransforms_[0].matWorld_ *= RotY(worldTransforms_[0].rotation_);
		worldTransforms_[0].matWorld_ *= RotZ(worldTransforms_[0].rotation_);
		worldTransforms_[0].matWorld_ *= Trans(worldTransforms_[0].translation_);

		worldTransforms_[0].TransferMatrix();
	}
	//子の更新(子は子で初期化)
	{
		worldTransforms_[1].matWorld_ = Identity();
		worldTransforms_[1].matWorld_ *= Scale(worldTransforms_[1].scale_);
		worldTransforms_[1].matWorld_ *= RotX(worldTransforms_[1].rotation_);
		worldTransforms_[1].matWorld_ *= RotY(worldTransforms_[1].rotation_);
		worldTransforms_[1].matWorld_ *= RotZ(worldTransforms_[1].rotation_);
		worldTransforms_[1].matWorld_ *= Trans(worldTransforms_[1].translation_);
		//２つ目
		worldTransforms_[1].matWorld_ *= worldTransforms_[1].parent_->matWorld_;

		worldTransforms_[1].TransferMatrix();
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
	model_->Draw(worldTransforms_[0], debugCamera_->GetViewProjection(), textureHandle_);
	model_->Draw(worldTransforms_[1], debugCamera_->GetViewProjection(), textureHandle_);

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
