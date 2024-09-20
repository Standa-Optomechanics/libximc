#ifndef QSDEFINEGENERATOR_HH
#define QSDEFINEGENERATOR_HH

#include "common.hh"
#include "visitor.hh"
#include "basegenerator.hh"

namespace xigen
{

	class QsdefineGenerator : protected DefaultVisitor, public Noncopyable, public BaseGenerator
	{
		protected:

			virtual bool visitCommand (Command& command, size_t cookie)
			{
				return !cookie;
			}

			virtual void visitFlagset (Flagset& flagset)
			{
				m_savedFields.clear();
			}

			virtual void visitFlagsetPost (Flagset& )
			{
				size_t dummy, digits;
				helpers::determineFlagsMaxWidth(m_savedFields, dummy, digits);

				char formatString[32], buf[32];
				sprintf(formatString, "0x%%.%dX", (int)digits );

				for (Flags::iterator it = m_savedFields.begin(); it != m_savedFields.end(); ++it)
				{
					const ConstantField& field = **it;

					stream() << "qengine->globalObject().setProperty(\"" << field.name() << "\", ";
					sprintf( buf, formatString, field.value() );
					stream() << buf << ");\n";
				}
			}

			virtual void visitConstantField (ConstantField& field)
			{
				m_savedFields.push_back( &field );
			}

		private:

			std::ostringstream m_os;

			Flags m_savedFields;

			std::ostream& stream()
			{
				return m_os;
			}

			void doGenerate (Protocol* protocol, std::ostream* os)
			{
				protocol->accept( *this );
				*os << m_os.str();
			}
	};

}

#endif

/* vim: set ts=2 sw=2: */
