#include "Audio/OggDataStream.hpp"
#include "ETApplicationInterfaces.hpp"
#include "Core/Buffer.hpp"

#include <stb_vorbis.c>

OggDataStream::OggDataStream(const Buffer& buff) :
    oggStream(nullptr),
    channels(0),
    sampleRate(0),
    numSamples(0) {

    oggStream = stb_vorbis_open_memory(
        static_cast<const unsigned char*>(buff.getReadData()), 
            static_cast<int>(buff.getSize()), nullptr, nullptr);

    if(!oggStream) {
        return;
    }

    stb_vorbis_info orbisInfo = stb_vorbis_get_info(oggStream);
    channels = orbisInfo.channels;
    sampleRate = orbisInfo.sample_rate;
    numSamples = static_cast<std::size_t>(stb_vorbis_stream_length_in_samples(oggStream) * orbisInfo.channels);
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