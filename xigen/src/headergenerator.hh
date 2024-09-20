#ifndef HEADERGENERATOR_HH
#define HEADERGENERATOR_HH

#include "common.hh"
#include "visitor.hh"
#include "modegenerator.hh"

namespace xigen
{

	class HeaderGenerator : public Noncopyable, public ModeGenerator
	{
		protected:

			virtual void startStructs()
			{
				stream() << std::endl;
				Comment::DoxyType doxyType = m_mode == modeGenStructCalb ? Comment::doxyStructCalb : Comment::doxyStruct;

				/* Add references to getter/setter functions to the struct doxycomment */
				std::string names = " @see ";
				names += m_current->functionName();
				if (m_current->paired && m_current->pairedCommand->functionName() != m_current->functionName())
					names += ", " + m_current->pairedCommand->functionName();

				if (m_enableComments && m_current->doxyComments.find( doxyType ) != m_current->doxyComments.end())
					stream() << helpers::addEdgeDoxygenString(m_current->doxyComments[doxyType], "", names) << "\n";

				stream() << "\ttypedef struct\n\t{\n";
			}

			virtual void startFuncs()
			{
				emitFunctionHead( *m_current );
			}

			virtual void visitCommandPost (Command& command)
			{
				if (!m_current)
					return;

				switch (m_mode)
				{
					case modeGenReader:
					case modeGenWriter:
						stream() << ");\n" << std::endl;
						break;
					case modeGenReaderCalb:
					case modeGenWriterCalb:
						stream() << ", const calibration_t* calibration);\n" << std::endl;
						break;
					case modeGenStruct:
						stream() << "\t} " << command.structName() << "_t;\n";
						break;
					case modeGenStructCalb:
						stream() << "\t} " << command.structCalbName() << "_t;\n";
						break;
				}
				m_current = NULL;
			}

			/* Collect all flags that referenced specified flagset */
			class CollectFlagFieldRefs : public DefaultVisitor
			{
				std::string m_flagset;
				Command* m_current;
				std::string m_refs;
			public:
				CollectFlagFieldRefs (std::string flagset) : m_flagset(flagset), m_current(NULL) { }
				virtual bool visitCommand (Command& command, size_t cookie) { m_current = &command; return !cookie; }
				virtual void visitFlagField (FlagField& field)
				{
					if (field.flagset() == m_flagset && m_current->master)
					{
						if (!m_refs.empty())
							m_refs += ", ";
						m_refs += m_current->structName() + "_t::" + field.name();
						m_refs += ", " + m_current->functionName();
						if (m_current->paired && m_current->functionName() != m_current->pairedCommand->functionName())
							m_refs += ", " + m_current->pairedCommand->functionName();
					}
				}
				std::string refs() { return m_refs; }
			};

			virtual void visitFlagset (Flagset& flagset)
			{
				ModeGenerator::visitFlagset( flagset );
				if (shouldGeneratePublicHeader() && m_enableComments)
				{
					stream() << "\n";
					std::string referencesFields;
					CollectFlagFieldRefs visitor(flagset.name());
					m_protocol->accept(visitor);

					if (!flagset.comment()->getComment().empty())
						stream() << helpers::addEdgeDoxygenString(*(flagset.comment()),
							" @anchor flagset_" + tolower(flagset.name()), visitor.refs().empty() ? "" : " @see " + visitor.refs());
					stream() << "\n//@{\n";
				}
			}

			virtual void visitFlagsetPost (Flagset& )
			{
				if (shouldGeneratePublicHeader())
				{
					printFormattedFlagset( "0x%%.%dX" );

					if (m_enableComments)
						stream() << "//@}\n";
					stream() << "\n";
				}
			}

			virtual void printFormattedFlag (std::string name, std::string value,
					const ConstantField& field)
			{
				stream() << "#define " << name << value << " ";

				if (field.getComment() && m_enableComments)
					stream() << "\t" << field.getComment()->getComment();
				stream() << "\n";
			}


			template <class F>
			void do_printFunctionArgument (F& field)
			{
				if (helpers::fieldPurpose( field ) == DataField::Purpose::answer)
					stream() << ", service_result_t*" << toString( field.name() );
				else
					stream() << ", " << do_emitDeclaration( field, m_mode == modeGenReader || m_mode == modeGenReaderCalb);
			}

			void do_printFunctionArgument (ArrayField& field)
			{
				XI_ASSERT(helpers::fieldPurpose( field ) == DataField::Purpose::normal);
				stream() << ", ";
				if (!(m_mode == modeGenReader || m_mode == modeGenReaderCalb))
					stream() << "const ";
				stream() << mapToNativeType( field.type(), true ) << "* " << toString( field.name() );
			}

			template <class F>
			std::string do_emitDeclaration (const F& field, bool usePointer = false)
			{
					return mapToNativeType( field.type(), false ) + (usePointer?"* ":" ") + toString( field.name() );
			}

			std::string do_emitDeclaration (const ArrayField& field)
			{
				return mapToNativeType( field.type(), false ) + " " + toString( field.name() ) + "["
					+ field.dimExpression( field.type() == VariableEnum::Char ? 1 : 0 ) + "]";
			}

