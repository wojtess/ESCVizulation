#include "engine.h"
#define PI 3.14159265358979323846
#include <algorithm>
#include <cmath>
#include <ctime>
#include <chrono>

#include <cstdio>

uint64_t timeSinceEpochMillisec() {
  using namespace std::chrono;
  return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

Phase::Phase(std::shared_ptr<Buffer<float>> historicData, std::shared_ptr<std::atomic<float>> currentData) {
    this->currentData = currentData;
    this->historicData = historicData;
}

void Phase::setValue(float value) {
    if(this->freqnecy != 0) {
        auto peroid = 1000/this->freqnecy;
        auto epoch = timeSinceEpochMillisec();
        if(epoch - this->lastStore > peroid) {
            this->lastStore = epoch;
            historicData->push(value);
        }
    }
    // if(currentData->load() != value) {
        
    // }
    currentData->store(value);
}

Engine::Engine(): phaseA_PWM_controller(50), phaseB_PWM_controller(50), phaseC_PWM_controller(50) {
    this->phase_A = std::shared_ptr<Phase>(new Phase{std::make_shared<Buffer<float>>(20), {std::make_shared<std::atomic<float>>()}});
    this->phase_B = std::shared_ptr<Phase>(new Phase{std::make_shared<Buffer<float>>(20), {std::make_shared<std::atomic<float>>()}});
    this->phase_C = std::shared_ptr<Phase>(new Phase{std::make_shared<Buffer<float>>(20), {std::make_shared<std::atomic<float>>()}});
    this->phase_A_PWM = std::shared_ptr<Phase>(new Phase{std::make_shared<Buffer<float>>(20), {std::make_shared<std::atomic<float>>()}});
    this->phase_B_PWM = std::shared_ptr<Phase>(new Phase{std::make_shared<Buffer<float>>(20), {std::make_shared<std::atomic<float>>()}});
    this->phase_C_PWM = std::shared_ptr<Phase>(new Phase{std::make_shared<Buffer<float>>(20), {std::make_shared<std::atomic<float>>()}});
    this->phase_first = std::shared_ptr<Phase>(new Phase{std::make_shared<Buffer<float>>(20), {std::make_shared<std::atomic<float>>()}});
    this->phase_second = std::shared_ptr<Phase>(new Phase{std::make_shared<Buffer<float>>(20), {std::make_shared<std::atomic<float>>()}});
    this->phase_third = std::shared_ptr<Phase>(new Phase{std::make_shared<Buffer<float>>(20), {std::make_shared<std::atomic<float>>()}});

    this->target_rpm = std::make_shared<std::atomic<float>>(5000);
}

void Engine::startThread() {
    this->thread = std::thread(&Engine::loop, this);
    this->working = true;
}

void Engine::stopThreadAndWait() {
    this->working = false;
    this->thread.join();
}

void Engine::setPulseWidth(uint64_t width) {
    phaseA_PWM_controller.pulseWidth = width;
    phaseB_PWM_controller.pulseWidth = width;
    phaseC_PWM_controller.pulseWidth = width;
}

uint64_t Engine::getPulseWidth() {
    return phaseA_PWM_controller.pulseWidth;
}

void Engine::setBufferSize(int size) {
    phase_A->historicData->setSize(size);
    phase_B->historicData->setSize(size);
    phase_C->historicData->setSize(size);
    phase_A_PWM->historicData->setSize(size);
    phase_B_PWM->historicData->setSize(size);
    phase_C_PWM->historicData->setSize(size);
    phase_first->historicData->setSize(size);
    phase_second->historicData->setSize(size);
    phase_third->historicData->setSize(size);
}

int Engine::getBufferSize() {
    return phase_A->historicData->getSize();
}

void Engine::loop() {
    while(this->working) {
        auto epoch = timeSinceEpochMillisec();

        float timeBeetwenRuns =  60/*60 seconds*/ * 1000/*1000 miliseconds*/ * 3/*three phases*/ / this->target_rpm->load();
        // printf("timeBeetwenRuns: %f, epoch - this->lastTimeRun: %ld\n", timeBeetwenRuns, (epoch - this->lastTimeRun));

        if(epoch - this->runStartTime > timeBeetwenRuns) {
            this->runStartTime = epoch;
        }
        float phaseEpoch = (epoch - this->runStartTime) / timeBeetwenRuns * PI * 2;

        float phaseFirst = sin(phaseEpoch + (0 * PI/180.0));
        phaseFirst = std::max(phaseFirst, 0.0f);
        float phaseSecond = sin(phaseEpoch + (120 * PI/180.0));
        phaseSecond = std::max(phaseSecond, 0.0f);
        float phaseThird = sin(phaseEpoch + (240 * PI/180.0));
        phaseThird = std::max(phaseThird, 0.0f);

        float phaseA = phaseFirst - phaseSecond;
        float phaseB = phaseSecond - phaseThird;
        float phaseC = phaseThird - phaseFirst;


        if(phaseA_PWM_controller.calculate(phaseA * 65535)) {
            phase_A_PWM->setValue(1);
        } else {
            phase_A_PWM->setValue(0);
        }

        if(phaseB_PWM_controller.calculate(phaseB * 65535)) {
            phase_B_PWM->setValue(1);
        } else {
            phase_B_PWM->setValue(0);
        }

        if(phaseC_PWM_controller.calculate(phaseC * 65535)) {
            phase_C_PWM->setValue(1);
        } else {
            phase_C_PWM->setValue(0);
        }

        // if(!(epoch - this->lastTimeRun > timeBeetwenRuns)) {
        //     continue;
        // }

        // this->lastTimeRun = epoch;

        // currentPhase++;
        // if(currentPhase >= 3) {
        //     currentPhase = 0;
        // }
        // float phaseA = 0;
        // float phaseB = 0;
        // float phaseC = 0;
        // if(currentPhase == 0) {
        //     phaseA = 1;
        // } else if(currentPhase == 1) {
        //     phaseB = 1;
        // } else if(currentPhase == 2) {
        //     phaseC = 1;
        // }
        
        // float phaseA = sin((double)(epoch) * speed + (0 * PI/180.0));
        // float phaseB = sin((double)(epoch) * speed + (120 * PI/180.0));
        // float phaseC = sin((double)(epoch) * speed + (240 * PI/180.0));

        this->phase_A->setValue(phaseA);
        this->phase_B->setValue(phaseB);
        this->phase_C->setValue(phaseC);

        this->phase_first->setValue(phaseFirst);
        this->phase_second->setValue(phaseSecond);
        this->phase_third->setValue(phaseThird);
        // printf("%f, %f, %f\n", phaseA, phaseB, phaseC);
    }
}

void Engine::setFreqnecyFetch(uint16_t f) {
    this->phase_A->freqnecy = f;
    this->phase_B->freqnecy = f;
    this->phase_C->freqnecy = f;
    this->phase_A_PWM->freqnecy = f;
    this->phase_B_PWM->freqnecy = f;
    this->phase_C_PWM->freqnecy = f;
    this->phase_first->freqnecy = f;
    this->phase_second->freqnecy = f;
    this->phase_third->freqnecy = f;
}

uint16_t Engine::getFreqnecyFetch() {
    return this->phase_A->freqnecy;
}

bool Engine::PWM::calculate(uint16_t value) {
    double dutyCycle = value / 65535.0;
    uint64_t onTime = pulseWidth * dutyCycle;
    auto epoch = timeSinceEpochMillisec();
    // if(epoch % pulseWidth > onTime) {
    //     //off
    //     this->value = false;
    //     // printf("epoch - this->toggleTimestamp: %ld, onTime: %ld, this->pulseWidth:%ld, this->value:%d, dutyCycle: %f, value:%d\n", epoch - this->toggleTimestamp, onTime, this->pulseWidth, this->value, dutyCycle, value);
    
    // } else {
    //     //on
    //     this->value = true;
    // }
    // if(epoch % pulseWidth > this->pulseWidth) {
    //     this->toggleTimestamp = epoch;
    //     // printf("epoch - this->toggleTimestamp: %ld, onTime: %ld, this->pulseWidth:%ld, this->value:%d, dutyCycle: %f, value:%d\n", epoch - this->toggleTimestamp, onTime, this->pulseWidth, this->value, dutyCycle, value);
    
    // }
    if(epoch - this->toggleTimestamp > onTime) {
        //off
        this->value = false;
        // printf("epoch - this->toggleTimestamp: %ld, onTime: %ld, this->pulseWidth:%ld, this->value:%d, dutyCycle: %f, value:%d\n", epoch - this->toggleTimestamp, onTime, this->pulseWidth, this->value, dutyCycle, value);
    } else {
        //on
        this->value = true;
    }
    if(epoch - this->toggleTimestamp > this->pulseWidth) {
        this->toggleTimestamp = epoch;
        // printf("epoch - this->toggleTimestamp: %ld, onTime: %ld, this->pulseWidth:%ld, this->value:%d, dutyCycle: %f, value:%d\n", epoch - this->toggleTimestamp, onTime, this->pulseWidth, this->value, dutyCycle, value);
    }
    return this->value;
}

Engine::PWM::PWM(uint64_t pulseWidth) {
    this->pulseWidth = pulseWidth;
}