#ifndef QTSCRIPTGENERATOR_FROMSCRIPT_CALB_HH
#define QTSCRIPTGENERATOR_FROMSCRIPT_CALB_HH

#include "common.hh"
#include "visitor.hh"
#include "basegenerator.hh"

namespace xigen
{

	class QtscriptGenerator_fromscript_calb : protected DefaultVisitor, public Noncopyable, public BaseGenerator
	{
		protected:

			virtual bool visitCommand (Command& command, size_t cookie)
			{
				if (cookie >= 1)
					return false;
				visitCommandImpl( command );
				return true;
			}

			virtual void visitCommandImpl (Command& command)
			{
				m_current = NULL;

				if (!command.is("public"))
					return;
				if (!command.calb)
					return;

				if (command.communicatorWriter && command.paired)
					return;
				if (command.unsynced)
					return;

				if (command.withAnyFields() && !command.is("inline"))
				{
					pushState( command );
					stream() << std::endl
						<< "static void fromScriptValue(const QScriptValue &obj, " << command.structCalbName() << "_t &s) {" << "\n";
				}
			}

			virtual void visitCommandPost (Command& command)
			{
				if (!m_current)
					return;

				stream() << "}\n";
				m_current = NULL;
			}

			virtual void visitFlagset (Flagset& flagset)
			{
				m_current = NULL;
			}

			virtual void visitDataField (DataField& field)
			{
				if (!m_current)
					return;

				if (field.calibrationType() == CalibrationEnum::normal)
					return;

				stream() << "	s." << field.name() << " = obj.property(\"" << field.name() << "\").to" << mapToQtType( field.type() ) << "();\n";
			}

			virtual void visitArrayField (ArrayField& field)
			{
				if (!m_current)
					return;

				if (field.calibrationType() == CalibrationEnum::normal)
					return;

				if (field.type() == VariableEnum::Char)
				{
					stream() << "\tstrncpy(s." << field.name() << ", obj.property(\"" << field.name()
						<< "\").toString().toLocal8Bit().data(), " << field.dimExpression() << ");\n";
				}
				else
				{
					stream() << "\tfor (int i=0; i<" << field.dimExpression() << "; i++) { s."
						<< field.name() << "[i] = obj.property(\"" << field.name() << "\").property(i).to" << mapToQtType( field.type() ) << "(); }\n";
				}
			}

			virtual void visitFlagField (FlagField& field)
			{
				if (!m_current)
					return;

				stream() << "	s." << field.name() << " = obj.property(\"" << field.name() << "\").to" << mapToQtType( field.type() ) << "();\n";
			}

		private:

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

			void doGenerate (Protocol* protocol, std::ostream* os)
			{
				m_current = NULL;

				protocol->accept( *this );

				*os << m_os.str();
			}

		public:

			explicit QtscriptGenerator_fromscript_calb ()
				:	m_current(NULL)
			{
			}

	};

}

#endif

/* vim: set ts=2 sw=2: */
