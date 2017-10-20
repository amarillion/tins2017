#ifndef _WRAPPERS_H_
#define _WRAPPERS_H_

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include "color.h"
#include <string>

class Error
{
public:
    enum Type {
       NONE,
       MEMORY,
       VIDEOMEMORY,
       SKIN_INI,
       SKIN_DAT,
       NO_FILE,
       LOAD_FILE,
       INDEX,
       UNKNOWN
    };

protected:
/**
	 The actual error code of the Error object.
*/
Type type;

public:
   /**
      The constructor for making a new Error object.
      Pass one of the possible error codes as the parameter to create an error.
   */
   Error(Type t = NONE) : type(t) {}

   virtual ~Error() {}

   virtual operator bool() const {
	   return (type != NONE);
	}

   /**
      Compares two error codes.
      Return true if the error codes are the same.
   */
   virtual bool operator==(const Type &t) {
	   return (type == t);
	}

   /**
      Compares two error codes.
      Return true if the error codes are different.
   */
   virtual bool operator!=(const Type &t) {
	   return (type != t);
	}

};

class Size {
   private:
      int wide;
      int high;

   public:
      Size(int w=0, int h=0) : wide(w), high(h) {}

      /*
      bool operator==(const Size &s) const;
      bool operator!=(const Size &p) const;
      void operator+=(const Size &s) { wide += s.w(); high += s.h(); }
      void operator-=(const Size &s) { wide -= s.w(); high -= s.h(); }
      Size operator+(const Size &s) const {
         Size res;
         res.w(s.w() + wide);
         res.h(s.h() + high);
         return res;
      }
      Size operator-(const Size &s) const {
         Size res;
         res.w(wide - s.w());
         res.h(high - s.h());
         return res;
      }
*/
      int w() const { return wide; }
      int h() const { return high; }
      void w(int v) { wide = v; }
      void h(int v) { high = v; }
};

class Rect;

class Point {
   protected:
      /**
         The underlaying x coordinate.
      */
      int posx;

      /**
         The underlaying y coordinate.
      */
      int posy;

   public:
      /**
         Default constructor.
      */
      Point(int x=0, int y=0);

      /**   */bool operator==(const Point &p) const;
      /**
         Operators for comparing two points.
      */
      bool operator!=(const Point &p) const;

      /**   */bool operator<(const Rect &r) const;
      /**
         Operators for checking whether the point is inside a rectangle or not.
         The < operator checks the inside of the rectangle only, while <= checks
         for borders as well.
      */
      bool operator<=(const Rect &r) const;

      /**   */void operator+=(const Point &p) { posx += p.x(); posy += p.y(); }
      /**   */void operator-=(const Point &p) { posx -= p.x(); posy -= p.y(); }
      /**   */Point operator+(const Point &p) const {
         Point res;
         res.x(p.x() + posx);
         res.y(p.y() + posy);
         return res;
      }
      /**
         Operators for doing simple vector arithmetic with two points.

         Examples:
<pre>
      Point a(10, 20);
      Point b(30, 40);

      Point c = a + b;   // c = (40, 60)
      Point d = b - a;   // d = (20, 20)
      c += a;            // c = (50, 80)
      c -= b;            // c = (20, 40)
</pre>
      */
      Point operator-(const Point &p) const {
         Point res;
         res.x(posx - p.x());
         res.y(posy - p.y());
         return res;
      }


      /**   */int x() const { return posx; }
      /**   */int y() const { return posy; }
      /**   */void x(int v) { posx = v; }
      /**
         Get and set functions.
      */
      void y(int v) { posy = v; }
};

class Rect {
   protected:
      /**
         Top left point (origin).
      */
      Point origin;

      /**
         The size of the rectangle.
      */
      Size dim;

   public:
      /**
         Default constructor.
      */
      Rect() :origin(0, 0),
	   dim(0, 0)
	{
	}


