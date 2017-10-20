#include <assert.h>
#include "resources.h"
#include "color.h"
#include "dom.h"
#include "util.h"
#include "strutil.h"
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_font.h>
#include "mainloop.h"

#ifdef USE_LUA
extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}
#endif

using namespace std;
using namespace xdom;

class ResourcesImpl
{
private:
	char errorMsg[1024];
	std::map <std::string, ALLEGRO_BITMAP*> sprites; // seperately loaded bitmaps
	std::map <std::string, Anim*> animlist;
	std::map <std::string, ALLEGRO_FONT*> fonts;
#ifdef USE_TEGEL
	std::map <std::string, TEG_MAP*> maps; // seperately loaded maps
	std::map <std::string, TEG_TILELIST*> tilelists;
#endif
	std::map <std::string, ALLEGRO_SAMPLE *> samples;
	std::map <std::string, ALLEGRO_AUDIO_STREAM*> duhlist;
#ifdef USE_DIALOG
	std::map <std::string, DomNode> dialogs;
#endif
#ifdef USE_LUA
	lua_State *L;
#endif
#ifdef USE_DIALOGS
	std::map<std::string, xdom::DomNode> dialogs;
#endif
	Resources &parent;
public:
	ResourcesImpl(Resources &_parent) : parent(_parent) {}

	Anim *getAnim (const std::string &id);
	void putAnim (const std::string &id, Anim *val);
	const std::map<std::string, Anim*> &getAnims() const;

	ALLEGRO_SAMPLE *getSampleIfExists(const std::string &id)
	{
		if (samples.find(id) == samples.end())
			return NULL;
		else
			return samples[id];
	}

	ALLEGRO_BITMAP *getBitmapIfExists(const std::string &id) { if (sprites.find(id) == sprites.end()) return NULL; else return sprites[id]; }

	void putBitmap(const string &id, ALLEGRO_BITMAP *bmp)
	{
		sprites.insert (pair<string, ALLEGRO_BITMAP *>(id, bmp));
	}

	ALLEGRO_SAMPLE *getSample (const std::string &id);
	const char *getErrorMsg() const;
#ifdef USE_TEGEL
	TEG_MAP *getMap (const std::string &id);
	TEG_TILELIST *getTilelist (const std::string &id);
#endif
#ifdef USE_ALSPC
	ALSPC_DATA *getSPC (const std::string &id);
#endif
#ifdef USE_LUA
	void setLuaState (lua_State *val);
#endif
#ifdef USE_DIALOG
	DomNode &getDialog (const string &id)
	{
		assert (dialogs.find (id) != dialogs.end());
		return dialogs[id];
	}
#endif
	// TODO: utility function?
	void split_path (const string &in, string &basename, string &extension)
	{
		size_t start = in.find_last_of ("\\/");
		if (start == string::npos) start = 0;
		else start += 1;

		size_t end = in.find_last_of ('.');
		if (end == string::npos) end = in.length();

		extension = in.substr (end + 1);
		basename = in.substr(start, end-start);

	}

