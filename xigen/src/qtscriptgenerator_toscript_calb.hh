#ifndef QTSCRIPTGENERATOR_TOSCRIPT_CALB_HH
#define QTSCRIPTGENERATOR_TOSCRIPT_CALB_HH

#include "common.hh"
#include "visitor.hh"
#include "basegenerator.hh"

namespace xigen
{

	class QtscriptGenerator_toscript_calb : protected DefaultVisitor, public Noncopyable, public BaseGenerator
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
					stream()
						<< "Q_DECLARE_METATYPE(" << command.structCalbName() << "_t);" << "\n"
						<< "static QScriptValue toScriptValue(QScriptEngine *engine, const " << command.structCalbName() << "_t" << " &s) {" << "\n"
						<< "\tQScriptValue obj = engine->newObject();" << "\n";
				}

			}

			virtual void visitCommandPost (Command& command)
			{
				if (!m_current)
					return;

				stream() << "	return obj;" << "\n" << "}" << "\n";
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

				std::string typecast;
				if (field.type() == VariableEnum::Int64s || field.type() == VariableEnum::Int64u)
					typecast = "(double)";
				else
					typecast = "";

				stream() << "	obj.setProperty(\"" << field.name() << "\", " << typecast << "s." << field.name() << ");\n";
			}

			virtual void visitArrayField (ArrayField& field)
			{
				if (!m_current)
					return;

				if (field.calibrationType() == CalibrationEnum::normal)
					return;

				if (field.type() == VariableEnum::Char)
				{
					stream() << "	obj.setProperty(\"" << field.name() << "\", s." << field.name() << ");\n";
				}
				else
				{
					stream() << "	QScriptValue " << field.name() << " = engine->newObject();" << "\n"
						<< "	for (int i=0; i<" << field.dimExpression() << "; i++) { " << field.name()
						<< ".setProperty(i, s." << field.name() << "[i]); }\n"
						<< "	obj.setProperty(\"" << field.name() << "\", " << field.name() << ");\n";
				}
			}

			virtual void visitFlagField (FlagField& field)
			{
				if (!m_current)
					return;

				stream() << "	obj.setProperty(\"" << field.name() << "\", s." << field.name() << ");\n";
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

			explicit QtscriptGenerator_toscript_calb ()
				:	m_current(NULL)
			{
			}

	};

}

#endif

/* vim: set ts=2 sw=2: */
