#include <cppunit/TestCase.h>

#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include "updatechecker.h"
#include "molbi.h"

using namespace std;

class MolbiTest : public CppUnit::TestCase {

public:
	CPPUNIT_TEST_SUITE( MolbiTest );

	CPPUNIT_TEST( testMatchWithStopCodon );

	CPPUNIT_TEST_SUITE_END();

	void testMatchWithStopCodon() {

		CPPUNIT_ASSERT (! DNAModel::match(Peptide{ AA::Tyr }, Peptide{ AA::Thr }));
		CPPUNIT_ASSERT (! DNAModel::match(Peptide{ AA::Tyr }, Peptide{ }));
		CPPUNIT_ASSERT (! DNAModel::match(Peptide{ AA::Tyr }, Peptide{ AA::Thr }));
		CPPUNIT_ASSERT (! DNAModel::match(Peptide{ AA::Tyr }, Peptide{ AA::Tyr, AA::Tyr }));

		CPPUNIT_ASSERT (DNAModel::match(Peptide{}, Peptide{}));
		CPPUNIT_ASSERT (DNAModel::match(Peptide{ AA::Ile }, Peptide{ AA::Ile }));
		CPPUNIT_ASSERT (DNAModel::match(Peptide{ AA::Ile }, Peptide{ AA::Ile, AA::STP }));
		CPPUNIT_ASSERT (DNAModel::match(Peptide{ AA::STP }, Peptide{}));
		CPPUNIT_ASSERT (DNAModel::match(Peptide{ AA::Ile, AA::STP, AA::Val }, Peptide{ AA::Ile }));
		CPPUNIT_ASSERT (DNAModel::match(Peptide{ AA::Ile, AA::STP, AA::Val }, Peptide{ AA::Ile, AA::STP }));
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION( MolbiTest );
