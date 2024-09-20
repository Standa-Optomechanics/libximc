#ifndef FWHEADERGENERATOR_HH
#define FWHEADERGENERATOR_HH

#include "common.hh"
#include "visitor.hh"
#include "basegenerator.hh"
#include "generatorhelper.hh"

namespace xigen
{


	class FwHeaderGenerator : protected DefaultVisitor, public Noncopyable, public BaseGenerator
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

			bool printOptionalDoxyComment (Command& command, Comment::DoxyType type)
			{
				if (m_enableComments && command.doxyComments.find( type ) != command.doxyComments.end())
				{
					stream()  << "// "
						<< m_locale->extractDoxygenCommentOneLine(command.doxyComments[type] ) << "\n";
					return true;
				}
				return false;
			}


			std::string getStructName (Command& command)
			{
				std::string communicatorName;
				if (!command.communicatorWriter && command.communicatorReader)
				{
					communicatorName = command.communicatorReader->name;
				}
				else if (command.communicatorWriter && !command.communicatorReader)
				{
					communicatorName = command.communicatorWriter->name;
				}
				if (communicatorName.size() != 4)
						throw ast_error( "Wrong command name extracted " + communicatorName, &command );
				return tolower(communicatorName);
			}

			void printCommCodeLine (const Communicator* comm)
			{
				stream() << "#define " << toupper(comm->name) << "_CMD\t"
								<< comm->asHexString() << "\n";
			}

			virtual void visitCommandImpl (Command& command)
			{
				m_current = NULL;

				if (m_mode == modeGenCommandCode)
				{
					if (command.unsynced)
						return;

					pushState( command );

					if (command.communicatorWriter)
						printCommCodeLine(command.communicatorWriter);
					else if (command.communicatorReader)
						printCommCodeLine(command.communicatorReader);
					return;
				}
				else if (m_mode == modeGenStruct)
				{
					if (command.communicatorReader && command.paired)
						return;
					if (command.unsynced)
						return;

					// do not keep complicated commands (ex-service)
					if (command.communicable() == Communicable::both)
						return;

 					if (command.withAnyFields())
					{
						pushState( command );
						stream() << std::endl;

						if (!printOptionalDoxyComment( command, Comment::doxyStruct ))
							if (!printOptionalDoxyComment( command, Comment::doxyWrite ))
								printOptionalDoxyComment( command, Comment::doxyRead );

						stream() << "typedef struct " << getStructName(command)
							<< "_cmd_str {\n";
					}
					return;
				}

				if (!command.is("public"))
					return;

			}

			virtual void visitCommandPost (Command& command)
			{
				if (!m_current)
					return;

				switch (m_mode)
				{
					case modeGenStruct:
						stream() << "} " << getStructName(command) << "_cmd_str; \n";
						break;
				}
				m_current = NULL;
			}

			virtual void visitFlagset (Flagset& flagset)
			{
				m_mode = modeGenFlagset;
				m_current = NULL;
				m_savedFields.clear();

				if (!(m_mode == modeGenStruct || m_mode == modeGenFlagset))
					return;

				if (m_enableComments)
					stream() << "\n// " << m_locale->extractDoxygenCommentOneLine(*(flagset.comment()) ) << "\n\n";
			}


			virtual void visitFlagsetPost (Flagset& )
			{
				size_t maxNameWidth;
				size_t digits;

				if (!(m_mode == modeGenStruct || m_mode == modeGenFlagset))
					return;

				helpers::determineFlagsMaxWidth(m_savedFields, maxNameWidth, digits);

				char formatString[32], buf[32];
				sprintf(formatString, "0x%%.%dX", (int)digits );

				for (Flags::iterator it = m_savedFields.begin(); it != m_savedFields.end(); ++it)
				{
					const ConstantField& field = **it;

					stream() << "#define " << field.name() << std::string(maxNameWidth+3-field.name().size(), ' ');

					sprintf( buf, formatString, field.value() );
					stream() << buf << " ";
					if (field.getComment() && m_enableComments)
						stream() << "\t// " << m_locale->extractDoxygenCommentOneLine(*(field.getComment()) );
					stream() << "\n";
				}

				stream() << "\n";
			}