	// returns 0 on failure, and sets error message.
	int addSingleFile(const string *i)
	{
		string basename;
		string extension;

		split_path (*i, basename, extension);

		// must have an extension, to be able to recognize file type.
		//TODO: error message?
		if (extension == "") return 0;

#ifdef USE_TEGEL
		else if (equalsIgnoreCase (extension, "tll"))
		{
			TEG_TILELIST *temp = teg_loadtiles(i->c_str());
			if (!temp)
			{
				snprintf(errorMsg, sizeof(errorMsg), "error load TLL %s with error %s", i->c_str(), teg_error);
				return 0;
			}
			else
			{
				tilelists[basename] = temp;
			}
		}
#endif
		else if (equalsIgnoreCase (extension, "ttf"))
		{
			// generate fonts with sizes 8, 16, 24, etc. to 48
			for (int k = 8; k < 48; k += 8)
			{
				ALLEGRO_FONT *temp;
				if (!(temp = al_load_font(i->c_str(), k, 0)))
				{
					snprintf(errorMsg, sizeof(errorMsg), "error load AlFont %s", i->c_str());
					return 0;
				} else {
					char namebuf[128];
					snprintf(namebuf, sizeof(namebuf), "%s_%i", basename.c_str(), k);
					string name = string(namebuf);
					//temp->setFontSize(k); //TODO
					fonts[name] = temp;
				}
			}
		}
		else if (equalsIgnoreCase (extension, "tga"))
		{
			ALLEGRO_FONT *temp;
			if (!(temp = al_load_bitmap_font(i->c_str())))
			{
				snprintf(errorMsg, sizeof(errorMsg), "error load AlFont %s", i->c_str());
				return 0;
			} else {
				fonts[basename] = temp;
			}
		}
		else if (equalsIgnoreCase (extension, "mod") || equalsIgnoreCase (extension, "xm"))
		{
			ALLEGRO_AUDIO_STREAM *temp;
			if (!(temp = al_load_audio_stream (i->c_str(), 4, 2048))) //TODO: correct values for al_load_audio_stream
			{
				snprintf(errorMsg, sizeof(errorMsg), "error loading DUMB %s", i->c_str());
				return 0;
			}
			else {
				al_set_audio_stream_playmode(temp, ALLEGRO_PLAYMODE_LOOP );
				assert (duhlist.find(basename) == duhlist.end()); // fails if you overload the same id.
				duhlist[basename] = temp;
			}
		}
		else if (equalsIgnoreCase  (extension, "ogg"))
		{
           ALLEGRO_SAMPLE *sample_data = NULL;
           sample_data = al_load_sample(i->c_str());
           if (!sample_data)
           {
        	   log ("error loading OGG %s", i->c_str());
        	    //TODO: write to log but don't quit. Sound is not essential.
//				snprintf(errorMsg, sizeof(errorMsg), "error loading OGG %s", i->c_str());
//				return 0;
           }
           else
           {
        	   samples[basename] = sample_data;
           }
		}
	#ifdef USE_LUA
		else if (equalsIgnoreCase (extension, "lua"))
		{
			assert (L != NULL);
			int result = luaL_loadfile (L, i->c_str());
			if (result == LUA_ERRFILE)
			{
				snprintf(errorMsg, sizeof(errorMsg), "error reading lua script %s", i->c_str());
			}
		}
	#endif
		else if (equalsIgnoreCase (extension, "bmp") || equalsIgnoreCase (extension, "png"))
		{
			ALLEGRO_BITMAP *bmp;
			bmp = al_load_bitmap (i->c_str());
			if (!bmp)
			{
				snprintf(errorMsg, sizeof(errorMsg), "error loading BMP/PNG %s", i->c_str());
				return 0;
			}
			else
			{
				assert (al_get_bitmap_width(bmp) > 0);
				assert (al_get_bitmap_height(bmp) > 0);
				sprites[basename] = bmp;
			}
		}
		else if (equalsIgnoreCase (extension, "wav"))
		{
			ALLEGRO_SAMPLE *wav;
			wav = al_load_sample (i->c_str());
			if (!wav)
			{
				snprintf(errorMsg, sizeof(errorMsg), "error loading WAV %s", i->c_str());
				return 0;
			}
			else
			{
				samples[basename] = wav;
			}
		}
		else if (equalsIgnoreCase (extension, "xml"))
		{
			DomNode node = xmlParseFile(*i);

			vector<DomNode>::iterator n;
			for (n = node.children.begin(); n != node.children.end(); ++n)
			{
				if (n->name == "anim")
					Anim::loadFromXml (*n, &parent, animlist);

#ifdef USE_DIALOG
				if (n->name == "dialog")
				{
					string name = n->attributes["name"];
					dialogs.insert (pair <string, DomNode>(name, (*n)));
				}
#endif

			}
		}

		return 1;
	}

	int addDir(const char *dir)
	{
		//TODO: use listDir function from util.cpp

		ALLEGRO_FS_ENTRY *entry = al_create_fs_entry(dir);

		if (al_get_fs_entry_mode(entry) & ALLEGRO_FILEMODE_ISDIR) {
			ALLEGRO_FS_ENTRY *next;

			al_open_directory(entry);

			while (1) {
				next = al_read_directory(entry);
				if (!next)
					break;

				string name(al_get_fs_entry_name(next));
				addSingleFile (&name);

				al_destroy_fs_entry(next);
			}
			al_close_directory(entry);
		}

		al_destroy_fs_entry(entry);

		return 0;
	}

	int addFiles(const char *pattern)
	{
		vector<string> filenames;
		glob (pattern, filenames);
		for (vector<string>::iterator i = filenames.begin();
				i != filenames.end(); ++i)
		{
			int result = addSingleFile (&(*i));
			if (result == 0) return 0;
		}
		return 1;
	}

	//TODO: rename to getMusic
	ALLEGRO_AUDIO_STREAM *getMusic (const string &id)
	{
	    if (duhlist.find (id) == duhlist.end())
	    {
			allegro_message ("Couldn't find DUH (Music) '%s'", id.c_str());
			exit(1);
	    }
	    return duhlist[id];
	}

