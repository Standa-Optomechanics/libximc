#ifndef WIKIGENERATOR_HH
#define WIKIGENERATOR_HH

#include "common.hh"
#include "visitor.hh"
#include "basegenerator.hh"
#include "generatorhelper.hh"

namespace xigen
{


	class WikiGenerator : protected IVisitor, public Noncopyable, public BaseGenerator
	{
		protected:
			virtual void visitProtocol (Protocol& )
			{
			}

			virtual void visitCommands ()
			{
			}

			virtual bool visitCommand (Command& command, size_t cookie)
			{
				if (cookie >= (size_t)modeNULL)
				{
					finishCommand( command );
					return false;
				}
				m_mode = (Mode)cookie;
				visitCommandImpl( command );
				return true;
			}

			virtual void visitCommandImpl (Command& command)
			{
				m_current = NULL;

				if (command.unsynced)
					return;

				if (!command.is("public") && !command.is("publicstruct"))
					return;

				pushState( command );
				if (m_mode == modeGenHeader)
				{
					switch (command.communicable())
					{
						case Communicable::both:
						case Communicable::reader:
							emitFunctionHead( command, command.communicatorReader );
							break;
						case Communicable::writer:
							emitFunctionHead( command, command.communicatorWriter );
							break;
					}
				}
			}

			bool printDoxyComment (Command& command, Comment::DoxyType type)
			{
				if (command.doxyComments.find( type ) != command.doxyComments.end())
				{
					std::string descr = m_locale->extractDoxygenComment(command.doxyComments[type] );
					// if (!descr.empty() && descr[descr.size() - 1] != '.')
					// 	descr += ".";
					stream() << "\n*" << m_locale->wikiDescription() << ":*\n" << descr << "\n";
					return true;
				}
				return false;
			}

			void printOptionalDoxySection (Command& command)
			{
				if (command.doxyComments.find( Comment::doxySection ) != command.doxyComments.end())
				{
					std::string comment = m_locale->extractDoxygenComment(command.doxyComments[Comment::doxySection]);
					stream() << "h2. " << comment << "\n";
					comment = m_locale->extractDoxygenCommentDescription(command.doxyComments[Comment::doxySection]);
					if (!comment.empty())
					{
						stream() << "\n" << comment << "\n";
					}
				}
			}

			virtual void visitCommandPost (Command& command)
			{
				if (!m_current)
					return;

				switch (m_mode)
				{
					case modeGenHeader:
					{
						Communicator *comm = command.communicatorReader ? command.communicatorReader : command.communicatorWriter;
						stream() << ")</code></pre>";
						stream() << m_locale->wikiCommandCode() << " (CMD): \"" << tolower(comm->name)
							<< "\" " << xigen::tolower( m_locale->wikiOr() ) << " " << comm->asHexStringUpper() << ".\n";
						break;
					}

					case modeGenStruct:
						if (m_current->withAnswer() && m_current->is("crc"))
							stream() << "|INT16U|CRC|" << m_locale->wikiCRC() << "|\n";

						if (command.communicatorReader || command.communicatorWriter)
							printDoxyComment( command, Comment::doxyRead ) ||
								printDoxyComment( command, Comment::doxyWrite );
						else if (command.communicatorWriter)
							printDoxyComment( command, Comment::doxyWrite );
						else if (command.communicatorReader)
							printDoxyComment( command, Comment::doxyRead );
						break;
				}

				m_current = NULL;
			}

			virtual void visitRequest ()
			{
				if (!m_current)
					return;

				if (m_mode == modeGenStruct)
				{
					int amount = 0;
					if (m_current->communicatorWriter)
						amount = m_current->communicatorWriter->size;
					else
						amount = 4;

					stream() << "\n*" << m_locale->wikiRequest() << ":* ("
						<< amount << " " << m_locale->pluralizeBytes( amount ) << ")\n";
					stream() << "\n|INT32U|CMD|" << m_locale->wikiCommand() << "|\n";
				}
			}

			virtual void visitAnswer ()
			{
				if (!m_current)
					return;

				if (m_mode == modeGenStruct)
				{
					if (m_current->withFields() && m_current->is("crc"))
						stream() << "|INT16U|CRC|" << m_locale->wikiCRC() << "|\n";

					int amount = 0;
					if (m_current->is("answer"))
					{
						if (m_current->communicatorReader)
							amount = m_current->communicatorReader->size;
						else
							amount = 4;
					}

					stream() << "\n*" << m_locale->wikiAnswer() << ":* ("
						<< amount << " " << m_locale->pluralizeBytes( amount ) << ")\n";
					if (m_current->is("answer"))
						stream() << "\n|INT32U|CMD|" << m_locale->wikiCommandBack() << "|\n";
				}
			}

			virtual void visitFlagset (Flagset& flagset)
			{
				m_current = NULL;
			}

			virtual void visitFlagsetPost (Flagset& )
			{
				if (!fieldGuard()) return;
			}

			virtual void visitDataField (DataField& field)
			{
				if (!fieldGuard()) return;

				if (field.calibrationType() == CalibrationEnum::calb)
					return;

				switch (m_mode)
				{
					case modeGenHeader:
						if (m_current->is("inline"))
							printFunctionArgument( field );
						break;

					case modeGenStruct:
						stream() << emitDeclaration( field );
						finishStructField( field );
						break;
				}
			}

