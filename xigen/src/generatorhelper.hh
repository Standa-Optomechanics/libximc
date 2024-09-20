#ifndef GENERATORHELPER_HH
#define GENERATORHELPER_HH

#include "common.hh"
#include "visitor.hh"
#include "basegenerator.hh"

namespace xigen
{

	class ast_error : public std::logic_error
	{
	public:
		explicit ast_error (const std::string& message)
			: std::logic_error(message)
		{
		}
		explicit ast_error (const std::string& message, Command* command)
			: std::logic_error(message + ";\nhappened at " + (command ? command->toString() : std::string("<unknown>")))
		{
		}

		virtual ~ast_error() throw() { }
	};

	namespace helpers
	{

		inline DataField::Purpose::Type fieldPurpose (const DataField& field) { return field.purpose(); }
		template <class F>
		inline DataField::Purpose::Type fieldPurpose (const F& ) { return DataField::Purpose::normal; }

		// argument value/ref passing helpers
		template <class Field>
		inline bool fieldPassByValue (Field& ) { return true; }

		inline bool fieldPassByValue (ArrayField& ) { return false; }

		inline std::string getCommandName (Command& command,
				bool isCalibrated, bool isStripImpl)
		{
			std::string commandName = isCalibrated
				? command.functionCalbName() : command.functionName();
			if (isStripImpl)
			{
				if (isCalibrated)
					commandName = dropTail( commandName, "_calb" );
				commandName = dropTail( dropTail( commandName, "_unsynced"), "_impl" );
				if (isCalibrated)
					commandName += "_calb";
			}
			return commandName;
		}

		inline std::string emitFunctionHead (Command& command, bool reader,
				bool withExportMacro, bool isCalibrated, bool isStripImpl)
		{
			std::string result = std::string("result_t ")  +
				(withExportMacro ? "XIMC_API " : "") +
				getCommandName(command, isCalibrated, isStripImpl) +
				" (device_t id";

			if (!command.is("inline") && command.withAnyFields())
			{
				result += (reader ? ", " : ", const ") +
					(isCalibrated ? command.structCalbName() : command.structName()) +
					"_t* " +
					(isCalibrated ? command.structParameterCalbName() : command.structParameterName());
			}
			return result;
		}

		inline void replace_all (std::string& s, const std::string& from, const std::string& to)
		{
			if (from.empty())
				return;

			size_t start = s.find(from, 0);
			while (start != std::string::npos)
			{
				s.replace(start, from.size(), to);
				start += to.size() - from.size();
				start = s.find(from, start);
			}
		}

		inline void stringBetweenTwo (std::string& s,
				const std::string& prefix, const std::string& postfix)
		{
			// prefix
			std::string marker = prefix;
			size_t ind = s.find(marker);
			if (ind != std::string::npos)
			{
				s = s.substr(ind + marker.size());
				// suffix
				marker = postfix;
				ind = s.find(marker);
				if (ind != std::string::npos)
					s = s.substr(0, ind-1);
			}
			// trim
			trim(s, " *\r\t");
		}

		// wraps an !foobar! substring with newlines
		inline std::string fixImgLink (const std::string& s)
		{
			const std::string trails = " \t";
			size_t ind1 = s.find("!"), ind2;
			if (ind1 != std::string::npos)
			{
				ind2 = s.find("!", ind1+1);
				if (ind2 != std::string::npos)
				{
					return trim_copy(s.substr(0, ind1), trails)
						+ "\n" + trim_copy(s.substr(ind1, ind2-ind1+1), trails)
						+ "\n" + trim_copy(s.substr(ind2+1), trails);
				}
			}
			return s;
		}

		inline void checkDoesNotEndWithNewline (const Comment& comment)
		{
			std::string s = comment.getComment();
			if (comment.type() == Comment::doxyNone ||
					comment.type() == Comment::doxySection)
				return;
			if (!s.empty() &&
						(*s.rbegin() == '\n' || *s.rbegin() == '\r'))
				throw std::runtime_error("Doxygen comment must not end with newline: \"" + s + "\"");
		}


		typedef enum { none, onlyOneLine, skipFirstLine } CommentParseFlags;

		inline std::string extractLanguageDoxygenComment(const Comment& comment,
				const std::string& lang, CommentParseFlags flags = none)
		{
			checkDoesNotEndWithNewline(comment);
			const std::string prefix = "\\" + lang;
			const std::string postfix = "\\end" + lang;
			std::vector<std::string> strings;
			parseToList(comment.getComment(), "\n", " *\r\t", strings);
			size_t start = (size_t)-1;
			std::string result;
			for (size_t i = 0; i < strings.size(); ++i)
			{
				if (strings[i].find(prefix) != std::string::npos)
				{
					start = i;
				}
				if (start != std::string::npos)
				{
					if (strings[i].find(postfix) != std::string::npos)
					{
						// single line comment
						if (start == i)
						{
							if (!(flags & skipFirstLine))
								result = strings[i];
							stringBetweenTwo(result, prefix, postfix);
							result = fixImgLink(result);
						}
						break;
					}
					// do not include first line if we want only description (because skipFirstLine=true)
					if (strings[i].find("@name") != std::string::npos)
					{
						if (flags & skipFirstLine)
							continue;
						else
						{
							result = strings[i].substr(strings[i].find("@name")+5);
							break;
						}
					}
					if (strings[i].find("@") != std::string::npos)
					{
						if (flags & skipFirstLine)
							continue;
						else
							break;
					}
					if (start != i)
					{
						if (!result.empty())
							result += " ";
						result += fixImgLink(strings[i]);
						if (flags & onlyOneLine)
							break;
					}
				}
			}
			replace_all(result, " \\a ", " ");
			return result;
		}

