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


    // 定义相机模式枚举
    enum class CameraMode {
        FirstPerson,
        ThirdPerson
    };

    CameraMode mode = CameraMode::FirstPerson; // 默认相机模式


    // 构造函数
    Camera(Vec2 _size, Vec3 _pos, Vec3 _rot, float _nearPlane = 0.1f, float _farPlane = 100.f);

    // 获取单例实例
    static Camera* GetInstance();

    // 获取当前相机模式
    CameraMode GetMode() const { return mode; }
    // 设置相机模式
    void SetMode(CameraMode newMode) { mode = newMode; }

    // 切换相机模式
    void ToggleMode();

    // 自由视角更新
    void Update(float _dt) override;
    // 更新视图和投影矩阵
    void UpdateMat();
    // 返回视图投影矩阵
    Matrix GetViewProjMat() const;

    // 析构函数
    ~Camera();
};
