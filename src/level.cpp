#include "level.h"

Script scripts[NUM_SCRIPTS] = {
	{
		// 0: effectively no script
		{ Cmd::ACTIVATE_ALL, "" },
	}, {
		// 1
		{ Cmd::SAY, "Ok, let's have a look at our patient.\n\n\n(press any key to continue)" },
		{ Cmd::SAY, "Ah yes, I see. There is something wrong with the patient's genes.\n" },
		{ Cmd::SAY, "With our mutation ray here,\nwe can mutate faulty genes to fix them, and cure the patient.\n"
				"Don't worry, this won't hurt a bit.\nI mean, this probably won't hurt.\n" },
		{ Cmd::BIGEYES, "" },
		{ Cmd::SAY, "OK, this might hurt a little." },
		{ Cmd::NORMALEYES, "" },
		{ Cmd::ACTIVATE_GENE, "" },
		{ Cmd::SAY, "Let's look at a faulty gene.\n"
				"Genes are made up of four letters,\n"
				"<A>, <C>, <T> and <G>\n"
				"Our first gene is just three letters long: <T><G><T>." },
		{ Cmd::ACTIVATE_TARGET, "" },
		{ Cmd::SAY, "Each group of three letters makes something.\n"
				"This gene should make 'Tryptophan'.\n"
				"For that we need <T><G><G>\n" },
		{ Cmd::ACTIVATE_TRANSLATION, "" },
		{ Cmd::SAY,
				"What we have looks almost right.\n"
				"The gene is currently <T><G><T>, producing 'Cysteine'.\n"
				"But we need <T><G><G> to make Tryptophan.\n" },
		{ Cmd::SAY,
				"We just need to mutate the last <T> to a <G>\n"
				"Here is how you do that:\n"
				"Activate the mutation card with ENTER. "
				"Move it\nto the right spot with LEFT or RIGHT, and press ENTER to apply.\n"
				}
	}, { // 2
		{ Cmd::ACTIVATE_TARGET, "" },
		{ Cmd::SAY, "Our patient isn't fully healed yet.\n"
					"Let's fix another gene.\n"
					"This gene should make 'Glutamate', three times in a row.\n"
					"Glutamate always goes nice with Chinese food!\n" },
		{ Cmd::ACTIVATE_GENE, "" },
		{ Cmd::SAY, "Groups of three letters are translated into something.\n"
					"We call a group of three a 'codon'\n"
					"Codons are redundant. That means that some combinations\n"
					"Lead to the same result" },
		{ Cmd::SAY, "For example, the codons <G><A><G> and <G><A><A> both translate\nto Glutamate."
					"That's why the Glutamate card has two rows of symbols.\n" },
		{ Cmd::ACTIVATE_TRANSLATION, "" },
		{ Cmd::SAY, "Our goal is to make Glutamate three times.\n"
					"We're almost there. Only one letter is out of place.\n"
					"Do you see which one it is?\n"
					"Move the mutation card over to apply the mutation."},
	}, { // 3
		{ Cmd::ACTIVATE_ALL, "" },
		{ Cmd::SAY, "Let's look at another defective gene.\n"
					"Now we have a choice. There are two possible mutation cards\n"
					"A transversion and a transition" },
		{ Cmd::SAY, "A transversion is a different kind of mutation than a transition.\n"
					"A transition swaps <A> with <G>, or <T> with <C>.\n"
					"A transversion swaps <A> with <C>, or <T> with <G>.\n"
					"If you look closely at the mutation card, the symbol shows this." },
	}, {
		// 4
		{ Cmd::ACTIVATE_ALL, "" },
		{ Cmd::SAY, "There is a third kind of mutation: the complement.\n"
					"The complement swaps <G> with <C>, or <A> with <T>." },
	}, {
		// 5
		{ Cmd::ACTIVATE_ALL, "" },
		{ Cmd::SAY, "Sometimes a mutation deletes a letter.\n"
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
		{ Cmd::SAY, "The 'reverse complement' changes the direction of translation.\n"
					"Each letter is complemented: <A> to <T>, and <C> to <G>.\n"
					"And the entire gene is reversed.\n"
			},
	}, {
		// 8
		{ Cmd::ACTIVATE_ALL, "" },
		{ Cmd::SAY, "A stop codon halts the translation.\n"
					"They are <T><A><A>, <T><G><A> or <T><A><G>.\n"
					"Insert a stop codon to shorten the resulting sequence.\n"
			},
	}, {
		// 9
		{ Cmd::SAY, "Well done!\n"
					"You made our patient feel better!\n"
		},
		{ Cmd::ADVANCE_LEVEL, "" }
	}, {
		// 10
		{ Cmd::SAY,	"Congratulations, you completed the first trial!\n\n"
					"Tryptophan and Cysteine are both a kind of 'amino acid'\n"
					"Amino acids are the building blocks of 'proteins',\n"
					"the tiny machines that power your cells" },
		{ Cmd::ADVANCE_LEVEL, "" }
	}, {
		// 11
		{ Cmd::SAY,	"Glutamate is another amino acid\n"
					"In total there are 20 different kinds.\n"
					"Chains of amino acids\n"
					"form and endless variety of proteins\n" },
		{ Cmd::SAY, "Well done!\n"
					"You made our patient feel better!\n"
		},
		{ Cmd::ADVANCE_LEVEL, "" }
	}, {
		// 12: reset text...
		{ Cmd::SAY, "Oh, looks like you used up all your mutation cards\nBut you haven't found the solution yet.\n"
				"Press F1 or click the reset button to try again."
		}
	}
};

Script dutch[NUM_SCRIPTS] = {
	{
		// 0: effectively no script
		{ Cmd::ACTIVATE_ALL, "" },
	}, {
		// 1
		{ Cmd::SAY, "Ok, laten we eens kijken naar onze patient.\n\n\n(druk op een toets om verder te gaan)" },
		{ Cmd::SAY, "Oh, ik zie het al. Er is iets mis met de genen van onze patient.\n" },
		{ Cmd::SAY, "Ik heb hier een mutatie-straal, waarmee we\ngenen kunnen repareren, om de patient te genezen.\n"
				"Maak je geen zorgen, hoor! Dit gaat geen pijn doen.\nIk bedoel, het doet meestal geen pijn.\n" },
		{ Cmd::BIGEYES, "" },
		{ Cmd::SAY, "OK, misschien doet het wel een beetje pijn." },
		{ Cmd::NORMALEYES, "" },
		{ Cmd::ACTIVATE_GENE, "" },
		{ Cmd::SAY, "Laten we eens kijken naar een verkeerd gen.\n"
				"Genen bestaan uit vier letters,\n"
				"<A>, <C>, <T> and <G>\n"
				"Ons eerste gen is maar drie letters lang: <T><G><T>." },
		{ Cmd::ACTIVATE_TARGET, "" },
		{ Cmd::SAY, "Iedere groep van drie letters maakt iets.\n"
				"Dit gen zou 'Tryptophan' moeten maken.\n"
				"Daarvoor hebben we <T><G><G> nodig.\n" },
		{ Cmd::ACTIVATE_TRANSLATION, "" },
		{ Cmd::SAY,
				"Wat we hebben is al bijna goed.\n"
				"Het gen <T><G><T> maakt nu 'Cysteine'.\n"
				"Maar we hebben <T><G><G> nodig om Tryptophan te maken.\n" },
		{ Cmd::SAY,
				"We hoeven allen maar die laaste <T> in een <G> te muteren\n"
				"Hier is hoe je dat doet:\n"
				"Activeer je mutatie-kaart met ENTER. "
				"Beweeg het naar de\ngoede plek met LINKS of RECHTS, en druk op ENTER om te muteren."
				}
	}, { // 2
		{ Cmd::ACTIVATE_TARGET, "" },
		{ Cmd::SAY, "Onze patient is nog niet helemaal beter.\n"
					"Laten we nog een gen repareren.\n"
					"Dit gen zou 'Glutamate' moeten maken, drie keer achter elkaar.\n"
					"Glutamate is altijd lekker bij Chinees eten!\n" },
		{ Cmd::ACTIVATE_GENE, "" },
		{ Cmd::SAY, "Groepjes van drie letters worden samen vertaald.\n"
					"We noemen zo'n groepje van drie een 'codon'\n"
					"Codons zijn redundant. Dat betekent dat sommige combinaties\n"
					"hetzelfde resultaat opleveren" },
		{ Cmd::SAY, "Bijvoorbeeld, de codons <G><A><G> en <G><A><A> vertalen allebei naar\nGlutamate."
					"Daarom heeft de Glutamate kaart twee rijen met symbolen." },
		{ Cmd::ACTIVATE_TRANSLATION, "" },
		{ Cmd::SAY, "Ons doel is om drie keer Glutamate te maken.\n"
					"We zijn er bijna. Maar een letter is verkeerd.\n"
					"Zie je welke het is?\n"
					"Verplaats de mutatie-kaart naar die plek en voer hem uit."},
	}, { // 3
		{ Cmd::ACTIVATE_ALL, "" },
		{ Cmd::SAY, "Laten we nog een ander foutief gen bekijken.\n"
					"Hier hebben we een keus. Er zijn twee mogelijke mutatie kaarten\n"
					"Een transversion en een transition" },
		{ Cmd::SAY, "Een transversion is een ander soort mutatie dan een transition.\n"
					"Een transition ruilt <A> met <G>, of <T> met <C>.\n"
					"Een transversion ruilt <A> met <C>, of <T> met <G>.\n"
					"Als je goed kijkt zie je het ook aan de symbolen op de mutatiekaart." },
	}, {
		// 4
		{ Cmd::ACTIVATE_ALL, "" },
		{ Cmd::SAY, "Er is een derde soort mutatie: het complement.\n"
					"Het complement ruilt <G> met <C>, of <A> met <T>." },
	}, {
		// 5
		{ Cmd::ACTIVATE_ALL, "" },
		{ Cmd::SAY, "Een deletion is een mutatie die een letter wist.\n"
					"Dit veroorzaakt dan een frame shift.\n"
					"Alle volgende aminozuren kunnen daardoor beinvloed worden\n"
					 },
	}, {
		// 6
		{ Cmd::ACTIVATE_ALL, "" },
		{ Cmd::SAY, "Een insertion kan het effect van een deletion ongedaan maken.\n"
			},
	}, {
		// 7
		{ Cmd::ACTIVATE_ALL, "" },
		{ Cmd::SAY, "De 'reverse complement' veranderd de richting van de vertaling.\n"
					"Iedere letter wordt dan gecomplementeerd: <A> naar <T>, en <C> naar <G>.\n"
					"En dan wordt het hele gen omgedraaid.\n"
			},
	}, {
		// 8
		{ Cmd::ACTIVATE_ALL, "" },
		{ Cmd::SAY, "Een stop codon stopt de vertaling.\n"
					"Ze zijn <T><A><A>, <T><G><A> of <T><A><G>.\n"
					"Voeg een stop codon in, om de keten te verkorten.\n"
			},
	}, {
		// 9
		{ Cmd::SAY, "Goed gedaan!\n"
					"Onze patient voelt zich al wat beter!\n"
		},
		{ Cmd::ADVANCE_LEVEL, "" }
	}, {
		// 10
		{ Cmd::SAY,	"Gefeliciteerd, je hebt de eerste test volbracht!\n\n"
					"Tryptophan en Cysteine zijn allebei een soort 'aminozuur'\n"
					"Aminozuren zijn de bouwstenen van eiwitten,\n"
					"de kleine machines die je cellen laten werken" },
		{ Cmd::ADVANCE_LEVEL, "" }
	}, {
		// 11
		{ Cmd::SAY,	"Glutamate is nog een soort aminozuur\n"
					"In totaal zijn er 20 verschillende.\n"
					"Ketens van aminozuren vormen een eindeloze variatie aan eiwitten\n" },
		{ Cmd::SAY, "Goed gedaan!\n"
					"Onze patient voelt zich al wat beter!\n"
		},
		{ Cmd::ADVANCE_LEVEL, "" }
	}, {
		// 12: reset text...
		{ Cmd::SAY, "Oh, je mutatiekaarten zijn op.\nMaar je hebt nog niet de juiste oplossing.\n"
				"Druk F1 of klik op de resetknop om opnieuw te proberen."
		}
	}
};

LevelInfo levelInfo[NUM_LEVELS] = {

	{  1, 10, { AA::Trp }, "TGT", { MutationId::TRANSVERSION } },
	{  2, 11, { AA::Glu, AA::Glu, AA::Glu }, "GAGGACGAA", { MutationId::TRANSVERSION } },

	{  3, 9, { AA::Leu, AA::Lys }, "TTCACA", { MutationId::TRANSITION, MutationId::TRANSVERSION } },

	{  4, 9, { AA::Pro, AA::Asp }, "CATCAT", { MutationId::COMPLEMENT, MutationId::TRANSVERSION } },
	{  5, 9, { AA::Val, AA::Thr }, "GATTACA", { MutationId::DELETION } },

	{  6, 9, { AA::Ser, AA::Ser, AA::Ser }, "TCTCTCTCT", { MutationId::DELETION, MutationId::INSERTION_T } },

	{  7, 9, { AA::Leu, AA::Ile, AA::Gly, AA::Pro }, "GGGCCCAATTAA", { MutationId::REVERSE_COMPLEMENT } },

	{  7, 9, { AA::Gly, AA::Gly, AA::Asp }, "ACA" "CCA" "CC", { MutationId::REVERSE_COMPLEMENT, MutationId::INSERTION_G, MutationId::TRANSVERSION } },

	{  0, 9, { AA::Arg, AA::Asp, AA::Leu }, "AGC" "GCT" "TTT", { MutationId::COMPLEMENT, MutationId::COMPLEMENT, MutationId::TRANSITION, MutationId::TRANSITION } },

	{  0, 9, { AA::Cys, AA::Ile, AA::Ser, AA::His }, "TGG" "TGT" "TCA" "CAT", { MutationId::DELETION, MutationId::INSERTION_A, MutationId::COMPLEMENT } },

	{  8, 9, { AA::Ser }, "TCTTGGACATC", { MutationId::DELETION } },

	{  0, 9, { AA::His, AA::Pro, AA::Ala, AA::Ala}, "CCG" "CCC" "GGC" "CCG", { MutationId::DELETION, MutationId::INSERTION_A, MutationId::TRANSITION, MutationId::TRANSVERSION } },

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
