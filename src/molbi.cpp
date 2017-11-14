#include "molbi.h"
#include "util.h"
#include "color.h"

NucleotideInfo nucleotideInfo[NUM_NUCLEOTIDES] = {
	{ 'T', "Tyrosine", nullptr },
	{ 'C', "Cytosine", nullptr },
	{ 'A', "Adenine", nullptr },
	{ 'G', "Guanosine", nullptr }
};

AminoAcidInfo aminoAcidInfo[NUM_AMINO_ACIDS] = {
	{ 'A', "Ala", "Alanine",       {"GCT", "GCC", "GCA", "GCG"}, nullptr, nullptr },
	{ 'R', "Arg", "Argenine",      {"CGT", "CGC", "CGA", "CGG", "AGA", "AGG"}, nullptr, nullptr },
	{ 'N', "Asn", "Asparagine",    {"AAT", "AAC"}, nullptr, nullptr },
	{ 'D', "Asp", "Aspartate",     {"GAT", "GAC"}, nullptr, nullptr },
	{ 'C', "Cys", "Cysteine",      {"TGT", "TGC"}, nullptr, nullptr },
	{ 'Q', "Gln", "Glutamine",     {"CAA", "CAG"}, nullptr, nullptr },
	{ 'E', "Glu", "Glutamate",     {"GAA", "GAG"}, nullptr, nullptr },
	{ 'G', "Gly", "Glycine",       {"GGT", "GGC", "GGA", "GGG"}, nullptr, nullptr },
	{ 'H', "His", "Histidine",     {"CAT", "CAC"}, nullptr, nullptr },
	{ 'I', "Ile", "Isoleucine",    {"ATT", "ATC", "ATA"}, nullptr, nullptr },
	{ 'L', "Leu", "Leucine",       {"CTT", "CTC", "CTA", "CTG", "TTA", "TTG"}, nullptr, nullptr },
	{ 'K', "Lys", "Lysine",        {"AAA", "AAG"}, nullptr, nullptr },
	{ 'M', "Met", "Methionine",    {"ATG"}, nullptr, nullptr },
	{ 'F', "Phe", "Phenylalan.",   {"TTT", "TTC"}, nullptr, nullptr },
	{ 'P', "Pro", "Proline",       {"CCT", "CCC", "CCA", "CCG"}, nullptr, nullptr },
	{ 'S', "Ser", "Serine",        {"TCT", "TCC", "TCA", "TCG", "AGT", "AGC"}, nullptr, nullptr },
	{ 'T', "Thr", "Threonine",     {"ACT", "ACC", "ACA", "ACG"}, nullptr, nullptr },
	{ 'W', "Trp", "Tryptophan",    {"TGG"}, nullptr, nullptr },
	{ 'Y', "Tyr", "Tyrosine",      {"TAT", "TAC"}, nullptr, nullptr },
	{ 'V', "Val", "Valine",        {"GTT", "GTC", "GTA", "GTG"}, nullptr, nullptr },
	{ '*', "***", "Stop",          {"TAA", "TAG", "TGA"}, nullptr, nullptr }
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

CodonTable::CodonTable() {

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

int CodonTable::getCodonIndex(int i, int j, int k) {
	Assert (i >= 0 && i < NUM_NUCLEOTIDES, "Invalid nucleotide");
	Assert (j >= 0 && j < NUM_NUCLEOTIDES, "Invalid nucleotide");
	Assert (k >= 0 && k < NUM_NUCLEOTIDES, "Invalid nucleotide");

	return i + j * NUM_NUCLEOTIDES + k * NUM_NUCLEOTIDES * NUM_NUCLEOTIDES;
};


/** Converts "ATG" to the index of the corresponding amino acid */
int CodonTable::codonIndexFromString(const std::string &val) {
	Assert (val.size() == 3, "Not a valid codon string");

	return getCodonIndex(
		(int)getNucleotideIndex(val.at(0)),
		(int)getNucleotideIndex(val.at(1)),
		(int)getNucleotideIndex(val.at(2))
	);
}

AA CodonTable::getIndexByThreeLetterCode(const std::string &threeLetterCode) {
	Assert (threeLetterCode.size() == 3, "Three letter code must have three letters");
	Assert (aaIndexByThreeLetterCode.find(threeLetterCode) != aaIndexByThreeLetterCode.end(), "Unknown three-letter code");
	return aaIndexByThreeLetterCode[threeLetterCode];
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

char DNAModel::getComplement(char nt) {
	switch (nt) {
		case 'A': return 'T';
		case 'T': return 'A';
		case 'G': return 'C';
		case 'C': return 'G';
		default: Assert (false, "Invalid nucleotide");
		return ' ';
	}
}

char DNAModel::getTransversion(char nt) {
	switch (nt) {
		case 'G': return 'T';
		case 'T': return 'G';
		case 'A': return 'C';
		case 'C': return 'A';
		default: Assert (false, "Invalid nucleotide");
		return ' ';
	}
}

char DNAModel::getTransition(char nt) {
	switch (nt) {
		case 'G': return 'A';
		case 'A': return 'G';
		case 'T': return 'C';
		case 'C': return 'T';
		default: Assert (false, "Invalid nucleotide");
		return ' ';
	}
}

OligoNt DNAModel::applyMutation(const OligoNt &src, int pos, MutationId mutation) {
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

void DNAModel::applyMutation (int pos, MutationId mutation) {

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

Peptide DNAModel::translate(const OligoNt &myData) {
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

/** ignores stop codons and everything after */
bool DNAModel::match(Peptide aPep, Peptide bPep) {
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

CodonTable codonTable;
