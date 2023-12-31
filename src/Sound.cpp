#include "xroads/Xroads.h"

#include "soloud.h"
#include "soloud_wav.h"

namespace Xroads
{
    const int MAX_SOUNDS = 1000;

    std::vector<SoLoud::Wav> waves;
    std::vector<std::vector<short>> datas;
    Xr::VectorMap<Sm32, size_t> sounds;
    SoLoud::Soloud soloud;

    float soundvolume = 0.6f, musicvolume = 1.0f;

    const std::vector<Sm32> musics_menu{{"rule001"_sm}};
    const std::vector<Sm32> musics_ingame{{"rule002"_sm}};
    std::vector<Sm32> musics = musics_menu;
    const double WAITING_TIME = 0.5;
    double curr_waiting_time = WAITING_TIME;
    int current_music_id = -1;
    Sm32 current_set = "menu"_sm;

    struct CurrentMusic
    {
    private:
        bool on=true;
    public:
        void SetMusic(bool val)
        {
            GetEngine().config.SetValue<bool>("music",val);
            on = val;
            //std::cout << "SETTING MUSIC " << val << std::endl;
        }
        bool GetMusic()
        {
            return on;
        }
        void UpdateVolume()
        {
            if (soloud_handle != -1)
            {
                soloud.setVolume(soloud_handle, musicvolume);
            }
        }
        int soloud_handle=0;
        Sm32 name = ""_sm;

    } current_music;

    bool Sound::GetMusicState()
    {
        return current_music.GetMusic();
    }

    void Sound::ToggleMusicState()
    {
        current_music.SetMusic(!current_music.GetMusic());

        if (current_music.GetMusic())
            PlayMusic(current_music.name);
        else
            StopMusic();
    }

    void Sound::Load(const Xr::SmallString<32>& name)
    {
        if (waves.size() >= MAX_SOUNDS)
            return;
        waves.push_back(SoLoud::Wav());
        datas.push_back(std::vector<short>());
        std::string filename = std::string("sounds/")+name.to_string()+".wav";
        waves.back().load(filename.c_str());
        waves.back().setInaudibleBehavior(false,true);
        waves.back().setSingleInstance(0);
        sounds[name] = waves.size()-1;
    }

    void Sound::LoadMusic(const Xr::SmallString<32>& name)
    {
        if (waves.size() >= MAX_SOUNDS)
            return;
        waves.push_back(SoLoud::Wav());
        datas.push_back(std::vector<short>());
        std::string filename = std::string("music/")+name.to_string()+".ogg";
        waves.back().load(filename.c_str());
        sounds[name] = waves.size()-1;
    }

    void Sound::Play(const Xr::SmallString<32>& name, float volume, float pan, float speed)
    {
        auto it = sounds.find(name);
        if (it == sounds.end())
        {
            Sound::Load(name);
            it = sounds.find(name);
        }
        if (it == sounds.end())
            return; //failed.

        if (speed == 1.0f)
            soloud.play(waves[it->second], volume*soundvolume, pan);
        else
        {
            int h = soloud.play(waves[it->second], volume*soundvolume, pan, true);
            soloud.setRelativePlaySpeed(h, speed); // change a parameter
            soloud.setPause(h, 0);
        }
    }

    void Sound::SetSoundVolume(int n)
    {
        soundvolume = float(n)/100.0f;
    }

    void Sound::SetMusicVolume(int n)
    {
        musicvolume = float(n)/100.0f;
        current_music.UpdateVolume();
    }

    void Sound::PlayMusic(const Xr::SmallString<32>& name)
    {
        current_music.name = name;
        if (!current_music.GetMusic())
            return;
        auto it = sounds.find(name);
        if (it == sounds.end())
            return;
        StopMusic();
        waves[it->second].setLooping(1);
        current_music.soloud_handle = soloud.play(waves[it->second], musicvolume);
        soloud.setProtectVoice(current_music.soloud_handle,true);
        curr_waiting_time = WAITING_TIME;
    }

    void Sound::StopMusic()
    {
        soloud.stop(current_music.soloud_handle);
        current_music.soloud_handle = 0;
    }

    void Sound::LoadBuffer(const Xr::SmallString<32>& name, std::vector<short>& data)
    {
        if (waves.size() >= MAX_SOUNDS)
            return;
        waves.push_back(SoLoud::Wav());
        datas.push_back(data);
        waves.back().loadRawWave16(datas.back().data(), datas.back().size(),14318160.0/288.0);
        sounds[name] = waves.size()-1;
    }

    void Sound::Init()
    {
        //std::cout << "Reserve sounds." << std::endl;
        waves.reserve(MAX_SOUNDS);
        datas.reserve(MAX_SOUNDS);
        //std::cout << "Done." << std::endl;
        if (soloud.init(SoLoud::Soloud::CLIP_ROUNDOFF, SoLoud::Soloud::WASAPI, SoLoud::Soloud::AUTO, 2048, 2) != 0)
        {
            if (soloud.init(SoLoud::Soloud::CLIP_ROUNDOFF, SoLoud::Soloud::WINMM, SoLoud::Soloud::AUTO, 2048, 2) != 0)
            {
                if (soloud.init(SoLoud::Soloud::CLIP_ROUNDOFF, SoLoud::Soloud::WINMM, SoLoud::Soloud::AUTO, 4096, 2) != 0)
                {
                    if (soloud.init(SoLoud::Soloud::CLIP_ROUNDOFF, SoLoud::Soloud::NULLDRIVER, SoLoud::Soloud::AUTO, SoLoud::Soloud::AUTO, 2) != 0)
                    {
                        Xr::Kill("No sound device found, and not even the null driver works.");
                    }
                }
            }
        }
        //cout << "Audio: " << soloud.mBackendString << endl;
        std::cout << "Samplerate: " << soloud.mSamplerate << std::endl;
        current_music.SetMusic(true);
    }

    #include <cstdio>

    void Sound::CheckDefer(double time_delta)
    {
        if (musics.size() > 0)
        {
            if (current_music.soloud_handle == -1 || !soloud.isValidVoiceHandle(current_music.soloud_handle))
            {
                curr_waiting_time -= time_delta;
                if (curr_waiting_time <= 0)
                {
                    //cout << "music pos: " <<  << endl;
                    current_music_id = (current_music_id+1)%musics.size();
                    PlayMusic(musics[current_music_id]);
                }
            }
        }
    }

    void Sound::Quit()
    {
        for ([[maybe_unused]] auto it : sounds)
            ;//TODO: delete the sounds neatly and nicely
        sounds.clear();
        soloud.deinit();
    }

    void Sound::ChangeMusicSet(const Xr::SmallString<32>& name)
    {
        if (current_set == name)
            return;
        current_set = name;
        if (name == "menu"_sm)
            musics = musics_menu;
        if (name == "ingame"_sm)
            musics = musics_ingame;
        StopMusic();
        curr_waiting_time = WAITING_TIME;
        current_music_id = 0;
    }
}