      /**
         Constructor for making a rectangle from coordinates of origin, width and height.
      */
      Rect(int x, int y, int w, int h)    :origin(x, y),
    		   dim(w, h)
    		{
    		}


      /**
         Constructor for making a rectangle from the origin point and size.
      */
      Rect(const Point &p, const Size &s)  :origin(p),
    		   dim(s) {}


      virtual ~Rect() {}

      /**
         Operator for comparing two Rects.
         Two rectangles are equal if they have the same origin point and size.
      */
      bool operator==(const Rect &r) const;

      bool operator>(const Rect &r) const;
      bool operator>=(const Rect &r) const;
      bool operator>(const Point &p) const;
      bool operator>=(const Point &p) const;

      /** */void operator+=(const Point &p) { origin += p; }
      /** */void operator-=(const Point &p) { origin -= p; }
      /** */Rect operator+(const Point &p) const { return Rect(origin + p, dim); }
      /**
         Operators for doing simple arithmetic with Rects (i.e. moving).
      */
      Rect operator-(const Point &p) const { return Rect(origin - p, dim); }

      /**
         Returns true if this rectangle intersects with the passed rectangle (excluding the edges).
      */
      bool Intersects(const Rect &r) const;

      /** */Point topLeft() const { return origin; }
      /** */Point bottomRight() const { return Point(origin.x() + dim.w(), origin.y() + dim.h()); }
      /** */Size size() const { return dim; }
      /** */int x() const { return origin.x(); }
      /** */int y() const { return origin.y(); }
      /** */int x2() const { return origin.x() + dim.w(); }
      /** */int y2() const { return origin.y() + dim.h(); }
      /** */int w() const { return dim.w(); }
      /** */int h() const { return dim.h(); }
      /** */void topLeft(const Point& p) { origin = p; }
      /** */void bottomRight(const Point& p) { x2(p.x()); y2(p.y()); }
      /** */void size(const Size& s) { dim = s; }
      /** */void x(int v) { origin.x(v); }
      /** */void y(int v) { origin.y(v); }
      /** */void x2(int v) { dim.w(v - origin.x()); }
      /** */void y2(int v) { dim.h(v - origin.y()); }
      /** */void w(int v) { dim.w(v); }
      /**
         Bsic get and set functions.
      */
      void h(int v) { dim.h(v); }
};

// MAS wrapper class
class Font
{
public:
	Error Load(const char *fname, int size = 12, bool flag = true) { return Error::NONE; /* TODO */ }
	void Create() { /* TODO */ }
	void Destroy() { /* TODO */ }
};

// MAS wrapper class
class Sample
{
public:
	void Stop() { /* TODO */ }
	void Play(int vol) { /* TODO */ }
	void Create(int, bool, int, int) { /* TODO */ }
	Error Load(const char *filename) { return Error::NONE; /* TODO */ }
	void Set(Sample &sam, bool copy) { /* TODO */ }
	void Destroy() { /* TODO */ }
};

#define _MASKING_BITMAP_STORE_AND_SET_ALL(target)      \
   ALLEGRO_STATE state;                                \
   al_store_state(&state, ALLEGRO_STATE_TARGET_BITMAP  \
                        | ALLEGRO_STATE_BLENDER);      \
   al_set_target_bitmap(target);                       \
   al_set_blender(ALLEGRO_ADD,                         \
                  ALLEGRO_ALPHA,                       \
                  ALLEGRO_INVERSE_ALPHA);

#define _MASKING_BITMAP_STORE_AND_SET(target)          \
   ALLEGRO_STATE state;                                \
   al_store_state(&state, ALLEGRO_STATE_TARGET_BITMAP);\
   al_set_target_bitmap(target);

#define _MASKING_BITMAP_RESTORE al_restore_state(&state);

