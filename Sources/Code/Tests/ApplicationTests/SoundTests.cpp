#include "SoundTests.hpp"
#include "Audio/ETSound.hpp"
#include "Core/ETTasks.hpp"
#include "AudioBufferQueue.hpp"

#include <thread>

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

TEST_F(SoundTests, CheckBufferQueueSingle) {
    AudioBufferQueue bufferQueue;

    bufferQueue.init(1);

    for(int i = 0; i < 10; ++i) {
        {
            auto writeBuffs = bufferQueue.peekWrites();
            ASSERT_EQ(writeBuffs.size(), 1);
            auto buff = writeBuffs[0];

            buff->write(&i, sizeof(int));
            bufferQueue.submitWrites(writeBuffs);
        }

        {
            auto readBuff = bufferQueue.peekRead();
            ASSERT_TRUE(readBuff);

            int val = -1;

            int readSize = sizeof(int);
            ASSERT_EQ(readSize, readBuff->getAvaibleForRead());
            readBuff->read(&val, sizeof(int));

            EXPECT_EQ(val, i);
            ASSERT_EQ(readBuff->getAvaibleForRead(), 0);

            bufferQueue.tryPopRead();
        }
    }
}

TEST_F(SoundTests, CheckBufferQueueParallel) {
    AudioBufferQueue bufferQueue;

    const int MAX_ITERATION = 10;

    bufferQueue.init(5);

    std::thread readThread([&bufferQueue, MAX_ITERATION](){
        int currIter = 0;
        while(currIter < MAX_ITERATION) {
            auto readBuff = bufferQueue.peekRead();
            if(!readBuff) {
                continue;
            }

            {
                int size = readBuff->getAvaibleForRead();
                ASSERT_EQ(size, 10 * sizeof(int));

                int data[5];
                readBuff->read(&data, 5 * sizeof(int));

                for(int i = 0; i < 5; ++i) {
                    EXPECT_EQ(data[i], currIter + i);
                }
            }

            bufferQueue.tryPopRead();
            readBuff = bufferQueue.peekRead();
            ASSERT_TRUE(readBuff);

            {
                ASSERT_EQ(readBuff->getAvaibleForRead(), 5 * sizeof(int));
                int data[5];
                readBuff->read(&data, 5 * sizeof(int));

                for(int i = 0; i < 5; ++i) {
                    EXPECT_EQ(data[i], 5 + currIter + i);
                }
            }

            bufferQueue.tryPopRead();
            ++currIter;
        }
    });

    int currIter = 0;
    while(currIter < MAX_ITERATION) {
        auto writeBuffers = bufferQueue.peekWrites();
        if(writeBuffers.empty()) {
            continue;
        }
        int data[10];
        for(auto& buffer : writeBuffers) {
            for(int i = 0; i < 10; ++i) {
                data[i] = i + currIter;
            }
            buffer->write(&data, sizeof(int) * 10);
            ++currIter;
        }
        bufferQueue.submitWrites(writeBuffers);
    }

    readThread.join();
}