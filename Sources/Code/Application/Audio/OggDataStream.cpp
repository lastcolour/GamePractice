#include "Audio/OggDataStream.hpp"
#include "ETApplicationInterfaces.hpp"
#include "Core/Buffer.hpp"

#include <stb_vorbis.c>

#include <cassert>

OggDataStream::OggDataStream(const Buffer& buff) :
    oggBuffer(buff),
    oggStream(nullptr),
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

int OggDataStream::readSamples(void* outData, int reqSamplesCount) {
    int readSamplesCount = stb_vorbis_get_samples_short_interleaved(oggStream, channels,
        static_cast<int16_t*>(outData), reqSamplesCount);
    assert(readSamplesCount >= 0);
    return readSamplesCount * channels;
}