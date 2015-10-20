#include <string.h>
#include <assert.h>
#include "freesasa.h"
#include "classifier.h"
#include "util.h"

// count references to default classifier
static unsigned int default_classifier_refcount = 0;
static freesasa_classifier *default_classifier = NULL;


/*
   ProtOr-definitions Based on: 

   Tsai, J., Taylor, R., Chothia, C., & Gerstein, M. (1999). The
   packing density in proteins: standard radii and volumes. Journal of
   molecular biology, 290(1), 253-266.

   Automatically generated from CONECT records for 24 amino acids (20
   ordinary plus SEC, PYL, ASX and GLX), ACE and NH2 capping groups,
   and a number of nucleic acids. The peptide backbone N is classed as
   N3H2, but should be N3H1. Since these have the same radius this
   error has not been corrected. 
*/

static const char *protor_type_input[] = {
    // from paper
    "C3H0 1.61 Apolar",    "C3H1 1.76 Apolar",    "C4H1 1.88 Apolar",    "C4H2 1.88 Apolar",
    "C4H3 1.88 Apolar",
    "N3H0 1.64 Polar",    "N3H1 1.64 Polar",    "N3H2 1.64 Polar",    "N4H3 1.64 Polar",
    "O1H0 1.42 Polar",    "O2H1 1.46 Polar",
    "S2H0 1.77 Polar",    "S2H1 1.77 Polar",
    // Nucleic acids
    "P4H0 1.9 Polar ",
    "O2H0 1.46 Polar",
    // extra
    "O2H2 1.46 Water # OHO",
    "SE2H1 1.9 Polar # SEC",
    "X1H0 1.5 Polar # ASX, GLX",
    "N2H0 1.64 Polar # PYL",
    "N2H2 1.64 Polar # NH2 is really N3H2",
};

