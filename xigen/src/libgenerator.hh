#ifndef LIBGENERATOR_HH
#define LIBGENERATOR_HH

#include "common.hh"
#include "visitor.hh"
#include "basegenerator.hh"
#include "generatorhelper.hh"

namespace xigen
{

	class LibGenerator : protected DefaultVisitor, public Noncopyable, public BaseGenerator
	{
		protected:
			virtual bool visitCommand (Command& command, size_t cookie)
			{
				if (cookie >= (size_t)modeNULL)
					return false;
				m_mode = (Mode)cookie;
				clear();
				/*if (!cookie) ; // clear first time */
				visitCommandImpl( command );
				return true;
			}

			// TODO think how to move it out
			bool shouldWriteHeader (Command* command) const
			{
				XI_ASSERT(command);
				switch (command->communicable())
				{
					case Communicable::both:   return m_mode == modeGenWriter || (m_mode == modeGenWriterCalb && command->calb);
					case Communicable::reader: return m_mode == modeGenReader || (m_mode == modeGenReaderCalb && command->calb);
					case Communicable::writer: return m_mode == modeGenWriter || (m_mode == modeGenWriterCalb && command->calb);
					default: throw ast_error("Wrong communicator", command);
				}
			}

			bool shouldWriteFooter (Command* command) const
			{
				XI_ASSERT(command);
				switch (command->communicable())
				{
					case Communicable::both:   return m_mode == modeGenReaderBody || (m_mode == modeGenReaderCalbBody && command->calb);
					case Communicable::reader: return m_mode == modeGenReaderBody || (m_mode == modeGenReaderCalbBody && command->calb);
					case Communicable::writer: return m_mode == modeGenWriterBody || (m_mode == modeGenWriterCalbBody && command->calb);
					default: throw ast_error("Wrong communicator", command);
				}
			}

			void recheckCalbCommand()
			{
				if (m_current && !m_current->calb &&
							(m_mode == modeGenWriterCalb || m_mode == modeGenReaderCalb || m_mode == modeGenWriterCalbBody || m_mode == modeGenReaderCalbBody))
					m_current = NULL;
			}

			void visitCommandImpl (Command& command)
			{
				m_current = NULL;
				m_location = locationNone;

				switch (command.communicable())
				{
					case Communicable::both:
					{
						switch (m_mode)
						{
							case modeGenWriter:
							case modeGenReader:
							case modeGenReaderBody:
							case modeGenWriterBody:
								pushState( command );
								recheckCalbCommand();
								if (shouldWriteHeader( &command ))
									emitFunctionHead( command );
						}
						break;
					}
					case Communicable::reader:
					{
						switch (m_mode)
						{
							case modeGenReader:
							case modeGenReaderBody:
							case modeGenReaderCalb:
							case modeGenReaderCalbBody:
								pushState( command );
								recheckCalbCommand();
								if (shouldWriteHeader( &command ))
									emitFunctionHead( command );
						}
						break;
					}
					case Communicable::writer:
					{
						switch (m_mode)
						{
							case modeGenWriter:
							case modeGenWriterBody:
							case modeGenWriterCalb:
							case modeGenWriterCalbBody:
								pushState( command );
								recheckCalbCommand();
								if (shouldWriteHeader( &command ))
									emitFunctionHead( command );
						}
						break;
					}
				}
			}

			Communicator* getCommunicator(Command& command)
			{
				Communicator* communicator;
				switch (m_mode)
				{
					case modeGenReader:
					case modeGenReaderBody:
					case modeGenReaderCalb:
					case modeGenReaderCalbBody:
						communicator = command.communicatorReader;
						break;

					case modeGenWriter:
					case modeGenWriterBody:
					case modeGenWriterCalb:
					case modeGenWriterCalbBody:
						communicator = command.communicatorWriter;
						break;
				}
				if (m_current->communicable() == Communicable::both)
				{
					if (!communicator)
						communicator = command.communicatorReader;
					if (!communicator)
						communicator = command.communicatorWriter;
				}

				if (!communicator)
					throw ast_error( "can't determine communicator", &command );
				if (!communicator->isString)
					throw ast_error( "hex communicator are not supported yet", &command );
				return communicator;
			}

