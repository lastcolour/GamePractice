#include "SoundTests.hpp"
#include "Audio/Sound.hpp"
#include "Core/ETTasks.hpp"
#include "AudioBufferQueue.hpp"
#include "SoundData.hpp"
#include "MixGraph/OggDataStream.hpp"
#include "Core/ETAssets.hpp"

#include <thread>

namespace {

const char* TEST_SOUND_NAME = "Sounds/Game/Music/8bit_Bossa.ogg";
const char* TEST_MONO_SOUND = "Sounds/UI/buttonPress_01.ogg";
const int MAX_PARALLEL_SOUNDS = 16;

void DoAudioSystemStep() {
    for(int i = 0; i < 3; ++i) {
        ET_SendEvent(&ETSoundUpdateTask::ET_updateSound);
        ET_SendEvent(&ETAssetsUpdateTask::ET_updateAssets, 0.f);
    }
}

} // namespace

TEST_F(SoundTests, CheckCreateSound) {
    Sound sound;
    sound.setFile(TEST_SOUND_NAME);

    ASSERT_FALSE(sound.isPlaying());

    {
        sound.play();

        DoAudioSystemStep();

        EXPECT_TRUE(sound.isPlaying());
    }

    {
        sound.pause();

        DoAudioSystemStep();

        EXPECT_FALSE(sound.isPlaying());
    }

    {
        sound.resume();

        DoAudioSystemStep();

        EXPECT_TRUE(sound.isPlaying());
    }

    {
        sound.stop();

        DoAudioSystemStep();

        EXPECT_FALSE(sound.isPlaying());
    }
}

TEST_F(SoundTests, CheckCreateManySound) {
    std::vector<Sound> sounds;
    for(int i = 0; i < MAX_PARALLEL_SOUNDS; ++i) {
        Sound sound;
        sound.setFile(TEST_SOUND_NAME);
        sounds.emplace_back(std::move(sound));
    }

    ASSERT_EQ(sounds.size(), MAX_PARALLEL_SOUNDS);

    for(size_t i = 0, sz = sounds.size(); i < sz; ++i) {
        auto& sound = sounds[i];
        sound.play();
    }

    DoAudioSystemStep();

    for(size_t i = 0, sz = sounds.size(); i < sz; ++i) {
        auto& sound = sounds[i];
        EXPECT_TRUE(sound.isPlaying());
    }
}

TEST_F(SoundTests, CheckInvalidSound) {
    Sound sound;

    {
        sound.play();

        DoAudioSystemStep();

        ASSERT_FALSE(sound.isPlaying());
    }

    {
        sound.fadeInPlay(1.f);

        DoAudioSystemStep();

        ASSERT_FALSE(sound.isPlaying());
    }

    {
        sound.resume();

        DoAudioSystemStep();

        ASSERT_FALSE(sound.isPlaying());
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
            ASSERT_EQ(readSize, readBuff->getReadSize());
            readBuff->read(&val, sizeof(int));

            EXPECT_EQ(val, i);
            ASSERT_EQ(readBuff->getReadSize(), 0);

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
                int size = readBuff->getReadSize();
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
                ASSERT_EQ(readBuff->getReadSize(), 5 * sizeof(int));
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

TEST_F(SoundTests, CheckStereoMixOfMonoSource) {
    std::shared_ptr<SoundData> soundData(new SoundData);

    {
        Buffer buff;
        ET_SendEventReturn(buff, &ETAssets::ET_loadAsset, TEST_MONO_SOUND);
        ASSERT_TRUE(buff);
        soundData->setLoading();
        soundData->setLoaded(buff);
    }

    OggDataStream oggDataStream;
    ASSERT_TRUE(oggDataStream.open(soundData));
    ASSERT_EQ(oggDataStream.getChannels(), 1);

    int SAMPLES = 512;

    std::vector<float> data(SAMPLES * 2, 0.f);
    {
        bool looped = false;
        int readCount = oggDataStream.readF32(&data[0], 2, SAMPLES, looped);
        oggDataStream.close();
        ASSERT_EQ(readCount, SAMPLES);
    }

    int zerosCount = 0;
    for(int i = 0; i < SAMPLES; ++i) {
        ASSERT_FLOAT_EQ(data[2 * i], data[2 * i + 1]);
        if(abs(data[2 * i]) < 0.001f) {
            ++zerosCount;
        }
    }

    EXPECT_LE(zerosCount, SAMPLES / 8);
}