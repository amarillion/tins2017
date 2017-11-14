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

#endif