/**
   A wrapper for the ALLEGRO_BITMAP structure.
   It has all the blitting, sprite drawing and primitive drawing functions,
   copy constructors, overloaded assignment operators and a few special GUI
   drawing functions (like drawing a pseudo 3D rectangle for example). At the
   time I wrote this class I thought it was a good idea to have wrappers for
   all the Allegro structures but I have since changed my mind. This class
   however is almost complete (some functions are missing though) and as far
   as I know it works and since I used it throughout the rest of the library
   I decided to keep it in anyway. I suggest you keep using the drawing and
   bitmap manipulation functions Allegro provides instead of learning the
   interface of this class though, but there's no harm using this class so
   if you want take a look at the actual header:
   \URL[bitmap.h]{"../../include/MASkinG/bitmap.h"}
*/
class Bitmap {
   public:
      // subtype of the bitmap
      enum Type {
         MEMORY,    // memory bitmap
         VIDEO,      // video bitmap
         UNDEFINED
      };

   protected:
      ALLEGRO_BITMAP *data;   // the actual bitmap data
      Type type;
      bool allocated;      // did we allocate the data or did we just make a pointer?
      Size thick;         // size of the central area (for tiled blit)

   public:
      // ctors & dtor
      Bitmap();
      Bitmap(int w, int h, Type t = VIDEO);
      Bitmap(const Size& s, Type t = VIDEO);
      Bitmap(const char *file, Type t = VIDEO);
      Bitmap(ALLEGRO_BITMAP *src);  //make a pointer!
      Bitmap(const Bitmap &src);
      virtual ~Bitmap();

      operator ALLEGRO_BITMAP*() const;
      Bitmap& operator=(ALLEGRO_BITMAP *bmp); // assignment from Allegro bitmap: makes a pointer!
      Bitmap& operator=(const Bitmap &bmp);   // assignment from another bitmap: makes a copy!
      virtual Bitmap& Set(ALLEGRO_BITMAP *bmp, bool copy=false, Type=UNDEFINED);
      virtual Bitmap& Set(const Bitmap &bmp, bool copy=false, Type=UNDEFINED);
      virtual int ThickX() const { return thick.w(); }
      virtual int ThickY() const { return thick.h(); }
      virtual const Size& Thick() const { return thick; }
      virtual void ThickX(int x) { thick.w(x); }
      virtual void ThickY(int y) { thick.h(y); }
      virtual void Thick(const Size &s) { thick = s; }

      // loading and saving functions
      virtual Error Load(const char *file, Type t = VIDEO);
      virtual Error Load(const std::string &file, Type t = VIDEO);
      virtual void Save(const char *file);

      // basic bitmap manipulation functions
      Bitmap&      Create(const Size &s, Type t = VIDEO) { return Create(s.w(), s.h(), t); }
      Bitmap&      Create(int w, int h, Type t = VIDEO);
      Bitmap&      CreateSubBitmap(const Bitmap &parent, const Rect &r) { return CreateSubBitmap(parent, r.x(), r.y(), r.w(), r.h()); }
      Bitmap&      CreateSubBitmap(const Bitmap &parent, int x, int y, int w, int h);
      void      Destroy();
      int   w()      const { return al_get_bitmap_width(data); }
      int   h()      const { return al_get_bitmap_height(data); }
      //Size   size()      const { return Size(w(), h()); }
      bool   IsMemory()   const { return al_get_bitmap_flags(data) & ALLEGRO_MEMORY_BITMAP; }
      bool   IsSubBitmap()   const { return al_is_sub_bitmap(data); }
      bool   IsVideo()   const { return !IsMemory(); }
      void   SetClip(const Rect &r)   { SetClip(r.x(), r.y(), r.x2(), r.y2()); }
      void   SetClip(int x1, int y1, int x2, int y2)   {
         _MASKING_BITMAP_STORE_AND_SET(data)
         al_set_clipping_rectangle(x1,y1,x2-x1+1,y2-y1+1);
         _MASKING_BITMAP_RESTORE}
      Rect   GetClip();