			bool anyCalbMode() const
			{
				return
					m_mode == modeGenReaderCalb ||
					m_mode == modeGenWriterCalb ||
					m_mode == modeGenReaderCalbBody ||
					m_mode == modeGenWriterCalbBody;
			}

			virtual void visitCommandPost (Command& command)
			{
				if (!m_current)
					return;

				// XXX it was a hack about service commands - they are always unlocked. keep for a while
				bool criteria = command.is("lock") && command.communicable() != Communicable::both && !anyCalbMode();
				std::string optLock = criteria ? "\tlock( id );\n" : "";
				std::string optUnlockerOpen = criteria ? "unlocker( id, " : "";
				std::string optUnlockerClose = criteria ? " )" : "";

				// fully dataless command
				if (!command.withAnyFields())
				{
					if (m_mode == modeGenWriter)
					{
						if (command.is("answer"))
							stream() << ")\n{\n"
								<< optLock
								<< "\treturn " << optUnlockerOpen << "command_checked_echo_str" << command.ioSyncSuffix()
									<< "( id, \"" << getCommunicator( command )->name << "\" )" << optUnlockerClose << ";\n"
								<< "}\n\n\n";
						else
							stream() << ")\n{\n"
								<< optLock
								<< "\treturn " << optUnlockerOpen << "command_checked_str" << command.ioSyncSuffix()
									<< "( id, \"" << getCommunicator( command )->name << "\", NULL, 0 )" << optUnlockerClose << ";\n"
									<< "}\n\n\n";
					}
					return;
				}

				if (shouldWriteHeader( &command ))
				{
					if (m_mode == modeGenReaderCalb || m_mode == modeGenWriterCalb)
						stream() << ", const calibration_t* calibration";

					stream()
						<< ")\n"
						<< "{\n";

					if (m_mode != modeGenWriterCalb)
						stream() << "\tresult_t result;\n";

					if (m_withSresultResult)
						stream() << "\tservice_result_t sresult;\n";

					if ((m_mode == modeGenReaderCalb || m_mode == modeGenWriterCalb) && !m_current->is("inline"))
					{
						stream() << "\t" << command.structName() << "_t inner;\n";
					}

					if (!anyCalbMode())
					{
						bool writeCommand = command.communicable() == Communicable::both || command.communicable() == Communicable::writer;
						bool writeResponse = command.withAnswer();
						//if (command.communicable() == Communicable::both || command.communicable() == Communicable::reader)

						if (writeCommand)
							stream() << "\tbyte command[" << (command.communicatorWriter ? command.communicatorWriter->size : 4) << "]"
								<< (writeResponse ? "" : ", *p  = command" ) << ";\n";
						if (writeResponse)
							stream() << "\tbyte response[" << command.communicatorReader->size << "]"
								<< (writeCommand ? "" : ", *p  = response" ) << ";\n";
						if (writeCommand && writeResponse)
							stream() << "\tbyte* p = command;\n";
					}
					else if (command.is("inline"))
					{
						// local proxy variables
						const FieldNests nests = m_current->allNests();
						for (FieldNests::const_iterator itNest = nests.begin(); itNest != nests.end(); ++itNest)
						{
							const FieldNest* nest = *itNest;
							stream() << "\t" << mapToNativeType( nest->fieldNormal->type(), false ) << " "
								<< nest->fieldNormal->declaration() << ";\n";
							if (nest->fieldMicro)
							{
								stream() << "\t" << mapToNativeType( nest->fieldMicro->type(), false ) << " "
									<< nest->fieldMicro->declaration() << ";\n";
							}
						}
					}
					if (m_withIndexVariable)
						stream() << "\tunsigned int i;\n";

					if (!optLock.empty())
						stream() << "\n" << optLock;
				}

				std::string optCheckInOverrun;

				std::string resultExpression;

				switch (m_mode)
				{
					case modeGenWriter:
						stream()
							<< "\n"
							<< "\tpush_str( &p, \""
							<< getCommunicator( command )->name
							<< "\" );\n";
						break;

					case modeGenReader:
						if (command.withAnswer())
						{
							if (command.communicable() == Communicable::both)
							{
								// continue to read answer
								stream()
									<< "\n"
									<< "\tp = response;\n"
									<< "\tp += 4;\n";
							}
							else
							{
								// only read command
								stream()
									<< "\n"
									<< "\tif ((result = command_checked_str" << command.ioSyncSuffix() << "( id, \"" << getCommunicator( command )->name
									<< "\", response, sizeof(response) )) != result_ok)\n"
									<< "\t	return " << optUnlockerOpen << "result" << optUnlockerClose << ";\n"
									<< "\tp += 4;\n\n";
							}
						}
						break;

					case modeGenWriterBody:
						{
							if (command.withFields())
								stream()
									<< "\tpush_crc( &p, command, p-command );\n";
							stream()
								<< "\n\tif ((result = " << (m_withDynamicArray ? "check_out_atleast_overrun" : "check_out_overrun") << "( p-command, sizeof(command) )) != result_ok)\n"
								<< "\t\treturn " << optUnlockerOpen << "result" << optUnlockerClose << ";\n";

							if (!command.withFields() && command.withAnswer())
								resultExpression = "command_checked_str" + command.ioSyncSuffix() + "( id, \"" + getCommunicator( command )->name +
									"\", response, sizeof(response) )";
							else if (command.withFields() && !command.withAnswer())
								resultExpression = "command_checked_echo" + command.ioSyncSuffix() + "( id, command, "
									+ (m_withDynamicArray ? "p - command" : "sizeof(command)") + std::string(")");
							else
								resultExpression = "command_checked" + command.ioSyncSuffix() + "( id, command, "
									+ (m_withDynamicArray ? "p - command" : "sizeof(command)") + std::string(", response, sizeof(response) )");

							if (!shouldWriteFooter( &command ))
							{
								stream() << "\n\tif ((result = " << resultExpression << ") != result_ok)\n"
									<< "\t\treturn " << optUnlockerOpen << "result" << optUnlockerClose << ";\n";
								resultExpression.clear();
							}

							break;
						}

					case modeGenWriterCalbBody:
					{
						resultExpression = command.functionName() + "(id, " +
							(command.is("inline") ? join(m_inlineCalbProxyArgs, ", ") : "&inner")
							+ ")";
						break;
					}

					case modeGenReaderBody:
					{
						if (command.withAnswer())
						{
							// commands without answer do not need output buffer check at all
							optCheckInOverrun = command.is("crc") ? "" : "_without_crc";
							resultExpression = "check_in_overrun" + optCheckInOverrun + "( id, p-response, sizeof(response), response )";
							if (!shouldWriteFooter( &command ) || m_withSresultResult)
							{
								stream()
									<< "\n"
									<< "\tif ((result = " << resultExpression << ") != result_ok)\n"
									<< "\t\treturn result_error;\n"
									<< "\t\n";
								resultExpression.clear();
							}
						}

						if (m_withSresultResult)
							stream()
								<< "\tswitch (sresult)\n"
								<< "\t{\n"
								<< "\t\tcase RESULT_OK: \n"
								<< "\t\t\tresult = result_ok;\n"
								<< "\t\t\tbreak;\n"
								<< "\t\tdefault: \n"
								<< "\t\t\tdump_service_result( L\"" << command.name() << "\", sresult );\n"
								<< "\t\t\tresult = result_error;\n"
								<< "\t\t\tbreak;\n"
								<< "\t}\n";
						break;
					}

					case modeGenReaderCalb:
					{
						stream()
							<< "\n"
							<< "\tif ((result = " << command.functionName() << "(id, "
							<< (command.is("inline") ? join(m_inlineCalbProxyArgs, ", ") : "&inner")
							<< ")) != result_ok)\n"
							<< "\t	return " << optUnlockerOpen << "result" << optUnlockerClose << ";\n"
							<< "\n";
						break;
					}
				}

				if (shouldWriteFooter( &command ))
				{
					if (resultExpression.empty())
						resultExpression = "result";
					stream() << "\n\treturn " << optUnlockerOpen << resultExpression << optUnlockerClose << ";\n}\n\n";
				}

				m_current = NULL;
			}

