#ifndef QTSCRIPTGENERATOR_GETSETFUNC_HH
#define QTSCRIPTGENERATOR_GETSETFUNC_HH

#include "common.hh"
#include "visitor.hh"
#include "basegenerator.hh"
#include "generatorhelper.hh"

namespace xigen
{

	class QtscriptGenerator_getsetfunc : protected DefaultVisitor, public Noncopyable, public BaseGenerator
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

				if (command.communicatorWriter && command.communicatorReader)
				{
					switch (m_mode)
					{
						case modeGenReader:
						case modeGenWriter:
						case modeGenReaderCalb:
						case modeGenWriterCalb:
							pushState( command );
							emitFunctionHead( command );
							break;
					}
				}
				else if (!command.communicatorWriter && command.communicatorReader)
				{
					if (m_mode == modeGenReader || m_mode == modeGenReaderCalb)
					{
						pushState( command );
						emitFunctionHead( command );
					}
				}
				else if (command.communicatorWriter && !command.communicatorReader)
				{
					if (m_mode == modeGenWriter || m_mode == modeGenWriterCalb)
					{
						pushState( command );
						emitFunctionHead( command );
					}
				}
				else
					throw ast_error("Wrong communicator", &command );
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
				if ((command.functionName().substr(0, 4) != "get_") &&
					(command.functionName().substr(0, 4) != "set_"))
					return;
				
				if (m_mode == modeGenReader) {
					stream() << "// READER\n"
						<< command.structName() << "_t AxisObject::" << command.functionName() << "() const" << "\n"
						<< "{" << "\n"
						<< "\t" << command.structName() << "_t s;" << "\n"
						<< "\tdevinterface->" << command.functionName() << "(&s);" << "\n"
						<< "\treturn s;" << "\n"
						<< "}" << "\n"
						;
				}
				else if (command.calb && m_mode == modeGenReaderCalb) {
					stream() << "// READER CALB\n"
						<< command.structCalbName() << "_t AxisObject::" << command.functionCalbName() << "(calibration_t calb) const" << "\n"
						<< "{" << "\n"
						<< "\t" << command.structCalbName() << "_t s;" << "\n"
						<< "\tdevinterface->" << command.functionCalbName() << "(&s, &calb);" << "\n"
						<< "\treturn s;" << "\n"
						<< "}" << "\n"
						;
				}
				else if (m_mode == modeGenWriter)
				{
					stream() << "// WRITER\n"
						<< "void AxisObject::" << command.functionName() << "(" << command.structName() << "_t s) const" << "\n"
						<< "{" << "\n"
						<< "\tdevinterface->" << command.functionName() << "(&s);" << "\n"
						<< "}" << "\n"
						;
				}
				else if (command.calb && m_mode == modeGenWriterCalb)
				{
					stream() << "// WRITER CALB\n"
						<< "void AxisObject::" << command.functionCalbName() << "(" << command.structCalbName() << "_t s, calibration_t calb) const" << "\n"
						<< "{" << "\n"
						<< "\tdevinterface->" << command.functionCalbName() << "(&s, &calb);" << "\n"
						<< "}" << "\n"
						;
				}
			}

			void doGenerate (Protocol* protocol, std::ostream* os)
			{
				m_mode = (Mode)0;
				m_current = NULL;

				protocol->accept( *this );

				*os << m_os.str();
			}

		public:

			explicit QtscriptGenerator_getsetfunc ()
				:	m_current(NULL)
			{
			}

	};

}

#endif

/* vim: set ts=2 sw=2: */
