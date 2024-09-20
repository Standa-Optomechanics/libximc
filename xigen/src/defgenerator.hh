#ifndef DEFGENERATOR_HH
#define DEFGENERATOR_HH

#include "common.hh"
#include "visitor.hh"
#include "basegenerator.hh"
#include "generatorhelper.hh"

namespace xigen
{

	class DefGenerator : protected DefaultVisitor, public Noncopyable, public BaseGenerator
	{
		protected:
			virtual void visitProtocol (Protocol& )
			{
				m_counter = 0;
			}

			virtual bool visitCommand (Command& command, size_t cookie)
			{
				if (cookie >= 1)
					return false;
				visitCommandImpl( command );
				return true;
			}

			virtual void visitCommandImpl (Command& command)
			{
				if (command.is("public"))
				{
					stream() << "\t" << command.functionName() << " @" << (++m_counter) << "\n";
					if (command.calb)
						stream() << "\t" << command.functionCalbName() << " @" << (++m_counter) << "\n";
				}
			}

		private:

			std::ostream* m_os;
			int m_counter;

			std::ostream& stream()
			{
				return *m_os;
			}

			void doGenerate (Protocol* protocol, std::ostream* os)
			{
				m_os = os;
				protocol->accept( *this );
			}

		public:

			explicit DefGenerator ()
				: m_os(NULL), m_counter(0)
			{
			}
	};

}

#endif

/* vim: set ts=2 sw=2: */