			void printFunctionArgument (DataField& field)	{ do_printFunctionArgument( field ); }
			void printFunctionArgument (FlagField& field)	{ do_printFunctionArgument( field ); }
			void printFunctionArgument (ArrayField& field)	{ do_printFunctionArgument( field ); }

			std::string emitDeclaration (const DataField& field)			{ return do_emitDeclaration( field ); }
			std::string emitDeclaration (const FlagField& field)			{ return do_emitDeclaration( field ); }
			std::string emitDeclaration (const ConstantField& field)	{ return do_emitDeclaration( field ); }
			std::string emitDeclaration (const ArrayField& field)			{ return do_emitDeclaration( field ); }

			void finishStructField (Field& field)
			{
				if (field.getComment() && m_enableComments)
					stream() << "\t" << field.getComment()->getComment();
				ModeGenerator::finishStructField( field );
			}

			virtual void finishStructFlagField (FlagField& field)
			{
				if (field.getComment() && m_enableComments && !field.flagset().empty())
				{
					for (Flagsets::iterator it = m_protocol->flagsets.begin(); it != m_protocol->flagsets.end(); ++it)
					{
						if (field.flagset() == (*it)->name())
						{
							stream() << "\t/**<"
								<< " \\english \\ref flagset_" << tolower((*it)->name())
								<< " \"" << helpers::extractLanguageDoxygenName(*((*it)->comment()), "english")
								<< "\". \\endenglish"
								<< " \\russian \\ref flagset_" << tolower((*it)->name())
								<< " \"" << helpers::extractLanguageDoxygenName(*((*it)->comment()), "russian")
								<< "\". \\endrussian"
								<< " */";
						}
					}
				}
				ModeGenerator::finishStructFlagField( field );
			}

		private:

			std::ostringstream m_osStructs;
			std::ostringstream m_osMetalen;
			std::ostringstream m_osFunctions;

			std::ostream& stream()
			{
				switch (m_mode)
				{
					case modeGenStruct:
					case modeGenStructCalb:
					case modeGenFlagset:
						return m_osStructs;
					case modeGenMetalen:
						return m_osMetalen;
					default:
						return m_osFunctions;
				}
			}

			void printOptionalDoxyComment (Command& command, Comment::DoxyType type)
			{
				if (m_enableComments && command.doxyComments.find( type ) != command.doxyComments.end())
					stream() << command.doxyComments[type].getComment() << "\n";
			}

			void emitFunctionHead (Command& command)
			{
				if (m_mode == modeGenReader || m_mode == modeGenWriter)
				{
					if (m_mode != modeGenReader)
					{
						if (m_enableComments && command.doxyComments.find( Comment::doxySection ) != command.doxyComments.end())
							stream() << command.doxyComments[Comment::doxySection].getComment() << "\n";
					}
					printOptionalDoxyComment( command,
						m_mode == modeGenReader ? Comment::doxyRead : Comment::doxyWrite );
				}

				if (m_mode == modeGenReaderCalb || m_mode == modeGenWriterCalb)
				{
					printOptionalDoxyComment( command,
						m_mode == modeGenReaderCalb ? Comment::doxyReadCalb : Comment::doxyWriteCalb );
				}

				stream() << "\t" << helpers::emitFunctionHead( command, m_mode == modeGenReader || m_mode == modeGenReaderCalb,
						command.is("public"),
						command.calb && (m_mode == modeGenReaderCalb || m_mode == modeGenWriterCalb),
						false );
			}

			virtual bool acceptMode(Mode mode)
			{
				return true;
			}

			virtual void output (std::ostream* os)
			{
				if (shouldGeneratePublicHeader())
				{
					if (!m_ximcVersion.empty())
						*os
							<< "\n/** @cond DO_NOT_WANT */"
							<< "\n#define LIBXIMC_VERSION " << m_ximcVersion
							<< "\n/** @endcond */\n\n";

					std::string trimmed_protocol_version = trim_copy(m_protocol->version, " \t\r\nv");
					*os
						<< "\n/** @cond DO_NOT_WANT */"
						<< "\n#define LIBXIMC_PROTOCOL_VERSION " << trimmed_protocol_version
						<< "\n/** @endcond */\n\n";
				}

				if (!m_osStructs.str().empty())
				{
					echoBanner( "BEGIN OF GENERATED struct declarations", os );
					*os << m_osStructs.str();
				}

				if (!m_osMetalen.str().empty() && !shouldGeneratePublicHeader())
				{
					echoBanner( "BEGIN OF GENERATED metadata declarations", os );
					*os << "\n" << m_osMetalen.str() << "\n\n";
				}

				echoBanner( "BEGIN OF GENERATED function declarations", os );
				*os << m_osFunctions.str();

				echoBanner( "END OF GENERATED CODE", os );
			}

			void echoBanner (std::string bannerText, std::ostream* os)
			{
				if (m_enableComments)
					*os << "\n/*\n"
						<< " "		<< std::string( bannerText.size()+4, '-' ) << "\n"
						<< "   "	<< bannerText << "\n"
						<< " "		<< std::string( bannerText.size()+4, '-' ) << "\n"
						<< "*/\n";
			}

		public:

			explicit HeaderGenerator (bool ashouldGeneratePublicHeader)
				:	ModeGenerator(ashouldGeneratePublicHeader)
			{
			}

	};

}

#endif

/* vim: set ts=2 sw=2: */
