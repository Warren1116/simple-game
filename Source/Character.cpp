#include "Character.h"
#include "Stage.h"

// 行列更新処理
void Character::UpdateTransform()
{
	// スケール行列を作成
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	// 回転行列を作成
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	// 位置行列を作成
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	// ３つの行列を組み合わせ、ワールド行列を作成
	DirectX::XMMATRIX W = S * R * T;
	// 計算したワールド行列を取り出す
	DirectX::XMStoreFloat4x4(&transform, W);
}



// 垂直速力更新処理
void Character::UpdateVerticalVelocity(float elapsedFrame)
{
	// 重力処理
	
}

// 垂直移動更新処理
void Character::UpdateVerticalMove(float elapsedTime)
{
	// 進行方向の移動量
	float my = velocity.y * elapsedTime;

		// レイの開始位置は足元より少し上
		DirectX::XMFLOAT3 start = { position.x, position.y + stepOffset, position.z };
		// レイの終了位置は移動後の位置
		DirectX::XMFLOAT3 end = { position.x, position.y + my, position.z };

		// レイキャストによる地面判定
		HitResult hit;
		if (Stage::Instance().RayCast(start, end, hit))
		{
			// 地面に接地している
			position.y = hit.position.y;

			velocity.y = hit.position.y;
		}


	
}

void Character::UpdateHorizontalVelocity(float elapsedFrame)
{
	////XZ平面の速力を減速する
	//float length = sqrtf(velocity.x * velocity.x + velocity.y * velocity.y + velocity.z * velocity.z);

	//if (length > 0.0f)
	//{
	//	//摩擦力
	//	float friction = this->friction * elapsedFrame;

	//	friction *= airControl;

	//	//摩擦による横方向の減速処理
	//	if (length > friction)
	//	{
	//		//単位ベクトル化
	//		float vx = velocity.x / length;
	//		float vy = velocity.y / length;
	//		float vz = velocity.z / length;

	//		velocity.x -= vx * friction;
	//		velocity.y -= vy * friction;
	//		velocity.z -= vz * friction;
	//	}
	//	//横方向の速力が摩擦力以下になったので速力を無効化
	//	else
	//	{
	//		velocity.x = 0.0f;
	//		velocity.y = 0.0f;
	//		velocity.z = 0.0f;
	//	}
	//}
	////XZ平面の速力を加速する
	//if (length <= maxMoveSpeed)
	//{
	//	float moveVecLength = sqrtf(moveVecX * moveVecX + moveVecY * moveVecY + moveVecZ * moveVecZ);
	//	if (moveVecLength > 0.0f)
	//	{
	//		float acceleration = this->acceleration * elapsedFrame;

	//		//空中にいるときは加速力を減らす
	//		velocity.x += moveVecX * acceleration;
	//		velocity.y += moveVecY * acceleration;
	//		velocity.z += moveVecZ * acceleration;

	//		float length = sqrtf(velocity.x * velocity.x +velocity.y * velocity.y + velocity.z * velocity.z);
	//		if (length > maxMoveSpeed)
	//		{
	//			float vx = velocity.x / length;
	//			float vy = velocity.y / length;
	//			float vz = velocity.z / length;

	//			velocity.x = vx * maxMoveSpeed;
	//			velocity.y = vy * maxMoveSpeed;
	//			velocity.z = vz * maxMoveSpeed;
	//		}
	//	}
	//}
	//moveVecX = 0.0f;
	//moveVecY = 0.0f;
	//moveVecZ = 0.0f;

	//velocity.x = moveVecX;
	//velocity.y = moveVecY;
	//velocity.z = moveVecZ;

	float friction = this->friction * elapsedFrame;

	float speed = this->maxMoveSpeed;
	velocity.x = transform._21 * speed/* * friction*/; // direction
	velocity.y = transform._22 * speed/* * friction*/; // velocity = direction * speed;
	velocity.z = transform._23 * speed /** friction*/;

}

