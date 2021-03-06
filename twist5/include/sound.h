#ifndef SOUND_H_
#define SOUND_H_

#ifdef USE_ALSPC
struct ALSPC_DATA;
struct ALSPC_PLAYER;
#endif

struct ALLEGRO_SAMPLE;
struct ALLEGRO_CONFIG;
struct ALLEGRO_VOICE;
struct ALLEGRO_AUDIO_STREAM;
struct ALLEGRO_MIXER;

class Resources;

/**
 * Global sound manager
 */
class Sound
{
protected:
	void getSoundFromConfig(ALLEGRO_CONFIG *config);
	void initSound();
#ifdef USE_ALSPC
	void pollMusic();
#endif
private:
	bool inited;
	void propertyChanged ();
	bool soundInstalled;
	bool fSound;
	bool fMusic;
	ALLEGRO_VOICE *voice;
	ALLEGRO_AUDIO_STREAM *currentMusic;
	ALLEGRO_MIXER *mixer;
#ifdef USE_ALSPC
    bool stereo;
    bool hifi;
    ALSPC_DATA *currentMusic;
    ALSPC_PLAYER *alspc_player;
#endif
public:
	Sound();
	bool isSoundOn() { return fSound; }
	bool isSoundInstalled() { return soundInstalled; }
	void setSoundInstalled(bool value) { soundInstalled = value; }
	void playSample (ALLEGRO_SAMPLE *s);
	bool isMusicOn() { return fMusic; }
	void setSoundOn(bool value);
	void setMusicOn(bool value);
	void doneSound();
	void playMusic (ALLEGRO_AUDIO_STREAM *duh, float volume = 1.0f);
	void stopMusic ();
#ifdef USE_ALSPC
	void playMusic (ALSPC_DATA *alspc_data);
	void stopMusic ();
    bool isStereoOn() { return stereo; }
	void setStereoOn (bool value);
    bool isHifiOn() { return hifi; }
	void setHifiOn (bool value);
#endif
};

#endif
