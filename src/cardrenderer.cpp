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
	al_draw_rectangle(x1 + 1, y1, x2, y2 - 1, outer, w);
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

	ALLEGRO_BITMAP *drawSmallNtCard(int idx) {
		ALLEGRO_BITMAP *result = al_create_bitmap (NT_SMALL_WIDTH, NT_SMALL_HEIGHT);
		Assert(result, "couldn't create a bitmap");

		const NucleotideInfo *info = &nucleotideInfo[idx];

		ALLEGRO_COLOR mainColor = getNucleotideColor((NT)idx, 0.5);
		ALLEGRO_COLOR shadeColor = getNucleotideColor((NT)idx, 1.0);

		al_set_target_bitmap(result);
		drawOutlinedRect(0, 0, NT_SMALL_WIDTH, NT_SMALL_HEIGHT, mainColor, shadeColor, 1.0);

		draw_shaded_textf(font, WHITE, GREY, NT_SMALL_WIDTH / 2, 2, ALLEGRO_ALIGN_CENTER, "%c", info->code);

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

			stringstream ss2;
			ss2 << info->code;
			ALLEGRO_BITMAP *bmp2 = drawSmallNtCard(i);
			res->putBitmap(ss2.str(), bmp2);
		}

	}

	void drawCodons(AminoAcidInfo *info, double x1, double y1) {
		int yco = y1 + 16;
		const int INTERNAL_SPACING = 4;
		const int INTERNAL_WIDTH = (AA_WIDTH - (2 * AA_PADDING) - (2 * INTERNAL_SPACING)) / 3;
		const int INTERNAL_STEPSIZE = INTERNAL_WIDTH + INTERNAL_SPACING;
		const int INTERNAL_HEIGHT = 10;
		const int INTERNAL_VERTICAL_STEPSIZE = INTERNAL_HEIGHT + 1;

		for (auto codon : info->codons) {
			for (int c = 0; c < 3; ++c) {

				int xco = x1 + c * INTERNAL_STEPSIZE + AA_PADDING;

				char nt = codon.at(c);
				NT ntIdx = getNucleotideIndex(nt);
				ALLEGRO_COLOR mainCol = getNucleotideColor(ntIdx, 1.0);
				ALLEGRO_COLOR shadeCol = getNucleotideColor(ntIdx, 0.5);

				al_draw_filled_rectangle(xco, yco, xco + INTERNAL_WIDTH, yco + INTERNAL_HEIGHT, mainCol);
				al_draw_rectangle(xco, yco, xco + INTERNAL_WIDTH, yco + INTERNAL_HEIGHT, shadeCol, 1.0);
				al_draw_textf(font, WHITE, xco + (INTERNAL_WIDTH / 2), yco + 2, ALLEGRO_ALIGN_CENTER, "%c", nt);
			}
			yco += INTERNAL_VERTICAL_STEPSIZE;
		}
	}

	ALLEGRO_BITMAP *drawAminoAcid(int idx, bool withCodons) {
		double x1 = 0;
		double y1 = 0;
		int w = AA_WIDTH;
		int h = AA_HEIGHT;

		AminoAcidInfo *info = &aminoAcidInfo[idx];
		ALLEGRO_BITMAP *result = al_create_bitmap (w, h);
		al_set_target_bitmap(result);

		drawOutlinedRect(x1, y1, x1 + w, y1 + h, LIGHT_BLUE, al_map_rgb_f(0.25, 0.25, 0.5), 1.0);

		draw_shaded_textf(font, WHITE, GREY, x1 + AA_PADDING, y1 + AA_PADDING, ALLEGRO_ALIGN_LEFT, info->fullName.c_str());

		if (withCodons) {
			// draw the DNA target sequences...
			drawCodons(info, x1, y1);
		};

		return result;
	};

	void drawAminoAcidCards() {
		for (int i = 0; i < NUM_AMINO_ACIDS; ++i) {
			AminoAcidInfo *info = &aminoAcidInfo[i];

			ALLEGRO_BITMAP *bmp = drawAminoAcid(i, true);
			Assert(bmp, "couldn't create a bitmap");
			stringstream ss1;
			ss1 << "AA_" << info->threeLetterCode;
			res->putBitmap(ss1.str().c_str(), bmp);
			info->bmpFull = bmp;

			ALLEGRO_BITMAP *bmp2 = drawAminoAcid(i, false);
			Assert(bmp2, "couldn't create a bitmap");
			stringstream ss2;
			ss2 << "AA_SIMPLE_" << info->threeLetterCode;
			res->putBitmap(ss2.str().c_str(), bmp2);
			info->bmpSimple = bmp2;
		}
	}

	void drawMutationCards() {
		for (int i = 0; i < NUM_MUTATIONS; ++i) {
			MutationInfo *info = &mutationInfo[i];
			ALLEGRO_BITMAP *bmp = drawMutationCard(info, static_cast<MutationId>(i));
			stringstream ss;
			ss << "MUT_" << info->name;
			res->putBitmap(ss.str().c_str(), bmp);
			info->card = bmp;
		}
	}

	ALLEGRO_BITMAP *drawMutationCard (MutationInfo *info, MutationId mutationId) {
		ALLEGRO_BITMAP *result = al_create_bitmap(MUTCARD_W, MUTCARD_H);
		al_set_target_bitmap(result);

		ALLEGRO_COLOR color = al_map_rgb_f(0, 0.5, 0);

		double x1 = 0;
		double y1 = 0;
		double x2 = x1 + MUTCARD_W;
		double y2 = y1 + MUTCARD_H;
		al_draw_filled_rectangle(x1, y1, x2, y2, color);
		al_draw_rectangle(x1, y1, x2, y2, GREEN, 1.0);

		draw_shaded_textf(font, WHITE, GREY, x1 + 5, y1 + 5, ALLEGRO_ALIGN_LEFT, info->name.c_str());

		drawLogo (mutationId, 40, 20);

		return result;
	}

	void drawLogo (MutationId mutationId, int dx, int dy) {
		const int SIZE = 6;
		const int SIZE_2 = SIZE / 2;
		const int SPACING = 6;
		const int STEP = SIZE + SPACING;

		int xx = dx;
		int yy = dy;

		int xm = xx + SIZE_2;
		int ym = yy + SIZE_2;

		switch (mutationId) {
		case MutationId::TRANSVERSION:
			al_draw_line (xm, ym, xm + STEP, ym + STEP, BLACK, 2.0);
			al_draw_line (xm + STEP, ym, xm, ym + STEP, BLACK, 2.0);
			break;
		case MutationId::COMPLEMENT:
			al_draw_line (xm, ym, xm, ym + STEP, BLACK, 2.0);
			al_draw_line (xm + STEP, ym, xm + STEP, ym + STEP, BLACK, 2.0);
			break;
		case MutationId::TRANSITION:
			al_draw_line (xm, ym, xm + STEP, ym, BLACK, 2.0);
			al_draw_line (xm, ym + STEP, xm + STEP, ym + STEP, BLACK, 2.0);
			break;
		default: return; // don't draw any logo
		}

		drawOutlinedRect(xx, yy, xx + SIZE, yy + SIZE,
				getNucleotideColor(NT::A, 1.0),
				getNucleotideColor(NT::A, 0.5), 1.0);

		drawOutlinedRect(xx + STEP, yy, xx + STEP + SIZE, yy + SIZE,
				getNucleotideColor(NT::G, 1.0),
				getNucleotideColor(NT::G, 0.5), 1.0);

		drawOutlinedRect(xx, yy + STEP, xx + SIZE, yy + STEP + SIZE,
				getNucleotideColor(NT::T, 1.0),
				getNucleotideColor(NT::T, 0.5), 1.0);

		drawOutlinedRect(xx + STEP, yy + STEP, xx + STEP + SIZE, yy + STEP + SIZE,
				getNucleotideColor(NT::C, 1.0),
				getNucleotideColor(NT::C, 0.5), 1.0);

	}

};

void renderCards() {
	CardRenderer renderer;
	renderer.drawNucleotideCards();
	renderer.drawRibosome();
	renderer.drawAminoAcidCards();
	renderer.drawMutationCards();
}

