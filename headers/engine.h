#include <thread>
#include <atomic>
#include <memory>
#include "helpers.h"

struct Phase {
    std::shared_ptr<Buffer<float>> historicData;
    std::shared_ptr<std::atomic<float>> currentData;
    std::atomic<uint16_t> freqnecy;

    private:
        uint64_t lastStore;

    public:
        Phase(std::shared_ptr<Buffer<float>>, std::shared_ptr<std::atomic<float>>);

        void setValue(float value);
};

class Engine {
    private:
        class PWM {
            private:
                uint64_t toggleTimestamp;
                bool value = true;
            public:
                PWM(uint64_t);
                uint64_t pulseWidth;
                bool calculate(uint16_t value);
        };
    private:
        //internal varibles for engine itself
        uint64_t lastTimeRun;
        uint64_t runStartTime;
        //value from 0 to 2
        int currentPhase;

        PWM phaseA_PWM_controller;
        PWM phaseB_PWM_controller;
        PWM phaseC_PWM_controller;
    private:
        std::thread thread;
        std::atomic_bool working;
        void loop();
    public:
        Engine();
        void startThread();
        void stopThreadAndWait();
        void setPulseWidth(uint64_t width);
        uint64_t getPulseWidth();
        void setFreqnecyFetch(uint16_t f);
        uint16_t getFreqnecyFetch();
        int getBufferSize();
        void setBufferSize(int);
        std::shared_ptr<Phase> phase_A;
        std::shared_ptr<Phase> phase_B;
        std::shared_ptr<Phase> phase_C;
        std::shared_ptr<Phase> phase_A_PWM;
        std::shared_ptr<Phase> phase_B_PWM;
        std::shared_ptr<Phase> phase_C_PWM;
        std::shared_ptr<Phase> phase_first;
        std::shared_ptr<Phase> phase_second;
        std::shared_ptr<Phase> phase_third;
        std::shared_ptr<std::atomic<float>> target_rpm;
};

