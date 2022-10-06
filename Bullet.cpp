#include "Bullet.h"


void Bullet::Initialize(Model* model, const Vector3& position)
{
	//NULLチェック
	assert(model_);
	model_ = model;

	textureHandle_ = TextureManager::Load("mario.png");

	worldTransform_.Initialize();

	worldTransform_.translation_ = position;
}

void Bullet::Update()
{
	worldTransform_.matWorld_ = Identity();
	worldTransform_.matWorld_ *= Scale(worldTransform_.scale_);
	worldTransform_.matWorld_ *= RotX(worldTransform_.rotation_);
	worldTransform_.matWorld_ *= RotY(worldTransform_.rotation_);
	worldTransform_.matWorld_ *= RotZ(worldTransform_.rotation_);
	worldTransform_.matWorld_ *= Trans(worldTransform_.translation_);
	//行列の再計算
	worldTransform_.TransferMatrix();
}

void Bullet::Draw(const ViewProjection& viewProjection)
{

	//モデルの描画
	model_->Draw(worldTransform_, viewProjection, textureHandle_);
}
