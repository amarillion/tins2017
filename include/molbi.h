#ifndef _MOLBI_H_
#define _MOLBI_H_

#include <string>
#include <vector>
#include <map>
#include <allegro5/allegro.h>
#include "data.h"

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
	ALLEGRO_BITMAP *card;
};

const int NUM_MUTATIONS = 9;
extern MutationInfo mutationInfo[NUM_MUTATIONS];

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
	CodonTable();

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

	static Peptide translate(const OligoNt &myData);

	Peptide translate() {
		return translate(data);
	}

	static char getComplement(char nt);
	static char getTransversion(char nt);
	static char getTransition(char nt);

	/** ignores stop codons and everything after */
	static bool match(Peptide aPep, Peptide bPep);

	static OligoNt applyMutation(const OligoNt &src, int pos, MutationId mutation);
	void applyMutation (int pos, MutationId mutation);

	/** in-place modification. Turn this DNA sequence into its reverse complement */
	static OligoNt reverseComplement(const OligoNt &src) {
		OligoNt newData;
		for (auto it = src.rbegin(); it != src.rend(); it++) {
			newData += getComplement(*it);
		}
		return newData;
	}
};

#endif
