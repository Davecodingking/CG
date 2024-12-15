#pragma once
#include "Behaviour.h"

class Camera : public Behaviour {
    Matrix viewMat;
    Matrix projMat;
    Matrix viewProj;

    float moveSpeed;
    float rotSpeed;

    static Camera* instance; // singleton instance
public:


    // �������ģʽö��
    enum class CameraMode {
        FirstPerson,
        ThirdPerson
    };

    CameraMode mode = CameraMode::FirstPerson; // Ĭ�����ģʽ


    // ���캯��
    Camera(Vec2 _size, Vec3 _pos, Vec3 _rot, float _nearPlane = 0.1f, float _farPlane = 100.f);

    // ��ȡ����ʵ��
    static Camera* GetInstance();

    // ��ȡ��ǰ���ģʽ
    CameraMode GetMode() const { return mode; }
    // �������ģʽ
    void SetMode(CameraMode newMode) { mode = newMode; }

    // �л����ģʽ
    void ToggleMode();

    // �����ӽǸ���
    void Update(float _dt) override;
    // ������ͼ��ͶӰ����
    void UpdateMat();
    // ������ͼͶӰ����
    Matrix GetViewProjMat() const;

    // ��������
    ~Camera();
};
