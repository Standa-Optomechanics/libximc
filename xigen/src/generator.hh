#ifndef GENERATOR_HH
#define GENERATOR_HH

#include "common.hh"

namespace xigen
{

	typedef enum { genNone, genHeader, genInternalHeader, genFwHeader, genFwLib,
		genCode, genPascal, genCSharp, genJava, genJNI, genPython, genDef, genWiki,
		genQsdefine, genQtscriptToscript, genQtscriptFromscript, genQtscriptToscriptCalb,
		genQtscriptFromscriptCalb, genQtscriptGetsetfunc, genQtscriptGetsethead,
		genQtscriptRegistermt, genQtscriptHighlights, genQtscriptComparison
	} GenType;

	class Protocol;
	class Locale;

	class IGenerator
	{
		public:

			virtual ~IGenerator() { }

			virtual void setProtocol (Protocol* protocol) = 0;
			virtual void setOutputFile (std::string file) = 0;
			virtual void setTemplateFile (std::string file) = 0;
			virtual void setVersionFile (std::string file) = 0;
			virtual void enableComments (bool enable) = 0;
			virtual void setVerbose (bool verbose) = 0;
			virtual void setLocale (Locale* locale) = 0;

			virtual void generate() = 0;
	};

	class GeneratorFactory
	{
		public:
			static IGenerator* createGenerator (GenType type);
	};


}

#endif


/* vim: set ts=2 sw=2: */
