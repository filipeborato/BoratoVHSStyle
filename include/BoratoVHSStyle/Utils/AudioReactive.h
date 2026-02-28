#pragma once

#include <atomic>
#include <juce_core/juce_core.h>

namespace bvs {

struct AudioReactive {

    void setSmoothedLevel(float lin) { level.store(juce::jlimit(0.f,1.f,lin)); }

    float getLevel() const { return level.load(); }

private:

    std::atomic<float> level {0.f};

};

}