			virtual void visitRequest ()
			{
				m_location = locationRequest;
			}

			virtual void visitAnswer ()
			{
				m_location = locationAnswer;
			}

			template <class F>
			std::string emitFieldPointer (const F& field, bool forceNoPointer)
			{
				if (m_current->is("inline"))
					return (forceNoPointer ? "" : "*") + field.name();
				else
					return m_current->structParameterName() + "->" + field.name();
			}

			template <class F>
			void emitGenericDataField (F& field)
			{
				if (!m_current)
					return;

				if (helpers::fieldPurpose( field ) == DataField::Purpose::answer)
					m_withSresultAnswer = true;
				else if (helpers::fieldPurpose( field ) == DataField::Purpose::result)
					m_withSresultResult = true;

				switch (m_mode)
				{
					case modeGenReader:
					case modeGenWriter:
						{
							if (m_current->is("inline") && shouldWriteHeader( m_current ))
							{
								// special handling for reader with answer
								if (helpers::fieldPurpose( field ) == DataField::Purpose::answer)
								{
									stream() << ", service_result_t* " << toString( field.name() );
								}
								else if (m_mode == modeGenReader &&
										m_current->communicatorReader && helpers::fieldPurpose( field ) == DataField::Purpose::normal &&
										helpers::reducedCalibrationType( field ) != CalibrationEnum::calb)
								{
									stream() << ", " << mapToNativeType( field.type(), false ) << "* "  << toString( field.name() );
								}
								else if (m_mode == modeGenWriter &&
										m_current->communicatorWriter && helpers::fieldPurpose( field ) == DataField::Purpose::normal &&
										helpers::reducedCalibrationType( field ) != CalibrationEnum::calb)
								{
									stream() << ", " << mapToNativeType( field.type(), false ) << " " << toString( field.name() );
								}
							}
							break;
						}

					case modeGenReaderCalb:
					case modeGenWriterCalb:
						{
							if (m_current->is("inline") && shouldWriteHeader( m_current ))
							{
								XI_ASSERT(helpers::fieldPurpose( field ) == DataField::Purpose::normal);
								if (m_mode == modeGenReaderCalb && m_current->communicatorReader &&
										helpers::reducedCalibrationType( field ) != CalibrationEnum::normal)
								{
									stream() << ", " << mapToNativeType( field.type(), false ) << "* " << helpers::prefixedCalbName( field );
								}
								else if (m_mode == modeGenWriterCalb && m_current->communicatorWriter &&
										helpers::reducedCalibrationType( field ) != CalibrationEnum::normal)
								{
									stream() << ", " << mapToNativeType( field.type(), false ) << " " << helpers::prefixedCalbName( field );
								}
							}
							break;
						}

					case modeGenReaderBody:
						if (m_location == locationAnswer &&
								(!m_current->calb || helpers::reducedCalibrationType( field ) != CalibrationEnum::calb))
                        {
							if (!(m_withSresultResult || m_withSresultAnswer))
							stream() << "\t" << emitFieldPointer( field, helpers::fieldPurpose( field ) == DataField::Purpose::result )
								<< " = pop_" << mapToProtoSerializer( field.type() ) << "( &p );\n";
							else
							stream() << "\t" << emitFieldPointer( field, helpers::fieldPurpose( field ) == DataField::Purpose::result )
								<< " = (service_result_t)"
								<< "pop_" << mapToProtoSerializer( field.type() ) << "( &p );\n";
                        }
						break;

					case modeGenWriterBody:
						if (m_location == locationRequest &&
								(!m_current->calb || helpers::reducedCalibrationType( field ) != CalibrationEnum::calb))
							stream()
								<< "\tpush_" << mapToProtoSerializer( field.type() ) << "( &p, "
								<< emitFieldPointer( field, m_current->is("inline") ) << " );\n";
						break;

					case modeGenReaderCalbBody:
						if (m_location == locationAnswer)
						{
							if (m_current->is("inline"))
							{
								if (helpers::reducedCalibrationType(field) == CalibrationEnum::calb)
								{
									const FieldNest* nest = m_current->findNestByField( &field );
									stream() << "\tXI_normal_to_calibrate" << ((nest->fieldCalb->type() == VariableEnum::CFloat) ? "_corr" : "")
											 << (nest->fieldMicro ? "" : "_short") << "("
											 << helpers::prefixedCalbName( *(nest->fieldCalb) ) << ", "
											 << nest->fieldNormal->name() << ", ";
									if (nest->fieldMicro)
											stream() << nest->fieldMicro->name() << ", ";
									stream() << "calibration" << ((nest->fieldCalb->type() == VariableEnum::CFloat) ? " , id" : "") << " );\n";
								}
							}
							else
							{
								switch (helpers::reducedCalibrationType(field))
								{
									case CalibrationEnum::calb:
									{
										const FieldNest* nest = m_current->findNestByField( &field );
										stream() << "\tXI_normal_to_calibrate" << ((nest->fieldCalb->type() == VariableEnum::CFloat) ? "_corr" : "")
												 << (nest->fieldMicro ? "" : "_short") << "("
												 << m_current->structParameterCalbName() << "->" << nest->fieldCalb->name() << ", "
												 << "inner." << nest->fieldNormal->name() << ", ";
										if (nest->fieldMicro)
												 stream() << "inner." << nest->fieldMicro->name() << ", ";
										stream() << "calibration" << ((nest->fieldCalb->type() == VariableEnum::CFloat) ? " , id" : "") << " );\n";
										break;
									}
									case CalibrationEnum::none:
										stream() << "\t"
											<< m_current->structParameterCalbName() << "->" << field.name()
											<< " = inner." << field.name() << ";\n";
										break;
								}
							}
						}
						break;

					case modeGenWriterCalbBody:
						if (m_location == locationRequest)
						{
							if (m_current->is("inline"))
							{
								if (helpers::reducedCalibrationType(field) == CalibrationEnum::calb)
								{
										const FieldNest* nest = m_current->findNestByField( &field );
										stream() << "\tXI_calibrate_to_normal" << ((nest->fieldCalb->type() == VariableEnum::CFloat) ? "_corr" : "")
												 << ((nest->fieldCalb->type() == VariableEnum::CDFloat) ? "_Dcorr" : "")
												 << (nest->fieldMicro ? "" : "_short") << "("
											 	 << helpers::prefixedCalbName( *(nest->fieldCalb) ) << ", "
												 << nest->fieldNormal->name() << ", ";
										if (nest->fieldMicro)
												 stream() << nest->fieldMicro->name() << ", ";
										stream() << "calibration" << ((nest->fieldCalb->type() == VariableEnum::CFloat) ? " , id " : "") 
												 << ((nest->fieldCalb->type() == VariableEnum::CDFloat) ? " , id " : "")
												 << " );\n";
								}
							}
							else
							{
								switch (helpers::reducedCalibrationType(field))
								{
									case CalibrationEnum::calb:
									{
										const FieldNest* nest = m_current->findNestByField( &field );
										stream() << "\tXI_calibrate_to_normal" << ((nest->fieldCalb->type() == VariableEnum::CFloat) ? "_corr" : "")
											<< ((nest->fieldCalb->type() == VariableEnum::CDFloat) ? "_Dcorr" : "")
											<< (nest->fieldMicro ? "" : "_short") << "("
											 << m_current->structParameterCalbName() << "->" << nest->fieldCalb->name() << ", "
											 << "inner." << nest->fieldNormal->name() << ", ";
										if (nest->fieldMicro)
											stream() << "inner." << nest->fieldMicro->name() << ", ";
										stream() << "calibration" << ((nest->fieldCalb->type() == VariableEnum::CFloat) ? " , id" : "") 
											<< ((nest->fieldCalb->type() == VariableEnum::CDFloat) ? " , id " : "")
											<< " );\n";
										break;
									}
									case CalibrationEnum::none:
										stream() << "\t"
											<< "inner." << field.name()
											<< " = " << m_current->structParameterCalbName() << "->" << field.name() << ";\n";
										break;
								}
							}
						}
						break;
				}
			}

