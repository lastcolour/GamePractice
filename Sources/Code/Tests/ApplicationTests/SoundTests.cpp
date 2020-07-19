#include "SoundTests.hpp"
#include "Audio/ETSound.hpp"

namespace {

const char* TEST_SOUND_NAME = "UI/buttonPress_01.ogg";
const int MAX_PARALLEL_SOUNDS = 16;

} // namespace

TEST_F(SoundTests, CheckCreateSound) {
    std::unique_ptr<Sound> sound;
    ET_SendEventReturn(sound, &ETSoundManager::ET_createSound, TEST_SOUND_NAME);
    ASSERT_TRUE(sound);

    ASSERT_FALSE(sound->isPlaying());

    sound->play();

    ASSERT_TRUE(sound->isPlaying());

    sound->pause();

    ASSERT_FALSE(sound->isPlaying());

    sound->resume();

    ASSERT_TRUE(sound->isPlaying());

    sound->stop();

    ASSERT_FALSE(sound->isPlaying());
}

TEST_F(SoundTests, CheckCreateManySound) {
    std::vector<std::shared_ptr<Sound>> sounds;
    for(int i = 0; i < MAX_PARALLEL_SOUNDS; ++i) {
        std::unique_ptr<Sound> sound;
        ET_SendEventReturn(sound, &ETSoundManager::ET_createSound, TEST_SOUND_NAME);
        if(sound) {
            sounds.push_back(std::move(sound));
        }
    }

    ASSERT_EQ(sounds.size(), MAX_PARALLEL_SOUNDS);

    for(size_t i = 0, sz = sounds.size(); i < sz; ++i) {
        auto& sound = sounds[i];
        sound->play();
        EXPECT_TRUE(sound->isPlaying());
    }
}