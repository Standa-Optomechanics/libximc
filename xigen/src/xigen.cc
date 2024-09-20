#include "common.hh"
#include "driver.hh"
#include "parsercontext.hh"
#include "locale.hh"
#include "generator.hh"
#include "postprocess.hh"
#include "sizecheck.hh"
#include <getopt.h>


int main(int argc, char *argv[])
{
	xigen::ParserContext context;
	yy::Driver driver(context);

	std::string grammarFile;
	std::string outputFile;
	std::string templateFile;
	std::string versionFile;

	xigen::GenType generatorType = xigen::genNone;
	xigen::Lang language = xigen::english;

	std::string usageStr =
		"Usage: xigen [OPTIONS] -i GRAMMAR -o OUTPUT\n"\
		" -i, --input            input grammar file\n"\
		" -o, --output           output file\n"\
		" -h, --help             display this text\n"\
		" -p, --trace-parse      trace syntax output\n"\
		" -s, --trace-scan       trace lexer output\n"\
		" -a, --trace-ast        trace AST\n"\
		" -v, --verbose          trace all\n"\
		" -w, --warn             enable warnings\n"\
		" -n, --no-comments      don't generate comments\n"\
		" -t, --template         template file\n"\
		" -l, --language         language\n"\
		" -x, --ximc-version     file with ximc version\n"\
		"\n"\
		"One of the following modes must be specified:\n"\
		" --check-syntax              check syntax only (default)\n"\
		" --gen-header                generate C header\n"\
		" --gen-internal-header       generate C internal header\n"\
		" --gen-fw-header             generate firmware header\n"\
		" --gen-fw-lib                generate firmware code\n"\
		" --gen-code                  generate C serializer\n"\
		" --gen-pascal                generate Delphi stub library\n"\
		" --gen-csharp                generate C# library\n"\
		" --gen-java                  generate Java wrapper class\n"\
		" --gen-jni                   generate Java JNI library\n"\
		" --gen-python                generate Python stub library\n"\
		" --gen-def                   generate MSVC linker index\n"\
		" --gen-wiki                  generate wiki\n"\
		" --gen-qsdefine              generate QTScript defines (used in Xilab)\n"\
		" --gen-qtscript-toscript     generate QTScript toscript (used in Xilab)\n"\
		" --gen-qtscript-fromscript   generate QTScript fromscript (used in Xilab)\n"\
		" --gen-qtscript-toscript-calb     generate QTScript toscript-calb (used in Xilab)\n"\
		" --gen-qtscript-fromscript-calb   generate QTScript fromscript-calb (used in Xilab)\n"\
		" --gen-qtscript-getsetfunc   generate QTScript getsetfunc (used in Xilab)\n"\
		" --gen-qtscript-getsethead   generate QTScript getsethead (used in Xilab)\n"\
		" --gen-qtscript-registermt   generate QTScript registermt (used in Xilab)\n"\
		" --gen-qtscript-highlights   generate QTScript highlights (used in Xilab)\n"\
		"";

	static struct option long_options[] =
	{
		{"help",										no_argument,				0,	'h'},
		{"trace-parse",							no_argument,				0,	'p'},
		{"trace-scan",							no_argument,				0,	's'},
		{"trace-ast",								no_argument,				0,	'a'},
		{"no-comments",							no_argument,				0,	'n'},
		{"verbose",									no_argument,				0,	'v'},
		{"warn",										no_argument,				0,	'w'},
		{"input",										required_argument,	0,	'i'},
		{"output",									required_argument,	0,	'o'},
		{"template",								optional_argument,	0,	't'},
		{"ximc-version",						required_argument,	0,	'x'},
		{"language",								required_argument,	0,	'l'},
		{"check-syntax",						no_argument,				(int*)&generatorType,	xigen::genNone},
		{"gen-header",							no_argument,				(int*)&generatorType,	xigen::genHeader},
		{"gen-internal-header",			no_argument,				(int*)&generatorType,	xigen::genInternalHeader},
		{"gen-fw-header",						no_argument,				(int*)&generatorType,	xigen::genFwHeader},
		{"gen-fw-lib",							no_argument,				(int*)&generatorType,	xigen::genFwLib},
		{"gen-code",								no_argument,				(int*)&generatorType,	xigen::genCode},
		{"gen-pascal",							no_argument,				(int*)&generatorType,	xigen::genPascal},
		{"gen-csharp",							no_argument,				(int*)&generatorType,	xigen::genCSharp},
		{"gen-java",								no_argument,				(int*)&generatorType,	xigen::genJava},
		{"gen-jni",									no_argument,				(int*)&generatorType,	xigen::genJNI},
		{"gen-python",							no_argument,				(int*)&generatorType,	xigen::genPython},
		{"gen-def",									no_argument,				(int*)&generatorType,	xigen::genDef},
		{"gen-wiki",								no_argument,				(int*)&generatorType,	xigen::genWiki},
		{"gen-qsdefine",						no_argument,				(int*)&generatorType,	xigen::genQsdefine},
		{"gen-qtscript-toscript",		no_argument,				(int*)&generatorType,	xigen::genQtscriptToscript},
		{"gen-qtscript-fromscript",	no_argument,				(int*)&generatorType,	xigen::genQtscriptFromscript},
		{ "gen-qtscript-toscript-calb", no_argument, (int*)&generatorType, xigen::genQtscriptToscriptCalb },
		{ "gen-qtscript-fromscript-calb", no_argument, (int*)&generatorType, xigen::genQtscriptFromscriptCalb },
		{ "gen-qtscript-getsetfunc", no_argument, (int*)&generatorType, xigen::genQtscriptGetsetfunc },
		{ "gen-qtscript-getsethead", no_argument, (int*)&generatorType, xigen::genQtscriptGetsethead },
		{ "gen-qtscript-registermt", no_argument, (int*)&generatorType, xigen::genQtscriptRegistermt },
		{ "gen-qtscript-highlights", no_argument, (int*)&generatorType, xigen::genQtscriptHighlights },
		{ "gen-qtscript-comparison", no_argument, (int*)&generatorType, xigen::genQtscriptComparison },
		{ 0, 0, 0, 0 }
	};

	if (argc == 1)
	{
		std::cerr << usageStr << std::endl;
		return 1;
	}

	for (;;)
	{
		/* getopt_long stores the option index here. */
		int option_index = 0;

		int c = getopt_long (argc, argv, "hpsacvwni:o:t:x:l:",	long_options, &option_index);

		/* Detect the end of the options. */
		if (c == -1)
			break;

		switch (c)
		{
			case 0:
				/* If this option set a flag, do nothing else now. */
				if (long_options[option_index].flag != 0)
					break;
				break;

			case 'h':
				std::cerr << usageStr << std::endl;
				return 0;

			case 'p':
				driver.trace_parsing = true;
				break;

			case 's':
				driver.trace_scanning = true;
				break;

			case 'a':
				driver.trace_ast = true;
				break;

			case 'n':
				driver.no_comments = true;
				break;

			case 'v':
				driver.trace_parsing = true;
				driver.trace_scanning = true;
				driver.trace_ast = true;
				break;

			case 'w':
				driver.trace_warnings = true;
				break;

			case 'i':
				grammarFile = optarg;
				break;

			case 'o':
				outputFile = optarg;
				break;

			case 't':
				templateFile = optarg;
				break;

			case 'x':
				versionFile = optarg;
				break;

			case 'l':
				if (!strcmp( optarg, "russian" ))
					language = xigen::russian;
				else if (!strcmp( optarg, "english" ))
					language = xigen::english;
				else
				{
					std::cerr << "Ich spreche kein Deutsch. Only russian and english languages are supported";
					return 1;
				}
				break;

			case '?':
				std::cerr << "Syntax error\n" << usageStr << std::endl;
				return 1;

			default:
				abort();
		}
	}

	try
	{
		if (!driver.parse_file(grammarFile))
			throw std::runtime_error("Parsing fails");

		xigen::PostProcessor postProcessor( driver.trace_ast, driver.trace_warnings, driver.no_comments );
		xigen::SizeCheck sizeCheckVisitor( driver.trace_ast );
		postProcessor.pre( driver.getContext().protocol );
		driver.getContext().protocol.accept( postProcessor );
		postProcessor.post( driver.getContext().protocol );
		driver.getContext().protocol.accept( sizeCheckVisitor );

		if (generatorType != xigen::genNone)
		{
			xigen::IGenerator* generator = xigen::GeneratorFactory::createGenerator( generatorType );
			XI_ASSERT(generator);

			generator->enableComments( !driver.no_comments );
			generator->setVerbose( driver.trace_ast );
			generator->setOutputFile( outputFile );
			generator->setTemplateFile( templateFile );
			generator->setProtocol( &driver.getContext().protocol );
			generator->setVersionFile( versionFile );

			xigen::Locale* locale = xigen::LocaleFactory::createLocale( language );
			generator->setLocale( locale );

			generator->generate();

			delete locale;
			delete generator;
		}

		//std::cerr << "Done" << std::endl;
		return 0;
	}
	catch (std::exception& e)
	{
		std::cerr << "ERROR: " << e.what() << std::endl;
		if (!outputFile.empty())
			unlink( outputFile.c_str() );
		return -1;
	}

}

/* vim: set ts=2 sw=2: */
