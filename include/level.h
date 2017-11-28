#ifndef _LEVEL_H_
#define _LEVEL_H_

#include "molbi.h"

struct LevelInfo {
	int scriptId; // -1 for none
	int endScriptId;
	Peptide targetPeptide;
	OligoNt startGene;

	// cards available in this level
	std::vector<MutationId> mutationCards;
};

enum class Cmd { SAY, BIGEYES, NORMALEYES, ACTIVATE_ALL, ACTIVATE_GENE, ACTIVATE_TARGET, ACTIVATE_TRANSLATION,
		WAIT_FOR_KEY, WAIT_FOR_MUTATION_SELECTED, WHEN_MATCH, ADVANCE_LEVEL };

struct Statement {
	Cmd cmd;
	std::string text;
};

using Script = std::vector<Statement>;

const int NUM_SCRIPTS = 13;
extern Script scripts[NUM_SCRIPTS];
const int RESET_SCRIPT_ID = 12;

const int NUM_LEVELS = 12;
extern LevelInfo levelInfo[NUM_LEVELS];

#endif