      // drawing primitives
      void   Putpixel(const Point &p, ALLEGRO_COLOR c) {   Putpixel(p.x(), p.y(), c); }
      ALLEGRO_COLOR Getpixel(const Point &p) const { return Getpixel(p.x(), p.y()); }
      void   Line(const Point &p1, const Point &p2, ALLEGRO_COLOR c) { Line(p1.x(), p1.y(), p2.x(), p2.y(), c); }
      void   Vline(const Point &p, int y2, ALLEGRO_COLOR c) {   Vline(p.x(), p.y(), y2, c); }
      void   Hline(const Point &p, int x2, ALLEGRO_COLOR c) {   Hline(p.x(), p.y(), x2, c); }
      void   Rectangle(const Rect &r, ALLEGRO_COLOR c) {   Rectangle(r.x(), r.y(), r.x2(), r.y2(), c); }
      void   Rectfill(const Rect &r, ALLEGRO_COLOR c) {   Rectfill(r.x(), r.y(), r.x2(), r.y2(), c); }
      void   Triangle(const Point &p1, const Point &p2, const Point &p3, ALLEGRO_COLOR c);
      void   Circle(const Point &p, int r, ALLEGRO_COLOR c) {   Circle(p.x(), p.y(), r, c); }
      void   Circlefill(const Point &p, int r, ALLEGRO_COLOR c) {   Circlefill(p.x(), p.y(), r, c); }
      void   Ellipse(const Point &p, const Size &s, ALLEGRO_COLOR c) { Ellipse(p.x(), p.y(), s.w(), s.h(), c); }
      void   Ellipsefill(const Point &p, const Size &s, ALLEGRO_COLOR c) { Ellipsefill(p.x(), p.y(), s.w(), s.h(), c); }
      void   Arc(const Point &p, int a1, int a2, int r, ALLEGRO_COLOR c) { Arc(p.x(), p.y(), a1, a2, r, c);}
      /* TODO
      void   Spline(const int points[8], ALLEGRO_COLOR c) { spline(data, points, c); }
      void   Floodfill(const Point &p, ALLEGRO_COLOR c) { floodfill(data, p.x(), p.y(), c); }
      */

      void   Putpixel(int x, int y, const ALLEGRO_COLOR &c) {
         _MASKING_BITMAP_STORE_AND_SET_ALL(data)
         al_put_pixel(x+0.5, y+0.5, c);
         _MASKING_BITMAP_RESTORE
      }

      ALLEGRO_COLOR Getpixel(int x, int y) const { return al_get_pixel(data, x, y); }

      void   Line(int x1, int y1, int x2, int y2, const ALLEGRO_COLOR &c) {
         _MASKING_BITMAP_STORE_AND_SET_ALL(data)
         al_draw_line(x1+0.5, y1+0.5, x2+0.5, y2+0.5, c, 1);
         _MASKING_BITMAP_RESTORE
      }

      void   Vline(int x, int y1, int y2, const ALLEGRO_COLOR &c) {
         _MASKING_BITMAP_STORE_AND_SET_ALL(data)
         al_draw_line(x+0.5, y1+0.5, x+0.5, y2+0.5, c, 1);
         _MASKING_BITMAP_RESTORE
      }

      void   Hline(int x1, int y, int x2, const ALLEGRO_COLOR &c) {
         _MASKING_BITMAP_STORE_AND_SET_ALL(data)
         al_draw_line(x1+0.5, y+0.5, x2+0.5, y+0.5, c, 1);
         _MASKING_BITMAP_RESTORE
      }

      void   Rectangle(int x1, int y1, int x2, int y2, const ALLEGRO_COLOR &c) {
         _MASKING_BITMAP_STORE_AND_SET_ALL(data)
         al_draw_rectangle(x1+0.5, y1+0.5, x2+0.5, y2+0.5, c, 1);
         _MASKING_BITMAP_RESTORE
      }

      void   Rectfill(int x1, int y1, int x2, int y2, const ALLEGRO_COLOR &c) {
         _MASKING_BITMAP_STORE_AND_SET_ALL(data)
         al_draw_filled_rectangle(x1+0.5, y1+0.5, x2+0.5, y2+0.5, c);
         _MASKING_BITMAP_RESTORE
      }

