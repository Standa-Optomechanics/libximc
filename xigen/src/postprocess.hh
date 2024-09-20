#ifndef POSTPROCESS_HH
#define POSTPROCESS_HH

#include "common.hh"
#include "visitor.hh"
#include "generatorhelper.hh"

namespace xigen
{


	class PostProcessor : public DefaultVisitor, public Noncopyable
	{
		protected:
			void doVisitBaseCommand (Command& command)
			{
				m_current = &command;

				m_currentNest = NULL;

				if (command.communicatorReader && command.communicatorWriter && command.communicatorUniversal)
					throw ast_error("Wrong command.communicator configuration", &command);

				if (command.communicatorReader)
					if (command.communicatorReader->isString &&
								command.communicatorReader->name.size() != 4)
						throw ast_error( "wrong communicator code", &command);
				if (command.communicatorWriter)
					if (command.communicatorWriter->isString &&
								command.communicatorWriter->name.size() != 4)
						throw ast_error( "wrong communicator code", &command);

				if (command.communicatorUniversal)
						throw ast_error( "universal commnunicator still here for " + command.toString());
			}

			virtual bool visitCommand (Command& command, size_t cookie)
			{
				if (!cookie)
				{
					// manually remove crc flag from commands without sent fields
					// because 'without crc' looks little silly
					if (!command.withAnyFields() && command.is("crc"))
					{
						Features f;
						f["crc"] = false;
						command.addFeatures(f);
					}

					doVisitBaseCommand( command );
				}
				return !cookie;
			}

			virtual void visitCommandPost (Command& command)
			{
				finishFieldNest();
				if (command.is("crc"))
				{
					if (!command.withAnyFields())
						throw ast_error("Found weird command without fields and with crc", &command );
					// replaced the following
					if (command.communicable() == Communicable::both && !command.withFields() && command.withAnswer())
						throw ast_error("Found weird command without fields and with crc", &command );
				}
			}

			virtual void visitFlagset (Flagset& flagset)
			{
				if (!flagset.comment())
					throw ast_error("Flagset without doxygen comment is not allowed");
			}

			virtual void visitDataField (DataField& field)
			{
				checkFieldName( field );
				if (field.type() == VariableEnum::Char || field.type() == VariableEnum::Byte)
					throw ast_error( "Single char/byte is not supported yet, please use int8" );
				handleFieldToNest( field );
			}

			virtual void visitConstantField (ConstantField& field)
			{
				checkFieldName( field );
				if (m_current && m_current->is("inline"))
					throw ast_error( "Inline constant field is not supported", m_current );
				finishFieldNest();
			}

			virtual void visitArrayField (ArrayField& field)
			{
				checkFieldName( field );
				handleFieldToNest( field );
			}

			virtual void visitFlagField (FlagField& field)
			{
				if (field.type() == VariableEnum::Char)
					throw ast_error("Char flags are not allowed", m_current);
				checkFieldName( field );
				finishFieldNest();
			}

			template <class F>
			void checkFieldName (const F& field)
			{
				if (m_deniedNames.find( field.name() ) != m_deniedNames.end())
					throw ast_error( "Can't use such variable name " + field.name(), m_current );
				checkFieldComment( field );
				if (helpers::fieldPurpose( field ) == DataField::Purpose::answer &&
						!m_current->is("inline"))
					throw ast_error("Command with answer field must be inline");
			}

			template <class F>
			void checkFieldComment(const F& field)
			{
				if (m_warnings && !m_no_comments && !field.getComment() && 
						field.name() != "deprecated" &&
						helpers::fieldPurpose( field ) == DataField::Purpose::normal)
					std::cerr << "WARN there is no comment for command " << m_current->name()
						<< " field " << field.name() << std::endl;
			}