			virtual void visitDataField (DataField& field)
			{
				if (anyCalbMode() && m_current)
				{
					switch (field.calibrationType())
					{
						case CalibrationEnum::normal:
							m_inlineCalbProxyArgs.push_back( (m_current->communicable() == Communicable::reader ? "&" : "") + field.name() );
							break;

						case CalibrationEnum::none:
							m_inlineCalbProxyArgs.push_back( field.name() );
							break;
					}
				}

				emitGenericDataField( field );
			}

			virtual void visitConstantField (ConstantField& )
			{
				if (!m_current)
					return;
				throw ast_error( "Unsupported constant field", m_current );
			}

			virtual void visitArrayField (ArrayField& field)
			{
				if (!m_current)
					return;

				m_withIndexVariable = m_withIndexVariable || field.type() != VariableEnum::Char;
				m_withDynamicArray = m_withDynamicArray || field.isDynamic();

				m_inlineCalbProxyArgs.push_back( field.name() );

				// old weird comment: write function header only for writer if both available
				switch (m_mode)
				{
					case modeGenReader:
						if (m_location == locationAnswer && m_current->is("inline") &&
								field.calibrationType() != CalibrationEnum::calb)
							stream() << ", " << mapToNativeType( field.type(), true ) << "* " << helpers::prefixedCalbName( field );
						break;

					case modeGenReaderCalb:
						if (m_location == locationAnswer && m_current->is("inline") &&
								field.calibrationType() != CalibrationEnum::normal)
							stream() << ", " << mapToNativeType( field.type(), true ) << "* " << helpers::prefixedCalbName( field );
						break;

					case modeGenWriter:
						if (m_location == locationRequest && m_current->is("inline") &&
								field.calibrationType() != CalibrationEnum::calb)
							stream() << ", const " << mapToNativeType( field.type(), true ) << "* " << helpers::prefixedCalbName( field );
						break;

					case modeGenWriterCalb:
						if (m_location == locationRequest && m_current->is("inline") &&
								field.calibrationType() != CalibrationEnum::normal)
							stream() << ", const " << mapToNativeType( field.type(), true ) << "* " << helpers::prefixedCalbName( field );
						break;

					case modeGenReaderBody:
						if (m_location == locationAnswer &&
								(!m_current->calb || helpers::reducedCalibrationType( field ) != CalibrationEnum::calb))
						{
							if (field.type() == VariableEnum::Char)
								stream()
										<< "\tpop_str( &p, "
										<< emitFieldPointer( field, false ) << ", " << field.dimExpression() << " );\n";
							else
								stream()
									<< "\tfor (i = 0; i < " << field.dimExpression() << "; ++i)\n"
									<< "\t\t" << emitFieldPointer( field, false ) << "[i]"
										<< " = pop_" << mapToProtoSerializer( field.type() ) << "( &p );\n";
						}
						break;

					case modeGenWriterBody:
						if (m_location == locationRequest &&
								(!m_current->calb || helpers::reducedCalibrationType( field ) != CalibrationEnum::calb))
						{
							if (field.type() == VariableEnum::Char)
								stream()
										<< "\tpush_data( &p, " << emitFieldPointer( field, true )
										<< ", " << field.dimExpression() << " );\n";
							else
								stream()
									<< "\tfor (i = 0; i < " << field.dimExpression() << "; ++i)\n"
									<< "\t\tpush_" << mapToProtoSerializer( field.type() ) << "( &p, "
									<< emitFieldPointer( field, true ) <<  "[i] );\n";
						}
						break;

					case modeGenReaderCalbBody:
						if (m_location == locationAnswer)
						{
							if (m_current->is("inline"))
							{
								if (helpers::reducedCalibrationType(field) == CalibrationEnum::calb)
								{
									const FieldNest* nest = m_current->findNestByField( &field );
									stream() << "\tfor (i = 0; i < " << field.dimExpression() << "; ++i)\n\t{\n";
									stream() << "\t\tXI_normal_to_calibrate" << ((nest->fieldCalb->type() == VariableEnum::CFloat) ? "_corr" : "") 
											 << (nest->fieldMicro ? "" : "_short") << "("
											 << helpers::prefixedCalbName( *(nest->fieldCalb) ) << "[i], "
											 << nest->fieldNormal->name() << "[i], ";
									if (nest->fieldMicro)
											stream() << nest->fieldMicro->name() << "[i], ";
									stream() << "calibration" << ((nest->fieldCalb->type() == VariableEnum::CFloat) ? " , id" : "") << " );\n";
									stream() << "\t};\n";
								}
							}
							else
							{
								switch (helpers::reducedCalibrationType(field))
								{
									case CalibrationEnum::calb:
									{
										const FieldNest* nest = m_current->findNestByField( &field );
										stream() << "\tfor (i = 0; i < " << field.dimExpression() << "; ++i)\n\t{\n";
										stream() << "\t\tXI_normal_to_calibrate" << ((nest->fieldCalb->type() == VariableEnum::CFloat) ? "_corr" : "")
												 << (nest->fieldMicro ? "" : "_short") << "("
												 << m_current->structParameterCalbName() << "->" << nest->fieldCalb->name() << "[i], "
												 << "inner." << nest->fieldNormal->name() << "[i], ";
										if (nest->fieldMicro)
												 stream() << "inner." << nest->fieldMicro->name() << "[i], ";
										stream() << "calibration" << ((nest->fieldCalb->type() == VariableEnum::CFloat) ? " , id" : "") << " );\n";
										stream() << "\t};\n";
										break;
									}

									case CalibrationEnum::none:
										stream()
											<< "\tfor (i = 0; i < " << field.dimExpression() << "; ++i)\n"
											<< "\t\t" << m_current->structCalbName() << "->" << field.name() << "[i] = inner." << field.name() << "[i];\n";
										break;
								}
							}
						}
						break;

					case modeGenWriterCalbBody:
						if (m_location == locationRequest)
						{
							if (m_current->is("inline"))
							{
								if (helpers::reducedCalibrationType(field) == CalibrationEnum::calb)
								{
										const FieldNest* nest = m_current->findNestByField( &field );
										stream() << "\tfor (i = 0; i < " << field.dimExpression() << "; ++i)\n\t{\n";
										stream() << "\t\tXI_calibrate_to_normal" << ((nest->fieldCalb->type() == VariableEnum::CFloat) ? "_corr" : "")
												 << ((nest->fieldCalb->type() == VariableEnum::CDFloat) ? "_Dcorr" : "")
												 << (nest->fieldMicro ? "" : "_short") << "("
											 	 << helpers::prefixedCalbName( *(nest->fieldCalb) ) << "[i], "
												 << nest->fieldNormal->name() << "[i], ";
										if (nest->fieldMicro)
												 stream() << nest->fieldMicro->name() << "[i], ";
										stream() << "calibration" << ((nest->fieldCalb->type() == VariableEnum::CFloat) ? " , id " : "") 
												 << ((nest->fieldCalb->type() == VariableEnum::CDFloat) ? " , id " : "")
												 << " );\n";
										stream() << "\t};\n";
								}
							}
							else
							{
								switch (helpers::reducedCalibrationType(field))
								{
									case CalibrationEnum::calb:
									{
										const FieldNest* nest = m_current->findNestByField( &field );
										stream() << "\tfor (i = 0; i < " << field.dimExpression() << "; ++i)\n\t{\n";
										stream() << "\t\tXI_calibrate_to_normal" << ((nest->fieldCalb->type() == VariableEnum::CFloat) ? "_corr" : "")
											 << ((nest->fieldCalb->type() == VariableEnum::CDFloat) ? "_Dcorr" : "")
											 << (nest->fieldMicro ? "" : "_short") << "("
											 << m_current->structParameterCalbName() << "->" << nest->fieldCalb->name() << "[i], "
											 << "inner." << nest->fieldNormal->name() << "[i], ";
										if (nest->fieldMicro)
											stream() << "inner." << nest->fieldMicro->name() << "[i], ";
										stream() << "calibration" << ((nest->fieldCalb->type() == VariableEnum::CFloat) ? " , id " : "") 
											<< ((nest->fieldCalb->type() == VariableEnum::CDFloat) ? " , id " : "")
											<< " );\n";
										stream() << "\t};\n";
										break;
									}
									case CalibrationEnum::none:
										stream()
											<< "\tfor (i = 0; i < " << field.dimExpression() << "; ++i)\n"
											<< "\t\tinner." << field.name() << "[i] = " << m_current->structCalbName() << "->" << field.name() << "[i];\n";
										break;
								}
							}
						}
						break;
				}
			}

