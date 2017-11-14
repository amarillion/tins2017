#include "analyzer.h"
#include "molbi.h"
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>

using namespace std;

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

public:
	PuzzleAnalyzer(LevelInfo level, ostream &os) : level(level), os(os) {

		// initalize currentSolutions
		currentSolution = firstSolution(level);
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

};

void analyzePuzzle(LevelInfo level) {
	std::ofstream ofs ("analysis.txt", std::ofstream::out);
	PuzzleAnalyzer a = PuzzleAnalyzer(level, ofs);
	a.bruteForce();
	ofs.close();
}
