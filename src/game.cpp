#include <assert.h>

#include "game.h"
#include "color.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_audio.h>

#include "engine.h"
#include "util.h"
#include "textstyle.h"
#include "button.h"
#include "panel.h"
#include "button.h"

#include "data.h"
#include "messagebox.h"
#include <algorithm>
#include "timer.h"
#include "text.h"
#include <sstream>
#include <fstream>

#include "mainloop.h"

using namespace std;

enum class AA {
	Ala, Arg, Asn, Asp, Cys, Gln, Glu, Gly, His, Ile, Leu, Lys, Met, Phe, Pro, Ser, Thr, Trp, Tyr, Val, STP
};

struct AminoAcidInfo {
	char code;
	string threeLetterCode;
	string fullName;
	vector<string> codons;
};

const int NUM_AMINO_ACIDS = 21;
AminoAcidInfo aminoAcidInfo[NUM_AMINO_ACIDS] = {
	{ 'A', "Ala", "Alanine",       {"GCT", "GCC", "GCA", "GCG"} },
	{ 'R', "Arg", "Argenine",      {"CGT", "CGC", "CGA", "CGG", "AGA", "AGG"} },
	{ 'N', "Asn", "Asparagine",    {"AAT", "AAC"} },
	{ 'D', "Asp", "Aspartate",     {"GAT", "GAC"} },
	{ 'C', "Cys", "Cysteine",      {"TGT", "TGC"} },
	{ 'Q', "Gln", "Glutamine",     {"CAA", "CAG"} },
	{ 'E', "Glu", "Glutamate",     {"GAA", "GAG"} },
	{ 'G', "Gly", "Glycine",       {"GGT", "GGC", "GGA", "GGG"} },
	{ 'H', "His", "Histidine",     {"CAT", "CAC"} },
	{ 'I', "Ile", "Isoleucine",    {"ATT", "ATC", "ATA"} },
	{ 'L', "Leu", "Leucine",       {"CTT", "CTC", "CTA", "CTG", "TTA", "TTG"} },
	{ 'K', "Lys", "Lysine",        {"AAA", "AAG"} },
	{ 'M', "Met", "Methionine",    {"ATG"} },
	{ 'F', "Phe", "Phenylalan.", {"TTT", "TTC"} },
	{ 'P', "Pro", "Proline",       {"CCT", "CCC", "CCA", "CCG"} },
	{ 'S', "Ser", "Serine",        {"TCT", "TCC", "TCA", "TCG", "AGT", "AGC"} },
	{ 'T', "Thr", "Threonine",     {"ACT", "ACC", "ACA", "ACG"} },
	{ 'W', "Trp", "Tryptophan",    {"TGG"} },
	{ 'Y', "Tyr", "Tyrosine",      {"TAT", "TAC"} },
	{ 'V', "Val", "Valine",        {"GTT", "GTC", "GTA", "GTG"} },
	{ '*', "***", "Stop",          {"TAA", "TAG", "TGA"} },
};

enum class MutationId {
	TRANSVERSION, // transversion:  G<->T   A<->C
	TRANSITION, // transition:    G<->A   T<->C
	COMPLEMENT, // complement:    G<->C   A<->T
	REVERSE_COMPLEMENT, INSERTION_A, INSERTION_C, INSERTION_T, INSERTION_G, DELETION,
};

struct MutationInfo {
	string name;
};

const int NUM_MUTATIONS = 9;
MutationInfo mutationInfo[NUM_MUTATIONS] {
	{ "TRANSVERSION" },
	{ "TRANSITION" },
	{ "COMPLEMENT" },
	{ "REVERSE COMPLEMENT" },
	{ "INSERT A" },
	{ "INSERT C" },
	{ "INSERT T" },
	{ "INSERT_G" },
	{ "DELETION" }
};

const int NUM_NUCLEOTIDES = 4;
struct NucleotideInfo {
	char code;
	string name;
	ALLEGRO_BITMAP *card;
};

enum class NT { T, C, A, G };

NucleotideInfo nucleotideInfo[NUM_NUCLEOTIDES] = {
	{ 'T', "Tyrosine", nullptr },
	{ 'C', "Cytosine", nullptr },
	{ 'A', "Adenine", nullptr },
	{ 'G', "Guanosine", nullptr }
};

using Peptide = vector<AA>;
using OligoNt = string;

enum class Cmd { SAY, BIGEYES, NORMALEYES, ACTIVATE_ALL, ACTIVATE_GENE, ACTIVATE_TARGET, ACTIVATE_TRANSLATION };

struct Statement {
	Cmd cmd;
	string text;
};

using Script = vector<Statement>;

const int NUM_SCRIPTS = 9;
Script scripts[NUM_SCRIPTS] = {
	{
		// 0: effectively no script
		{ Cmd::ACTIVATE_ALL, "" },
	}, {
		// 1
		{ Cmd::SAY, "Ok, let's have a look at our patient." },
		{ Cmd::SAY, "Ah yes, I see. The patient needs an antibiotic,\nadministered directly in the cells.\n" },
		{ Cmd::SAY, "With a targeted gene mutation,\nthe patient will produce the antibiotic by itself.\n"
				"We'll use our silly mutation weapon here to mutate the patient.\n"
				"Don't worry, this won't hurt a bit.\nI mean, this probably won't hurt.\n" },
		{ Cmd::BIGEYES, "" },
		{ Cmd::SAY, "OK, this might hurt a little." },
		{ Cmd::NORMALEYES, "" },
		{ Cmd::ACTIVATE_TARGET, "" },
		{ Cmd::SAY, "The antibiotic we need is just a single amino acid: Tryptophan.\n"
					"To produce it, we need a three-letter genetic code of TGT.\n"
					"Look at the symbols on the tryptophan card,\nto see which genetic code it needs." },
		{ Cmd::SAY, "We need TGT to produce Tryptophan, This gene looks almost right.\n"
				"It is TGG, producing Cysteine.\n"
				"We just need one mutation.\n"
				"Use the cursor keys and enter to activate a card.\n"
				"Move it over to the right spot and press enter to apply.\n"
				}
	}, { // 2
		{ Cmd::ACTIVATE_ALL, "" },
		{ Cmd::SAY, "Let's try a different amino acid.\n"
					"Glutamate, always goes nice with chinese food!\n" },
		{ Cmd::SAY,	"A three-letter genetic code is also called a codon.\n"
					"The genetic code is redundant.\n"
					"Several possible codons produce the same amino acid."},
		{ Cmd::SAY, "For example, Glutamate corresponds to the codons GAG or GAA.\n"
					"That's why there are two rows of symbols on the Glutamate card.\n"
					"Move the mutation card over to apply the mutation."},
	}, { // 3
		{ Cmd::ACTIVATE_ALL, "" },
		{ Cmd::SAY, "A transversion is a different kind of mutation than a transition.\n"
					"A transition changes A into G, and a T into C.\n"
					"A transversion changes A into a C, and a T into G.\n"
					"If you look closely, the symbol on the mutation card shows this." },
	}, {
		// 4
		{ Cmd::ACTIVATE_ALL, "" },
		{ Cmd::SAY, "The complement of a nucleotide.\n"
					"Is the one it pairs with.\n"
					"G pairs with C, A pairs with T." },
	}, {
		// 5
		{ Cmd::ACTIVATE_ALL, "" },
		{ Cmd::SAY, "Sometimes a mutation deletes a base.\n"
					"This causes a frame shift.\n"
					"All amino acids that are downstream\n"
					"can be affected." },
	}, {
		// 6
		{ Cmd::ACTIVATE_ALL, "" },
		{ Cmd::SAY, "An insertion can undo the effect of a deletion.\n"
			},
	}, {
		// 7
		{ Cmd::ACTIVATE_ALL, "" },
		{ Cmd::SAY, "The reverse complement changes the direction of translation.\n"
					"The entire sequence is rotated by 180 degrees.\n"
			},
	}, {
		// 8
		{ Cmd::ACTIVATE_ALL, "" },
		{ Cmd::SAY, "A stop codon halts the translation.\n"
					"They are TAA, TGA or TAG.\n"
			},
	}
};

