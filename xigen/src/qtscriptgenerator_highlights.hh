#ifndef QTSCRIPTGENERATOR_HIGHLIGHTS_HH
#define QTSCRIPTGENERATOR_HIGHLIGHTS_HH

#include "common.hh"
#include "visitor.hh"
#include "basegenerator.hh"
#include "generatorhelper.hh"

namespace xigen
{

	class QtscriptGenerator_highlights : protected DefaultVisitor, public Noncopyable, public BaseGenerator
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
							pushState( command );
							emitFunctionHead( command );
							break;
					}
				}
				else if (!command.communicatorWriter && command.communicatorReader)
				{
					if (m_mode == modeGenReader)
					{
						pushState( command );
						emitFunctionHead( command );
					}
				}
				else if (command.communicatorWriter && !command.communicatorReader)
				{
					if (m_mode == modeGenWriter)
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
				stream() << command.functionName() << "|";
				if (command.calb)
				{
					stream() << command.functionCalbName() << "|";
				}
			}
			void writeHeader()
			{
				m_os << " rule.pattern = QRegExp(\"\\\\b(";
			}
			void writeFooter()
			{
				m_os << "log|msleep|new_axis|new_file|new_calibration|get_next_serial|read|write)\\\\b\");";
			}

			void doGenerate (Protocol* protocol, std::ostream* os)
			{
				m_mode = (Mode)0;
				m_current = NULL;

				writeHeader();
				protocol->accept( *this );
				writeFooter();
				*os << m_os.str();
			}

		public:

			explicit QtscriptGenerator_highlights ()
				:	m_current(NULL)
			{
			}

	};

}

#endif

/* vim: set ts=2 sw=2: */
