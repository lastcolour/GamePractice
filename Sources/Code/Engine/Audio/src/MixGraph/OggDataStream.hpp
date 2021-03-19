#ifndef __OGG_DATA_STREAM_HPP__
#define __OGG_DATA_STREAM_HPP__

class LinearResampler;

struct stb_vorbis;

class OggDataStream {
public:

    OggDataStream();
    ~OggDataStream();

    void setSampleOffset(int sampleOffset);

    bool open(Buffer& buffer);
    void close();
    bool isOpened() const;

    int readI16(void* out, int channels, int samples, bool looped);
    int readF32(void* out, int channels, int samples, bool looped);

    int getChannels() const;
    int getSampleRate() const;
    int getTotalSamples() const;

    LinearResampler* getResampler();
    void setResampler(LinearResampler* newResampler);

private:

    struct ProxyStbAllocData {
        char* ptr;
        int size;
    };

private:

    Buffer oggBuffer;
    Buffer sbtAllocData;
    ProxyStbAllocData allocDataDescr;
    stb_vorbis* oggStream;
    std::unique_ptr<LinearResampler> resampler;
    int oggChannels;
    int oggSampleRate;
    int oggSampleCount;
};

#endif /* __OGG_DATA_STREAM_HPP__ */