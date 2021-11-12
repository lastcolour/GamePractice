#ifndef __SOUND_TEST_HPP__
#define __SOUND_TEST_HPP__

#include "TestUtils/EngineTests.hpp"

class SoundTests : public EngineTests {
public:

    static void SetUpTestCase() {
        CreateTestApp(ETestAppModules::CheckSound);
    }
};

#endif /* __SOUND_TEST_HPP__ */