			template <class F>
			void handleFieldToNest (F& field)
			{
				switch (field.calibrationType())
				{
					case CalibrationEnum::calb:
					{
						if (m_currentNest == NULL)
							m_currentNest = new FieldNest();
						else
						{
							// no two calbs in row
							if (m_currentNest->fieldCalb && !m_currentNest->fieldNormal)
								throw ast_error( "Wrong calibration layout: two calb in row" );
							// finish filled nest
							if (m_currentNest->fieldCalb && m_currentNest->fieldNormal)
							{
								m_current->addFieldNest(m_currentNest);
								// start new nest
								m_currentNest = new FieldNest();
							}
						}
						m_currentNest->fieldCalb = &field;
						break;
					}

					case CalibrationEnum::normal:
					{
						if (m_currentNest == NULL || !m_currentNest->fieldCalb)
							throw ast_error( "Wrong calibration layout: normal field without preceding calb field", m_current );
						if (m_currentNest->fieldNormal)
							m_currentNest->fieldMicro = &field;
						else
							m_currentNest->fieldNormal = &field;
						break;
					}

					case CalibrationEnum::none:
						finishFieldNest();
						break;

				}
			}

			void finishFieldNest ()
			{
				if (m_currentNest && !m_currentNest->fieldCalb)
					throw ast_error( "Wrong calibration layout: calibration fields nest is not completed", m_current );
				if (m_currentNest && m_currentNest->fieldCalb && !m_currentNest->fieldNormal)
					throw ast_error( "Wrong calibration layout: calibration fields nest is not completed", m_current );

				// finish filled nest
				if (m_currentNest && m_currentNest->fieldCalb && m_currentNest->fieldNormal)
				{
					m_current->addFieldNest(m_currentNest);
					m_currentNest = NULL;
				}
			}


		private:
			Command* m_current;
			const bool m_verbose;
			const bool m_warnings;
			const bool m_no_comments;

			std::set<std::string> m_deniedNames;

			FieldNest* m_currentNest;

			void joinCommandCommunicators (Command& command)
			{
				if (command.communicatorReader && command.communicatorWriter
						&& command.communicatorReader->size == command.communicatorWriter->size
						&& command.communicatorReader->name != command.communicatorWriter->name
						&& command.communicatorReader->isString && command.communicatorWriter->isString
						&& command.communicatorReader->name.substr(1) == command.communicatorWriter->name.substr(1)
						&& command.communicatorReader->name[0] == 'g' && command.communicatorWriter->name[0] == 's')
				{
					if (command.communicatorUniversal)
						delete command.communicatorUniversal;
					command.communicatorUniversal = new Communicator(command.communicatorReader->name.substr(1,std::string::npos),
							command.communicatorReader->size);
					command.communicatorReader = command.communicatorWriter = NULL;
				}
			}

		public:

			PostProcessor(bool verbose, bool warnings, bool no_comments)
				: m_current(NULL), m_verbose(verbose), m_warnings(warnings), m_no_comments(no_comments), m_currentNest(NULL)
			{
				m_deniedNames.insert("result");
				m_deniedNames.insert("i");
				m_deniedNames.insert("p");
				m_deniedNames.insert("command");
				m_deniedNames.insert("response");
				m_deniedNames.insert("id");
			}

			void pre (Protocol& protocol)
			{
				for (Commands::iterator it = protocol.commands.begin(); it != protocol.commands.end(); )
				{
					Command* command = *it;
					joinCommandCommunicators( *command );
					if (command->communicatorUniversal)
					{
						Command* forked = command->forkUniversal();
						it = protocol.commands.insert( it, forked );
					}
					else
						++it;
				}

				for (Commands::iterator it = protocol.commands.begin(); it != protocol.commands.end(); )
				{
					Command* command = *it;
					if (command->is("dualsync"))
					{
						Command* forked = command->forkDualSync();
						it = protocol.commands.insert( it, forked );
						++it;
						++it;
					}
					else
						++it;
				}

				for (Commands::iterator it = protocol.commands.begin(); it != protocol.commands.end(); ++it)
					(*it)->rebaseFields();
			}

			void post (Protocol& protocol)
			{
				if (m_verbose)
				{
					std::cerr << "Post-processed commands" << std::endl;
					for (Commands::iterator it = protocol.commands.begin(); it != protocol.commands.end(); ++it)
						std::cerr << **it << std::endl;
				}
			}

	};

}

#endif


/* vim: set ts=2 sw=2: */
