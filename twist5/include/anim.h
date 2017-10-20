#ifndef ANIM_H
#define ANIM_H

#include <stdio.h>
#include <vector>
#include <map>
#include <string>
#include <assert.h>
#include "dom.h"
#include <memory>
#include <allegro5/allegro.h>

struct ALLEGRO_BITMAP;

class IDirectionModel
{
public:
	virtual int idToIndex(const std::string &id) = 0;
	virtual ~IDirectionModel() {}
};

class DirectionModel : public IDirectionModel
{
	private:
		int numDirections;
		const char ** directions;
	public:
		DirectionModel (const char * _directions[], int _numDirections)
		{
			numDirections = _numDirections;
			directions = _directions;
		}
		
//		int getNumDirections() { return numDirections; }
		virtual int idToIndex(const std::string &id) override
		{
			for (int i = 0; i < numDirections; ++i)
			{
				if (id == directions[i]) return i;
			}
			return -1;
		}

		virtual ~DirectionModel() {}
};

class IBitmapProvider
{
public:
	   virtual ALLEGRO_BITMAP *getBitmap (const std::string &id) = 0;
	   virtual void putBitmap(const std::string &id, ALLEGRO_BITMAP *bmp) = 0;
};

class CompositePart
{
friend class Frame;
friend class Anim;
friend class AnimBuilder;
private:
	ALLEGRO_BITMAP *rle;
	int hotx;
	int hoty;

	/** originx,y + regionw,h define an area of the spritesheet */
	int originx;
	int originy;
	int regionw;
	int regionh;

	int transformFlag;
	bool isRegion;
public:
	CompositePart () : CompositePart(nullptr) {}
	CompositePart (ALLEGRO_BITMAP *_rle, int _hotx = 0, int _hoty = 0) :
			rle (_rle), hotx (_hotx), hoty (_hoty), originx(0), originy(0), regionw(0), regionh(0), transformFlag(0), isRegion(false) {}
	ALLEGRO_BITMAP *getRle() { return rle; }
};

/* 
	cumulative: end of frame time, cumulative, so that frames[0].cumulative == frames[0].length
		and frames[frames.size()-1].cumulative == totalLength
*/
class Frame
{
friend class Sequence;
friend class Anim;
friend class AnimBuilder;
private:
	int length;
	int cumulative; 
	std::vector<CompositePart> parts;
public:
	Frame () : length (0), cumulative (0), parts() {}
	Frame (CompositePart _onlyPart, int _length = 0) :
		length (_length), cumulative(0), parts() { parts.push_back (_onlyPart); }

	/** deprecated */ void draw(ALLEGRO_BITMAP *buf, int x, int y) const;
	void draw(int x, int y) const;
	void draw_tinted(int x, int y, ALLEGRO_COLOR tint) const;

	CompositePart &getPart(int i = 0) { return parts[i]; }
	void getCompositeBounds (int &sprx, int &spry, int &width, int &height) const;
};

class Sequence
{
friend class Anim;
friend class AnimBuilder;
private:
	std::vector <Frame> frames;
	int totalLength;
	bool loop; //TODO: send event at end of non-loop
public:
	Sequence() : frames(), totalLength(0), loop(true) {}
	const Frame *getFrame (int time) const;
	void add (Frame &f);
	int size() { return frames.size(); }
};

class AnimBuilder;

/**
	Anim consists of one or more states
	states consist of one or more dirs
	dir consists of one or more frames
	frames consist of one or more parts

	There may be 1, 2, 4 or 8 dirs for a sprite
	The dir you ask for will be normalized to the number of dirs available.
*/
class Anim
{
	friend class AnimBuilder;
private:
	// states	// dirs		  
	std::vector < std::vector < Sequence > > frames;
	
	static std::shared_ptr<IDirectionModel> model;
public:
	int sizex;
	int sizey;
	int sizez;
	
	Anim();
	~Anim() {}
	
	static void loadFromXml (xdom::DomNode &n, IBitmapProvider *res, std::map<std::string, Anim*> &result);
 	void drawFrame (ALLEGRO_BITMAP *buf, unsigned int state, unsigned int dir, int time, int x, int y) const;
 	int getNumStates () const { return frames.size(); }
 	
 	int getTotalDirs (int state = 0) { return frames[state].size(); }
 	int getTotalLength (int state, int dir) { return frames[state][dir].totalLength; }

 	const Frame *getFrame (unsigned int state, unsigned int dir, int time) const;

	void add (Frame &f, unsigned int state = 0, unsigned int dir = 0);

 	static void setDirectionModel (std::shared_ptr<IDirectionModel> value)
 	{
 		model = value;
 	}

 	/** NB: sets all frames to the same bitmap, regardless if they were all different or the same to begin with */
 	void replaceSpriteSheet(ALLEGRO_BITMAP *newSpriteSheet)
 	{
 		for (auto & state : frames)
 		{
 			for (auto & dir : state)
 			{
 				for (auto & frame : dir.frames)
 				{
 					for (auto & part : frame.parts)
 					{
 						part.rle = newSpriteSheet;
 					}
 				}
 			}
 		}
 	}

};

/**
	holds a combination of Anim, state and dir
*/
class AnimRunner
{
public:
	Anim *anim;
	int state;
	int dir;
	int startTime;
	
	void drawFrame (ALLEGRO_BITMAP *buf, int x, int y, int currentTime) const;
};

#endif
