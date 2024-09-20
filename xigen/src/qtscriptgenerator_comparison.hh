#ifndef QTSCRIPTGENERATOR_COMPARISON_HH
#define QTSCRIPTGENERATOR_COMPARISON_HH

#include "common.hh"
#include "visitor.hh"
#include "basegenerator.hh"
#include "generatorhelper.hh"

namespace xigen
{

	class QtscriptGenerator_comparison : protected DefaultVisitor, public Noncopyable, public BaseGenerator
	{
		protected:
			virtual bool visitCommand (Command& command, size_t cookie)
			{
				if (cookie >= (size_t)modeNULL)
					return false;
				m_mode = (Mode)cookie;
				visitCommandImpl( command );
				return true;
			}

			virtual void visitCommandImpl (Command& command)
			{
				m_current = NULL;

				if (!command.is("public"))
					return;
				if (command.paired && command.communicatorReader)
				{
					if (m_mode == modeGenReader) {
						pushState(command);
						emitFunctionHead(command);
					}
				}
				else if (command.paired && command.communicatorWriter)
				{
				}
				else if (!command.paired && !command.communicatorWriter && command.communicatorReader)
				{
					if (m_mode == modeGenReader)
					{
						pushState( command );
						emitFunctionHead( command );
					}
				}
				else if (!command.paired && command.communicatorWriter && !command.communicatorReader)
				{
					if (m_mode == modeGenWriter)
					{
						pushState(command);
						emitFunctionHead(command);
					}
				}
				else
					throw ast_error("Wrong communicator", &command );

			}

			virtual void visitCommandPost(Command& command)
			{
				if (!m_current)
					return;
				if (!command.withAnyFields())
					return;

				stream() << "\t" << "return eq;" << std::endl << "}" << std::endl << std::endl
				;
				m_current = NULL;
			}

			virtual void visitFlagset (Flagset& flagset)
			{
				m_current = NULL;
			}

		private:

			typedef enum { modeGenWriter, modeGenReader, modeGenWriterCalb, modeGenReaderCalb, modeNULL } Mode;
			Mode m_mode;

			std::ostringstream m_os;

			Command* m_current;

			std::ostream& stream()
			{
				return m_os;
			}

			void pushState(Command& command)
			{
				m_current = &command;
			}

			void emitFunctionHead (Command& command)
			{
				if (!command.withAnyFields())
					return;
				stream()
					<< "bool operator==(const libximc::" << command.structName()
					<< "_t& l, const libximc::" << command.structName() << "_t& r)" << std::endl
					<< "{" << std::endl << "\t" << "bool eq = true;" << std::endl;
				;
			}

			virtual void visitDataField(DataField& field)
			{
				if (!m_current)
					return;

				if (field.calibrationType() == CalibrationEnum::calb)
					return;

				stream() << "\t" << "eq &= (l." << field.name() << " == r." << field.name() << ");" << std::endl;
			}

			virtual void visitArrayField(ArrayField& field)
			{
				if (!m_current)
					return;

				if (field.calibrationType() == CalibrationEnum::calb)
					return;

				if (field.type() == VariableEnum::Char)
				{
					stream() << "\t" << "eq &= (strcmp(l." << field.name() << ", r." << field.name()
						<< ") == 0);" << std::endl;
				}
				else
				{
					stream() << "\tfor (int i=0; i<" << field.dimExpression() << "; i++) { eq &= (l."
						<< field.name() << "[i] == r." << field.name() << "[i]); }\n";
				}
			}

			virtual void visitFlagField(FlagField& field)
			{
				if (!m_current)
					return;

				stream() << "\t" << "eq &= (l." << field.name() << " == r." << field.name() << ");" << std::endl;
			}

			void doGenerate (Protocol* protocol, std::ostream* os)
			{
				m_mode = (Mode)0;
				m_current = NULL;

				protocol->accept( *this );

				*os << m_os.str();
			}

		public:

			explicit QtscriptGenerator_comparison ()
				:	m_current(NULL)
			{
			}

	};

}

#endif

/* vim: set ts=2 sw=2: */