static const char *protor_atom_input[] = {
    // Since these were automatically generated, ANY is skipped
    "ALA N N3H2",    "ALA CA C4H1",    "ALA C C3H0",    "ALA O O1H0",
    "ALA CB C4H3",   "ALA OXT O2H1",

    "ARG N N3H2",    "ARG CA C4H1",    "ARG C C3H0",    "ARG O O1H0",
    "ARG CB C4H2",   "ARG CG C4H2",    "ARG CD C4H2",   "ARG NE N3H1",
    "ARG CZ C3H0",   "ARG NH1 N3H2",   "ARG NH2 N3H2",  "ARG OXT O2H1",

    "ASN N N3H2",    "ASN CA C4H1",    "ASN C C3H0",    "ASN O O1H0",
    "ASN CB C4H2",   "ASN CG C3H0",    "ASN OD1 O1H0",  "ASN ND2 N3H2",
    "ASN OXT O2H1",

    "ASP N N3H2",    "ASP CA C4H1",    "ASP C C3H0",    "ASP O O1H0",
    "ASP CB C4H2",   "ASP CG C3H0",    "ASP OD1 O1H0",  "ASP OD2 O2H1",
    "ASP OXT O2H1",

    "CYS N N3H2",    "CYS CA C4H1",    "CYS C C3H0",    "CYS O O1H0",
    "CYS CB C4H2",   "CYS SG S2H1",    "CYS OXT O2H1",

    "GLN N N3H2",    "GLN CA C4H1",    "GLN C C3H0",    "GLN O O1H0",
    "GLN CB C4H2",   "GLN CG C4H2",    "GLN CD C3H0",   "GLN OE1 O1H0",
    "GLN NE2 N3H2",  "GLN OXT O2H1",

    "GLU N N3H2",    "GLU CA C4H1",    "GLU C C3H0",    "GLU O O1H0",
    "GLU CB C4H2",   "GLU CG C4H2",    "GLU CD C3H0",   "GLU OE1 O1H0",
    "GLU OE2 O2H1",  "GLU OXT O2H1",

    "GLY N N3H2",    "GLY CA C4H2",    "GLY C C3H0",    "GLY O O1H0",
    "GLY OXT O2H1",

    "HIS N N3H2",    "HIS CA C4H1",    "HIS C C3H0",    "HIS O O1H0",
    "HIS CB C4H2",   "HIS CG C3H0",    "HIS ND1 N3H1",  "HIS CD2 C3H1",
    "HIS CE1 C3H1",  "HIS NE2 N3H1",   "HIS OXT O2H1",

    "ILE N N3H2",    "ILE CA C4H1",    "ILE C C3H0",    "ILE O O1H0",
    "ILE CB C4H1",   "ILE CG1 C4H2",   "ILE CG2 C4H3",  "ILE CD1 C4H3",
    "ILE OXT O2H1",

    "LEU N N3H2",    "LEU CA C4H1",    "LEU C C3H0",    "LEU O O1H0",
    "LEU CB C4H2",   "LEU CG C4H1",    "LEU CD1 C4H3",  "LEU CD2 C4H3",
    "LEU OXT O2H1",

    "LYS N N3H2",    "LYS CA C4H1",    "LYS C C3H0",    "LYS O O1H0",
    "LYS CB C4H2",   "LYS CG C4H2",    "LYS CD C4H2",   "LYS CE C4H2",
    "LYS NZ N4H3",   "LYS OXT O2H1",

    "MET N N3H2",    "MET CA C4H1",    "MET C C3H0",    "MET O O1H0",
    "MET CB C4H2",   "MET CG C4H2",    "MET SD S2H0",   "MET CE C4H3",
    "MET OXT O2H1",

    "PHE N N3H2",    "PHE CA C4H1",    "PHE C C3H0",    "PHE O O1H0",
    "PHE CB C4H2",   "PHE CG C3H0",    "PHE CD1 C3H1",  "PHE CD2 C3H1",
    "PHE CE1 C3H1",  "PHE CE2 C3H1",   "PHE CZ C3H1",   "PHE OXT O2H1",

    "PRO N N3H1",    "PRO CA C4H1",    "PRO C C3H0",    "PRO O O1H0",
    "PRO CB C4H2",   "PRO CG C4H2",    "PRO CD C4H2",   "PRO OXT O2H1",

    "SER N N3H2",    "SER CA C4H1",    "SER C C3H0",    "SER O O1H0",
    "SER CB C4H2",   "SER OG O2H1",    "SER OXT O2H1",

    "THR N N3H2",    "THR CA C4H1",    "THR C C3H0",    "THR O O1H0",
    "THR CB C4H1",   "THR OG1 O2H1",   "THR CG2 C4H3",  "THR OXT O2H1",

    "TRP N N3H2",    "TRP CA C4H1",    "TRP C C3H0",    "TRP O O1H0",
    "TRP CB C4H2",   "TRP CG C3H0",    "TRP CD1 C3H1",  "TRP CD2 C3H0",
    "TRP NE1 N3H1",  "TRP CE2 C3H0",   "TRP CE3 C3H1",  "TRP CZ2 C3H1",
    "TRP CZ3 C3H1",  "TRP CH2 C3H1",   "TRP OXT O2H1",

    "TYR N N3H2",    "TYR CA C4H1",    "TYR C C3H0",    "TYR O O1H0",
    "TYR CB C4H2",   "TYR CG C3H0",    "TYR CD1 C3H1",  "TYR CD2 C3H1",
    "TYR CE1 C3H1",  "TYR CE2 C3H1",   "TYR CZ C3H0",   "TYR OH O2H1",
    "TYR OXT O2H1",

    "VAL N N3H2",    "VAL CA C4H1",    "VAL C C3H0",    "VAL O O1H0",
    "VAL CB C4H1",   "VAL CG1 C4H3",   "VAL CG2 C4H3",  "VAL OXT O2H1",

    "ASX N N3H2",    "ASX CA C4H1",    "ASX C C3H0",    "ASX O O1H0",
    "ASX CB C4H2",   "ASX CG C3H0",    "ASX XD1 X1H0",  "ASX XD2 X1H0",
    "ASX OXT O2H1",

    "GLX N N3H2",    "GLX CA C4H1",    "GLX C C3H0",    "GLX O O1H0",
    "GLX CB C4H2",   "GLX CG C4H2",    "GLX CD C3H0",   "GLX XE1 X1H0",
    "GLX XE2 X1H0",  "GLX OXT O2H1",

    "PYL CB2 C4H3",  "PYL CG2 C4H1",   "PYL CD2 C4H2",  "PYL CE2 C3H1",
    "PYL N2 N2H0",   "PYL CA2 C4H1",   "PYL C2 C3H0",   "PYL O2 O1H0",
    "PYL NZ N3H1",   "PYL CE C4H2",    "PYL CD C4H2",   "PYL CG C4H2",
    "PYL CB C4H2",   "PYL CA C4H1",    "PYL C C3H0",    "PYL OXT O2H1",
    "PYL O O1H0",    "PYL N N3H2",

    "SEC N N3H2",    "SEC CA C4H1",    "SEC CB C4H2",   "SEC SE SE2H1",
    "SEC C C3H0",    "SEC O O1H0",     "SEC OXT O2H1",  "HOH O O2H2",

    "NH2 N N2H2",

    "ACE C C3H1",    "ACE O O1H0",    "ACE CH3 C4H3",

    "A OP3 O2H1",    "A P P4H0",      "A OP1 O1H0",    "A OP2 O2H1",
    "A O5' O2H0",    "A C5' C4H2",    "A C4' C4H1",    "A O4' O2H0",
    "A C3' C4H1",    "A O3' O2H1",    "A C2' C4H1",    "A O2' O2H1",
    "A C1' C4H1",    "A N9 N3H0",     "A C8 C3H1",     "A N7 N2H0",
    "A C5 C3H0",     "A C6 C3H0",     "A N6 N3H2",     "A N1 N2H0",
    "A C2 C3H1",     "A N3 N2H0",     "A C4 C3H0", 

    "C OP3 O2H1",    "C P P4H0",      "C OP1 O1H0",    "C OP2 O2H1",
    "C O5' O2H0",    "C C5' C4H2",    "C C4' C4H1",    "C O4' O2H0",
    "C C3' C4H1",    "C O3' O2H1",    "C C2' C4H1",    "C O2' O2H1",
    "C C1' C4H1",    "C N1 N3H0",     "C C2 C3H0",     "C O2 O1H0",
    "C N3 N2H0",     "C C4 C3H0",     "C N4 N3H2",     "C C5 C3H1",
    "C C6 C3H1",

    "G OP3 O2H1",   "G P P4H0",       "G OP1 O1H0",    "G OP2 O2H1",
    "G O5' O2H0",   "G C5' C4H2",     "G C4' C4H1",    "G O4' O2H0",
    "G C3' C4H1",   "G O3' O2H1",     "G C2' C4H1",    "G O2' O2H1",
    "G C1' C4H1",   "G N9 N3H0",      "G C8 C3H1",     "G N7 N2H0",
    "G C5 C3H0",    "G C6 C3H0",      "G O6 O1H0",     "G N1 N3H1",
    "G C2 C3H0",    "G N2 N3H2",      "G N3 N2H0",     "G C4 C3H0",

    "I OP3 O2H1",    "I P P4H0",      "I OP1 O1H0",    "I OP2 O2H1",
    "I O5' O2H0",    "I C5' C4H2",    "I C4' C4H1",    "I O4' O2H0",
    "I C3' C4H1",    "I O3' O2H1",    "I C2' C4H1",    "I O2' O2H1",
    "I C1' C4H1",    "I N9 N3H0",     "I C8 C3H1",     "I N7 N2H0",
    "I C5 C3H0",     "I C6 C3H0",     "I O6 O1H0",     "I N1 N3H1",
    "I C2 C3H1",     "I N3 N2H0",     "I C4 C3H0",

    "T OP3 O2H1",    "T P P4H0",      "T OP1 O1H0",    "T OP2 O2H1",
    "T O5' O2H0",    "T C5' C4H2",    "T C4' C4H1",    "T O4' O2H0",
    "T C3' C4H1",    "T O3' O2H1",    "T C2' C4H2",    "T C1' C4H1",
    "T N1 N3H0",     "T C2 C3H0",     "T O2 O1H0",     "T N3 N3H1",
    "T C4 C3H0",     "T O4 O1H0",     "T C5 C3H0",     "T C7 C4H3",
    "T C6 C3H1",

    "U OP3 O2H1",    "U P P4H0",      "U OP1 O1H0",    "U OP2 O2H1",
    "U O5' O2H0",    "U C5' C4H2",    "U C4' C4H1",    "U O4' O2H0",
    "U C3' C4H1",    "U O3' O2H1",    "U C2' C4H1",    "U O2' O2H1",
    "U C1' C4H1",    "U N1 N3H0",     "U C2 C3H0",     "U O2 O1H0",
    "U N3 N3H1",     "U C4 C3H0",     "U O4 O1H0",     "U C5 C3H1",
    "U C6 C3H1",

    "DA OP3 O2H1",    "DA P P4H0",    "DA OP1 O1H0",    "DA OP2 O2H1",
    "DA O5' O2H0",    "DA C5' C4H2",  "DA C4' C4H1",    "DA O4' O2H0",
    "DA C3' C4H1",    "DA O3' O2H1",  "DA C2' C4H2",    "DA C1' C4H1",
    "DA N9 N3H0",     "DA C8 C3H1",   "DA N7 N2H0",     "DA C5 C3H0",
    "DA C6 C3H0",     "DA N6 N3H2",   "DA N1 N2H0",     "DA C2 C3H1",
    "DA N3 N2H0",     "DA C4 C3H0",

    "DC OP3 O2H1",    "DC P P4H0",    "DC OP1 O1H0",    "DC OP2 O2H1",
    "DC O5' O2H0",    "DC C5' C4H2",  "DC C4' C4H1",    "DC O4' O2H0",
    "DC C3' C4H1",    "DC O3' O2H1",  "DC C2' C4H2",    "DC C1' C4H1",
    "DC N1 N3H0",     "DC C2 C3H0",   "DC O2 O1H0",     "DC N3 N2H0",
    "DC C4 C3H0",     "DC N4 N3H2",   "DC C5 C3H1",     "DC C6 C3H1",

    "DG OP3 O2H1",    "DG P P4H0",    "DG OP1 O1H0",    "DG OP2 O2H1",
    "DG O5' O2H0",    "DG C5' C4H2",  "DG C4' C4H1",    "DG O4' O2H0",
    "DG C3' C4H1",    "DG O3' O2H1",  "DG C2' C4H2",    "DG C1' C4H1",
    "DG N9 N3H0",     "DG C8 C3H1",   "DG N7 N2H0",     "DG C5 C3H0",
    "DG C6 C3H0",     "DG O6 O1H0",   "DG N1 N3H1",     "DG C2 C3H0",
    "DG N2 N3H2",     "DG N3 N2H0",   "DG C4 C3H0",

    "DI OP3 O2H1",    "DI P P4H0",    "DI OP1 O1H0",    "DI OP2 O2H1",
    "DI O5' O2H0",    "DI C5' C4H2",  "DI C4' C4H1",    "DI O4' O2H0",
    "DI C3' C4H1",    "DI O3' O2H1",  "DI C2' C4H2",    "DI C1' C4H1",
    "DI N9 N3H0",     "DI C8 C3H1",   "DI N7 N2H0",     "DI C5 C3H0",
    "DI C6 C3H0",     "DI O6 O1H0",   "DI N1 N3H1",     "DI C2 C3H1",
    "DI N3 N2H0",

    "DI C4 C3H0",    "DT OP3 O2H1",    "DT P P4H0",    "DT OP1 O1H0",
    "DT OP2 O2H1",   "DT O5' O2H0",    "DT C5' C4H2",  "DT C4' C4H1",
    "DT O4' O2H0",   "DT C3' C4H1",    "DT O3' O2H1",  "DT C2' C4H2",
    "DT C1' C4H1",   "DT N1 N3H0",     "DT C2 C3H0",   "DT O2 O1H0",
    "DT N3 N3H1",    "DT C4 C3H0",     "DT O4 O1H0",   "DT C5 C3H0",
    "DT C7 C4H3",    "DT C6 C3H1",

    "DU OP3 O2H1",   "DU P P4H0",      "DU OP1 O1H0",  "DU OP2 O2H1",
    "DU O5' O2H0",   "DU C5' C4H2",    "DU C4' C4H1",  "DU O4' O2H0",
    "DU C3' C4H1",   "DU O3' O2H1",    "DU C2' C4H2",  "DU C1' C4H1",
    "DU N1 N3H0",    "DU C2 C3H0",     "DU O2 O1H0",   "DU N3 N3H1",
    "DU C4 C3H0",    "DU O4 O1H0",     "DU C5 C3H1",   "DU C6 C3H1",
};


