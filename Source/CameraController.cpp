#include <imgui.h>
#include "CameraController.h"
#include "Camera.h"
#include "Input/Input.h"

// 更新処理
void CameraController::Update(float elapsedTime) {
    if (cameraType == CameraType1) {
        GamePad& gamePad = Input::Instance().GetGamePad();
        float ax = gamePad.GetAxisRY();
        float ay = gamePad.GetAxisRX();

        // カメラの回転速度
        float speed = rollSpeed * elapsedTime;

        // スティックの入力値に合わせてX軸とY軸を回転
        angle.x += angle.x * speed;
        angle.y += angle.y * speed;


        // X軸のカメラ回転を制限
        if (angle.x < minAngleX) {
            angle.x = minAngleX;
        }
        if (angle.x > maxAngleX) {
            angle.x = maxAngleX;
        }

        // Y軸の回転値を-3.14~3.14に収まるようにする
        if (angle.y < -DirectX::XM_PI) {
            angle.y += DirectX::XM_2PI;
        }
        if (angle.y > DirectX::XM_PI) {
            angle.y -= DirectX::XM_2PI;
        }
        // カメラ回転値を回転行列に変換
        DirectX::XMMATRIX Transform = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);

        // 回転行列から前方向ベクトルを取り出す
        DirectX::XMVECTOR Front = Transform.r[2];
        DirectX::XMFLOAT3 front;
        DirectX::XMStoreFloat3(&front, Front);

        // 注視点から後ろベクトル方向に一定距離離れたカメラ視点を求める
        DirectX::XMFLOAT3 eye;
        eye.x = target.x - front.x * range;
        eye.y = target.y - front.y * range;
        eye.z = target.z - front.z * range;

        // カメラの視点と注視点を設定
        Camera::Instance().SetLookAt(eye, target, DirectX::XMFLOAT3(0, 1, 0));
    }
    else if (cameraType == CameraType2) {
        DirectX::XMVECTOR Front = DirectX::XMLoadFloat3(&targetDirection);
        Front = DirectX::XMVector3Normalize(Front);

        DirectX::XMStoreFloat3(&targetDirection, Front);

        DirectX::XMFLOAT3 eye;
        eye.x = target.x - targetDirection.x * range;
        eye.y = target.y - targetDirection.y * range + 1.0f;
        eye.z = target.z - targetDirection.z * range;

        // カメラの視点と注視点を設定
        Camera::Instance().SetLookAt(eye, target, DirectX::XMFLOAT3(0, 1, 0));
    }
}

void CameraController::DrawDebugGUI() {
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

    if (ImGui::Begin("CameraController")) {
        // CameraType
        ImGui::InputInt("CameraType", &cameraType);
        // RollSpeed
        ImGui::InputFloat("RollSpeed", &rollSpeed);
        // Range
        ImGui::InputFloat("Range", &range);
    }
    ImGui::End();
}