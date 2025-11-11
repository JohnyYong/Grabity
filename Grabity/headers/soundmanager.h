#include <pch.h>
class Sound {
public:
    Sound(FMOD::System* system, const std::string& filePath);
    ~Sound();

    void play();
    void stop();
    void playLoop();

private:
    FMOD::System* system;
    FMOD::Sound* sound;
    FMOD::Channel* channel;
};

class SoundManager {
public:
    ~SoundManager();
    static SoundManager& getInstance();

    void storeSound(const std::string& filePath);
    std::vector<std::unique_ptr<Sound>>& getSounds();
    std::unique_ptr<Sound> loadSound(const std::string& filePath);

private:
    std::vector<std::unique_ptr<Sound>> sounds;
    SoundManager();
    SoundManager(const SoundManager&) = delete;
    SoundManager& operator=(const SoundManager&) = delete;
    FMOD::System* system;
};