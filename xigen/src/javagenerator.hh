#ifndef JAVAGENERATOR_HH
#define JAVAGENERATOR_HH

#include "common.hh"
#include "visitor.hh"
#include "modegenerator.hh"
#include "generatorhelper.hh"

namespace xigen
{

	class JavaGenerator : public Noncopyable, public ModeGenerator
	{
		private:
			helpers::FindUniqueAnswerField m_uniqueAnswerField;

			bool isWriterInline() const
			{
				return m_current->is("inline") && m_current->communicable() != Communicable::writer &&
						m_current->withAnyFields() && !m_uniqueAnswerField;
			}

			bool vinline() const
			{
				return m_current->is("inline") && !isWriterInline();
			}
		protected:

			virtual void onVisitCommandFirstTime(Command& command)
			{
				m_uniqueAnswerField.clear();
				command.accept(m_uniqueAnswerField);
			}

			virtual void startStructs()
			{
				std::string structName = m_mode == modeGenStructCalb ? m_current->structCalbName() : m_current->structName();
				stream() << "\tpublic static class "  << structName << "_t\n\t{\n";
			}

			virtual void visitCommands()
			{
				stream() << "\t}; // flags\n\n";
			}

			virtual void startFuncs()
			{
				emitFunctionHead( *m_current );
			}

			virtual void visitCommandPost (Command& command)
			{
				if (!m_current)
					return;

				switch (m_mode)
				{
					case modeGenReader:
					case modeGenWriter:
						stream() << ");" << std::endl;
						break;
					case modeGenReaderCalb:
					case modeGenWriterCalb:
						stream() << ", calibration_t calibration);" << std::endl;
						break;
					case modeGenStruct:
					case modeGenStructCalb:
						stream() << "\t};\n";
						break;
				}
				m_current = NULL;
			}

			virtual void printFormattedFlag (std::string name, std::string value,
					const ConstantField& field)
			{
					stream() << "\t\tpublic static final long " << name << " = " << value << ";\n";
			}

			virtual void visitFlagsetPost (Flagset& )
			{
				printFormattedFlagset( "0x%%.%dX" );
				stream() << "\n";
			}

			template <class F>
			void do_printFunctionArgument (F& field)
			{
				XI_ASSERT(helpers::fieldPurpose( field ) == DataField::Purpose::normal);
				if (!(vinline() && m_uniqueAnswerField && m_uniqueAnswerField.get() == &field))
					stream() << ", " << emitDeclarationImpl( field );
			}

			template <class F>
			std::string do_emitDeclaration (const F& field)
			{
				return "public " + emitDeclarationImpl( field );
			}

			template <class F>
			std::string emitDeclarationImpl (const F& field)
			{
					return mapToNativeJavaType( field.type() ) + " " + toString( field.name() );
			}

			std::string emitDeclarationImpl (const ArrayField& field)
			{
					return mapToNativeJavaType( field.type() )
						+ (field.type() == VariableEnum::Char ? " " : "[] ") + toString( field.name() );
			}

			void printFunctionArgument (DataField& field)		{ do_printFunctionArgument( field ); }
			void printFunctionArgument (FlagField& field)		{ do_printFunctionArgument( field ); }
			void printFunctionArgument (ArrayField& field)	{ do_printFunctionArgument( field ); }

			std::string emitDeclaration (const DataField& field)			{ return do_emitDeclaration( field ); }
			std::string emitDeclaration (const FlagField& field)			{ return do_emitDeclaration( field ); }
			std::string emitDeclaration (const ConstantField& field)	{ return do_emitDeclaration( field ); }
			std::string emitDeclaration (const ArrayField& field)			{ return do_emitDeclaration( field ); }

		private:

			std::ostringstream m_osStructs;
			std::ostringstream m_osFunctions;

			std::ostream& stream()
			{
				return m_mode == modeGenStruct || m_mode == modeGenStructCalb || m_mode == modeGenFlagset
					? m_osStructs
					: m_osFunctions;
			}

			void emitFunctionHead (Command& command)
			{
				bool reader = m_mode == modeGenReader || m_mode == modeGenReaderCalb;
				bool isCalibrated = command.calb && (m_mode == modeGenReaderCalb || m_mode == modeGenWriterCalb);
				std::string commandName = isCalibrated ? command.functionCalbName() : command.functionName();

				std::string result = "\tpublic static native ";

				if (reader)
				{
					if (vinline() && m_uniqueAnswerField)
						result += mapToNativeJavaType(m_uniqueAnswerField.get()->type() ) + " ";
					else
						result += (isCalibrated ? command.structCalbName() : command.structName()) + "_t ";
				}
				else
				{
					result += "void ";
				}

				result += commandName + " (int id";

				if (!reader && !vinline() && command.withAnyFields())
				{
					result += (reader ? ", " : ", ") +
						(isCalibrated ? command.structCalbName() : command.structName()) +
						"_t " +
						(isCalibrated ? command.structParameterCalbName() : command.structParameterName());
				}
				stream() << "\n" << result;
			}

			virtual bool acceptMode(Mode mode)
			{
				return mode != modeGenMetalen;
			}

			virtual void output (std::ostream* os)
			{
				if (!m_ximcVersion.empty())
					*os << "\tpublic static final String LIBXIMC_VERSION = \"" << m_ximcVersion << "\";\n";

				echoBanner( "BEGIN OF GENERATED struct declarations", os );
				*os << "\tpublic static class Flags\n"
						<< "\t{\n\n";
				*os << m_osStructs.str();

				echoBanner( "BEGIN OF GENERATED function declarations", os );
				*os
					<< m_osFunctions.str() << "\n";

				echoBanner( "END OF GENERATED CODE", os );
			}

			void echoBanner (std::string bannerText, std::ostream* os)
			{
					*os << "\n/*\n"
						<< " "		<< std::string( bannerText.size()+4, '-' ) << "\n"
						<< "   "	<< bannerText << "\n"
						<< " "		<< std::string( bannerText.size()+4, '-' ) << "\n"
						<< "*/\n";
			}
	};

}

#endif

/* vim: set ts=2 sw=2: */
