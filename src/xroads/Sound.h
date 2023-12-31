#pragma once

namespace Xroads
{
    struct Sound
    {
        void Init();
        void Quit();
        void LoadBuffer(const SmallString<32>& name, std::vector<short>& data);
        void Play(const SmallString<32>& name, float volume=1.0f, float pan=0.0f, float speed=1.0f);
        void PlayMusic(const SmallString<32>& name); //does looped. if old music is still playing, stop it first.
        void StopMusic();
        bool GetMusicState();
        void ToggleMusicState();

        void ChangeMusicSet(const SmallString<32>& name);

        void SetSoundVolume(int n);
        void SetMusicVolume(int n);

        void Load(const SmallString<32>& name);
        void LoadMusic(const SmallString<32>& name);
        void LoadIMF(const SmallString<32>& name);
        //void LoadIMFDeferPlay(const SmallString<32>& name);
        void CheckDefer(double time_delta);
    };
}
