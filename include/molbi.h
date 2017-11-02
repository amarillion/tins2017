#ifndef _MOLBI_H_
#define _MOLBI_H_

#include <string>
#include <vector>
#include <map>
#include <allegro5/allegro.h>

enum class AA {
	Ala, Arg, Asn, Asp, Cys, Gln, Glu, Gly, His, Ile, Leu, Lys, Met, Phe, Pro, Ser, Thr, Trp, Tyr, Val, STP
};

struct AminoAcidInfo {
	char code;
	std::string threeLetterCode;
	std::string fullName;
	std::vector<std::string> codons;
	ALLEGRO_BITMAP *bmpFull;
	ALLEGRO_BITMAP *bmpSimple;
};

const int NUM_AMINO_ACIDS = 21;
extern AminoAcidInfo aminoAcidInfo[NUM_AMINO_ACIDS];

enum class MutationId {
	TRANSVERSION, // transversion:  G<->T   A<->C
	TRANSITION, // transition:    G<->A   T<->C
	COMPLEMENT, // complement:    G<->C   A<->T
	REVERSE_COMPLEMENT,
	INSERTION_A,
	INSERTION_C,
	INSERTION_T,
	INSERTION_G,
	DELETION,
};

struct MutationInfo {
	std::string name;
};

const int NUM_MUTATIONS = 9;
const MutationInfo mutationInfo[NUM_MUTATIONS] {
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
	std::string name;
	ALLEGRO_BITMAP *card;
};

enum class NT { T, C, A, G };

extern NucleotideInfo nucleotideInfo[NUM_NUCLEOTIDES];

using Peptide = std::vector<AA>;
using OligoNt = std::string;

NT getNucleotideIndex(char c);

class CodonTable {
private:
	std::vector<AA> codonTable;
	std::map<std::string, AA> aaIndexByThreeLetterCode;
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

	int getCodonIndex(int i, int j, int k);

	/** Converts "ATG" to the index of the corresponding amino acid */
	int codonIndexFromString(const std::string &val);

	AA getCodon(int i, int j, int k) {
		int idx = getCodonIndex(i, j, k);
		return codonTable[idx];
	}

	AA getIndexByThreeLetterCode(const std::string &threeLetterCode);
};

ALLEGRO_COLOR getNucleotideColor(NT idx, float shade);

extern CodonTable codonTable;

#endif
