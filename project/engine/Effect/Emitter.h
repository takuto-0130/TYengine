#pragma once
#include "struct.h"
#include <d3d12.h>
#include <wrl.h>

//struct EmitterParam {
//    Vector3 position = { 0,0,0 };
//    float emitRate = 10.0f;   // 秒間発生数
//
//    uint32_t emitMode = 1;    // 0=OFF, 1=LOOP, 2=ONESHOT, 3=SEQUENTIAL
//    uint32_t emitRemaining = 0;
//
//    Vector3 direction = { 0,1,0 };
//    float angle = 0.5f;       // 方向のばらつき
//
//    float speedMin = 1.0f, speedMax = 2.0f;
//    float lifeMin = 1.0f, lifeMax = 3.0f;
//
//    Vector4 color = { 1,1,1,1 };
//};
//
//class Emitter {
//public:
//    void StartLoop() {
//        params_.emitMode = 1;
//    }
//
//    void Stop() {
//        params_.emitMode = 0;
//    }
//
//    void TriggerOnce(uint32_t count) {
//        params_.emitMode = 2;
//        params_.emitRemaining = count;
//    }
//
//    void TriggerSequential(uint32_t count) {
//        params_.emitMode = 3;
//        params_.emitRemaining = count;
//    }
//
//    void Update() {
//        // EmitterParamをGPUバッファにコピー
//        void* mapped = nullptr;
//        emitterBuffer_->Map(0, nullptr, &mapped);
//        memcpy(mapped, &params_, sizeof(EmitterParam));
//        emitterBuffer_->Unmap(0, nullptr);
//    }
//
//    EmitterParam& GetParams() { return params_; }
//
//private:
//    EmitterParam params_;
//    Microsoft::WRL::ComPtr<ID3D12Resource> emitterBuffer_; // CBV用
//};