#ifndef MODEGENERATOR_HH
#define MODEGENERATOR_HH

#include "generatorhelper.hh"
#include "basegenerator.hh"

namespace xigen
{

	class ModeGenerator : public BaseGenerator, protected IVisitor
	{
		protected:

			ModeGenerator (bool isGeneratePublicHeader = true)
				: m_shouldGeneratePublicHeader(isGeneratePublicHeader),
					m_mode((Mode)0), m_current(NULL), m_enableComments(false),
					m_lineTerminator(";")
			{
			}

			void pushState(Command& command)
			{
				m_current = &command;
			}

			void undoCommand()
			{
				m_current = NULL;
			}

			bool shouldWriteHeader (Command* command) const
			{
				XI_ASSERT(command);
				switch (command->communicable())
				{
					case Communicable::both:
						return m_mode == modeGenWriter || (m_mode == modeGenWriterCalb && command->calb);
					case Communicable::reader:
						return m_mode == modeGenReader || (m_mode == modeGenReaderCalb && command->calb);
					case Communicable::writer:
						return m_mode == modeGenWriter || (m_mode == modeGenWriterCalb && command->calb);
					default: throw ast_error("Wrong communicator", command);
				}
			}

			void pushFormattedFlag(ConstantField& field)
			{
				m_savedFields.push_back( &field );
			}

			virtual void printFormattedFlagset(std::string format)
			{
				size_t maxNameWidth;
				size_t digits;
				helpers::determineFlagsMaxWidth(m_savedFields, maxNameWidth, digits);
				char formatString[32], buf[32];
				sprintf(formatString, format.c_str(), (int)digits );

				for (std::vector<ConstantField*>::iterator it = m_savedFields.begin(); it != m_savedFields.end(); ++it)
				{
					const ConstantField& field = **it;
					sprintf( buf, formatString, field.value() );
					printFormattedFlag(
							field.name() + std::string(maxNameWidth+3-field.name().size(), ' '),
							buf, field );
				}
			}

			virtual void visitFlagset (Flagset& flagset)
			{
				m_mode = modeGenFlagset;
				m_current = NULL;
				m_savedFields.clear();
			}

			bool shouldGeneratePublicHeader() const { return m_shouldGeneratePublicHeader; }

		private:

			// visitor hooks

			virtual void visitProtocol (Protocol& ) { }
			virtual void visitCommands () { }
			virtual void visitRequest () { }
			virtual void visitAnswer () { };
			virtual void visitReservedField (ReservedField& ) { }

			virtual bool visitCommand (Command& command, size_t cookie)
			{
				for (; cookie < (size_t)modeNULL; ++cookie)
				{
					if (acceptMode( (Mode)cookie ))
					{
						m_mode = (Mode)cookie;
						clear();
						visitCommandImplOneRun( command );
						return true;
					}
				}
				return false;
			}

			virtual void visitCommandImplOneRun (Command& command)
			{
				m_current = NULL;

				onVisitCommandFirstTime( command );

				switch (m_mode)
				{
					case modeGenMetalen:
						pushState( command );
						break;

					case modeGenStruct:
					case modeGenStructCalb:
						{
							if (m_shouldGeneratePublicHeader != command.is("publicstruct"))
								return;

							if (command.communicatorWriter && command.paired)
								return;
							if (command.unsynced)
								return;

							if (m_mode == modeGenStructCalb && !command.calb)
								return;

							if (command.withAnyFields() && !command.is("inline"))
							{
								pushState( command );
								startStructs();
							}
							break;
						}

					case modeGenWriter:
					case modeGenReader:
					case modeGenWriterCalb:
					case modeGenReaderCalb:
					{
						if (m_shouldGeneratePublicHeader != command.is("public"))
							return;

						if (shouldWriteHeader( &command ))
						{
							pushState( command );
							startFuncs();
						}
						break;
					}
				}
			}

			virtual void visitDataField (DataField& field)
			{
				if (!m_current)
					return;

				switch (m_mode)
				{
					case modeGenReader:
					case modeGenWriter:
						if (field.purpose() != DataField::Purpose::result && vinline() && field.calibrationType() != CalibrationEnum::calb)
							printFunctionArgument( field );
						break;

					case modeGenReaderCalb:
					case modeGenWriterCalb:
						if (field.purpose() != DataField::Purpose::result && vinline() && field.calibrationType() != CalibrationEnum::normal)
							printFunctionArgument( field );
						break;

					case modeGenStruct:
						if (field.calibrationType() != CalibrationEnum::calb)
						{
							stream() << "\t\t" << emitDeclaration( field ) << m_lineTerminator;
							finishStructField( field );
						}
						break;

					case modeGenStructCalb:
						if (field.calibrationType() != CalibrationEnum::normal)
						{
							stream() << "\t\t" << emitDeclaration( field ) << m_lineTerminator;
							finishStructField( field );
						}
						break;
				}
			}