class DissolveEffect {

private:
	ALLEGRO_SHADER *shader;

public:
	DissolveEffect()
	{
		shader = al_create_shader(ALLEGRO_SHADER_AUTO);
		assert(shader);
/*
		const char *checker_vertex_shader_src =
			"attribute vec4 al_pos;"
			"uniform mat4 al_projview_matrix;"
			"void main()"
			"{"
			"	gl_Position = al_projview_matrix * al_pos;"
			"}";
*/
		const char *checker_pixel_shader_src =

			"#version 130\n"
			"uniform float uTime;"
			"uniform sampler2D al_tex;\n"
			"uniform bool al_use_tex;\n"
			"varying vec4 varying_color;\n"
			"varying vec2 varying_texcoord;\n"

			"void main(void)"
			"{"
			"	vec2 st = gl_FragCoord.xy;"
			"	vec3 color = vec3(0.0);"
			"	float checkSize = 3.0;"
			"	st /= checkSize;"
			"	float step = 20.0;"
			"	float cellNo = mod(floor(st.x), 8.0) + 8.0 * mod(floor(st.y), 8.0);"
			"	float fmodResult = mod (floor(cellNo / step) + mod(cellNo, step) * step, 64.0);"
			"	if (fmodResult / 64.0 < uTime) {"
			"		discard;"
			"	}"
			"	if ( al_use_tex )\n"
			"		gl_FragColor = varying_color * texture2D( al_tex , varying_texcoord);\n"
			"	else\n"
			"		gl_FragColor = varying_color;\n"
			"}";

		bool ok;

		ok = al_attach_shader_source(shader, ALLEGRO_PIXEL_SHADER, checker_pixel_shader_src);
		//TODO: assert with message format...
		if (!ok) printf ("al_attach_shader_source failed: %s\n", al_get_shader_log(shader));
		assert(ok);

		ok = al_attach_shader_source(shader, ALLEGRO_VERTEX_SHADER,
				al_get_default_shader_source(ALLEGRO_SHADER_GLSL, ALLEGRO_VERTEX_SHADER));

		//TODO: assert with message format...
		if (!ok) printf ("al_attach_shader_source failed: %s\n", al_get_shader_log(shader));
		assert(ok);

		ok = al_build_shader(shader);
		//TODO: assert with message...
		if (!ok) printf ("al_build_shader failed: %s\n", al_get_shader_log(shader));
		assert(ok);
	}

	void withPattern(float time, function<void()> closure)
	{
		enable(time);
		closure();
		disable();
	}

	void enable(float time) {
		al_use_shader(shader);
		al_set_shader_float("uTime", time);
	}


	void disable() {
		al_use_shader(NULL);
	}

	ALLEGRO_SHADER *getShader() { return shader; }
};

struct LevelInfo {
	int scriptId; // -1 for none
	Peptide targetPeptide;
	OligoNt startGene;

	// cards available in this level
	vector<MutationId> mutationCards;
};

const int NUM_LEVELS = 13;
LevelInfo levelInfo[NUM_LEVELS] = {

	{  1, { AA::Trp }, "TGT", { MutationId::TRANSVERSION } },
	{  2, { AA::Glu, AA::Glu, AA::Glu }, "GAGGACGAA", { MutationId::TRANSVERSION } },

	{  3, { AA::Leu, AA::Lys }, "TTCACA", { MutationId::TRANSITION, MutationId::TRANSVERSION } },

	{  4, { AA::Pro, AA::Asp }, "CATCAT", { MutationId::COMPLEMENT, MutationId::TRANSVERSION } },
	{  5, { AA::Val, AA::Thr }, "GATTACA", { MutationId::DELETION } },

	{  6, { AA::Ser, AA::Ser, AA::Ser }, "TCTCTCTCT", { MutationId::DELETION, MutationId::INSERTION_T } },

	{  7, { AA::Leu, AA::Ile, AA::Gly, AA::Pro }, "GGGCCCAATTAA", { MutationId::REVERSE_COMPLEMENT } },

	{  0, { AA::Gly, AA::Gly, AA::Asp }, "ACA" "CCA" "CC", { MutationId::REVERSE_COMPLEMENT, MutationId::INSERTION_G, MutationId::TRANSVERSION } },

	{  0, { AA::Arg, AA::Asp, AA::Leu }, "AGC" "GCT" "TTT", { MutationId::COMPLEMENT, MutationId::COMPLEMENT, MutationId::TRANSITION, MutationId::TRANSITION } },

	{  0, { AA::Cys, AA::Ile, AA::Ser, AA::His }, "TGG" "TGT" "TCA" "CAT", { MutationId::DELETION, MutationId::INSERTION_A, MutationId::COMPLEMENT } },

	{  8, { AA::Ser }, "TCTTGGACATC", { MutationId::DELETION } },

	{  0, { AA::His, AA::Pro, AA::Ala, AA::Ala}, "CCG" "CCC" "GGC" "CCG", { MutationId::DELETION, MutationId::INSERTION_A, MutationId::TRANSITION, MutationId::TRANSVERSION } },

	// GATTACA
	// CAT CAT
	// TAG GAG ACT CAT

	// Arg:CGT/CGC/CGA/CGG/AGA/AGG
	// Asp:GAT/GAC
	// Tyr:TTT/TTC
	// Leu:CTT/CTC/CTA/CTG/TTA/TTG
	// Ile ATT/ATC/ATA"

	//TODO: // introducing stop codon

};

NT getNucleotideIndex(char c) {
	switch(c) {
	//TODO: use reverse lookup of NucleotideInfo instead of hard-coding order...
	case 'T': return NT::T;
	case 'C': return NT::C;
	case 'A': return NT::A;
	case 'G': return NT::G;
	default: Assert (false, "Not a valid nucleotide character");
		return NT::A; //DUMMY value
	break;
	}
}

ALLEGRO_COLOR getNucleotideColor(NT idx, float shade) {
	Assert (shade >= 0 && shade <= 1.0, "shade is not in range");
	Assert ((int)idx >= 0 && (int)idx < NUM_NUCLEOTIDES, "idx is not in range");

	switch (idx) {
	case NT::T: return al_map_rgb_f(shade * 1.0, 0.0, 0.0); // T -> RED
	case NT::C: return al_map_rgb_f(0.0, 0.0, shade * 1.0); // C -> BLUE
	case NT::A: return al_map_rgb_f(0.0, 1.0 * shade, 0.0); // A -> GREEN
	case NT::G: return al_map_rgb_f(0.5 * shade, 0.5 * shade, 0.5 * shade); // G -> BLACK(or grey)
	default: return BLACK;
	}
}

class CodonTable {
private:
	vector<AA> codonTable;
	map<string, AA> aaIndexByThreeLetterCode;
public:
	CodonTable() {

		codonTable.resize(64);
		for (int i = 0; i < NUM_AMINO_ACIDS; ++i) {
			AA aa = static_cast<AA>(i);
			auto aaInfo = &aminoAcidInfo[i];
			for (auto codon : aaInfo->codons) {
				int codonIdx = codonIndexFromString(codon);
				codonTable[codonIdx] = aa;
			}

			aaIndexByThreeLetterCode[aaInfo->threeLetterCode] = aa;
		}

	}

	int getCodonIndex(int i, int j, int k) {
		Assert (i >= 0 && i < NUM_NUCLEOTIDES, "Invalid nucleotide");
		Assert (j >= 0 && j < NUM_NUCLEOTIDES, "Invalid nucleotide");
		Assert (k >= 0 && k < NUM_NUCLEOTIDES, "Invalid nucleotide");

		return i + j * NUM_NUCLEOTIDES + k * NUM_NUCLEOTIDES * NUM_NUCLEOTIDES;
	};

