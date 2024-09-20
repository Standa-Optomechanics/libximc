#ifndef PASCALGENERATOR_HH
#define PASCALGENERATOR_HH

#include "common.hh"
#include "visitor.hh"
#include "modegenerator.hh"

namespace xigen
{


	class PascalGenerator : public Noncopyable, public ModeGenerator
	{
		protected:

			virtual void startStructs()
			{
				std::string structName = m_mode == modeGenStructCalb ? m_current->structCalbName() : m_current->structName();
				stream() << std::endl;
				stream() << "type " << structName << "_t = record\n";
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
						stream() << ") : XimcResult; stdcall; external XimcDll;\n";
						break;
					case modeGenReaderCalb:
					case modeGenWriterCalb:
						stream() << "; var calibration: calibration_t) : XimcResult; stdcall; external XimcDll;\n";
						break;
					case modeGenStruct:
					case modeGenStructCalb:
						stream() << "end;\n";
						break;
				}
				m_current = NULL;
			}

			virtual void printFormattedFlag (std::string name, std::string value,
					const ConstantField& field)
			{
					stream() << "\t" << name << " = " << value << ";\n";
			}

			virtual void visitFlagsetPost (Flagset& )
			{
				printFormattedFlagset( "$%%.%dX" );
				stream() << "\n";
			}

			template <class F>
			void do_printFunctionArgument (F& field)
			{
				XI_ASSERT(helpers::fieldPurpose( field ) == DataField::Purpose::normal);
				stream() << "; " << do_emitDeclaration( field, m_mode == modeGenReader || m_mode == modeGenReaderCalb );
			}

			// BURN IN HELL DELPHI. only open array could be a function parameter
			void do_printFunctionArgument (ArrayField& field)
			{
				XI_ASSERT(helpers::fieldPurpose( field ) == DataField::Purpose::normal);
				stream() << "; ";
				if (m_mode == modeGenReader || m_mode == modeGenReaderCalb)
					stream() << "out ";
				stream() << toString( field.name() ) << ": array of " <<
						mapToNativePascalType( remapBindingType( field.type() ), true );
			}

			template <class F>
			std::string do_emitDeclaration (const F& field, bool usePointer = false)
			{
					return (usePointer?"out " : "") + toString( field.name() )
						+ ": " + mapToNativePascalType( remapBindingType( field.type() ), false ) ;
			}

			std::string do_emitDeclaration (const ArrayField& field, bool usePointer = false)
			{
					return (usePointer?"out " : "") + toString( field.name() )
						+ ": "
						+ (field.type() == VariableEnum::Char ? "" : ("array [0.." + field.dimExpression( -1 ) + "] of " ))
						+ mapToNativePascalType( remapBindingType( field.type() ), true );
			}

			void printFunctionArgument (DataField& field) 	{ do_printFunctionArgument( field ); }
			void printFunctionArgument (FlagField& field) 	{ do_printFunctionArgument( field ); }
			void printFunctionArgument (ArrayField& field) 	{ do_printFunctionArgument( field ); }

			std::string emitDeclaration (const DataField& field) 			{ return do_emitDeclaration( field ); }
			std::string emitDeclaration (const FlagField& field) 			{ return do_emitDeclaration( field ); }
			std::string emitDeclaration (const ConstantField& field) 	{ return do_emitDeclaration( field ); }
			std::string emitDeclaration (const ArrayField& field) 		{ return do_emitDeclaration( field ); }


			void finishStructField (Field& field)
			{
					stream() << "\n";
			}

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

				std::string result =
					"function "  +
					commandName + " (id: Device";

				if (!command.is("inline") && command.withAnyFields())
				{
					result += (reader ? "; out " : "; var ") +
						(isCalibrated ? command.structParameterCalbName() : command.structParameterName()) +
						": " +
						(isCalibrated ? command.structCalbName() : command.structName()) +
						"_t";
				}
				stream() << "\n" << result;
			}

			virtual bool acceptMode(Mode mode)
			{
				return mode != modeGenMetalen;
			}

			virtual void output (std::ostream* os)
			{
				echoBanner( "BEGIN OF GENERATED struct declarations", os );

				if (!m_ximcVersion.empty())
					*os << "\nconst\n"
							<< "\tLIBXIMC_VERSION = '" << m_ximcVersion << "';\n";

				*os << "\nconst\n" << m_osStructs.str();

				echoBanner( "BEGIN OF GENERATED function declarations", os );
				*os  << m_osFunctions.str();

				echoBanner( "END OF GENERATED CODE", os );
			}

			void echoBanner (std::string bannerText, std::ostream* os)
			{
					*os << "\n{\n"
						<< " " 		<< std::string( bannerText.size()+4, '-' ) << "\n"
						<< "   "	<< bannerText << "\n"
						<< " "		<< std::string( bannerText.size()+4, '-' ) << "\n"
						<< "}\n";
			}

	};

}

#endif

/* vim: set ts=2 sw=2: */