static const char *oons_type_input[] = {
    // The original OONS classification
    "C_ALI 2.00 Apolar ",
    "C_ARO 1.75 Apolar",
    "C_CAR 1.55 Polar",
    "N 1.55 Polar",
    "O 1.40 Polar", // carbo- and hydroxyl oxygen have the same radius in OONS
    "S 2.00 Polar",

    // P and SE are not in the OONS paper, and should perhaps not be
    // smaller than S.
    "P 1.80 Polar",
    "SE 1.90 Polar",

    // Unknown polar, for ASX and GLX
    "U_POL 1.5 Polar",

    // Water
    "WATER 1.4 Water",
};

static const char *oons_atom_input[] = {
    // Polypeptide backbone
    "ANY C   C_CAR",
    "ANY O   O",
    "ANY CA  C_ALI",
    "ANY N   N",
    "ANY CB  C_ALI",
    "ANY OXT O",

    /* RNA/DNA, these are treated jointly since they are all rings and
       have overlapping atom names. The atoms of A, C, G, T, U, in
       both deoxyribose and ribose forms are included, and also
       Inosinic acid (I) and N (general nucleotide).
    */
    // PO4
    "ANY P P",
    "ANY OP1 O",
    "ANY OP2 O",
    "ANY OP3 O",
    "ANY O5' O",
    // Sugar
    "ANY C5' C_ALI",
    "ANY C4' C_ARO", 
    "ANY O4' O",
    "ANY C3' C_ARO",
    "ANY O3' O",
    "ANY C2' C_ARO",
    "ANY O2' O",
    "ANY C1' C_ARO",
    // Sidechains
    "ANY N1 N",
    "ANY N2 N",
    "ANY N3 N",
    "ANY N4 N",
    "ANY N6 N",
    "ANY N7 N",
    "ANY N9 N",
    "ANY C2 C_ARO",
    "ANY C4 C_ARO",
    "ANY C5 C_ARO",
    "ANY C6 C_ARO",
    "ANY C7 C_ARO",
    "ANY C8 C_ARO",
    "ANY O2 O",
    "ANY O4 O",
    "ANY O6 O",
    // Methylation
    "ANY CM2 C_ALI",

    // Amino acids
    "ARG CG C_ALI",   "ARG CD C_ALI",    "ARG NE N",    "ARG CZ C_ALI",
    "ARG NH1 N",      "ARG NH2 N",

    "ASN CG  C_CAR",  "ASN OD1 O",      "ASN ND2 N",

    "ASP CG  C_CAR",  "ASP OD1 O",      "ASP OD2 O",

    "CYS SG  S",

    "GLN CG  C_ALI",  "GLN CD  C_CAR",  "GLN OE1 O",    "GLN NE2 N",

    "GLU CG  C_ALI",  "GLU CD  C_CAR",  "GLU OE1 O",    "GLU OE2 O",

    "HIS CG  C_ARO",  "HIS ND1 N",      "HIS CD2 C_ARO",    "HIS NE2 N",
    "HIS CE1 C_ARO",

    "ILE CG1 C_ALI",    "ILE CG2 C_ALI",    "ILE CD1 C_ALI",

    "LEU CG  C_ALI",    "LEU CD1 C_ALI",    "LEU CD2 C_ALI",

    "LYS CG  C_ALI",    "LYS CD  C_ALI",    "LYS CE  C_ALI",    "LYS NZ  N",

    "MET CG  C_ALI",    "MET SD  S",    "MET CE  C_ALI",

    "PHE CG  C_ARO",    "PHE CD1 C_ARO",    "PHE CD2 C_ARO",    "PHE CE1 C_ARO",
    "PHE CE2 C_ARO",    "PHE CZ  C_ARO",

    "PRO CB  C_ARO",    "PRO CG  C_ARO",    "PRO CD  C_ARO",

    "SER OG  O",

    "THR OG1 O",    "THR CG2 C_ALI",

    "TRP CG  C_ARO",    "TRP CD1 C_ARO",    "TRP CD2 C_ARO",    "TRP NE1 N",
    "TRP CE2 C_ARO",    "TRP CE3 C_ARO",    "TRP CZ2 C_ARO",    "TRP CZ3 C_ARO",
    "TRP CH2 C_ARO",

    "TYR CG  C_ARO",    "TYR CD1 C_ARO",    "TYR CD2 C_ARO",    "TYR CE1 C_ARO",
    "TYR CE2 C_ARO",    "TYR CZ  C_ARO",    "TYR OH  O",

    "VAL CG1 C_ALI",    "VAL CG2 C_ALI",

    "ASX CG C_CAR",    "ASX XD1 U_POL",    "ASX XD2 U_POL",    "ASX AD1 U_POL",
    "ASX AD2 U_POL",

    "GLX CG C_ALI",    "GLX CD C_CAR",     "GLX XE1 U_POL",    "GLX XE2 U_POL",
    "GLX AE1 U_POL",   "GLX AE2 U_POL",

    // Seleno-cystein
    "SEC SE SE",
    "CSE SE SE", // is this really used?

    // Pyrolysine
    "PYL CG C_ALI",    "PYL CD C_ALI",    "PYL CE C_ALI",    "PYL NZ N",
    "PYL O2 O",    "PYL C2 C_CAR",    "PYL CA2 C_ARO",    "PYL CB2 C_ALI",
    "PYL CG2 C_ARO",    "PYL CD2 C_ARO",    "PYL CE2 C_ARO",    "PYL N2 N",

    // capping groups
    "ACE CH3 C_ALI",
    
    "NH2 NH2 N",

    //water
    "HOH O WATER",

    // add more here
};