			virtual void visitConstantField (ConstantField& field)
			{
				if (!fieldGuard()) return;

				switch (m_mode)
				{
					case modeGenHeader:
						if (m_current->is("inline"))
							printFunctionArgument( field );
						break;

					case modeGenStruct:
						stream() << emitDeclaration( field );
						finishStructField( field );
						break;
				}
			}

			virtual void visitArrayField (ArrayField& field)
			{
				if (!fieldGuard()) return;

				if (field.calibrationType() == CalibrationEnum::calb)
					return;

				switch (m_mode)
				{
					case modeGenHeader:
						if (m_current->is("inline"))
							printFunctionArgument( field );
						break;

					case modeGenStruct:
						stream() << emitDeclaration( field )
							<< " [" << field.dimExpression() << "]";
						finishStructField( field );
						break;
				}
			}

			virtual void visitFlagField (FlagField& field)
			{
				if (!fieldGuard()) return;

				if (m_mode == modeGenStruct)
				{
					stream() << emitDeclaration( field );
					finishStructField( field );

					// print flags values after a field
					for (Flagsets::iterator it = m_protocol->flagsets.begin(); it != m_protocol->flagsets.end(); ++it)
					{
						if (field.flagset() == (*it)->name())
						{
							size_t dummy, digits;
							helpers::determineFlagsMaxWidth((*it)->flags(), dummy, digits);
							char formatString[32], buf[32];
							sprintf(formatString, "0x%%.%dX", (int)digits );
							for (Flags::const_iterator itFlag = (*it)->flags().begin();
									itFlag != (*it)->flags().end(); ++itFlag)
							{
								sprintf( buf, formatString, (*itFlag)->value() );
								stream() << "|\\2.| " << buf << " - " << (*itFlag)->name();
								if ((*itFlag)->getComment())
									stream() << " (" << m_locale->extractDoxygenCommentOneLine(*((*itFlag)->getComment())) << ")";
								stream() << "|\n";
							}
						}
					}
				}

			}

			virtual void visitReservedField (ReservedField& field)
			{
				if (!fieldGuard()) return;

				if (m_mode == modeGenStruct)
				{
					stream() << "|INT8U|Reserved [" << field.getSize() << "]|" << m_locale->wikiReserved() << " ("
						<< field.getSize() << " "
						<< m_locale->pluralizeBytes(field.getSize()) << ")|\n";
				}
			}

			bool fieldGuard()
			{
				return m_current;
			}

			template <class F>
			void printFunctionArgument (F& field)
			{
				if (helpers::fieldPurpose( field ) == DataField::Purpose::answer && !m_current->is("inline"))
					throw ast_error("Command with answer field must be inline");

				if (helpers::fieldPurpose( field ) == DataField::Purpose::answer)
					stream() << ", service_result_t* " << toString( field.name() );
				else
					stream() << ", " << emitDeclaration( field, (bool)m_current->communicatorReader );
			}

			void finishStructField (Field& field)
			{
				stream() << "|";
				if (field.getComment())
					stream() << m_locale->extractDoxygenCommentOneLine( *(field.getComment()) );
				stream() << "|\n";
			}

		private:

			typedef enum { modeGenHeader, modeGenStruct, modeNULL } Mode;
			Mode m_mode;

			std::ostringstream m_os;
			std::ostringstream m_osCurrentCommand;

			Command* m_current;

			std::ostream& stream()
			{
				return m_osCurrentCommand;
			}

			void finishCommand (Command& command)
			{
				m_os << m_osCurrentCommand.str()
					<< std::endl;
				m_osCurrentCommand.str("");
			}

			void pushState(Command& command)
			{
				m_current = &command;
			}

			template <class F>
			std::string emitDeclaration (const F& field, bool usePointer = false)
			{
				switch (m_mode)
				{
					case modeGenHeader:
						return mapToNativeType( field.type(), false ) + (usePointer?"* ":" ") + toString( field.name() );
					case modeGenStruct:
						return "|" + mapToNativeWikiType( field.type() ) + "|" + toString( field.name() );
					default:
						throw std::runtime_error("Wrong mode" );
				}
			}

			void emitFunctionHead (Command& command, Communicator* comm)
			{
				if (!command.paired || command.communicatorWriter)
					printOptionalDoxySection(command);

				XI_ASSERT(comm);
				//stream() << "\n==========================================\n";
				stream() << "\nh3. " << m_locale->wikiCommand() << " " << toupper(comm->name) << "\n\n";

				stream() << "<pre><code>";
				stream() << helpers::emitFunctionHead( command, (bool)command.communicatorReader, false, false, true );
			}

			void doGenerate (Protocol* protocol, std::ostream* os)
			{
				m_mode = (Mode)0;
				m_current = NULL;

				protocol->accept( *this );

				*os << m_os.str();
			}

		public:

			explicit WikiGenerator ()
				:	m_current(NULL)
			{
			}

	};

}

#endif

/* vim: set ts=2 sw=2: */
