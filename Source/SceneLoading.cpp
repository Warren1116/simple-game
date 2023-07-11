#include  "Graphics/Graphics.h"
#include "Input/Input.h"
#include "SceneLoading.h"
#include "SceneManager.h"

// ������
void SceneLoading::Initialize() {
    // �X�v���C�g������
    sprite = new Sprite("Data/Sprite/road.png");
    Enter = new Sprite("Data/Sprite/Enter.png");
    tutorial[0] = new Sprite("Data/Sprite/cyutoriaru.png");
    tutorial[1] = new Sprite("Data/Sprite/chuto2.png");

    // �X���b�h�J�n
    thread = new std::thread(SceneLoading::LoadingThread, this); // thread���N������ۂ̓X���b�h�p�֐��ƃC���X�^���X��this�|�C���^�������ɓn��
}

// �I����
void SceneLoading::Finalize() {
    // �X���b�h�I����

    if (thread != nullptr) {
        thread->join();
        delete thread;
        thread = nullptr;
    }

    if (Enter != nullptr)
    {
        delete Enter;
        Enter = nullptr;
    }

    if (tutorial[0] != nullptr)
    {
        delete tutorial[0];
        tutorial[0] = nullptr;
    }

    if (tutorial[1] != nullptr)
    {
        delete tutorial[1];
        tutorial[1] = nullptr;
    }

    // �X�v���C�g�I����
    if (sprite != nullptr) {
        delete sprite;
        sprite = nullptr;
    }
}

// �X�V����
void SceneLoading::Update(float elapsedTime) {
    constexpr float speed = 180;
    angle += speed * elapsedTime;

    // ���̃V�[���̏��������������Ƃ��ɂȂɂ��{�^������������V�[����؂�ւ���
    GamePad& gamePad = Input::Instance().GetGamePad();
    const GamePadButton anyButton = GamePad::BTN_ENTER;
    float ax = gamePad.GetAxisLX();

    if (ax < 0) tutorialNum = 0;
    else if(ax > 0)  tutorialNum = 1;

    if (nextScene->IsReady() && (gamePad.GetButtonDown() & anyButton)) {
        SceneManager::Instance().ChangeScene(nextScene);
        nextScene = nullptr;
    }
}

// �`�揈��
void SceneLoading::Render() {
    Graphics& graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = graphics.GetDeviceContext();
    ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
    ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

    // ��ʃN���A&�����_�[�^�[�Q�b�g�ݒ�
    FLOAT color[] = { 0.0f,0.0f ,0.5f,1.0f }; // RGB(0.0�`1.0)
    dc->ClearRenderTargetView(rtv, color);
    dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    dc->OMSetRenderTargets(1, &rtv, dsv);

    //2D�X�v���C�g�`��
    {
        float screenWidth = static_cast<float>(graphics.GetScreenWidth());
        float screenHeight = static_cast<float>(graphics.GetScreenHeight());
        float textureWidth = static_cast<float>(tutorial[tutorialNum]->GetTextureWidth());
        float textureHeight = static_cast<float>(tutorial[tutorialNum]->GetTextureHeight());

        tutorial[tutorialNum]->Render(dc,
            0, 0, screenWidth, screenHeight,
            0, 0, textureWidth, textureHeight,
            0,
            1, 1, 1, 1);

        textureWidth = static_cast<float>(sprite->GetTextureWidth());
        textureHeight = static_cast<float>(sprite->GetTextureHeight());
        float positionX = screenWidth - textureWidth;
        float positionY = screenHeight - textureHeight;


        // ��ʉE���Ƀ��[�f�B���O�A�C�R����`��
        if (nextScene != nullptr && !nextScene->IsReady())
        {
            sprite->Render(dc,
                positionX, positionY, textureWidth, textureHeight,
                0, 0, textureWidth, textureHeight,
                angle,
                1, 1, 1, 1);
        }
        else
        {
            textureWidth = static_cast<float>(Enter->GetTextureWidth());
            textureHeight = static_cast<float>(Enter->GetTextureHeight());

            Enter->Render(dc,
                positionX - 500, positionY + 100, textureWidth, textureHeight,
                0, 0, textureWidth, textureHeight,
                0,
                1, 1, 1, 1);
        }
    }

}

// ���[�f�B���O�X���b�h
void SceneLoading::LoadingThread(SceneLoading* scene) {
    // COM�֘A�̏������ŃX���b�h���ɌĂԕK�v������
    CoInitialize(nullptr);

    // ���̃V�[���̏��������s��
    scene->nextScene->Initialize();

    // �X���b�h���I���O��COM�֘A�̏I����
    CoUninitialize();

    // ���̃V�[���̏��������ݒ�
    scene->nextScene->SetReady();
}