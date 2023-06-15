#include "Projectile.h"
#include "ProjectileManager.h"

// �R���X�g���N�^
Projectile::Projectile(ProjectileManager* manager) : manager(manager) {
    manager->Register(this);
}

// �f�o�b�O�v���~�e�B�u�`��
void Projectile::DrawDebugPrimitive() {

}

// �s��X�V����
void Projectile::UpdateTransform() {

}

// �j��
void Projectile::Destroy() {
    manager->Remove(this);
}