#ifndef PYTHONGENERATOR_HH
#define PYTHONGENERATOR_HH

#include "common.hh"
#include "visitor.hh"
#include "modegenerator.hh"

namespace xigen
{

	class PythonGenerator : public Noncopyable, public ModeGenerator
	{
		protected:

			virtual void startStructs()
			{
				stream() << std::endl;
				std::string structName = m_mode == modeGenStructCalb ? m_current->structCalbName() : m_current->structName();

				stream() << "class " << structName << "_t(Structure):\n"
					<< "\t_fields_ = [\n";
			}

			virtual void startFuncs()
			{
			}

			virtual void visitCommandPost (Command& command)
			{
				if (!m_current)
					return;

				switch (m_mode)
				{
					case modeGenStruct:
					case modeGenStructCalb:
						stream() << "\t]\n";
						break;
				}
				m_current = NULL;
			}

			virtual void visitFlagsetPost (Flagset& flagset)
			{
				stream() << "class " << flagset.name() << ":\n";
				printFormattedFlagset( "0x%%.%dX" );
				stream() << "\n";
			}

			virtual void printFormattedFlag (std::string name, std::string value,
					const ConstantField& field)
			{
					stream() << "\t" << name << " = " << value << "\n";
			}

			template <class F>
			std::string do_emitDeclaration (const F& field, bool usePointer = false)
			{
					return "(\"" + field.name() + "\", " + mapToPythonType( remapBindingType( field.type() ), false ) + ")";
			}

			std::string do_emitDeclaration (const ArrayField& field)
			{
				return "(\"" + field.name() + "\", " + mapToPythonType( remapBindingType( field.type() ), true )
					+ " * " + field.dimExpression( field.type() == VariableEnum::Char ? 1 : 0 ) + ")";
			}

			void printFunctionArgument (DataField& field)	{ }
			void printFunctionArgument (FlagField& field)	{ }
			void printFunctionArgument (ArrayField& field)	{ }

			std::string emitDeclaration (const DataField& field)			{ return do_emitDeclaration( field ); }
			std::string emitDeclaration (const FlagField& field)			{ return do_emitDeclaration( field ); }
			std::string emitDeclaration (const ConstantField& field)	{ return do_emitDeclaration( field ); }
			std::string emitDeclaration (const ArrayField& field)			{ return do_emitDeclaration( field ); }

		private:

			std::ostringstream m_osStructs;

			std::ostream& stream()
			{
				return m_osStructs;
			}

			virtual bool acceptMode(Mode mode)
			{
				return mode == modeGenStruct || mode == modeGenStructCalb || mode == modeGenFlagset;
			}

			virtual void output (std::ostream* os)
			{
				echoBanner( "BEGIN OF GENERATED code", os );
				*os << m_osStructs.str();
				echoBanner( "END OF GENERATED code", os );
			}

			void echoBanner (std::string bannerText, std::ostream* os)
			{
				if (m_enableComments)
					*os << "\n"
						<< "# "	<< std::string( bannerText.size()+4, '-' ) << "\n"
						<< "# "	<< bannerText << "\n"
						<< "# "	<< std::string( bannerText.size()+4, '-' ) << "\n";
			}

		public:

			explicit PythonGenerator ()
			{
				m_lineTerminator = ",";
			}

	};

}

#endif

/* vim: set ts=2 sw=2: */