	/** Converts "ATG" to the index of the corresponding amino acid */
	int codonIndexFromString(const string &val) {
		Assert (val.size() == 3, "Not a valid codon string");

		return getCodonIndex(
			(int)getNucleotideIndex(val.at(0)),
			(int)getNucleotideIndex(val.at(1)),
			(int)getNucleotideIndex(val.at(2))
		);
	}

	AA getCodon(int i, int j, int k) {
		int idx = getCodonIndex(i, j, k);
		return codonTable[idx];
	}

	AA getIndexByThreeLetterCode(const string &threeLetterCode) {
		Assert (threeLetterCode.size() == 3, "Three letter code must have three letters");
		Assert (aaIndexByThreeLetterCode.find(threeLetterCode) != aaIndexByThreeLetterCode.end(), "Unknown three-letter code");
		return aaIndexByThreeLetterCode[threeLetterCode];
	}
};

CodonTable codonTable;

class CodonTableView : public IComponent {

	virtual void draw(const GraphicsContext &gc) override {

		int x1 = getx() + gc.xofst;
		int y1 = gety() + gc.yofst;

		al_draw_rectangle(x1 + 1, y1 + 1, getw() - 1, geth() - 1, WHITE, 1.0);

		const int COL_WIDTH = getw() / 4;
		const int OUTER_ROW_HEIGHT = geth() / 4;
		const int INNER_ROW_HEIGHT = 16;

		for (int i = 0; i < NUM_NUCLEOTIDES; ++i) {
			for (int j = 0; j < NUM_NUCLEOTIDES; ++j) {
				for (int k = 0; k < NUM_NUCLEOTIDES; ++k) {

					// cell position
					int xco = x1 + j * COL_WIDTH + getx();
					int yco = y1 + i * OUTER_ROW_HEIGHT + k * INNER_ROW_HEIGHT;
					AA aaIdx = codonTable.getCodon(i, j, k);
					const AminoAcidInfo *aai = &aminoAcidInfo[static_cast<int>(aaIdx)];
					if (aai) {
						draw_shaded_text(sfont, BLACK, GREEN, xco, yco, ALLEGRO_ALIGN_LEFT, aai->threeLetterCode.c_str());
					}

				}
			}
		}
	};

};

// generic Layout variables
const int AA_WIDTH = 96;
const int AA_SPACING = 16; // between AA cards
const int AA_HEIGHT = 64;
const int AA_STEPSIZE = AA_WIDTH + AA_SPACING;

const int AA_PADDING = 8; // padding on inside of AA card.

const int NT_WIDTH = 30;
const int NT_HEIGHT = 80;
const int NT_SPACING = 8;
const int NT_STEPSIZE = NT_WIDTH + NT_SPACING;

const int SECTION_X = 80;
const int TARGET_PEPT_Y = 180;
const int CURRENT_PEPT_Y = 280;
const int GENE_Y = 380;
const int RIBOSOME_Y = GENE_Y - 50;
const int CURSOR_Y = 460;

const int BUTTONW = 120;
const int BUTTONH = 16;

const int MUTCARD_W = 120;
const int MUTCARD_H = 40;

class Sprite
{
protected:
	ALLEGRO_BITMAP *sprite;

	float x; // cell-x
	float y;

	int w;
	int h;

	bool alive;
	bool visible;
	bool awake;

	ALLEGRO_FONT *font;
public:
	bool isAlive() { return alive; }
	void kill() { alive = false; /* scheduled to be removed at next update from any list that is updated */ }
	bool isVisible() { return visible; }
	void setAwake(bool value) { awake = value; }
	bool isAwake() { return awake; }

	virtual void draw(const GraphicsContext &gc) {
		if (sprite) {
			al_draw_bitmap(sprite, x + gc.xofst, y + gc.yofst, 0);
		}
	};

	virtual void update() {};
	float getx() { return x; }
	float gety() { return y; }
	void setxy (int _x, int _y) { x = _x; y = _y; }
	void setx(int _x) { x = _x; }
	void sety(int _y) { y = _y; }
	Sprite() : sprite(nullptr), x(0), y(0), w(16), h(16), alive(true), visible(true), awake(true) {
		font = Engine::getResources()->getFont("builtin_font");
	}
	virtual ~Sprite() {}
	virtual void handleAnimationComplete() {};
};

class GameImpl;

class NucleotideSprite : public Sprite, public enable_shared_from_this<NucleotideSprite> {
private:
	GameImpl *parent;
	NucleotideInfo *info;
	char code;
	int pos;
	NT idx;
	DissolveEffect dissolve; // TODO - make static but initialize at the right moment...
	bool isAnimating = false;
	const int MAX_COUNTER = 100;
	int counter = 0;
public:
	NucleotideSprite(GameImpl *parent, double x, double y, char code, int pos) : parent(parent), code(code), pos(pos) {
		this->x = x;
		this->y = y;
		w = NT_WIDTH;
		h = NT_HEIGHT;
		idx = getNucleotideIndex(code);
		info = &nucleotideInfo[(int)idx];
		sprite = info->card;
		Assert(sprite, "No valid sprite");
	}

	void drawBitmap(const GraphicsContext &gc) {
		al_draw_bitmap (sprite, x + gc.xofst, y + gc.yofst, 0);
	}

	virtual void draw(const GraphicsContext &gc) {
		if (isAnimating) {
			dissolve.withPattern( (float)counter/(float)MAX_COUNTER,
				[=]() {	drawBitmap(gc); }
			);
		}
		else {
			drawBitmap(gc);
		}
	};

	void update() {
		if (isAnimating) {
			counter++;
			if (counter > MAX_COUNTER) {
				kill();
				isAnimating = false;
			}
		}
	}

	void startDissolve() {
		isAnimating = true;
		counter = 0;
	}

	void moveToPos(int _pos);

	bool validate(int _pos, char _code) {
		return (pos == _pos) && (code == _code);
	}
};

void drawOutlinedRect(int x1, int y1, int x2, int y2, ALLEGRO_COLOR outer, ALLEGRO_COLOR inner, float w) {
	al_draw_filled_rectangle(x1, y1, x2, y2, inner);
	al_draw_rectangle(x1 + 1, y1 + 1, x2, y2, outer, w);
}

class CardRenderer {

	Resources *res;
	ALLEGRO_FONT *font;

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

		NucleotideInfo *info = &nucleotideInfo[idx];

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

public:
	static void drawCards() {
		CardRenderer renderer;
		renderer.drawNucleotideCards();
		renderer.drawRibosome();
	}

};

class MoveAnimator : public Sprite {

	float destX, destY;
	float srcX, srcY;
	shared_ptr<Sprite> target;
	int totalSteps;
	int currentStep;
public:
	MoveAnimator (const shared_ptr<Sprite> &target, float destX, float destY, int steps) :
		destX(destX), destY(destY), target(target), totalSteps(steps), currentStep(0) {
		srcX = target->getx();
		srcY = target->gety();
		visible = false;
	}

	virtual void update() {
		if (!target) return;

		// interpolate...
		float delta = (float)currentStep / (float)totalSteps;
		float newx = srcX + delta * (destX - srcX);
		float newy = srcY + delta * (destY - srcY);
		target->setxy(newx, newy);

		currentStep++;

		if (currentStep > totalSteps) {
			target->handleAnimationComplete();
			kill();
			target = nullptr;
		}
	}
};

//TODO - implement
class DissolveAnimator : public Sprite {
	DissolveAnimator (const shared_ptr<Sprite> &target) {
	}
};


class AminoAcidSprite : public Sprite {
private:
	AminoAcidInfo *info;
	AA aaIdx;
public:
	AminoAcidSprite(double x, double y, AA aaIdx) : aaIdx(aaIdx) {

		this->x = x;
		this->y = y;
		w = AA_WIDTH;
		h = AA_HEIGHT;
		Assert ((int)aaIdx >= 0 && (int)aaIdx < NUM_AMINO_ACIDS, "Invalid amino acid index");
		info = &aminoAcidInfo[static_cast<int>(aaIdx)];
	}

