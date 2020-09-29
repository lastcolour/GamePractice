#include "SoundTests.hpp"
#include "Audio/ETSound.hpp"
#include "Core/ETTasks.hpp"

namespace {

const char* TEST_SOUND_NAME = "UI/buttonPress_01.ogg";
const int MAX_PARALLEL_SOUNDS = 16;

} // namespace

TEST_F(SoundTests, CheckCreateSound) {
    Sound sound;
    ET_SendEventReturn(sound, &ETSoundManager::ET_createSound, TEST_SOUND_NAME);

    ASSERT_TRUE(sound.isValid());
    ASSERT_FALSE(sound.isPlaying());

    {
        sound.play();
        ET_SendEvent(&ETSoundUpdateTask::ET_updateSound);

        ASSERT_TRUE(sound.isPlaying());
    }

    {
        sound.pause();
        ET_SendEvent(&ETSoundUpdateTask::ET_updateSound);

        ASSERT_FALSE(sound.isPlaying());
    }

    {
        sound.resume();
        ET_SendEvent(&ETSoundUpdateTask::ET_updateSound);

        ASSERT_TRUE(sound.isPlaying());
    }

    {
        sound.stop();
        ET_SendEvent(&ETSoundUpdateTask::ET_updateSound);

        ASSERT_FALSE(sound.isPlaying());
    }
}

TEST_F(SoundTests, CheckCreateManySound) {
    std::vector<Sound> sounds;
    for(int i = 0; i < MAX_PARALLEL_SOUNDS; ++i) {
        Sound sound;
        ET_SendEventReturn(sound, &ETSoundManager::ET_createSound, TEST_SOUND_NAME);
        EXPECT_TRUE(sound.isValid());
        sounds.emplace_back(std::move(sound));
    }

    ASSERT_EQ(sounds.size(), MAX_PARALLEL_SOUNDS);

    for(size_t i = 0, sz = sounds.size(); i < sz; ++i) {
        auto& sound = sounds[i];
        sound.play();
    }

    ET_SendEvent(&ETSoundUpdateTask::ET_updateSound);

    for(size_t i = 0, sz = sounds.size(); i < sz; ++i) {
        auto& sound = sounds[i];
        EXPECT_TRUE(sound.isPlaying());
    }
}