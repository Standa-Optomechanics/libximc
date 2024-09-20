#ifndef CSHARPGENERATOR_HH
#define CSHARPGENERATOR_HH

#include "common.hh"
#include "visitor.hh"
#include "modegenerator.hh"
#include "generatorhelper.hh"

namespace xigen
{

	class CSharpGenerator : public Noncopyable, public ModeGenerator
	{
		protected:

			virtual void startStructs()
			{
				std::string structName = m_mode == modeGenStructCalb ? m_current->structCalbName() : m_current->structName();
				stream() << "\n\t[StructLayout(LayoutKind.Sequential, CharSet=CharSet.Ansi)]\n"
					<< "\tpublic struct "  << structName << "_t\n\t{\n";
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
						stream() << ", ref calibration_t calibration);" << std::endl;
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
					stream() << "\t\tpublic const uint " << name << " = " << value << ";\n";
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
				stream() << ", " << emitDeclarationImpl( field, m_mode == modeGenReader || m_mode == modeGenReaderCalb );
			}

			void do_printFunctionArgument (ArrayField& field)
			{
				XI_ASSERT(helpers::fieldPurpose( field ) == DataField::Purpose::normal);
				stream() << ", " <<
					toString( "[MarshalAs(UnmanagedType.ByValArray, SizeConst = " + toString(field.getDim()) + ")] ") +
					emitDeclarationImpl( field, m_mode == modeGenReader || m_mode == modeGenReaderCalb );
			}

			template <class F>
			std::string do_emitDeclaration (const F& field, bool usePointer = false)
			{
				return "public " + emitDeclarationImpl( field, usePointer );
			}

			template <class F>
			std::string emitDeclarationImpl (const F& field, bool usePointer = false)
			{
					return (usePointer?"out " : "") + mapToNativeCSharpType( remapBindingType(field.type() ), false ) + " " + toString( field.name() );
			}

			std::string emitDeclarationImpl (const ArrayField& field, bool usePointer = false)
			{
					return (usePointer?"out " : "") + mapToNativeCSharpType( remapBindingType( field.type() ), true )
						+ (field.type() == VariableEnum::Char ? " " : "[] ") + toString( field.name() );
			}

			void printFunctionArgument (DataField& field) 	{ do_printFunctionArgument( field ); }
			void printFunctionArgument (FlagField& field) 	{ do_printFunctionArgument( field ); }
			void printFunctionArgument (ArrayField& field)
			{
				if (field.type() == VariableEnum::Char)
					throw ast_error( "Cannot generate C# string function arg" );
				do_printFunctionArgument( field );
			}

			std::string emitDeclaration (const DataField& field) 			{ return do_emitDeclaration( field ); }
			std::string emitDeclaration (const FlagField& field) 			{ return do_emitDeclaration( field ); }
			std::string emitDeclaration (const ConstantField& field) 	{ return do_emitDeclaration( field ); }

			std::string emitDeclaration (const ArrayField& field)
			{
				std::string result;
				if (field.type() == VariableEnum::Char)
				{
					result = "[MarshalAs(UnmanagedType.ByValTStr, SizeConst = " + toString(field.getSize()+1) + ")]\n\t\t";
					result += do_emitDeclaration( field, false );
				}
				else
				{
					result = "[MarshalAs(UnmanagedType.ByValArray, SizeConst = " + toString(field.getDim()) + ")]\n\t\t";
					result += do_emitDeclaration( field );
				}
				return result;
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

				std::string result = std::string("\t\t[DllImport(\"libximc.dll\")]\n") +
					"\t\tpublic static extern Result "  +
					commandName + " (int id";

				if (!command.is("inline") && command.withAnyFields())
				{
					result += (reader ? ", out " : ", ref ") +
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
					*os << "\n\tpublic static class Version\n"
							<< "\t{\n"
							<< "\t\tpublic const String version = \"" << m_ximcVersion << "\";\n"
							<< "\t};\n";

				echoBanner( "BEGIN OF GENERATED struct declarations", os );
				*os << "\tpublic static class Flags\n"
						<< "\t{\n\n";
				*os << m_osStructs.str();

				echoBanner( "BEGIN OF GENERATED function declarations", os );
				*os
					<< "\n\tpublic partial class API\n"
					<< "\t{"
					<< m_osFunctions.str()
					<< "\t};\n";

				echoBanner( "END OF GENERATED CODE", os );
			}

			void echoBanner (std::string bannerText, std::ostream* os)
			{
					*os << "\n/*\n"
						<< " " 		<< std::string( bannerText.size()+4, '-' ) << "\n"
						<< "   "	<< bannerText << "\n"
						<< " "		<< std::string( bannerText.size()+4, '-' ) << "\n"
						<< "*/\n";
			}
	};

}

#endif

/* vim: set ts=2 sw=2: */