	virtual void draw(const GraphicsContext &gc) {
		double x1 = x + gc.xofst;
		double y1 = y + gc.yofst;

		al_draw_filled_rectangle(x1, y1, x1 + w, y1 + h, al_map_rgb_f(0.5, 0, 0));
		al_draw_rectangle(x1, y1, x1 + w, y1 + h, RED, 1.0);

		draw_shaded_textf(font, WHITE, GREY, x1 + AA_PADDING, y1 + AA_PADDING, ALLEGRO_ALIGN_LEFT, info->fullName.c_str());

		// draw the DNA target sequences...

		int yco = y1 + 20;
		const int INTERNAL_SPACING = 4;
		const int INTERNAL_WIDTH = (AA_WIDTH - (2 * AA_PADDING) - (2 * INTERNAL_SPACING)) / 3;
		const int INTERNAL_STEPSIZE = INTERNAL_WIDTH + INTERNAL_SPACING;
		const int INTERNAL_HEIGHT = 5;
		const int INTERNAL_VERTICAL_STEPSIZE = INTERNAL_HEIGHT + 2;

		for (auto codon : info->codons) {
			for (int c = 0; c < 3; ++c) {

				int xco = x1 + c * INTERNAL_STEPSIZE + AA_PADDING;

				char nt = codon.at(c);
				NT ntIdx = getNucleotideIndex(nt);
				ALLEGRO_COLOR mainCol = getNucleotideColor(ntIdx, 1.0);
				ALLEGRO_COLOR shadeCol = getNucleotideColor(ntIdx, 0.5);

				al_draw_filled_rectangle(xco, yco, xco + INTERNAL_WIDTH, yco + INTERNAL_HEIGHT, shadeCol);
				al_draw_rectangle(xco, yco, xco + INTERNAL_WIDTH, yco + INTERNAL_HEIGHT, mainCol, 1.0);
			}
			yco += INTERNAL_VERTICAL_STEPSIZE;
		}
	};

};

/** grouping of sprites
 *
 * Note that sprites may always be members of multiple groups.
 *
 * SpriteGroups may be used for
 * - drawing
 * - updating
 * - z-ranking
 * collections of sprites
 *
 * But also for laying them out together, building a hierarchy of sprites,
 * or simply remembering which group of sprites belong together
 */
class SpriteGroup : public Sprite {
private:
	list<shared_ptr<Sprite>> sprites;

public:

	void push_back(const shared_ptr<Sprite> &val) {
		sprites.push_back(val);
	}

	void push_front(const shared_ptr<Sprite> &val) {
		sprites.push_front(val);
	}

	void killAll() {
		for (auto sp : sprites) {
			sp->kill();
		}
		sprites.clear();
	}

	virtual void update() {
		if (!awake) return;

		for (auto i : sprites)
		{
			if (i->isAlive() && i->isAwake()) i->update();
		}

		sprites.remove_if ( [](shared_ptr<Sprite> i) { return !(i->isAlive()); });
	}

	virtual void draw (const GraphicsContext &gc) {
		for (auto i : sprites)
		{
			if (i->isAlive() && i->isVisible()) i->draw(gc);
		}
	}

	void move(const shared_ptr<Sprite> &target, float destx, float desty, int steps) {
		auto animator = make_shared <MoveAnimator>(target, destx, desty, steps);
		sprites.push_back(animator);
	}

};

enum { EVT_PEPT_CHANGED = 1, EVT_OLIGO_CHANGED };

class PeptideModel : public DataWrapper {

private:
	Peptide data;
public:

	PeptideModel() : data() {
	}

	void setValue(Peptide val) {
		data = val;
		FireEvent(EVT_PEPT_CHANGED);
	}

	size_t size() {
		return data.size();
	}

	AA at(int idx) {
		return data[idx];
	}

	Peptide getValue() {
		return data; // should be copy
	}
};


class DNAModel : public ListWrapper {
private:
	OligoNt data;
public:
	DNAModel() : data() {
	}

	void setValue(OligoNt val) {
		data = val;
		FireEvent(ListWrapper::FULL_CHANGE, 0);
	}

	size_t size() {
		return data.size();
	}

	char at(int idx) {
		return data.at(idx);
	}

	static Peptide translate(const OligoNt &myData) {
		Peptide pept;

		for (size_t i = 0; i < myData.size() - 2; i += 3) {

			AA aa = codonTable.getCodon(
					(int)getNucleotideIndex(myData.at(i)),
					(int)getNucleotideIndex(myData.at(i+1)),
					(int)getNucleotideIndex(myData.at(i+2))
			);
			pept.push_back(aa);
		}

		return pept;
	}

	Peptide translate() {
		return translate(data);
	}

	static char getComplement(char nt) {
		switch (nt) {
			case 'A': return 'T';
			case 'T': return 'A';
			case 'G': return 'C';
			case 'C': return 'G';
			default: Assert (false, "Invalid nucleotide");
			return ' ';
		}
	}

	static char getTransversion(char nt) {
		switch (nt) {
			case 'G': return 'T';
			case 'T': return 'G';
			case 'A': return 'C';
			case 'C': return 'A';
			default: Assert (false, "Invalid nucleotide");
			return ' ';
		}
	}

	static char getTransition(char nt) {
		switch (nt) {
			case 'G': return 'A';
			case 'A': return 'G';
			case 'T': return 'C';
			case 'C': return 'T';
			default: Assert (false, "Invalid nucleotide");
			return ' ';
		}
	}

	/** ignores stop codons and everything after */
	static bool match(Peptide aPep, Peptide bPep) {
		bool stopCodonFound = false;
		size_t pos = 0;
		while (true) {
			AA a = (pos < aPep.size() ? aPep.at(pos) : AA::STP);
			AA b = (pos < bPep.size() ? bPep.at(pos) : AA::STP);

			if (a != b) { return false; }
			if (a == AA::STP) { return true; }
			pos++;
		}
	}

	static OligoNt applyMutation(const OligoNt &src, int pos, MutationId mutation) {
		OligoNt data = src;
		switch (mutation) {
		case MutationId::COMPLEMENT:
			data[pos] = getComplement(data[pos]);
			break;
		case MutationId::TRANSVERSION:
			data[pos] = getTransversion(data[pos]);
			break;
		case MutationId::TRANSITION:
			data[pos] = getTransition(data[pos]);
			break;
		case MutationId::DELETION:
			data.erase(pos, 1);
			break;
		case MutationId::REVERSE_COMPLEMENT:
			data = reverseComplement(src);
			break;
		case MutationId::INSERTION_A:
			data.insert(pos, "A");
			break;
		case MutationId::INSERTION_C:
			data.insert(pos, "C");
			break;
		case MutationId::INSERTION_G:
			data.insert(pos, "G");
			break;
		case MutationId::INSERTION_T:
			data.insert(pos, "T");
			break;
		default:
			Assert (false, "Invalid mutation id");
			break;
		}
		return data;
	};

	void applyMutation (int pos, MutationId mutation) {

		Assert (pos >= 0 && pos < (int)data.size(), "pos is out of range");
		OligoNt oldData = data;
		data = applyMutation(data, pos, mutation);


		if (oldData != data) {

			switch (mutation) {
			case MutationId::COMPLEMENT:
			case MutationId::TRANSITION:
			case MutationId::TRANSVERSION:
				FireEvent(ListWrapper::SINGLE_CHANGE, pos);
				break;
			case MutationId::DELETION:
				FireEvent(ListWrapper::DELETE, pos);
				break;
			case MutationId::INSERTION_A:
			case MutationId::INSERTION_C:
			case MutationId::INSERTION_T:
			case MutationId::INSERTION_G:
				FireEvent(ListWrapper::INSERT, pos);
				break;
			default:
				FireEvent(ListWrapper::FULL_CHANGE, 0);
				break;
			}
		}
	}