		inline std::string extractLanguageDoxygenName(const Comment& comment,
				const std::string& lang)
		{
			checkDoesNotEndWithNewline(comment);
			const std::string prefix = "\\" + lang;
			const std::string postfix = "\\end" + lang;
			std::vector<std::string> strings;
			parseToList(comment.getComment(), "\n", " *\r\t", strings);
			size_t start = (size_t)-1;
			std::string result;
			for (size_t i = 0; i < strings.size(); ++i)
			{
				if (strings[i].find(prefix) != std::string::npos)
				{
					start = i;
				}
				if (start != std::string::npos)
				{
					if (strings[i].find(postfix) != std::string::npos && start == i)
					{
						throw std::runtime_error("Cannot extract name for one-line doxygen:" + comment.getComment());
					}
					// do not include first line if we want only description (because skipFirstLine=true)
					if (strings[i].find("@name") != std::string::npos)
					{
						return trim_copy(strings[i].substr(strings[i].find("@name")+5), " \t.");
					}
				}
			}
			throw std::runtime_error("@name is not specified: " + comment.getComment());
		}

		inline std::string addEdgeDoxygenString (const Comment& comment,
				const std::string& first, const std::string& last)
		{
			std::string s = comment.getComment();
			size_t n;
			checkDoesNotEndWithNewline(comment);
			if (!first.empty())
			{
				n = s.find("\n");
				if (n != std::string::npos)
					s.insert(n+1, "\t*" + first + "\n");
			}
			if (!last.empty())
			{
				n = s.find_last_of("*/");
				while (n > 0 && s[n] != '\n')
					--n;
				s.insert(n+1, "\t*" + last + "\n");
			}
			return s;
		}

		inline void determineFlagsMaxWidth(const Flags& flags, size_t& maxNameWidth, size_t& maxValueDigits)
		{
			maxNameWidth = 0;
			int maxValue = 0;
			for (Flags::const_iterator it = flags.begin(); it != flags.end(); ++it)
			{
				maxNameWidth = std::max( maxNameWidth, (*it)->name().size() );
				maxValue = std::max( maxValue, (*it)->value() );
			}

			maxValueDigits = 0;
			while (maxValue)
			{
				maxValue /= 16;
				++maxValueDigits;
			}
			maxValueDigits = std::max( maxValueDigits, (size_t)2 );
		}

		// Aligned integer printer
		template <int Width>
		class AlignedInt
		{
			const int m_x;
		public:
			explicit AlignedInt(int x) : m_x(x) { }

			friend std::ostream& operator<< (std::ostream& os, const AlignedInt& ai)
			{
				os << std::setw(Width) << std::right << std::dec << ai.m_x << std::setw(0);
				return os;
			}
		};

		// Pluralize a word. Provided forms are for numbers 1, 4, 5
		// Thanks to mozilla for this code
		inline std::string pluralize (int n, std::string form1, std::string form2, std::string form3)
		{
			int form = n%10==1 && n%100!=11 ? 0 : n%10>=2 && n%10<=4 && (n%100<10 || n%100>=20) ? 1 : 2;
			switch (form)
			{
				case 0: return form1;
				case 1: return form2;
				case 2: return form3;
				default: throw std::runtime_error("World collapses");
			}
		}

		/* dehack template */
		template <class F>
		inline CalibrationEnum::Type reducedCalibrationType(const F& ) { return CalibrationEnum::none; }
		inline CalibrationEnum::Type reducedCalibrationType(const DataField& field) { return field.calibrationType(); }
		inline CalibrationEnum::Type reducedCalibrationType(const ArrayField& field) { return field.calibrationType(); }
		inline CalibrationEnum::Type reducedCalibrationType(const CalibrableTypeableField& field) { return field.calibrationType(); }

		template <class F>
		std::string prefixedCalbName(const F& field)
		{
			return (reducedCalibrationType( field ) == CalibrationEnum::calb ? "c" : "") + field.name();
		}

		class FindUniqueAnswerField : public DefaultVisitor
		{
			TypeableField* m_field;
			size_t m_count;
			void handle (TypeableField& field)
			{
				if (!m_field)
					m_field = &field;
				m_count++;
			}
		public:
			FindUniqueAnswerField() { clear(); }
			virtual void clear() { m_field = NULL; m_count = 0; }
			virtual void visitDataField (DataField& field)   { handle(field); }
			virtual void visitArrayField (ArrayField& field) { handle(field); }
			virtual void visitFlagField (FlagField& field)   { handle(field); }
			TypeableField* get() const { return m_count == 1 ? m_field : NULL; }
			operator bool() const { return m_field && m_count == 1; }
		};

		class ExtraFeaturesVisitor : public DefaultVisitor
		{
			bool m_withArrays;
		public:
			ExtraFeaturesVisitor() { clear(); }
			virtual void clear() { m_withArrays = false; }
			virtual void visitArrayField (ArrayField& field) { if (field.type() != VariableEnum::Char) m_withArrays = true; }
			bool withArrays() const { return m_withArrays; }
		};


	}

}

#endif
/* vim: set ts=2 sw=2: */
