#include "Projectile.h"
#include "ProjectileManager.h"

// コンストラクタ
Projectile::Projectile(ProjectileManager* manager) : manager(manager) {
    manager->Register(this);
}

// デバッグプリミティブ描画
void Projectile::DrawDebugPrimitive() {

}

// 行列更新処理
void Projectile::UpdateTransform() {

}

// 破棄
void Projectile::Destroy() {
    manager->Remove(this);
}