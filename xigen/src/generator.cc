#include "common.hh"
#include "model.hh"
#include "locale.hh"
#include "generator.hh"
#include "headergenerator.hh"
#include "libgenerator.hh"
#include "defgenerator.hh"
#include "pascalgenerator.hh"
#include "csharpgenerator.hh"
#include "javagenerator.hh"
#include "jnigenerator.hh"
#include "pythongenerator.hh"
#include "fwheadergenerator.hh"
#include "fwlibgenerator.hh"
#include "wikigenerator.hh"
#include "qsdefinegenerator.hh"
#include "qtscriptgenerator_toscript.hh"
#include "qtscriptgenerator_fromscript.hh"
#include "qtscriptgenerator_toscript_calb.hh"
#include "qtscriptgenerator_fromscript_calb.hh"
#include "qtscriptgenerator_getsetfunc.hh"
#include "qtscriptgenerator_getsethead.hh"
#include "qtscriptgenerator_registermt.hh"
#include "qtscriptgenerator_highlights.hh"
#include "qtscriptgenerator_comparison.hh"

namespace xigen
{

	IGenerator* GeneratorFactory::createGenerator (GenType type)
	{
		switch (type)
		{
			case genNone:
				throw std::runtime_error( "Wrong generator type" );

			case genHeader:
				return new HeaderGenerator( true );

			case genInternalHeader:
				return new HeaderGenerator( false );

			case genQsdefine:
				return new QsdefineGenerator();

			case genQtscriptToscript:
				return new QtscriptGenerator_toscript();

			case genQtscriptFromscript:
				return new QtscriptGenerator_fromscript();

			case genQtscriptToscriptCalb:
				return new QtscriptGenerator_toscript_calb();

			case genQtscriptFromscriptCalb:
				return new QtscriptGenerator_fromscript_calb();

			case genQtscriptGetsetfunc:
				return new QtscriptGenerator_getsetfunc();

			case genQtscriptGetsethead:
				return new QtscriptGenerator_getsethead();

			case genQtscriptRegistermt:
				return new QtscriptGenerator_registermt();

			case genQtscriptHighlights:
				return new QtscriptGenerator_highlights();

			case genQtscriptComparison:
				return new QtscriptGenerator_comparison();

			case genFwHeader:
				return new FwHeaderGenerator();

			case genFwLib:
				return new FwLibGenerator();

			case genCode:
				return new LibGenerator();

			case genDef:
				return new DefGenerator();

			case genCSharp:
				return new CSharpGenerator();

			case genJava:
				return new JavaGenerator();

			case genJNI:
				return new JNIGenerator();

			case genPascal:
				return new PascalGenerator();

			case genPython:
				return new PythonGenerator();

			case genWiki:
				return new WikiGenerator();

			default:
				throw std::runtime_error( "Generator is not known" );
		}
	}

}



/* vim: set ts=2 sw=2: */