      void   Triangle(int x1, int y1, int x2, int y2, int x3, int y3, const ALLEGRO_COLOR &c) {
         _MASKING_BITMAP_STORE_AND_SET_ALL(data)
         al_draw_triangle(x1+0.5, y1+0.5, x2+0.5, y2+0.5, x3+0.5, y3+0.5, c, 1);
         _MASKING_BITMAP_RESTORE
      }

      void   Circle(int x, int y, int r, const ALLEGRO_COLOR &c) {
         _MASKING_BITMAP_STORE_AND_SET_ALL(data)
         al_draw_circle(x+0.5, y+0.5, r, c, 1);
         _MASKING_BITMAP_RESTORE
      }

      void   Circlefill(int x, int y, int r, const ALLEGRO_COLOR &c) {
         _MASKING_BITMAP_STORE_AND_SET_ALL(data)
         al_draw_filled_circle(x+0.5, y+0.5, r, c);
         _MASKING_BITMAP_RESTORE
      }

      void   Ellipse(int x, int y, int rx, int ry, const ALLEGRO_COLOR &c) {
         _MASKING_BITMAP_STORE_AND_SET_ALL(data)
         al_draw_ellipse(x+0.5, y+0.5, rx, ry, c, 1);
         _MASKING_BITMAP_RESTORE
      }

      void   Ellipsefill(int x, int y, int rx, int ry, const ALLEGRO_COLOR &c) {
         _MASKING_BITMAP_STORE_AND_SET_ALL(data)
         al_draw_filled_ellipse(x+0.5, y+0.5, rx, ry, c);
         _MASKING_BITMAP_RESTORE
      }

      void   Arc(int x, int y, int a1, int a2, int r, const ALLEGRO_COLOR &c) {
         _MASKING_BITMAP_STORE_AND_SET_ALL(data)
         al_draw_arc(x+0.5, y+0.5, r, a1/255.f*2*ALLEGRO_PI, a2/255.f*2*ALLEGRO_PI, c, 1);
         _MASKING_BITMAP_RESTORE
      }
      /*  TODO
      void   Floodfill(int x, int y, int c) { floodfill(data, x, y, c); }
      */

      // blitting functions
      void   Clear() { al_clear_to_color(WHITE); }

      void   Clear(ALLEGRO_COLOR c) {
         _MASKING_BITMAP_STORE_AND_SET_ALL(data)
         al_clear_to_color(c);
         _MASKING_BITMAP_RESTORE
      }

      void   Blit(Bitmap &dest, const Point &pSrc, const Point &pDest, const Size &s) const {
         _MASKING_BITMAP_STORE_AND_SET_ALL(dest)
         al_draw_bitmap_region(data, pSrc.x(), pSrc.y(), s.w(), s.h(), pDest.x(), pDest.y(), 0);
         _MASKING_BITMAP_RESTORE
      }

