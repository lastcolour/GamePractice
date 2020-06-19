#include "OggDataStream.hpp"
#include "Core/Buffer.hpp"

#include <stb_vorbis.c>

#include <cassert>

OggDataStream::OggDataStream(const Buffer& buff) :
    oggBuffer(buff),
    oggStream(nullptr),
    sound(nullptr),
    channels(0),
    sampleRate(0),
    numSamples(0) {

    oggStream = stb_vorbis_open_memory(
        static_cast<const unsigned char*>(oggBuffer.getReadData()), 
            static_cast<int>(oggBuffer.getSize()), nullptr, nullptr);

    if(!oggStream) {
        return;
    }

    stb_vorbis_info orbisInfo = stb_vorbis_get_info(oggStream);
    channels = orbisInfo.channels;
    sampleRate = orbisInfo.sample_rate;
    numSamples = stb_vorbis_stream_length_in_samples(oggStream) * orbisInfo.channels;
}

OggDataStream::~OggDataStream() {
    if(!oggStream) {
        stb_vorbis_close(oggStream);
        oggStream = nullptr;
    }
}

bool OggDataStream::isOpened() const {
    if(!oggStream) {
        return false;
    }
    return true;
}

void OggDataStream::setSampleOffset(int sampleOffset) {
    assert(oggStream != nullptr && "Invalid OGG stream");
    stb_vorbis_seek_start(oggStream);
    stb_vorbis_seek_frame(oggStream, sampleOffset);
}

int OggDataStream::fillF32(void* outData, int samplesCount, int channels, bool looped) {
    int readCount = readF32(outData, samplesCount, channels);
    if(readCount < samplesCount && looped) {
        int leftCount = samplesCount - readCount;
        while(leftCount > 0) {
            setSampleOffset(0);
            int offset = readCount * channels;
            readCount += readF32(static_cast<float*>(outData) + offset, leftCount, channels);
            leftCount = samplesCount - readCount;
        }
    }
    return readCount;
}

int OggDataStream::fillI16(void* outData, int samplesCount, int channels, bool looped) {
    int readCount = readI16(outData, samplesCount, channels);
    if(readCount < samplesCount && looped) {
        int leftCount = samplesCount - readCount;
        while(leftCount > 0) {
            setSampleOffset(0);
            int offset = readCount * channels;
            readCount += readI16(static_cast<int16_t*>(outData) + offset, leftCount, channels);
            leftCount = samplesCount - readCount;
        }
    }
    return readCount;
}

int OggDataStream::readI16(void* outData, int samplesCount, int channels) {
    int readSamplesCount = stb_vorbis_get_samples_short_interleaved(oggStream, channels,
        static_cast<int16_t*>(outData), samplesCount * channels);
    return readSamplesCount;
}

int OggDataStream::readF32(void* outData, int samplesCount, int channels) {
    int readSamplesCount = stb_vorbis_get_samples_float_interleaved(oggStream, channels,
        static_cast<float*>(outData), samplesCount * channels);
    return readSamplesCount;
}