			virtual void visitFlagField (FlagField& field)
			{
				emitGenericDataField( field );
			}

			virtual void visitReservedField (ReservedField& field)
			{
				if (!m_current)
					return;

				switch (m_mode)
				{
					case modeGenReaderBody:
						if (m_location == locationAnswer)
							stream() << "\tpop_garbage( &p, " << field.getSize() << " );\n";
						break;

					case modeGenWriterBody:
						if (m_location == locationRequest)
							stream() << "\tpush_garbage( &p, " << field.getSize() << " );\n";
						break;
				}
			}


		private:

			typedef enum {
				modeGenWriter, modeGenWriterBody,
				modeGenReader, modeGenReaderBody,
				modeGenWriterCalb, modeGenWriterCalbBody,
				modeGenReaderCalb, modeGenReaderCalbBody,
				modeNULL } Mode;
			typedef enum { locationNone, locationRequest, locationAnswer } Location;

			Mode m_mode;
			Location m_location;

			std::ostringstream m_os;

			Command* m_current;
			bool m_withIndexVariable;
			bool m_withSresultAnswer;
			bool m_withSresultResult;
			bool m_withDynamicArray;
			bool m_enableComments;

			std::vector<std::string> m_inlineCalbProxyArgs;

			std::ostream& stream()
			{
				return m_os;
			}

			void pushState(Command& command)
			{
				m_current = &command;
			}

			void clear()
			{
				// set flags at first command run
				m_withIndexVariable = false;
				m_withSresultAnswer = false;
				m_withSresultResult = false;
				m_withDynamicArray = false;
				m_inlineCalbProxyArgs.clear();
			}

			void emitFunctionHead (Command& command)
			{
				stream() << helpers::emitFunctionHead( command,
						m_mode == modeGenReader || m_mode == modeGenReaderCalb,
						command.is("public"),
						m_mode == modeGenReaderCalb || m_mode == modeGenWriterCalb,
						false
				);
			}

			void doGenerate (Protocol* protocol, std::ostream* os)
			{
				m_mode = (Mode)0;
				m_current = NULL;

				protocol->accept( *this );

				echoBanner( "BEGIN OF GENERATED function definitions", os );

				*os << m_os.str();

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

			void enableComments (bool enable)
			{
				m_enableComments = enable;
			}

		public:

			LibGenerator ()
				: m_current(NULL), m_enableComments(true)
			{
			}

	};

}

#endif

/* vim: set ts=2 sw=2: */
