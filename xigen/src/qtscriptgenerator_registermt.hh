#ifndef QTSCRIPTGENERATOR_REGISTERMT_HH
#define QTSCRIPTGENERATOR_REGISTERMT_HH

#include "common.hh"
#include "visitor.hh"
#include "basegenerator.hh"
#include "generatorhelper.hh"

namespace xigen
{

	class QtscriptGenerator_registermt : protected DefaultVisitor, public Noncopyable, public BaseGenerator
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
				if ((command.functionName().substr(0, 4) != "get_") &&
					(command.functionName().substr(0, 4) != "set_"))
					return;

				if (m_mode == modeGenReader)
				{
					stream()
						<< "qScriptRegisterMetaType<" << command.structName() << "_t>(qengine, toScriptValue, fromScriptValue);" << "\n"
						;
					if (command.calb) {
						stream()
							<< "qScriptRegisterMetaType<" << command.structCalbName() << "_t>(qengine, toScriptValue, fromScriptValue);" << "\n"
							;
					}
				}
				else if (m_mode == modeGenWriter)
				{
					stream()
						<< "qScriptRegisterMetaType<" << command.structName() << "_t>(qengine, toScriptValue, fromScriptValue);" << "\n"
						;
					if (command.calb) {
						stream()
							<< "qScriptRegisterMetaType<" << command.structCalbName() << "_t>(qengine, toScriptValue, fromScriptValue);" << "\n"
							;
					}
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

			explicit QtscriptGenerator_registermt ()
				:	m_current(NULL)
			{
			}

	};

}

#endif

/* vim: set ts=2 sw=2: */