freesasa_classifier *
freesasa_classifier_oons()
{
    const int n_types = sizeof(oons_type_input)/sizeof(char *);
    const int n_atoms = sizeof(oons_atom_input)/sizeof(char *);
    freesasa_classifier *classifier 
        = freesasa_classifier_from_array(oons_type_input, oons_atom_input,
                                         n_types, n_atoms);
    if (classifier == NULL) {
        fail_msg("");
    }

    return classifier;
}

freesasa_classifier *
freesasa_classifier_default()
{
    
    const int n_types = sizeof(protor_type_input)/sizeof(char *);
    const int n_atoms = sizeof(protor_atom_input)/sizeof(char *);
    freesasa_classifier *classifier
        = freesasa_classifier_from_array(protor_type_input, protor_atom_input,
                                         n_types, n_atoms);
    if (classifier == NULL) {
        fail_msg("");
    }
    
    return classifier;
}

const freesasa_classifier *
freesasa_classifier_default_acquire()
{
    if (default_classifier == NULL) {
        assert(default_classifier_refcount == 0);
        default_classifier = freesasa_classifier_default();
        if (default_classifier == NULL) {
            fail_msg("Failed to load default classifier.");
            return NULL;
        }
    }
    ++default_classifier_refcount;
    return default_classifier;
}

