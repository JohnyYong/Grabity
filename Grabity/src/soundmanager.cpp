#include <soundmanager.h>

// SoundManager Implementation
SoundManager& SoundManager::getInstance() {
    static SoundManager instance;
    return instance;
}

SoundManager::SoundManager() : system(nullptr) {
    FMOD_RESULT result = FMOD::System_Create(&system);
    if (result != FMOD_OK) {
        std::cerr << "FMOD error! (" << result << ") " << FMOD_ErrorString(result) << std::endl;
        throw std::runtime_error("Failed to create FMOD system");
    }

    result = system->init(512, FMOD_INIT_NORMAL, nullptr);
    if (result != FMOD_OK) {
        std::cerr << "FMOD error! (" << result << ") " << FMOD_ErrorString(result) << std::endl;
        throw std::runtime_error("Failed to initialize FMOD system");
    }
}

SoundManager::~SoundManager() {
    if (system) {
        system->close();
        system->release();
    }
}
void SoundManager::storeSound(const std::string& filePath) {
    sounds.push_back(loadSound(filePath));
}

std::vector<std::unique_ptr<Sound>>& SoundManager::getSounds() {
    return sounds;
}

std::unique_ptr<Sound> SoundManager::loadSound(const std::string& filePath) {
    return std::make_unique<Sound>(system, filePath);
}
// Sound Implementation
Sound::Sound(FMOD::System* system, const std::string& filePath)
    : system(system), sound(nullptr), channel(nullptr) {
    FMOD_RESULT result = system->createSound(filePath.c_str(), FMOD_DEFAULT, nullptr, &sound);
    if (result != FMOD_OK) {
        std::cerr << "FMOD error! (" << result << ") " << FMOD_ErrorString(result) << std::endl;
        throw std::runtime_error("Failed to create sound");
    }
}

Sound::~Sound() {
}

void Sound::play() {
    system->update();
    FMOD_RESULT result = system->playSound(sound, nullptr, false, &channel);
    if (result != FMOD_OK) {
        std::cerr << "FMOD error! (" << result << ") " << FMOD_ErrorString(result) << std::endl;
    }

    if (channel) {
        channel->setUserData(this);
    }
}

void Sound::playLoop() {
    system->update();
    FMOD_RESULT result = system->playSound(sound, nullptr, false, &channel);
    if (result != FMOD_OK) {
        std::cerr << "FMOD error! (" << result << ") " << FMOD_ErrorString(result) << std::endl;
    }

    if (channel) {
        channel->setMode(FMOD_LOOP_NORMAL);
        channel->setUserData(this);
    }
}

void Sound::stop() {
    if (channel) {
        channel->stop();
    }
}
