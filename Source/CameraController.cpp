#include <imgui.h>
#include "CameraController.h"
#include "Camera.h"
#include "Input/Input.h"

// �X�V����
void CameraController::Update(float elapsedTime) {
    if (cameraType == CameraType1) {
        GamePad& gamePad = Input::Instance().GetGamePad();
        float ax = gamePad.GetAxisRY();
        float ay = gamePad.GetAxisRX();

        // �J�����̉�]���x
        float speed = rollSpeed * elapsedTime;

        // �X�e�B�b�N�̓��͒l�ɍ��킹��X����Y������]
        angle.x += angle.x * speed;
        angle.y += angle.y * speed;


        // X���̃J������]�𐧌�
        if (angle.x < minAngleX) {
            angle.x = minAngleX;
        }
        if (angle.x > maxAngleX) {
            angle.x = maxAngleX;
        }

        // Y���̉�]�l��-3.14~3.14�Ɏ��܂�悤�ɂ���
        if (angle.y < -DirectX::XM_PI) {
            angle.y += DirectX::XM_2PI;
        }
        if (angle.y > DirectX::XM_PI) {
            angle.y -= DirectX::XM_2PI;
        }
        // �J������]�l����]�s��ɕϊ�
        DirectX::XMMATRIX Transform = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);

        // ��]�s�񂩂�O�����x�N�g�������o��
        DirectX::XMVECTOR Front = Transform.r[2];
        DirectX::XMFLOAT3 front;
        DirectX::XMStoreFloat3(&front, Front);

        // �����_������x�N�g�������Ɉ�苗�����ꂽ�J�������_�����߂�
        DirectX::XMFLOAT3 eye;
        eye.x = target.x - front.x * range;
        eye.y = target.y - front.y * range;
        eye.z = target.z - front.z * range;

        // �J�����̎��_�ƒ����_��ݒ�
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

        // �J�����̎��_�ƒ����_��ݒ�
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