void
freesasa_classifier_default_release() {
    if (default_classifier_refcount == 0) return;
    if (--default_classifier_refcount == 0) {
        freesasa_classifier_free(default_classifier);
        default_classifier = NULL;
    }
}

struct symbol_radius {
    const char symbol[3];
    double radius;
};

/* Taken from: 
   
   Mantina et al. "Consistent van der Waals Radii for
   the Whole Main Group". J. Phys. Chem. A, 2009, 113 (19), pp
   5806–5812. 
   
   Many of these elements, if they occur in a PDB file, should
   probably rather be skipped than used in a SASA calculation, and
   ionization will change the effective radius.

*/
static const struct symbol_radius symbol_radius[] = {
    // elements that actually occur in the regular amino acids and nucleotides
    {" H", 1.10}, {" C", 1.70}, {" N", 1.55}, {" O", 1.52}, {" P", 1.80}, {" S", 1.80}, {"SE", 1.90}, 
    // some others, just because there were readily available values
    {" F", 1.47}, {"CL", 1.75}, {"BR", 1.83}, {" I", 1.98},
    {"LI", 1.81}, {"BE", 1.53}, {" B", 1.92}, 
    {"NA", 2.27}, {"MG", 1.74}, {"AL", 1.84}, {"SI", 2.10}, 
    {" K", 2.75}, {"CA", 2.31}, {"GA", 1.87}, {"GE", 2.11}, {"AS", 1.85}, 
    {"RB", 3.03}, {"SR", 2.49}, {"IN", 1.93}, {"SN", 2.17}, {"SB", 2.06}, {"TE", 2.06}, 
};

