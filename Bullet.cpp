#include "Bullet.h"


void Bullet::Initialize(Model* model, const Vector3& position)
{
	//NULLチェック
	assert(model);
	model_ = model;

	textureHandle_ = TextureManager::Load("mario.jpg");

	worldTransform_.Initialize();

	worldTransform_.translation_ = position;
}

void Bullet::Update()
{
	worldTransform_.matWorld_ = Affin::matTrans(worldTransform_.translation_);
	//行列の再計算
	worldTransform_.TransferMatrix();
}

void Bullet::Draw(const ViewProjection& viewProjection)
{
	//モデルの描画
	model_->Draw(worldTransform_, viewProjection, textureHandle_);
}
