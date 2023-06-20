#include "Character.h"
#include "Stage.h"

// �s��X�V����
void Character::UpdateTransform()
{
	// �X�P�[���s����쐬
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	// ��]�s����쐬
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	// �ʒu�s����쐬
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	// �R�̍s���g�ݍ��킹�A���[���h�s����쐬
	DirectX::XMMATRIX W = S * R * T;
	// �v�Z�������[���h�s������o��
	DirectX::XMStoreFloat4x4(&transform, W);
}



// �������͍X�V����
void Character::UpdateVerticalVelocity(float elapsedFrame)
{
	// �d�͏���
	//velocity.y += gravity * elapsedFrame;
}

// �����ړ��X�V����
void Character::UpdateVerticalMove(float elapsedTime)
{
	// �i�s�����̈ړ���
	float my = velocity.y * elapsedTime;

	// ������
	if (my < 0.0f)
	{
		// ���C�̊J�n�ʒu�͑�����菭����
		DirectX::XMFLOAT3 start = { position.x, position.y + stepOffset, position.z };
		// ���C�̏I���ʒu�͈ړ���̈ʒu
		DirectX::XMFLOAT3 end = { position.x, position.y + my, position.z };

		// ���C�L���X�g�ɂ��n�ʔ���
		HitResult hit;
		if (Stage::Instance().RayCast(start, end, hit))
		{
			// �n�ʂɐڒn���Ă���
			position.y = hit.position.y;

			// ���n����
			if (!isGround)
			{
				OnLanding();
			}
			isGround = true;
			velocity.y = hit.position.y;
		}
		else
		{
			// �󒆂ɕ����Ă���
			position.y += my;
			isGround = false;
		}
	}
	else
	{
		position.y += my;
		isGround = false;
	}
}



// �ړ�����
void Character::Move(float elapsedTime, float vx, float vz, float speed)
{
	speed *= elapsedTime;
	position.x += vx * speed;
	position.z += vz * speed;
}

// ���񏈗�
void Character::Turn(float elapsedTime, float vx, float vz, float speed)
{
	speed += elapsedTime;

	// �i�s�x�N�g�����[���x�N�g���̏ꍇ�͏�������K�v�Ȃ�
	if (vx == 0.0f && vz == 0.0f) return;

	// �i�s�x�N�g����P�ʃx�N�g����
	float vl = sqrtf(vx * vx + vz * vz);
	vx /= vl;
	vz /= vl;

	// ���g�̉�]�l����O���������߂�
	float frontX = sinf(angle.y);
	float frontZ = cosf(angle.y);
	float frontL =
		sqrtf(frontX * frontX + frontZ * frontZ);

	frontX /= frontL;
	frontZ /= frontL;

	// ��]�p�����߂邽�߁A�Q�̒P�ʃx�N�g���̓��ς��v�Z����
	float dot = (frontX * vx) + (frontZ * vz);

	// ���ϒl��-1.0�`1.0�ŕ\������Ă���A�Q�̒P�ʃx�N�g���̊p�x��
	// �������ق�1.0�ɋ߂Â��Ƃ��������𗘗p���ĉ�]���x�𒲐�����
	float rot = 1.0f - dot;
	if (rot > speed) rot = speed;

	// ���E������s�����߂ɂQ�̒P�ʃx�N�g���̊O�ς��v�Z����
	float cross = (frontZ * vx) - (frontX * vz);

	// 2D�̊O�ϒl�����̏ꍇ�����̏ꍇ���ɂ���č��E������s����
	// ���E������s�����Ƃɂ���č��E��]��I������
	if (cross <= 0.0f)
	{
		angle.y -= rot;
	}
	else
	{
		angle.y += rot;
	}
}

// �W�����v����
void Character::Jump(float speed)
{
	// ������̗͂�ݒ�
	velocity.y = speed;
}

// ���x�����X�V
void Character::UpdateVelocity(float elapsedTime)
{
	// �o�߃t���[��
	float elapsedFrame = 60.0f * elapsedTime;

	// �������͍X�V����
	UpdateVerticalVelocity(elapsedFrame);

	
	// �����ړ��X�V����
	UpdateVerticalMove(elapsedTime);

}