double
freesasa_guess_radius(const char* symbol)
{
    assert(symbol);
    int n_symbol = sizeof(symbol_radius)/sizeof(struct symbol_radius);
    for (int i = 0; i < n_symbol; ++i) {
        if (strcmp(symbol,symbol_radius[i].symbol) == 0)
            return symbol_radius[i].radius;
    }
    return -1.0;
}

//! The residue types that are returned by freesasa_classify_residue()
enum residue {
    //Regular amino acids
    ALA=0, ARG, ASN, ASP,
    CYS, GLN, GLU, GLY,
    HIS, ILE, LEU, LYS, 
    MET, PHE, PRO, SER,
    THR, TRP, TYR, VAL,
    //some non-standard ones
    CSE, SEC, PYL, PYH,
    ASX, GLX,
    //residue unknown
    RES_UNK,
    //capping N- and C-terminal groups (usually HETATM)
    ACE, NH2,
    //DNA
    DA, DC, DG, DT,
    DU, DI,
    //RNA (avoid one-letter enums)
    RA, RC, RG, RU, RI, RT,
    //generic nucleotide
    NN
};

// Residue types, make sure this always matches the corresponding enum.
static const char *residue_names[] = {
    //amino acids
    "ALA","ARG","ASN","ASP",
    "CYS","GLN","GLU","GLY",
    "HIS","ILE","LEU","LYS",
    "MET","PHE","PRO","SER",
    "THR","TRP","TYR","VAL",
    // non-standard amino acids
    "CSE","SEC","PYL","PYH", // SEC and PYL are standard names, CSE and PYH are found in some early files
    "ASX","GLX",
    "UNK",
    // capping groups
    "ACE","NH2",
    //DNA
    "DA","DC","DG","DT","DU","DI",
    //RNA
    "A","C","G","U","I","T",
    //General nucleotide
    "N"
};

static int
residue(const char *res_name,
        const char *atom_name,
        const freesasa_classifier *c)
{
    int len = strlen(res_name);
    char cpy[len+1];

    sscanf(res_name,"%s",cpy);
    for (int i = ALA; i <= NN; ++i) {
        if (! strcmp(cpy,residue_names[i])) return i;
    }
    return RES_UNK;
}

static const char*
residue2str(int the_residue,
            const freesasa_classifier *c)
{
    assert(the_residue >= ALA && the_residue <= NN);
    return residue_names[the_residue];
}

const freesasa_classifier freesasa_residue_classifier = {
    .radius = NULL,
    .sasa_class = residue,
    .class2str = residue2str,
    .n_classes = NN+1,
    .free_config = NULL,
    .config = NULL
};