void Character::UpdateHorizontalMove(float elapsedTime)
{
	float velocityLengthXZ = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
	if (velocityLengthXZ > 0.0f)
	{
		float mx = velocity.x * elapsedTime;
		float my = velocity.y * elapsedTime;
		float mz = velocity.z * elapsedTime;

		DirectX::XMFLOAT3 start = { position.x, position.y + stepOffset, position.z };
		DirectX::XMFLOAT3 end = { position.x + mx, position.y + stepOffset, position.z + mz };

		HitResult hit;
		if (Stage::Instance().RayCast(start, end, hit))
		{
			// 壁までのベクトル
			DirectX::XMVECTOR Start = DirectX::XMLoadFloat3(&start);
			DirectX::XMVECTOR End = DirectX::XMLoadFloat3(&end);
			DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(End, Start);

			// 壁の法線
			DirectX::XMVECTOR Normal = DirectX::XMLoadFloat3(&hit.normal);

			//入射ベクトルを法線に射影
			DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(DirectX::XMVectorNegate(Vec), Normal);

			// 補正位置の計算
			DirectX::XMVECTOR CollectPosition = DirectX::XMVectorMultiplyAdd(Normal, Dot, End);
			DirectX::XMFLOAT3 collectPosition;
			DirectX::XMStoreFloat3(&collectPosition, CollectPosition);

			// 壁ずり方向へレイキャスト
			HitResult hit2;
			if (!Stage::Instance().RayCast(hit.position, collectPosition, hit2))
			{
				// 壁ずり方向で壁に当たらなかったら補正位置に移動
				position.x = collectPosition.x;
				position.z = collectPosition.z;
			}
			else
			{
				position.x = hit2.position.x;
				position.z = hit2.position.z;
			}
		}
		else
		{
			// 移動
			position.x += mx;
			position.y += my;
			position.z += mz;
		}
	}

}

//// 移動処理
//void Character::Move(float vx, float vz, float speed)
//{
//	//speed *= elapsedTime;
//	position.x += vx * speed;
//	position.z += vz * speed;
//}

// 旋回処理
void Character::Turn(float elapsedTime, float vx, float vz, float speed)
{
	speed += elapsedTime;

	// 進行ベクトルがゼロベクトルの場合は処理する必要なし
	if (vx == 0.0f && vz == 0.0f) return;

	// 進行ベクトルを単位ベクトル化
	float vl = sqrtf(vx * vx + vz * vz);
	vx /= vl;
	vz /= vl;

	// 自身の回転値から前方向を求める
	float frontX = sinf(angle.y);
	float frontZ = cosf(angle.y);
	float frontL =
		sqrtf(frontX * frontX + frontZ * frontZ);

	frontX /= frontL;
	frontZ /= frontL;

	// 回転角を求めるため、２つの単位ベクトルの内積を計算する
	float dot = (frontX * vx) + (frontZ * vz);

	// 内積値は-1.0～1.0で表現されており、２つの単位ベクトルの角度が
	// 小さいほど1.0に近づくという性質を利用して回転速度を調整する
	float rot = 1.0f - dot;
	if (rot > speed) rot = speed;

	// 左右判定を行うために２つの単位ベクトルの外積を計算する
	float cross = (frontZ * vx) - (frontX * vz);

	// 2Dの外積値が正の場合か負の場合かによって左右判定を行える
	// 左右判定を行うことによって左右回転を選択する
	if (cross <= 0.0f)
	{
		angle.y -= rot;
	}
	else
	{
		angle.y += rot;
	}
}

// ジャンプ処理
//void Character::Jump(float speed)
//{
//	 上方向の力を設定
//	velocity.y = speed;
//}

// 速度処理更新
void Character::UpdateVelocity(float elapsedTime)
{
	// 経過フレーム
	float elapsedFrame = 60.0f * elapsedTime;

	// 垂直速力更新処理
	UpdateVerticalVelocity(elapsedFrame);

	UpdateHorizontalVelocity(elapsedFrame);

	
	// 垂直移動更新処理
	UpdateVerticalMove(elapsedTime);
	UpdateHorizontalMove(elapsedTime);

}