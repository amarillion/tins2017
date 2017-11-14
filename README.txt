==============================================================================
                     Peppy Protein Puzzle
==============================================================================


This is a game developed for the TINS 2017 Competition.
(See: https://tins.amarillion.org/2017/)


Code:      Martijn "Amarillion" van Iersel
Graphics:  Olivia "Max" Guerra Santin


------------------------------------------------------------------------------
  How to play
------------------------------------------------------------------------------


In this game you have to solve a series of puzzles.


There are three parts to the screen.

In the bottom, you see a genetic sequence (a gene),
using the four letters ACTG

In the top, you see a target amino acid sequence (a small protein, or peptide)
Using a combination of the 20 naturally occurring amino acids.

The gene is translated into a protein. You can see a Ribosome
come by to perform the translation. The result is displayed in the
middle of the screen.

In the bottom-right corner, there are several mutation cards.
You can select a mutation card, apply the mutation to the gene.

Use the cursor keys and enter to select a mutation card.
Afterwards, use the cursor keys to target the mutation
on a certain position in the gene, and press enter to apply it.

Each time you apply a mutation card, the ribosome re-translates your gene.

Your goal is to make the translation of the gene in the middle
match the target in the top.


------------------------------------------------------------------------------
  Competition rules
------------------------------------------------------------------------------

There were four additional requirements for TINS 2017


genre rule #119
Theme: Doctors & Health

  
  Doctor Raul is here to cure patient. Hopefully he will not make
  things worse by mutating him...


artistical rule #87
The game shall have a pause mode where all the characters dance to a funky tune


  Press space to enter pause mode and see the bunny dance!
  
  
artistical rule #83
The game must include a silly weapon or powerup.


  The mutation ray that you use is displayed in the corner.
  The idea that you perform a targeted mutation using a ray gun
  is very silly, scientifically speaking.
  

technical rule #40
Use a morphing effect somewhere in the game.

  Once you mutate a nucleotide, it morphs using a dissolve effect.
  This effect is implemented as a custom GLSL shader!


------------------------------------------------------------------------------
  Compilation
------------------------------------------------------------------------------

Linux:
	
	make

Windows:

	mingw32-make WINDOWS=1

Dependencies:

Developed with Allegro 5.2.2



