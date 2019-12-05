// SVE (Simple Vulkan Engine) Library
// Copyright (c) 2018-2019, Igor Barinov
// Licensed under the MIT License
#include "SoundSystem.h"
#include <cstdio>
#include <AL/al.h>
#include <AL/alc.h>
#include <vorbis/vorbisfile.h>
#include <cstdint>
#include <string>
#include <iostream>
#include <vector>

namespace Chewman
{

namespace
{

class OpenALProcessor
{
public:
    ~OpenALProcessor()
    {
        if (_device)
        {
            alcMakeContextCurrent(nullptr);
            alcDestroyContext(_context);
            alcCloseDevice(_device);
            _device = nullptr;
        }
    }

    static OpenALProcessor* getInstance()
    {
        if (_instance == nullptr)
        {
            _instance = new OpenALProcessor();
        }
        return _instance;
    }

    bool isActive() const
    {
        return _device != nullptr;
    }

    uint32_t registerSound(const std::string& sound, bool looped = false)
    {
        // Ogg loading code is based on http://github.com/tilkinsc

        ALenum error = 0;
        ALuint buffer = 0;
        FILE* fp = nullptr;
        OggVorbis_File vf {};
        vorbis_info * vi = nullptr;
        ALenum format = 0;
        std::vector<int16_t> bufferData;

        // open the file in read binary mode
        // TODO: use filesystem file read
        fp = fopen(sound.c_str(), "rb");
        if(fp == nullptr)
        {
            std::cerr << "Can't open sound file " << sound << std::endl;
            return -1;
        }

        alGenBuffers(1, &buffer);

        if(ov_open_callbacks(fp, &vf, NULL, 0, OV_CALLBACKS_NOCLOSE) < 0)
        {
            std::cerr << "Ogg sound file is not valid: " << sound << std::endl;
            fclose(fp);
            return -1;
        }

        vi = ov_info(&vf, -1);
        format = vi->channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;

        size_t data_len = ov_pcm_total(&vf, -1) * vi->channels * 2;
        bufferData.resize(data_len);

        for (size_t size = 0, offset = 0, sel = 0;
             (size = ov_read(&vf, (char*) bufferData.data() + offset, 4096, 0, sizeof(int16_t), 1, (int*) &sel)) != 0;
             offset += size) {
            if(size < 0)
            {
                std::cerr << "Incorrect ogg file: " << sound << std::endl;
                fclose(fp);
                ov_clear(&vf);
                return -1;
            }
        }

        alBufferData(buffer, format, bufferData.data(), data_len, vi->rate);
        fclose(fp);
        ov_clear(&vf);

        ALuint source;
        alGenSources((ALuint)1, &source);
        alSourcef(source, AL_PITCH, 1);
        alSourcef(source, AL_GAIN, 1);
        alSource3f(source, AL_POSITION, 0, 0, 0);
        alSource3f(source, AL_VELOCITY, 0, 0, 0);
        alSourcei(source, AL_LOOPING, looped ? AL_TRUE : AL_FALSE);
        alSourcei(source, AL_BUFFER, buffer);

        return source;
    }

    void playSound(uint32_t id)
    {
        alSourcePlay(id);
    }

    void stopSound(uint32_t id)
    {
        alSourceStop(id);
    }

private:
    static OpenALProcessor* _instance;

    OpenALProcessor()
    {
        _device = alcOpenDevice(nullptr);
        if (!_device) {
            return;
        }

        _context = alcCreateContext(_device, nullptr);
        if (!alcMakeContextCurrent(_context))
        {
            alcCloseDevice(_device);
            _device = nullptr;
        }
    }

    ALCdevice* _device;
    ALCcontext* _context;
};

OpenALProcessor* OpenALProcessor::_instance = nullptr;

class OpenALSound : public Sound
{
public:
    OpenALSound(uint32_t id)
    {
        _id = id;
    }

    void Play() override
    {
        OpenALProcessor::getInstance()->playSound(_id);
    }

    void Stop() override
    {
        OpenALProcessor::getInstance()->stopSound(_id);
    }

private:
    uint32_t _id;
};


} // anon namespace

SoundSystem* SoundSystem::_instance = nullptr;

SoundSystem::SoundSystem() = default;
SoundSystem::~SoundSystem() = default;

SoundSystem* SoundSystem::getInstance()
{
    if (_instance == nullptr)
    {
        _instance = new SoundSystem();
    }
    return _instance;
}

bool SoundSystem::isLoaded() const
{
    return OpenALProcessor::getInstance()->isActive();
}

std::shared_ptr<Sound> SoundSystem::createSound(const std::string& filename, bool loop)
{
    return std::make_shared<OpenALSound>(OpenALProcessor::getInstance()->registerSound(filename, loop));
}

void SoundSystem::initBackgroundMusic(const std::string& filename)
{
    _bgm = createSound(filename, true);
}

void SoundSystem::startBackgroundMusic()
{
    if (_bgm != nullptr)
        _bgm->Play();
}

void SoundSystem::stopBackgroundMusic()
{
    if (_bgm != nullptr)
        _bgm->Stop();
}

} // namespace Chewman