      void   PlainBlit(Bitmap &dest, const Point &pSrc, const Point &pDest, const Size &s) const {
         _MASKING_BITMAP_STORE_AND_SET_ALL(dest)
         al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);
         al_draw_bitmap_region(data, pSrc.x(), pSrc.y(), s.w(), s.h(), pDest.x(), pDest.y(), 0);
         _MASKING_BITMAP_RESTORE
      }

      void   StretchBlit(Bitmap &dest, const Rect &rSrc, const Rect &rDest) const {
         _MASKING_BITMAP_STORE_AND_SET_ALL(dest)
         al_draw_scaled_bitmap(data, rSrc.x(), rSrc.y(), rSrc.w(), rSrc.h(), rDest.x(), rDest.y(), rDest.w(),
                               rDest.h(), 0);
         _MASKING_BITMAP_RESTORE
      }

      void   Blit(Bitmap &dest, int sx, int sy, int dx, int dy, int w, int h) {
         _MASKING_BITMAP_STORE_AND_SET_ALL(dest)
         al_draw_bitmap_region(data, sx, sy, w, h, dx, dy, 0);
         _MASKING_BITMAP_RESTORE
      }

      void   StretchBlit(Bitmap &dest, int sx, int sy, int sw, int sh, int dx, int dy, int dw, int dh) {
         _MASKING_BITMAP_STORE_AND_SET_ALL(dest)
         al_draw_scaled_bitmap(data, sx, sy, sw, sh, dx, dy, dw, dh, 0);
         _MASKING_BITMAP_RESTORE
      }

      // special drawing functions
      void   DrawDottedRect(const Rect &r, ALLEGRO_COLOR c) { DrawDottedRect(r.x(), r.y(), r.x2(), r.y2(), c); }
      void   Draw3DFrame(const Rect &r, ALLEGRO_COLOR c1, ALLEGRO_COLOR c2, ALLEGRO_COLOR c3) { Draw3DFrame(r.x(), r.y(), r.x2(), r.y2(), c1, c2, c3); }
      void   Draw3DCircle(const Point &centre, int r, ALLEGRO_COLOR c1, ALLEGRO_COLOR c2, ALLEGRO_COLOR c3) { Draw3DCircle(centre.x(), centre.y(), r, c1, c2, c3); }
      void   DrawXMark(const Point &centre, int size, ALLEGRO_COLOR col) { DrawXMark(centre.x(), centre.y(), size, col); }
      void   DrawCheckMark(const Point &origin, ALLEGRO_COLOR col) { DrawCheckMark(origin.x(), origin.y(), col); }
      void   DrawArrow(const Point &origin, ALLEGRO_COLOR col, int dir) { DrawArrow(origin.x(), origin.y(), col, dir); }

      void   DrawDottedRect(int x1, int y1, int x2, int y2, const ALLEGRO_COLOR &c);
      void   Draw3DFrame(int x1, int y1, int x2, int y2,
                         const ALLEGRO_COLOR &c1, const ALLEGRO_COLOR &c2, const ALLEGRO_COLOR &c3);
      void   Draw3DCircle(int x, int y, int r,
                          const ALLEGRO_COLOR &c1, const ALLEGRO_COLOR &c2, const ALLEGRO_COLOR &c3);
      void   DrawXMark(int x, int y, int size, const ALLEGRO_COLOR &col);
      void   DrawCheckMark(int x, int y, const ALLEGRO_COLOR &col);
      void   DrawArrow(int x, int y, const ALLEGRO_COLOR &col, int dir);

      void TiledBlit(
         Bitmap &dest,   // destination
         int sx = 0,      // x offset of the source subbitmap
         int sy = 0,      // y offset of the source subbitmap
         int dx = 0,      // destination x
         int dy = 0,      // destination y
         int w = 0,      // blit width (0 ==> w=destination->w)
         int h = 0,      // blit height (0 ==> h=destination->h)
         int countx = 0,   // number of subbitmaps (horizontaly)
         int county = 0   // number of subbitmaps (verticaly)
      );
      void HorizontalTile(Bitmap &dest, int i, int x, int y, int w, int count);
      void VerticalTile(Bitmap &dest, int i, int x, int y, int h, int count);

      static Bitmap null;
};

// MAS wrapper class
class Cursor // wrapper for mouse cursor
{
private:
	ALLEGRO_MOUSE_CURSOR *data;
public:
	void Create(ALLEGRO_BITMAP *bmp = NULL, int n = 1);
	void SetAnimationInterval(int interval) { /* TODO */ };
	void Destroy() { /* TODO */ }
	void SetFocus(int, int) { /* TODO */ }
	static void ReadSkinData(ALLEGRO_CONFIG *config) { /* TODO */ }
};

class MASSettings
{
public:
	static int screenHeight;
	static int screenWidth;
	static bool fullscreen;
	static int gfxMode;
	static int colorDepth;
	static int soundVolume;
	static bool useVideoMemory;
	static void Save();
};

#endif