	~ResourcesImpl()
	{
		{
			map<string, ALLEGRO_BITMAP*>::iterator i;
			for (i = sprites.begin(); i != sprites.end(); i++)
			{
				al_destroy_bitmap (i->second);
			}
		}
		{
			map<string, Anim*>::iterator i;
			for (i = animlist.begin(); i != animlist.end(); i++)
			{
				delete (i->second);
			}
		}
		{
			map<string, ALLEGRO_AUDIO_STREAM*>::iterator i;
			for (i = duhlist.begin(); i != duhlist.end(); i++)
			{
				al_destroy_audio_stream(i->second);
			}
		}
	#ifdef USE_TEGEL
			{
				map<string, TEG_MAP*>::iterator i;
				for (i = maps.begin(); i != maps.end(); i++)
				{
					teg_destroymap (i->second);
				}
			}
			{
				map<string, TEG_TILELIST*>::iterator i;
				for (i = tilelists.begin(); i != tilelists.end(); i++)
				{
					teg_destroytiles (i->second);
				}
			}
	#endif
			{
				map<string, ALLEGRO_SAMPLE *>::iterator i;
				for (i = samples.begin(); i != samples.end(); i++)
				{
					al_destroy_sample (i->second);
				}
			}

	}

	ALLEGRO_BITMAP *getBitmap (const string &id)
	{
		if (sprites.find (id) == sprites.end())
		{
			allegro_message ("Couldn't find Bitmap sprite '%s'", id.c_str());
			exit(1);
		}

		return sprites[id];
	}

	ALLEGRO_FONT *getFont (const string &id)
	{
		if (fonts.find (id) == fonts.end())
		{
			// auto-vivicate fixed font...
			if (id == "builtin_font")
			{
				ALLEGRO_FONT* ff = al_create_builtin_font();
				fonts["builtin_font"] = ff;
				return ff;
			}
			else
			{
				allegro_message ("Couldn't find FONT '%s'", id.c_str());
				exit(1);
			}
		}
		else
			return fonts[id];
	}

#ifdef USE_TEGEL

	int addMapFile(const string &filename, const string &tilesname)
	{
		TEG_TILELIST *temptiles = getTilelist (tilesname);
		if (!temptiles)
		{
			snprintf(errorMsg, sizeof(errorMsg), "Could not find tiles named [%s]", tilesname.c_str());
			return 0;
		}

		return addMapFile(filename, temptiles);
	}

	int addMapFile(const string &filename, TEG_TILELIST *tiles)
	{
		string basename;
		string extension;

		split_path (filename, basename, extension);

		TEG_MAP *tempmap;
		if (!(tempmap = teg_loadmap (filename.c_str(), tiles)))
		{
			snprintf(errorMsg, sizeof(errorMsg), "Error while loading map: [%s]", teg_error);
			return 0;
		}
		else
		{
			assert (tempmap->tilelist == tiles);
	//		maps.push_back (tempmap);
			maps[basename] = tempmap;
		}
		return 1;
	}

	int addMapFile(const string &id, const string &filename, const string &tilesname)
	{
		TEG_MAP *tempmap;
		TEG_TILELIST *temptiles = getTilelist (tilesname);
		if (!temptiles)
		{
			snprintf(errorMsg, sizeof(errorMsg), "Could not find tiles named [%s]", tilesname.c_str());
			return 0;
		}

		assert (maps.find(id) == maps.end()); // trying to load same map twice
		if (!(tempmap = teg_loadmap (filename.c_str(), temptiles)))
		{
			snprintf(errorMsg, sizeof(errorMsg), "Error while loading map: [%s]", teg_error);
			return 0;
		}
		else
		{
			assert (tempmap->tilelist == temptiles);
	//		maps.push_back (tempmap);
			maps[id] = tempmap;
		}
		return 1;
	}

	int addMapFiles(const string &pattern, const string &tilesname)
	{
		TEG_TILELIST *temptiles = getTilelist (tilesname);
		if (!temptiles)
		{
			snprintf(errorMsg, sizeof(errorMsg), "Could not find tiles named [%s]", tilesname.c_str());
			return 0;
		}

		vector<string> filenames;
		glob (pattern, filenames);
		for (vector<string>::iterator i = filenames.begin();
				i != filenames.end(); ++i)
		{
			int result = addMapFile (*i, temptiles);
			if (result == 0) return 0;
		}
		return 1;
	}
#endif

	Anim *getAnimIfExists (const string &id)
	{
		if (animlist.find(id) == animlist.end()) return nullptr; else return animlist[id];
	}

};

Resources::Resources() : pImpl (new ResourcesImpl(*this)) {}

int Resources::addFiles(const char *pattern)
{
	return pImpl->addFiles(pattern);
}

ALLEGRO_BITMAP *Resources::getBitmap (const string &id)
{
	return pImpl->getBitmap (id);
}

