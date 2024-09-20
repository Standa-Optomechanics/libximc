#include "common.hh"
#include "model.hh"
#include "generatorhelper.hh"
#include "locale.hh"

namespace xigen
{
	// Russian
	class LocaleRussian : public Locale
	{
	public:
		std::string extractDoxygenComment(const Comment& comment) const
		{
			return helpers::extractLanguageDoxygenComment(comment,"russian");
		}

		std::string extractDoxygenCommentOneLine(const Comment& comment) const
		{
			return helpers::extractLanguageDoxygenComment(comment, "russian", helpers::onlyOneLine);
		}

		std::string extractDoxygenCommentDescription(const Comment& comment) const
		{
			return helpers::extractLanguageDoxygenComment(comment, "russian", helpers::skipFirstLine);
		}

		std::string pluralizeBytes (int amount) const
		{
			return helpers::pluralize( amount, "байт", "байта", "байт");
		}

		std::string wikiCommandCode () const	{ return "Код команды"; };
		std::string wikiCommand () const			{ return "Команда"; };
		std::string wikiCommandBack () const	{ return "Команда (возврат)"; };
		std::string wikiRequest () const			{ return "Запрос"; };
		std::string wikiAnswer () const				{ return "Ответ"; };
		std::string wikiCRC () const					{ return "Контрольная сумма"; };
		std::string wikiOr () const						{ return "Или"; };
		std::string wikiReserved () const			{ return "Зарезервировано"; };
		std::string wikiDescription () const	{ return "Описание"; };
	};

	// English
	class LocaleEnglish : public Locale
	{
	public:

		std::string extractDoxygenComment(const Comment& comment) const
		{
			return helpers::extractLanguageDoxygenComment(comment, "english");
		}

		std::string extractDoxygenCommentOneLine(const Comment& comment) const
		{
			return helpers::extractLanguageDoxygenComment(comment, "english", helpers::onlyOneLine);
		}

		std::string extractDoxygenCommentDescription(const Comment& comment) const
		{
			return helpers::extractLanguageDoxygenComment(comment, "english", helpers::skipFirstLine);
		}

		std::string pluralizeBytes (int amount) const
		{
			return helpers::pluralize( amount, "byte", "bytes", "bytes");
		}

		std::string wikiCommandCode () const	{ return "Command code"; };
		std::string wikiCommand () const			{ return "Command"; };
		std::string wikiCommandBack () const	{ return "Command (answer)"; };
		std::string wikiRequest () const			{ return "Request"; };
		std::string wikiAnswer () const				{ return "Answer"; };
		std::string wikiCRC () const					{ return "Checksum"; };
		std::string wikiOr () const						{ return "Or"; };
		std::string wikiReserved () const			{ return "Reserved"; };
		std::string wikiDescription () const	{ return "Description"; };
	};

	// Factory
	Locale* LocaleFactory::createLocale (Lang lang)
	{
		switch (lang)
		{
			case russian: return new LocaleRussian();
			case english: return new LocaleEnglish();
			default: throw std::runtime_error( "Wrong locale specified" );
		}
	}


}



/* vim: set ts=2 sw=2: */
