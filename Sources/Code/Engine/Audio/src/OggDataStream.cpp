#include "OggDataStream.hpp"
#include "Core/Buffer.hpp"

#include <stb_vorbis.c>

#include <cassert>

OggDataStream::OggDataStream() :
    oggBuffer(),
    oggStream(nullptr),
    oggChannels(0),
    oggSampleRate(0),
    oggSampleCount(0) {
}

OggDataStream::~OggDataStream() {
    close();
}

bool OggDataStream::open(Buffer& buffer) {
    assert(oggStream == nullptr && "Reopen OGG data stream");
    oggBuffer = buffer;
    oggStream = stb_vorbis_open_memory(
        static_cast<const unsigned char*>(oggBuffer.getReadData()),
            static_cast<int>(oggBuffer.getSize()), nullptr, nullptr);

    if(!oggStream) {
        return false;
    }

    stb_vorbis_info orbisInfo = stb_vorbis_get_info(oggStream);
    oggChannels = orbisInfo.channels;
    oggSampleRate = orbisInfo.sample_rate;
    oggSampleCount = stb_vorbis_stream_length_in_samples(oggStream);
    return true;
}

void OggDataStream::close() {
    if(!oggStream) {
        return;
    }
    stb_vorbis_close(oggStream);
    oggStream = nullptr;
    oggBuffer.reset();
    oggChannels = 0;
    oggSampleRate = 0;
    oggSampleCount = 0;
}

bool OggDataStream::isOpened() const {
    return oggStream != nullptr;
}

void OggDataStream::setSampleOffset(int sampleOffset) {
    assert(oggStream != nullptr && "Invalid OGG stream");
    stb_vorbis_seek_start(oggStream);
    stb_vorbis_seek_frame(oggStream, sampleOffset);
}

int OggDataStream::getChannels() const {
    return oggChannels;
}

int OggDataStream::getSampleRate() const {
    return oggSampleRate;
}

int OggDataStream::readF32(void* out, int channels, int samples, bool looped) {
    assert(oggStream != nullptr && "Invalid OGG stream");
    int readCount = stb_vorbis_get_samples_float_interleaved(oggStream, channels,
        static_cast<float*>(out), samples * channels);
    if(readCount < samples && looped) {
        int leftCount = samples - readCount;
        while(leftCount > 0) {
            stb_vorbis_seek_start(oggStream);
            int offset = readCount * channels;
            readCount = stb_vorbis_get_samples_float_interleaved(oggStream, channels,
                static_cast<float*>(out) + offset, leftCount * channels);
            leftCount -= readCount;
        }
    }
    return readCount;
}

int OggDataStream::readI16(void* out, int channels, int samples, bool looped) {
    assert(oggStream != nullptr && "Invalid OGG stream");
    int readCount = stb_vorbis_get_samples_short_interleaved(oggStream, channels,
        static_cast<int16_t*>(out), samples * channels);
    if(readCount < samples && looped) {
        int leftCount = samples - readCount;
        while(leftCount > 0) {
            setSampleOffset(0);
            int offset = readCount * channels;
            readCount = stb_vorbis_get_samples_short_interleaved(oggStream, channels,
                static_cast<int16_t*>(out) + offset, leftCount * channels);
            leftCount = samples - readCount;
        }
    }
    return readCount;
}

int OggDataStream::getTotalSamples() const {
    return oggSampleCount;
}