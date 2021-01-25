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
            auto writeBuffs = bufferQueue.getNextWrites();
            ASSERT_EQ(writeBuffs.size(), 1);
            auto buff = writeBuffs[i];

            int writeSize = sizeof(int);
            buff->resize(writeSize);
            memcpy(buff->getPtr(), &i, writeSize);
            buff->setReadSize(writeSize);
            bufferQueue.putWritesDone(writeBuffs);
        }

        {
            auto readBuff = bufferQueue.getNextRead();
            ASSERT_TRUE(readBuff);

            int val = -1;

            int readSize = sizeof(int);
            ASSERT_EQ(readSize, readBuff->getAvaibleSizeForRead());
            memcpy(&val, readBuff->getPtr(), readSize);
            readBuff->setReadDone(sizeof(int));

            EXPECT_EQ(val, i);
            ASSERT_EQ(readBuff->getAvaibleSizeForRead(), 0);

            bufferQueue.putReadDone(readBuff);
        }
    }
}

TEST_F(SoundTests, CheckBufferQueueParallel) {
    AudioBufferQueue bufferQueue;

    const int MAX_ITERATION = 5000;

    bufferQueue.init(5);

    std::thread readThread([&bufferQueue, MAX_ITERATION](){
        int currIter = 0;
        while(currIter < MAX_ITERATION) {
            auto readBuff = bufferQueue.getNextRead();
            if(!readBuff) {
                continue;
            }
            {
                void* ptr = readBuff->getPtr();
                int size = readBuff->getAvaibleSizeForRead();
                ASSERT_EQ(size, 10 * sizeof(int));
                int* intPtr = static_cast<int*>(ptr);
                for(int i = 0; i < 5; ++i) {
                    EXPECT_EQ(intPtr[i], currIter + i);
                }
                readBuff->setReadDone(5 * sizeof(int));
            }

            bufferQueue.putReadDone(readBuff);
            readBuff = bufferQueue.getNextRead();
            ASSERT_TRUE(readBuff);

            {
                void* ptr = readBuff->getPtr();
                int size = readBuff->getAvaibleSizeForRead();

                ASSERT_EQ(size, 5 * sizeof(int));

                int* intPtr = static_cast<int*>(ptr);
                for(int i = 0; i < 5; ++i) {
                    EXPECT_EQ(intPtr[i], 5 + i + currIter);
                }
                readBuff->setReadDone(size);
                bufferQueue.putReadDone(readBuff);
            }
            ++currIter;
        }
    });

    readThread.detach();

    int currIter = 0;
    while(currIter < MAX_ITERATION) {
        auto writeBuffers = bufferQueue.getNextWrites();
        if(writeBuffers.empty()) {
            continue;
        }

        int data[10];
        int readSize = sizeof(int) * 10;

        for(auto& buffer : writeBuffers) {
            buffer->resize(readSize);
            buffer->setReadSize(readSize);
            for(int i = 0; i < 10; ++i) {
                data[i] = i + currIter;
            }
            memcpy(buffer->getPtr(), &data[0], readSize);
            ++currIter;
        }
        bufferQueue.putWritesDone(writeBuffers);
    }
}