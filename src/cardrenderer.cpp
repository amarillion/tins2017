#include "cardrenderer.h"
#include "util.h"
#include "color.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <sstream>
#include "engine.h"
#include "molbi.h"
#include "layout_const.h"
#include "textstyle.h"

using namespace std;

void drawOutlinedRect(int x1, int y1, int x2, int y2, ALLEGRO_COLOR outer, ALLEGRO_COLOR inner, float w) {
	al_draw_filled_rectangle(x1, y1, x2, y2, inner);
	al_draw_rectangle(x1 + 1, y1 + 1, x2, y2, outer, w);
}

class CardRenderer {

	Resources *res;
	ALLEGRO_FONT *font;

public:
	CardRenderer() {
		res = Engine::getResources();
		font = res->getFont("builtin_font");
	}

	void drawRibosome() {
		int w = AA_WIDTH + 20;
		int h = 80;

		ALLEGRO_BITMAP *result = al_create_bitmap (w, h);
		al_set_target_bitmap(result);
		al_clear_to_color(MAGIC_PINK);

		al_draw_filled_rounded_rectangle(0, 0, w, h, 15, 15, GREY);

		al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);
		al_draw_filled_rectangle(10, 50, w-10, h-10, MAGIC_PINK);
		al_set_blender (ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA);

		draw_shaded_text(font, WHITE, GREY, 5, 10, ALLEGRO_ALIGN_LEFT, "RIBOSOME");

		res->putBitmap("RIBOSOME", result);
	}

	ALLEGRO_BITMAP *drawNucleotideCard(int idx) {
		ALLEGRO_BITMAP *result = al_create_bitmap (NT_WIDTH, NT_HEIGHT);
		Assert(result, "couldn't create a bitmap");

		const NucleotideInfo *info = &nucleotideInfo[idx];

		ALLEGRO_COLOR mainColor = getNucleotideColor((NT)idx, 0.5);
		ALLEGRO_COLOR shadeColor = getNucleotideColor((NT)idx, 1.0);

		al_set_target_bitmap(result);
		drawOutlinedRect(0, 0, NT_WIDTH, NT_HEIGHT, mainColor, shadeColor, 1.0);

		draw_shaded_textf(font, WHITE, GREY, 5, 5, ALLEGRO_ALIGN_LEFT, "%c", info->code);

		return result;
	}

	void drawNucleotideCards() {
		for (int i = 0; i < NUM_NUCLEOTIDES; ++i) {
			ALLEGRO_BITMAP *bmp = drawNucleotideCard(i);
			stringstream ss;
			ss << "NUCLEOTIDE_" << i;
			res->putBitmap(ss.str(), bmp);
			NucleotideInfo *info = &nucleotideInfo[i];
			info->card = bmp;
		}

	}

};

void renderCards() {
	CardRenderer renderer;
	renderer.drawNucleotideCards();
	renderer.drawRibosome();
}

