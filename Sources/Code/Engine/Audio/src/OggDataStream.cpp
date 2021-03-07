#include "OggDataStream.hpp"
#include "MixGraph/LinearResampler.hpp"
#include "Core/ETLogger.hpp"

#include <stb_vorbis.c>

#include <cassert>

namespace {

const size_t STB_INNER_BUFFER_SIZE = 1 << 18;

const char* getVorbisErrorText(int error) {
    const char* errorText = "Unknown";
    switch(error) {
        case VORBIS__no_error: {
            errorText = "no_error";
            break;
        }
        case VORBIS_need_more_data: {
            errorText = "need_more_data";
            break;
        }
        case VORBIS_invalid_api_mixing: {
            errorText = "invalid_api_mixing";
            break;
        }
        case VORBIS_outofmem: {
            errorText = "outofmem";
            break;
        }
        case VORBIS_feature_not_supported: {
            errorText = "feature_not_supported";
            break;
        }
        case VORBIS_too_many_channels: {
            errorText = "no_error";
            break;
        }
        case VORBIS_file_open_failure: {
            errorText = "file_open_failure";
            break;
        }
        case VORBIS_seek_without_length: {
            errorText = "seek_without_length";
            break;
        }
        case VORBIS_unexpected_eof: {
            errorText = "unexpected_eof";
            break;
        }
        case VORBIS_seek_invalid: {
            errorText = "seek_invalid";
            break;
        }
        case VORBIS_invalid_setup: {
            errorText = "invalid_setup";
            break;
        }
        case VORBIS_invalid_stream: {
            errorText = "invalid_stream";
            break;
        }
        case VORBIS_missing_capture_pattern: {
            errorText = "missing_capture_pattern";
            break;
        }
        case VORBIS_invalid_stream_structure_version: {
            errorText = "invalid_stream_structure_version";
            break;
        }
        case VORBIS_continued_packet_flag_invalid: {
            errorText = "continued_packet_flag_invalid";
            break;
        }
        case VORBIS_incorrect_stream_serial_number: {
            errorText = "incorrect_stream_serial_number";
            break;
        }
        case VORBIS_invalid_first_page: {
            errorText = "invalid_first_page";
            break;
        }
        case VORBIS_bad_packet_type: {
            errorText = "bad_packet_type";
            break;
        }
        case VORBIS_cant_find_last_page: {
            errorText = "cant_find_last_page";
            break;
        }
        case VORBIS_seek_failed: {
            errorText = "seek_failed";
            break;
        }
        case VORBIS_ogg_skeleton_not_supported: {
            errorText = "ogg_skeleton_not_supported";
            break;
        }
        default: {
            break;
        }
    }
    return errorText;
}

} // namespace

OggDataStream::OggDataStream() :
    oggBuffer(),
    oggStream(nullptr),
    oggChannels(0),
    oggSampleRate(0),
    oggSampleCount(0) {

    sbtAllocData.resize(STB_INNER_BUFFER_SIZE);
    allocDataDescr.ptr = reinterpret_cast<char*>(sbtAllocData.getWriteData());
    allocDataDescr.size = sbtAllocData.getSize(); 
}

OggDataStream::~OggDataStream() {
    close();
}

bool OggDataStream::open(Buffer& buffer) {
    assert(oggStream == nullptr && "Reopen OGG data stream");
    oggBuffer = buffer;

    int error = VORBIS__no_error;

    oggStream = stb_vorbis_open_memory(
        static_cast<const unsigned char*>(oggBuffer.getReadData()),
            static_cast<int>(oggBuffer.getSize()), &error,
                reinterpret_cast<stb_vorbis_alloc*>(&allocDataDescr));

    if(error != VORBIS__no_error) {
        LogError("[OggDataStream::open] Open ended with error: '%s'", getVorbisErrorText(error));
    }

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

LinearResampler* OggDataStream::getResampler() {
    return resampler.get();
}

void OggDataStream::setResampler(LinearResampler* newResampler) {
    resampler.reset(newResampler);
}