			virtual void visitConstantField (ConstantField& field)
			{
				switch (m_mode)
				{
					case modeGenStruct:
					case modeGenStructCalb:
						if (!m_current)
							return;
						stream() << "\t\t" << emitDeclaration( field ) << m_lineTerminator;
						finishStructField( field );
						break;

					case modeGenFlagset:
						pushFormattedFlag( field );
						break;
				}
			}

			virtual void visitArrayField (ArrayField& field)
			{
				if (!m_current)
					return;

				switch (m_mode)
				{
					case modeGenMetalen:
						if (field.isMetalen())
							stream() << "\t#define XIMC_METALEN_" << toupper(m_current->name())
								<< "_" << toupper(field.name()) << " " << field.getSize();
						break;

					case modeGenStruct:
							if (field.calibrationType() != CalibrationEnum::calb)
							{
								stream() << "\t\t" << emitDeclaration( field ) << m_lineTerminator;
								finishStructField( field );
							}
							break;

					case modeGenStructCalb:
							if (field.calibrationType() != CalibrationEnum::normal)
							{
								stream() << "\t\t" << emitDeclaration( field ) << m_lineTerminator;
								finishStructField( field );
							}
							break;

					case modeGenReader:
						if (vinline() && field.calibrationType() != CalibrationEnum::calb)
							printFunctionArgument( field );
						break;

					case modeGenWriter:
						if (vinline() && field.calibrationType() != CalibrationEnum::calb)
							printFunctionArgument( field );
						break;

					case modeGenReaderCalb:
						if (vinline() && field.calibrationType() != CalibrationEnum::normal)
							printFunctionArgument( field );
						break;

					case modeGenWriterCalb:
						if (vinline() && field.calibrationType() != CalibrationEnum::normal)
							printFunctionArgument( field );
						break;
				}
			}

			virtual void visitFlagField (FlagField& field)
			{
				if (!m_current)
					return;

				switch (m_mode)
				{
					case modeGenStruct:
					case modeGenStructCalb:
						stream() << "\t\t" << emitDeclaration( field ) << m_lineTerminator;
						finishStructFlagField( field );
						break;

					case modeGenReader:
					case modeGenWriter:
					case modeGenReaderCalb:
					case modeGenWriterCalb:
						if (vinline())
							printFunctionArgument( field );
						break;
				}
			}

			void doGenerate (Protocol* protocol, std::ostream* os)
			{
				m_mode = (Mode)0;
				m_current = NULL;

				protocol->accept( *this );

				output( os );
			}

			void enableComments (bool enable)
			{
				m_enableComments = enable;
			}


		// fields
		private:
			Flags m_savedFields;
			const bool m_shouldGeneratePublicHeader;

		protected:
			typedef enum { modeGenStruct, modeGenStructCalb,
				modeGenWriter, modeGenReader,
				modeGenWriterCalb, modeGenReaderCalb,
				modeGenFlagset, modeGenMetalen, modeNULL } Mode;
			Mode m_mode;

			Command* m_current;

			bool m_enableComments;
			std::string m_lineTerminator;

		protected:

			// header mode generator must override these hooks
			// additionally, visitFlagset is also useful

			virtual bool acceptMode(Mode mode) = 0;
			virtual void clear() { };
			virtual void output (std::ostream* os) = 0;
			virtual void startStructs() = 0;
			virtual void startFuncs() = 0;
			virtual std::ostream& stream() = 0;

			virtual void printFunctionArgument (DataField& field) = 0;
			virtual void printFunctionArgument (FlagField& field) = 0;
			virtual void printFunctionArgument (ArrayField& field) = 0;

			virtual std::string emitDeclaration (const DataField& field) = 0;
			virtual std::string emitDeclaration (const FlagField& field) = 0;
			virtual std::string emitDeclaration (const ConstantField& field)= 0;
			virtual std::string emitDeclaration (const ArrayField& field) = 0;

			virtual void finishStructField (Field& field) { stream() << "\n"; }
			virtual void finishStructFlagField (FlagField& field) { stream() << "\n"; }

			virtual void onVisitCommandFirstTime(Command& command) { }
			virtual bool vinline() const { return m_current && m_current->is("inline"); }

			virtual void printFormattedFlag (std::string name, std::string value, const ConstantField& field) { };
	};


}

#endif


/* vim: set ts=2 sw=2: */