	/** in-place modification. Turn this DNA sequence into its reverse complement */
	static OligoNt reverseComplement(const OligoNt &src) {
		OligoNt newData;
		for (auto it = src.rbegin(); it != src.rend(); it++) {
			newData += getComplement(*it);
		}
		return newData;
	}
};

class GameImpl;

class MutationCursor : public Sprite {
	GameImpl *parent;
	int pos;
	int counter = 0;
public:
	int getPos() { return pos; }
	DissolveEffect dissolve;
	MutationCursor(GameImpl *parent) : parent(parent), pos(0) {
		w = 32;
		h = 32;
		x = 0;
		h = 0;
		setPos(0);
	}

	void setPos(int newpos) {
		pos = newpos;
		counter = 0;
	}

	virtual void update() override {
		counter++;
	}

	virtual void draw(const GraphicsContext &gc) override {

		double x1 = getx() + gc.xofst;
		double y1 = gety() + gc.yofst;

		ALLEGRO_COLOR color1 = BLACK;
		ALLEGRO_COLOR color2 = WHITE;

		if ((counter / 5) % 2== 0) {
			color2 = YELLOW;
		}

		al_draw_line (x1, y1 + 16, x1 + 16, y1, color1, 5.0);
		al_draw_line (x1 + 16, y1, x1 + 32, y1 + 16, color1, 5.0);

		al_draw_line (x1, y1 + 16, x1 + 16, y1, color2, 3.0);
		al_draw_line (x1 + 16, y1, x1 + 32, y1 + 16, color2, 3.0);
	}
};

struct Solution {
	vector<MutationId> mutations;
	vector<int> positions;
};

class PuzzleAnalyzer {
private:
	LevelInfo level;
	Solution currentSolution;
	map<Peptide, int> solutionFrequency;
	ostream &os;

	PuzzleAnalyzer(LevelInfo level, ostream &os) : level(level), os(os) {

		// initalize currentSolutions
		currentSolution = firstSolution(level);
	}

	static Solution firstSolution(LevelInfo level) {
		Solution result;
		result.mutations = level.mutationCards;
		std::sort (result.mutations.begin(),
				result.mutations.end());
		result.positions.resize(result.mutations.size(), 0);
		return result;
	}

	static bool nextSolution(Solution &solution, const LevelInfo &level) {
		int size = level.startGene.size();
		// calculate the maximums...
		vector<int> sizes;

		for (auto mut : solution.mutations) {
			switch (mut) {
			case MutationId::REVERSE_COMPLEMENT:
				sizes.push_back(0);
				break;
			case MutationId::COMPLEMENT:
			case MutationId::TRANSITION:
			case MutationId::TRANSVERSION:
				sizes.push_back (size);
				break;
			case MutationId::INSERTION_A:
			case MutationId::INSERTION_C:
			case MutationId::INSERTION_T:
			case MutationId::INSERTION_G:
				sizes.push_back (size);
				size++;
				break;
			case MutationId::DELETION:
				sizes.push_back (size);
				size--;
				break;
			}
		}

		// transform the positions
		int pos = solution.mutations.size() - 1;

		bool carry = true;
		while (carry) {
			solution.positions[pos] += 1;
			if (solution.positions[pos] >= sizes[pos]) {
				solution.positions[pos] = 0;
				carry = true;
				pos--;
				if (pos < 0) {
					break;
				}
			} else {
				carry = false;
			}
		}

		bool result = true;
		if (carry) {
			result = std::next_permutation(solution.mutations.begin(), solution.mutations.end());
		}

		return result;
	}

	static int distanceScore(const Peptide &src, const Peptide &dest) {
		int minSize = min(src.size(), dest.size());
		int maxSize = max(src.size(), dest.size());

		int score = maxSize - minSize;

		for (int i = 0; i < minSize; i++) {
			if (src.at(i) != dest.at(i)) score++;
		}
		return score;
	}

	static string peptideToString(const Peptide &pept) {
		stringstream ss;
		for (size_t i = 0; i < pept.size(); ++i) {
			ss << aminoAcidInfo[static_cast<int>(pept.at(i))].threeLetterCode;
		}
		return ss.str();
	}

	static void analyseSolution(const Solution &solution, const LevelInfo &level, map<Peptide, int> &solutionFrequency, ostream &os) {

		OligoNt gene = level.startGene;
		Peptide pept;

		for (size_t i = 0; i < solution.mutations.size(); ++i) {

			os << (int)solution.mutations[i] << "#" << solution.positions[i] << " ";
			gene = DNAModel::applyMutation(gene, solution.positions[i], solution.mutations[i]);

			pept = DNAModel::translate(gene);
			if (DNAModel::match (pept, level.targetPeptide)) break; // already solved
		}

		os << gene << " " << peptideToString(pept);

		int dist = distanceScore(pept, level.targetPeptide);
		os << " " << dist;

		if (DNAModel::match(pept, level.targetPeptide)) { os << " *"; }
		os<< endl;

		if (solutionFrequency.find(pept) == solutionFrequency.end()) {
			solutionFrequency[pept] = 1;
		}
		else {
			solutionFrequency[pept] += 1;
		}
	}

	void showAnalysis() {
		os << "Solution frequency:" << endl;
		for (auto pair : solutionFrequency) {
			os << peptideToString(pair.first);
			os << " " << pair.second << endl;
		}
	}

	void bruteForce() {
		vector<MutationId> mutationCards = level.mutationCards;
		OligoNt current = level.startGene;
		std::sort (mutationCards.begin(), mutationCards.end());
		do {
			analyseSolution(currentSolution, level, solutionFrequency, os);
		} while (nextSolution(currentSolution, level));

		showAnalysis();
	}

public:

	static void analyze(LevelInfo level) {

		std::ofstream ofs ("analysis.txt", std::ofstream::out);
		PuzzleAnalyzer a = PuzzleAnalyzer(level, ofs);
		a.bruteForce();
		ofs.close();
	}
};

class Ribosome : public Sprite, public enable_shared_from_this<Ribosome> {
private:
	GameImpl *parent;
	int aaPos = -1;
	bool animating = false;
public:
	Ribosome(GameImpl *parent) : parent(parent) {
		sprite = Engine::getResources()->getBitmap("RIBOSOME");
		y = RIBOSOME_Y;
		x = -100;
	}

	virtual void update() override {

	}

	virtual void handleAnimationComplete() override {
		nextAnimation();
	}

	void nextAnimation();

};

class MutationCardSprite : public Sprite {

	MutationId mutationId;
	MutationInfo *info;
	bool focus = false;
	int counter = 0;
public:
	int origX;
	int origY;
	MutationCardSprite(MutationId mutationId, int xco, int yco) : mutationId(mutationId) {
		w = MUTCARD_W;
		h = MUTCARD_H;
		x = xco;
		y = yco;
		info = &mutationInfo[static_cast<int>(mutationId)];
		ALLEGRO_FONT *font = Engine::getResources()->getFont("builtin_font");
		origX = xco;
		origY = yco;
	}

	MutationId getMutationId() { return mutationId; }

	void setFocus(bool value) {
		focus = value;
		counter = 0;
	}

	virtual void update () override {
		counter ++;
	}

	virtual void draw (const GraphicsContext &gc) override {

		ALLEGRO_COLOR color1 = al_map_rgb_f(0, 0.5, 0);
		ALLEGRO_COLOR color2 = al_map_rgb_f(0.5, 0.5, 0);

		ALLEGRO_COLOR color = (focus && ((counter / 5) % 2 == 0) ? color2 : color1);

		double x1 = x + gc.xofst;
		double y1 = y + gc.yofst;
		double x2 = x1 + w;
		double y2 = y1 + h;
		al_draw_filled_rectangle(x1, y1, x2, y2, color);
		al_draw_rectangle(x1, y1, x2, y2, GREEN, 1.0);

		draw_shaded_textf(font, WHITE, GREY, x1 + 5, y1 + 5, ALLEGRO_ALIGN_LEFT, info->name.c_str());

		drawLogo (40, 20);
	}

