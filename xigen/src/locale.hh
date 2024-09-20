#ifndef LOCALE_HH
#define LOCALE_HH

#include "common.hh"

namespace xigen
{

	typedef enum { russian, english } Lang;

	class Locale
	{
		public:
			virtual ~Locale() { }
			virtual std::string extractDoxygenComment(const Comment& comment) const = 0;
			virtual std::string extractDoxygenCommentOneLine(const Comment& comment) const = 0;
			virtual std::string extractDoxygenCommentDescription(const Comment& comment) const = 0;
			virtual std::string pluralizeBytes (int amount) const = 0;

			virtual std::string wikiCommandCode () const = 0;
			virtual std::string wikiCommand () const = 0;
			virtual std::string wikiCommandBack () const = 0;
			virtual std::string wikiRequest () const = 0;
			virtual std::string wikiAnswer () const = 0;
			virtual std::string wikiCRC () const = 0;
			virtual std::string wikiOr () const = 0;
			virtual std::string wikiReserved () const = 0;
			virtual std::string wikiDescription () const = 0;
	};

	class LocaleFactory
	{
	public:
		static Locale* createLocale (Lang lang);
	};

}

#endif


/* vim: set ts=2 sw=2: */
