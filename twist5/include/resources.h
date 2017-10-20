#include <string>
#include <map>
#include "anim.h"
#include <vector>

#ifdef USE_TEGEL
#include <tegel5.h>
#endif

#ifdef USE_ALSPC
#include <alspc.h>
#endif

#ifdef USE_LUA
struct lua_State;
#endif

#ifndef RESOURCES_H
#define RESOURCES_H

class Anim;
class ResourcesImpl;

struct ALLEGRO_FONT;
struct ALLEGRO_SAMPLE;
struct ALLEGRO_AUDIO_STREAM;

class Resources : public IBitmapProvider
{
private:
	ResourcesImpl* pImpl;
public:
	Resources();

	/** adds all files that match glob pattern.
	 * returns 0 on failure. If there is a failure, you can examine the error message with getErrorMsg() */
	int addFiles(const char *pattern);
	int addDir(const char *dir);


	Anim *getAnim (const std::string &id);
	Anim *getAnimIfExists (const std::string &id);
	void putAnim (const std::string &id, Anim *val);
	const std::map<std::string, Anim*> &getAnims() const;
	const char *getErrorMsg() const;

	ALLEGRO_FONT *getFont (const std::string &id);

	virtual void putBitmap(const std::string &id, ALLEGRO_BITMAP *bmp);
	virtual ALLEGRO_BITMAP *getBitmap (const std::string &id);

	ALLEGRO_BITMAP *getBitmapIfExists(const std::string &id);
	ALLEGRO_SAMPLE *getSample (const std::string &id);
	ALLEGRO_SAMPLE *getSampleIfExists (const std::string &id);
#ifdef USE_TEGEL
	int addMapFile(const std::string &filename, const std::string &tilelist);
	int addMapFile(const std::string &id, const std::string &filename, const std::string &tilesname);
	int addMapFiles(const std::string &glob, const std::string &tilelist);

	TEG_MAP *getMap (const std::string &id);
	TEG_TILELIST *getTilelist (const std::string &id);
#endif
	//TODO: rename to getMusic
	ALLEGRO_AUDIO_STREAM *getMusic (const std::string &id);
#ifdef USE_ALSPC
	ALSPC_DATA *getSPC (const std::string &id);
#endif
#ifdef USE_LUA
	void setLuaState (lua_State *val);
#endif
#ifdef USE_DIALOG
	xdom::DomNode &getDialog (const std::string &id);
#endif
	virtual ~Resources();
};

#endif