	void drawLogo (int dx, int dy) {
		const int SIZE = 6;
		const int SIZE_2 = SIZE / 2;
		const int SPACING = 6;
		const int STEP = SIZE + SPACING;

		int xx = dx + getx();
		int yy = dy + gety();

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

class Controller : public Container {
private:
	GameImpl *parent;
	list<shared_ptr<MutationCardSprite>> cards;
	shared_ptr<MutationCursor> mutationCursor;
	list<shared_ptr<MutationCardSprite>>::iterator selectedCard;

public:
	Controller(GameImpl *parent) : parent(parent) {
	}

	// wrappers for GameImpl functions, so we can keep the main body here...
	void addToWorld(const shared_ptr<Sprite> &val);

	void handleEvent(ALLEGRO_EVENT &event);

	void initMutationCards(vector<MutationId> mutations) {

		cards.clear();

		int yco = gety();

		int i = 0;

		for (auto mut : mutations) {
			int xco = getx();
			if (i % 2 == 0) { xco += BUTTONW + 10; }

			auto mutCard = make_shared<MutationCardSprite>(mut, xco, yco);

			addToWorld(mutCard);

			if (i % 2 == 1) {
				yco += MUTCARD_H + 4;
			}

			cards.push_back(mutCard);
			i++;
		}

		selectedCard = cards.begin();
		(*selectedCard)->setFocus(true);

	}

	void changeCardFocus(int delta) {
		if (cards.size() == 0) return;

		int maxIt = cards.size();
		auto oldSelectedCard = selectedCard;

		if (selectedCard == cards.end()) selectedCard = cards.begin();

		while (selectedCard != cards.end() && maxIt >= 0) {
			if (delta > 0) {
				selectedCard++;
				if (selectedCard == cards.end()) selectedCard = cards.begin();
			}
			else {
				if (selectedCard == cards.begin()) {
					// TODO - ugly. No good way to wrap a bidirectional iterator?
					for (size_t i = 0; i < cards.size() - 1; ++i) { selectedCard++; }
				} else {
					selectedCard--;
				}
			}
			maxIt--;
		}

		if (oldSelectedCard != selectedCard) {
			if (oldSelectedCard != cards.end()) {
				(*oldSelectedCard)->setFocus(false);
			}
			if (selectedCard != cards.end()) {
				(*selectedCard)->setFocus(true);
			}
		}
	}

	void createMutationCursor() {
		if (mutationCursor) { mutationCursor->kill(); }
		mutationCursor = make_shared<MutationCursor>(parent);
		mutationCursor->setxy(SECTION_X, CURSOR_Y);
		addToWorld(mutationCursor);
	}

	// no draw method... just controlling the game

};

class TextBalloon : public IComponent {

	virtual void draw (const GraphicsContext &gc) override {
		const double shadow = 10;

		double x1 = getx() + gc.xofst;
		double y1 = gety() + gc.yofst;
		double x2 = x1 + getw() - shadow;
		double y2 = y1 + geth() - shadow;

		const double rx = 10;
		const double ry = 10;

		al_draw_filled_rounded_rectangle(x1 + shadow, y1 + shadow, x2 + shadow, y2 + shadow, rx, ry, GREY);
		al_draw_filled_rounded_rectangle(x1, y1, x2, y2, rx, ry, WHITE);
		al_draw_rounded_rectangle (x1, y1, x2, y2, rx, ry, BLACK, 2.0);
	};
};

class GameImpl : public Game {
private:
	int currentLevel;
public:
	SpriteGroup world;
private:

	// positions in this vector should match the current DNAmodel...
	vector<shared_ptr<NucleotideSprite>> geneGroup;

	SpriteGroup targetPeptideGroup;
	SpriteGroup currentPeptideGroup;

	DNAModel currentDNA;
	PeptideModel targetPeptide;
	PeptideModel currentPeptide;
	vector<MutationId> currentMutationCards; // TODO: also as model?

	shared_ptr<Controller> menu;

	shared_ptr<Text> drText;

	shared_ptr<Container> popup;
	shared_ptr<AnimComponent> patient;

	ActionFunc popupAction;

	Script::iterator currentScript;
	Script::iterator currentScriptEnd;

	bool uiEnabled = true;
public:

	bool isUIEnabled() { return uiEnabled; }

	GameImpl() : currentLevel(0), world() {

		menu = make_shared<Controller>(this);
		menu->setLayout(Layout::RIGHT_BOTTOM_W_H, 10, 10, (BUTTONW * 2) + 30, (BUTTONH + 10) * 8);
		add(menu);
		setFocus(menu);

		currentDNA.AddListener( [=] (int code, int pos) {
			currentPeptide.setValue(currentDNA.translate());

			switch(code) {
				default:
					generateGeneSprites();
					break;
				case ListWrapper::DELETE:
					geneSpritesDelete(pos);
					break;
				case ListWrapper::INSERT:
					geneSpritesInsert(pos);
					break;
				case ListWrapper::SINGLE_CHANGE:
					mutateGene(pos);
					break;
			};

		});

		currentPeptide.AddListener( [=] (int code) {

			generateRibosome();
			currentPeptideGroup.killAll();
		});

		targetPeptide.AddListener( [=] (int code) {
			peptideToSprites(targetPeptide, targetPeptideGroup, SECTION_X, TARGET_PEPT_Y);
		});

		auto button = Button::build([=](){ resetLevel(); }, "Reset (F1)")
			.layout(Layout::RIGHT_BOTTOM_W_H, 10, 10, 120, 24).get();
		add(button);

		auto b2 = Button::build([=](){ pauseScreen(); }, "Pause (Space)")
			.layout(Layout::RIGHT_BOTTOM_W_H, 140, 10, 120, 24).get();
		add(b2);

		Resources *res = Engine::getResources();
		auto img1 = BitmapComp::build(res->getBitmap("DrRaul01")).xywh(0, 10, 130, 130).get();
		add(img1);

		patient = AnimComponent::build(res->getAnim("Bigbunnybed")).layout(Layout::RIGHT_TOP_W_H, 0, 20, 300, 200).get();
		add(patient);

		auto balloon = make_shared<TextBalloon>();
		balloon->setLayout(Layout::LEFT_TOP_RIGHT_H, 135, 5, 250, 120);
		add (balloon);
		drText = Text::build(BLACK, ALLEGRO_ALIGN_LEFT, "").layout (Layout::LEFT_TOP_RIGHT_H, 140, 15, 255, 110).get();
		add(drText);

	}

	void resetLevel() {
		initLevel();
	}

	virtual ~GameImpl() {
	}

	virtual void initGame() override {
		initLevelAndScript();
	}

	// sanity test at startup. TODO: should be unit test
	void test() {
		assert (! DNAModel::match(Peptide{ AA::Tyr }, Peptide{ AA::Thr }));
		assert (! DNAModel::match(Peptide{ AA::Tyr }, Peptide{ }));
		assert (! DNAModel::match(Peptide{ AA::Tyr }, Peptide{ AA::Thr }));
		assert (! DNAModel::match(Peptide{ AA::Tyr }, Peptide{ AA::Tyr, AA::Tyr }));

		assert (DNAModel::match(Peptide{}, Peptide{}));
		assert (DNAModel::match(Peptide{ AA::Ile }, Peptide{ AA::Ile }));
		assert (DNAModel::match(Peptide{ AA::Ile }, Peptide{ AA::Ile, AA::STP }));
		assert (DNAModel::match(Peptide{ AA::STP }, Peptide{}));
		assert (DNAModel::match(Peptide{ AA::Ile, AA::STP, AA::Val }, Peptide{ AA::Ile }));
		assert (DNAModel::match(Peptide{ AA::Ile, AA::STP, AA::Val }, Peptide{ AA::Ile, AA::STP }));
	}

	virtual void init() override {
		test();
		CardRenderer::drawCards();
	}

	shared_ptr<AminoAcidSprite> peptideToSprite(PeptideModel &pep, SpriteGroup &group, int ax, int ay, int pos) {

		int xco = ax;
		int yco = ay;
		xco += AA_SPACING / 2;
		xco += pos * AA_STEPSIZE;

		AA aaIdx = pep.at(pos);

		auto aaSprite = make_shared<AminoAcidSprite>(xco, yco, aaIdx);
		world.push_front(aaSprite);
		group.push_back(aaSprite);

		return aaSprite;
	}

	void currentPeptideToSprite(int pos) {
		int xco = SECTION_X;
		int yco = CURRENT_PEPT_Y;
		xco += AA_SPACING / 2;
		xco += pos * AA_STEPSIZE;

		auto aa = peptideToSprite(currentPeptide, currentPeptideGroup, SECTION_X, RIBOSOME_Y, pos);
		world.move(aa, xco, yco, 40);
	}

	int getCurrentPeptideSize() {
		return currentPeptide.size();
	}

	void peptideToSprites(PeptideModel &pep, SpriteGroup &group, int ax, int ay) {

		group.killAll();

		int xco = ax;
		int yco = ay;

		xco += AA_SPACING / 2;

		for (size_t i = 0; i < pep.size(); ++i) {
			peptideToSprite(pep, group, ax, ay, i);
		}
	}

	void setUIEnabled(bool value) {
		uiEnabled = value;
	}

	void generateRibosome() {
		auto ribo = make_shared<Ribosome>(this);
		ribo->nextAnimation();

		checkWinCondition();
		world.push_back(ribo);
	}

	void generateGeneSprite(int pos) {
		int xco = SECTION_X + NT_STEPSIZE * pos;
		int yco = GENE_Y;

		if ((int)geneGroup.size() <= pos) {
			geneGroup.resize(pos + 1);
		}

		char nt = currentDNA.at(pos);

		auto ntSprite = make_shared<NucleotideSprite>(this, xco, yco, nt, pos);
		world.push_front(ntSprite); // insert below any dissolving sprites...
		geneGroup[pos] = ntSprite;
	}

	void geneSpritesDelete(int pos) {

		if (geneGroup[pos]) {
			geneGroup[pos]->startDissolve();
		}

		for (size_t i = pos; i < currentDNA.size(); ++i) {
			geneGroup[i] = geneGroup[i+1];
			geneGroup[i]->moveToPos(i);
		}
		geneGroup[currentDNA.size()] = nullptr;

		generateGeneSprite(pos);
		validateGeneSprites();
	}

	void geneSpritesInsert(int pos) {

		if (geneGroup.size() <= currentDNA.size()) {
			geneGroup.resize(currentDNA.size());
		}

		for (int i = currentDNA.size() - 1; i > pos; i --) {
			geneGroup[i] = geneGroup[i-1];
			geneGroup[i]->moveToPos(i);
		}

		generateGeneSprite(pos);
		validateGeneSprites();
	}

	void mutateGene(int pos) {
		if (geneGroup[pos]) {
			geneGroup[pos]->startDissolve();
		}
		generateGeneSprite(pos);
		validateGeneSprites();
	}

	void validateGeneSprites() {
		Assert(geneGroup.size() >= currentDNA.size(), "missing a gene sprite");
		for (size_t i = 0; i < currentDNA.size(); ++i) {
			Assert (geneGroup[i], "Missing a gene sprite");
			Assert (geneGroup[i]->validate(i, currentDNA.at(i)), "Gene sprite does not validate");
		}
	}

	void generateGeneSprites() {

		// clear all pre-existing nucleotides.
		for (size_t i = 0; i < geneGroup.size(); ++i) {
			if (geneGroup[i]) { geneGroup[i]->kill(); }
		}

		// generate fresh ones
		for (size_t i = 0; i < currentDNA.size(); ++i) {
			generateGeneSprite(i);
		}

		validateGeneSprites();
	}

	void advanceLevel() {
		currentLevel++;

		if (currentLevel >= NUM_LEVELS) {
			showMessage("You finished all levels, you won the game!",
					[=] () { pushMsg(Engine::E_QUIT); } );

		}
		else {
			showMessage("Level Complete! Press any key...",
					[=] () { initLevelAndScript(); } );
		}
	}

	void checkWinCondition() {
		if (DNAModel::match (currentPeptide.getValue(), targetPeptide.getValue())) {

			// immediately block UI to prevent modifying the sequence any further
			setUIEnabled(false);

			// we have to wait for the Ribosome to finish. Ideally we have a signal that it's ready...
			auto t1 = Timer::build(200, [=] () {

				setUIEnabled(true);
				advanceLevel();

			} ).get();

			add(t1);
		}
	}

	vector<ALLEGRO_SAMPLE_ID> started;

	void startLoop(const string &id)
	{
		ALLEGRO_SAMPLE_ID sampleid;
		ALLEGRO_SAMPLE *sample_data = Engine::getResources()->getSampleIfExists(id);

		if (!sample_data)
		{
			cout << "Could not find sample";
			return;
		}

		bool success = al_play_sample (sample_data, 1.0, 0.5, 1.0, ALLEGRO_PLAYMODE_LOOP, &sampleid);
		if (!success) {
			 cout << "Could not start sample";
		}

		started.push_back(sampleid);
	}

	void clearSound() {

		// stop any looped samples, leaving play-once samples untouched.
		for (vector<ALLEGRO_SAMPLE_ID>::iterator i = started.begin(); i != started.end(); ++i)
		{
			al_stop_sample(&(*i));
		}
		started.clear();
	}


	void pauseScreen() {
		if (popup) return; // can't pause twice...

		startLoop("junglebeat");

		popup = Container::build().layout(Layout::LEFT_TOP_RIGHT_H, 0, 200, 0, 200).get();
		popup->add (make_shared<ClearScreen>(al_map_rgba(0, 0, 0, 128)));

		auto t1 = Text::build(WHITE, ALLEGRO_ALIGN_CENTER, string("PAUSE")).layout (Layout::LEFT_TOP_RIGHT_H, 0, 50, 0, 100).get();
		popup->add (t1);

		Resources *res = Engine::getResources();
		auto img2 = AnimComponent::build(res->getAnim("dance")).layout(Layout::CENTER_TOP_W_H, 0, 100, 64, 64).get();
//		img2->setZoom(2.0);
		popup->add(img2);

		popupAction = [=](){
			pauseComponents(true);
			clearSound();
		};
		add (popup);

		pauseComponents(false);
	}

	void pauseComponents(bool val) {
		drText->setAwake(val);
		patient->setAwake(val);
		world.setAwake(val);
	}

	void showMessage(const char *text, ActionFunc actionFunc) {

		popup = Container::build().layout(Layout::LEFT_TOP_RIGHT_H, 0, 200, 0, 200).get();
		popup->add (make_shared<ClearScreen>(al_map_rgba(0, 0, 0, 128)));

		auto t1 = Text::build(WHITE, ALLEGRO_ALIGN_CENTER, string(text)).layout (Layout::LEFT_TOP_RIGHT_H, 0, 50, 0, 100).get();
		popup->add (t1);

		popupAction = actionFunc;
		add (popup);
	}

	void closePopup() {
		if (popup) {
			popup->kill();
			popup = nullptr;
		}
	}

	void initLevelAndScript() {
		Assert (currentLevel >= 0 && currentLevel < NUM_LEVELS, "currentLevel is out of range");
		LevelInfo *lev = &levelInfo[currentLevel];

		currentScript = scripts[lev->scriptId].begin();
		currentScriptEnd = scripts[lev->scriptId].end();

//		PuzzleAnalyzer::analyze(*lev);

		initLevel();
		nextScriptStep();
	}

	void setDoctorText(const string &text) {
		drText->setText(text);
		drText->startTypewriter(Text::LETTER_BY_LETTER, 2);
		drText->onAnimationComplete([=] () {
			add (Timer::build(100, [=]() {
				nextScriptStep();
			}).get());
		});
	}

	void nextScriptStep() {
		bool done = false;
		while (!done)
		{

			if (currentScript == currentScriptEnd) {
				break;
			}
				// execute step

			switch (currentScript->cmd) {
				case Cmd::SAY:
					setDoctorText (currentScript->text);
					done = true;
					break;
				case Cmd::BIGEYES:
					patient->setState(1);
					break;
				case Cmd::NORMALEYES:
					patient->setState(0);
					break;
				default:
					break;
			}

			// advance pointer
			currentScript++;
		}
	}

	void initLevel() {
		world.killAll();

		Assert (currentLevel >= 0 && currentLevel < NUM_LEVELS, "currentLevel is out of range");
		LevelInfo *lev = &levelInfo[currentLevel];

		currentDNA.setValue(lev->startGene);

		Peptide pept = lev->targetPeptide;
		targetPeptide.setValue(pept);

		currentMutationCards = lev->mutationCards;
		menu->initMutationCards(currentMutationCards);

		drText->setText("");
	}

	int getOligoSize() {
		return currentDNA.size();
	}

	void applyMutation (int pos, MutationId mutationId) {
		// first remove mutation card, before triggering level check etc...
		auto it = std::find(currentMutationCards.begin(), currentMutationCards.end(), mutationId);
		if (it != currentMutationCards.end()) {
			currentMutationCards.erase(it);
		}

		currentDNA.applyMutation(pos, mutationId);

		patient->setState(2);
		auto t1 = Timer::build(100, [=](){ patient->setState(0); }).get();
		add(t1);
	}

	virtual void update() override {
		world.update();
		Container::update();
	}

	virtual void draw(const GraphicsContext &gc) override {
		al_clear_to_color(al_map_rgb(172,232,135));
		if (Engine::isDebug())
		{
			al_draw_text(Engine::getFont(), LIGHT_BLUE, 0, 0, ALLEGRO_ALIGN_LEFT, "DEBUG ON");
		}

		world.draw(gc);
		Container::draw(gc);
	}

	void showCodonTable() {

		popup = Container::build().layout(Layout::LEFT_TOP_RIGHT_BOTTOM, 100, 100, 100, 100).get();
		popup->add (make_shared<ClearScreen>(WHITE));

		auto codonTableView = make_shared<CodonTableView>();
		codonTableView->setLayout(Layout::LEFT_TOP_RIGHT_BOTTOM, 40, 40, 40, 40);
		popup->add (codonTableView);

		popupAction = [=](){ pauseComponents(true); };
		add (popup);

		pauseComponents(false);
	}

	virtual void handleEvent(ALLEGRO_EVENT &event) override {

		if (popup) {
			if (event.type == ALLEGRO_EVENT_KEY_CHAR ||
				event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
					popupAction();
					closePopup();
			}
		}
		else {
			Container::handleEvent(event);

			if (event.type == ALLEGRO_EVENT_KEY_CHAR) {
				switch (event.keyboard.keycode) {
					case ALLEGRO_KEY_F2:
						showCodonTable();
						break;
					case ALLEGRO_KEY_F1:
						resetLevel();
						break;
					case ALLEGRO_KEY_SPACE:
						pauseScreen();
						break;
				}
			}

		}
	}

};

shared_ptr<Game> Game::newInstance()
{
	return make_shared<GameImpl>();
}


/*
void MutationCard::MsgLPress(const Point &p) {
	parent->createMutationCursor(mutationId);
}
*/

void NucleotideSprite::moveToPos(int _pos) {
	int xco = SECTION_X + NT_STEPSIZE * _pos;
	int yco = GENE_Y;
	pos = _pos;
	parent->world.move(shared_from_this(), xco, yco, 50);
}

void Ribosome::nextAnimation() {
	if (aaPos >= parent->getCurrentPeptideSize()) {
		kill();
		return;
	}
	else if (aaPos >= 0)
	{
		parent->currentPeptideToSprite(aaPos);
	}

	aaPos++;

	if (aaPos >= parent->getCurrentPeptideSize()) {
		// fall down
		parent->world.move(shared_from_this(), x, 600, 50);
	}
	else {
		int xco = aaPos * AA_STEPSIZE + (AA_SPACING / 2) + SECTION_X;
		int yco = RIBOSOME_Y;
		parent->world.move(shared_from_this(), xco, yco, 50);
	}
}

void Controller::addToWorld(const shared_ptr<Sprite> &val) {
	parent->world.push_back(val);
}

void Controller::handleEvent(ALLEGRO_EVENT &event) {

	if (event.type == ALLEGRO_EVENT_KEY_CHAR) {
		if (mutationCursor) {

			int pos = mutationCursor->getPos();
			int newpos = pos;

			switch (event.keyboard.keycode) {
			case ALLEGRO_KEY_LEFT:
			case ALLEGRO_KEY_A:
			case ALLEGRO_KEY_PAD_4:
			case ALLEGRO_KEY_TAB:
				newpos = max (0, pos-1);
				break;
			case ALLEGRO_KEY_RIGHT:
			case ALLEGRO_KEY_D:
			case ALLEGRO_KEY_PAD_6:
				newpos = min(pos+1, parent->getOligoSize() - 1);
				break;
			case ALLEGRO_KEY_ESCAPE:
				// move card back to where it came from...
				(*selectedCard)->setFocus(true);
				parent->world.move(*selectedCard, (*selectedCard)->origX, (*selectedCard)->origY, 20);
				mutationCursor->kill();
				mutationCursor = nullptr;
				break;
			case ALLEGRO_KEY_ENTER:
			case ALLEGRO_KEY_PAD_ENTER:
				if (!parent->isUIEnabled()) {
					//TODO: error buzzer sample
				}
				else {
					parent->applyMutation(pos, (*selectedCard)->getMutationId());

					(*selectedCard)->kill();
					selectedCard = cards.erase(selectedCard);
					if (selectedCard == cards.end()) selectedCard = cards.begin();
					if (selectedCard != cards.end()) {
						(*selectedCard)->setFocus(true);
					}

					mutationCursor->kill();
					mutationCursor = nullptr;
				}
				break;
			}

			if (pos != newpos) {
				mutationCursor->setPos(newpos);
				int newx = (SECTION_X + NT_STEPSIZE * newpos);
				parent->world.move(mutationCursor, newx, mutationCursor->gety(), 10);
			}
		}

		else {

			switch (event.keyboard.keycode) {
			case ALLEGRO_KEY_ENTER:
			case ALLEGRO_KEY_PAD_ENTER:
				if (selectedCard != cards.end()) {
					createMutationCursor();
					(*selectedCard)->setFocus(false);
					parent->world.move(*selectedCard, SECTION_X + 0, GENE_Y + 120, 20);
					break;
				}
			case ALLEGRO_KEY_LEFT:
			case ALLEGRO_KEY_UP:
			case ALLEGRO_KEY_PGUP:
			case ALLEGRO_KEY_W:
			case ALLEGRO_KEY_A:
			case ALLEGRO_KEY_PAD_4:
			case ALLEGRO_KEY_PAD_8:
				changeCardFocus(-1);
				break;
			case ALLEGRO_KEY_RIGHT:
			case ALLEGRO_KEY_DOWN:
			case ALLEGRO_KEY_PGDN:
			case ALLEGRO_KEY_S:
			case ALLEGRO_KEY_D:
			case ALLEGRO_KEY_PAD_6:
			case ALLEGRO_KEY_PAD_2:
			case ALLEGRO_KEY_TAB:
				changeCardFocus(+1);
				break;
			}
		}

	}
}