			virtual void visitDataField (DataField& field)
			{
				if (!m_current)
					return;

				if (m_current->calb && field.calibrationType() == CalibrationEnum::calb)
					return;

				switch (m_mode)
				{
					case modeGenStruct:
						stream() << "\t" << emitDeclaration( field ) << ";";
						finishStructField( field );
						break;
				}
			}

			virtual void visitConstantField (ConstantField& field)
			{
				switch (m_mode)
				{
					case modeGenStruct:
						if (!m_current)
							return;
						stream() << "\t" << emitDeclaration( field ) << ";";
						finishStructField( field );
						break;

					case modeGenFlagset:
						m_savedFields.push_back( &field );
						break;
				}
			}

			virtual void visitArrayField (ArrayField& field)
			{
				if (!m_current)
					return;

				if (m_current->calb && field.calibrationType() == CalibrationEnum::calb)
					return;

				if (!(m_mode == modeGenStruct || m_mode == modeGenFlagset))
					return;

				if (m_mode == modeGenStruct)
				{
					stream() << "\t" << emitDeclaration( field ) << "["
						<< field.dimExpression(0) << "];";
					finishStructField( field );
				}
			}

			virtual void visitFlagField (FlagField& field)
			{
				if (!m_current)
					return;

				if (!(m_mode == modeGenStruct || m_mode == modeGenFlagset))
					return;

				stream() << "\t" << emitDeclaration( field ) << ";";
				finishStructField(field);
				// print flags values after a field
				if (m_enableComments)
				{
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
								stream() << "\t// " << buf << " - " << (*itFlag)->name() << "\n";
							}
						}
					}
				}
			}

			virtual void visitReservedField (ReservedField& field)
			{
				if (!m_current)
					return;

				switch (m_mode)
				{
					case modeGenStruct:
						if (!m_current)
							return;

						stream() << "\tuint8_t\treserved";
						if (m_reservedIndex)
							stream() << m_reservedIndex;
						stream() << "[" << field.getSize() << "];\n";
						++m_reservedIndex;
						break;
				}

			}

			void finishStructField (Field& field)
			{
					if (field.getComment() && m_enableComments)
						stream() << "\t// " << m_locale->extractDoxygenCommentOneLine( *(field.getComment()) );
					stream() << "\n";
			}

			void enableComments (bool enable)
			{
				m_enableComments = enable;
			}

		private:

			typedef enum { modeGenCommandCode, modeGenStruct, modeGenFlagset, modeNULL } Mode;
			Mode m_mode;
			bool m_enableComments;

			std::ostringstream m_osDecls;
			std::ostringstream m_osStructs;

			Command* m_current;

			Flags m_savedFields;
			int m_reservedIndex;

			std::ostream& stream()
			{
				return m_mode == modeGenCommandCode
					? m_osDecls
					: m_osStructs;
			}

			void pushState(Command& command)
			{
				m_current = &command;
				if (m_mode == modeGenStruct)
					m_reservedIndex = 0;
			}

			template <class F>
			std::string emitDeclaration (const F& field)
			{
					return mapToNativeFwType( field.type() ) + "\t" + toString( field.name() );
			}

			void doGenerate (Protocol* protocol, std::ostream* os)
			{
				m_mode = (Mode)0;
				m_current = NULL;

				protocol->accept( *this );

				std::string trimmed_protocol_version = trim_copy(m_protocol->version, " \t\r\nv");
				*os
					<< "\n#define PROTOCOL_VERSION " << trimmed_protocol_version
					<< "\n#define PROTOCOL_VERSION_Q \"" << trimmed_protocol_version << "\""
					<< "\n\n";

				echoBanner( "BEGIN OF GENERATED struct declarations", os );

				*os << "\n// Command codes\n\n";
				*os << m_osDecls.str();

				*os << std::endl;

				*os << m_osStructs.str();

				echoBanner( "END OF GENERATED CODE", os );
			}

			void echoBanner (std::string bannerText, std::ostream* os)
			{
				if (m_enableComments)
					*os << "\n/*\n"
						<< " " 		<< std::string( bannerText.size()+4, '-' ) << "\n"
						<< "   "	<< bannerText << "\n"
						<< " "		<< std::string( bannerText.size()+4, '-' ) << "\n"
						<< "*/\n";
			}

		public:

			explicit FwHeaderGenerator ()
				: m_enableComments(true), m_current(NULL)
			{
			}

	};

}

#endif

/* vim: set ts=2 sw=2: */