ALLEGRO_FONT *Resources::getFont (const string &id)
{
	return pImpl->getFont(id);
}

ALLEGRO_SAMPLE *Resources::getSample (const string &id)
{
	return pImpl->getSample(id);
}

ALLEGRO_SAMPLE *ResourcesImpl::getSample (const string &id)
{
	if (samples.find (id) == samples.end())
	{
		allegro_message ("Couldn't find SAMPLE '%s'", id.c_str());
		exit(1);
	}

	return samples[id];
}
#ifdef USE_ALSPC

ALSPC_DATA *Resources::getSPC (const string &id)
{
	return pImpl->getSPC(id);
}

ALSPC_DATA *ResourcesImpl::getSPC (const string &id)
{
	//TODO
//	vector<DATAFILE*>::const_iterator i;
//	DATAFILE *temp = NULL;
//	for (i = data.begin(); i != data.end() && temp == NULL; ++i)
//	{
//		assert (*i);
//		temp = find_datafile_object (*i, id.c_str());
//		// check if object is of the right type
//		if (temp && temp->type != DAT_ALSPC)
//			temp = NULL;
//	}
//    if (temp)
//    	return (ALSPC_DATA*)temp->dat;
//	else
//	{
		allegro_message ("Couldn't find ALSPC '%s'", id.c_str());
		exit(1);
//	}
	return NULL;
}

#endif

Anim *Resources::getAnim (const string &id)
{
	return pImpl->getAnim(id);
}

Anim *Resources::getAnimIfExists (const string &id)
{
	return pImpl->getAnimIfExists(id);
}

Anim *ResourcesImpl::getAnim (const string &id)
{
	if (animlist.find (id) == animlist.end())
	{
		allegro_message ("Couldn't find Animation '%s'", id.c_str());
		exit(1);
	}
	else
	{
		return animlist[id];
	}
}

ALLEGRO_AUDIO_STREAM *Resources::getMusic (const string &id)
{
	return pImpl->getMusic(id);
}

#ifdef USE_TEGEL
TEG_MAP *Resources::getMap (const string &id)
{
	return pImpl->getMap (id);
}

TEG_MAP *ResourcesImpl::getMap (const string &id)
{
	if (maps.find(id) == maps.end())
	{
		allegro_message ("Couldn't find MAP '%s'", id.c_str());
		exit(1);
	}
	return maps[id];
}

TEG_TILELIST *Resources::getTilelist (const string &id)
{
	return pImpl->getTilelist (id);
}

TEG_TILELIST *ResourcesImpl::getTilelist (const string &id)
{
	if (tilelists.find (id) == tilelists.end())
	{
		allegro_message ("Couldn't find TILELIST '%s'", id.c_str());
		exit(1);
	}

	return tilelists[id];
}

int Resources::addMapFile(const string &filename, const string &tilesname)
{
	return pImpl->addMapFile (filename, tilesname);
}

int Resources::addMapFile(const string &id, const string &filename, const string &tilesname)
{
	return pImpl->addMapFile (id, filename, tilesname);
}

int Resources::addMapFiles(const string &pattern, const string &tilesname)
{
	return pImpl->addMapFiles (pattern, tilesname);
}

#endif

const map<string, Anim*> &Resources::getAnims() const
{
	return pImpl->getAnims();
}

const map<string, Anim*> &ResourcesImpl::getAnims() const
{
	return animlist;
}

void Resources::putBitmap(const string &id, ALLEGRO_BITMAP *bmp)
{
	pImpl->putBitmap (id, bmp);
}

void Resources::putAnim (const string &id, Anim *val)
{
	pImpl->putAnim (id, val);
}

void ResourcesImpl::putAnim (const string &id, Anim *val)
{
	animlist.insert (pair<string, Anim*>(id, val));
}

const char *Resources::getErrorMsg() const
{
	return pImpl->getErrorMsg();
}

const char *ResourcesImpl::getErrorMsg() const
{
	return errorMsg;
}

#ifdef USE_LUA
void Resources::setLuaState (lua_State *val)
{
	pImpl->setLuaState (val);
}

void ResourcesImpl::setLuaState (lua_State *val)
{
	L = val;
}

#endif

ALLEGRO_SAMPLE *Resources::getSampleIfExists(const std::string &id)
{
	return pImpl->getSampleIfExists(id);
}

ALLEGRO_BITMAP *Resources::getBitmapIfExists(const std::string &id)
{
	return pImpl->getBitmapIfExists(id);
}

int Resources::addDir(const char *dir)
{
	return pImpl->addDir(dir);
}

Resources::~Resources() { delete pImpl; }

#ifdef USE_DIALOG
DomNode &Resources::getDialog (const string &id)
{
	return pImpl->getDialog(id);
}
#endif
