#ifndef JNIGENERATOR_HH
#define JNIGENERATOR_HH

#include "common.hh"
#include "visitor.hh"
#include "modegenerator.hh"
#include "generatorhelper.hh"

namespace xigen
{

	class EmitArrayJNIDecl : public DefaultVisitor
	{
		std::ostream& m_os;
		CalibrationEnum::Type m_antiRequiredCalb;
	public:
		EmitArrayJNIDecl(std::ostream& os, CalibrationEnum::Type antiRequiredCalb)
			: m_os(os), m_antiRequiredCalb(antiRequiredCalb) { }
		virtual void visitArrayField (ArrayField& field)
		{
			if (helpers::reducedCalibrationType( field ) != m_antiRequiredCalb)
				m_os << "\t" << mapToNativeJNIType(field.type() ) << "* c_" << field.name() << ";\n";
		}
	};

	class EmitArrayCPreallocDecl : public DefaultVisitor
	{
		std::ostream& m_os;
		const CalibrationEnum::Type m_antiRequiredCalb;
	public:
		EmitArrayCPreallocDecl(std::ostream& os, CalibrationEnum::Type antiRequiredCalb)
			: m_os(os), m_antiRequiredCalb(antiRequiredCalb) { }
		virtual void visitArrayField (ArrayField& field)
		{
			if (helpers::reducedCalibrationType( field ) != m_antiRequiredCalb)
				m_os << "\t" << mapToNativeType(field.type(), false) << " c_" << field.declaration() << ";\n";
		}
	};

	class EmitArrayDtor : public DefaultVisitor
	{
		std::ostream& m_os;
		const CalibrationEnum::Type m_antiRequiredCalb;
	public:
		EmitArrayDtor(std::ostream& os, CalibrationEnum::Type antiRequiredCalb)
			: m_os(os), m_antiRequiredCalb(antiRequiredCalb) { }
		virtual void visitArrayField (ArrayField& field)
		{
			if (helpers::reducedCalibrationType( field ) != m_antiRequiredCalb)
				m_os << "\t(*env)->Release" << mapToNativeJNICamelType(field.type(), false)
					<< "ArrayElements(env, "<< field.name() << ", c_" << field.name() << ", 0);\n";
		}
	};

	class ArgsCollector : public DefaultVisitor
	{
		std::vector<std::string> m_inlineArgs;
		CalibrationEnum::Type m_antiRequiredCalb;
		bool m_reader;

	public:
		void setup(CalibrationEnum::Type antiRequiredCalb, bool reader)
		{
			m_antiRequiredCalb = antiRequiredCalb;
			m_reader = reader;
			m_inlineArgs.clear();
		}
		virtual void visitDataField (DataField& field)
		{
			if (field.calibrationType() != m_antiRequiredCalb)
			{
				switch (field.calibrationType())
				{
					case CalibrationEnum::normal:
						m_inlineArgs.push_back( (m_reader ? "&" : "") + field.name() );
						break;

					case CalibrationEnum::calb:
						m_inlineArgs.push_back( field.name() );
						break;

					case CalibrationEnum::none:
						m_inlineArgs.push_back( field.name() );
						break;
				}
			}
		}

		virtual void visitArrayField (ArrayField& field)
		{
			if (field.calibrationType() != m_antiRequiredCalb)
				m_inlineArgs.push_back( "c_" + field.name() );
		}

		virtual void visitFlagField (FlagField& field)
		{
			if (CalibrationEnum::none != m_antiRequiredCalb)
			{
					m_inlineArgs.push_back( field.name() );
			}
		}
		const std::vector<std::string>& inlineArgs() const { return m_inlineArgs; }
	};

	class JNIGenerator : protected DefaultVisitor, public Noncopyable, public BaseGenerator
	{
		protected:
			virtual bool visitCommand (Command& command, size_t cookie)
			{
				if (cookie >= (size_t)modeNULL)
					return false;
				m_mode = (Mode)cookie;
				clear();
				//if (!cookie) clearFirstTime();
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

			void recheckCommand()
			{
				if (m_current && !m_current->calb &&
							(m_mode == modeGenWriterCalb || m_mode == modeGenReaderCalb || m_mode == modeGenWriterCalbBody || m_mode == modeGenReaderCalbBody))
				{
					m_current = NULL;
					return;
				}

				if (isWriterInline())
				{
					std::cerr << "Inline command " << m_current->name() << " is replaced with non-inline\n";
				}
			}

			void visitCommandImpl (Command& command)
			{
				m_current = NULL;
				m_location = locationNone;

				if (!command.is("public") && !command.is("publicstruct"))
					return;
				if (command.unsynced)
					return;

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
								recheckCommand();
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
								recheckCommand();
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
								recheckCommand();
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

			bool anyReaderMode() const
			{
				return
					m_mode == modeGenReader ||
					m_mode == modeGenReaderCalb ||
					m_mode == modeGenReaderBody ||
					m_mode == modeGenReaderCalbBody;
			}

			bool isWriterInline() const
			{
				return m_current->is("inline") && m_current->communicable() != Communicable::writer &&
						m_current->withAnyFields() && !m_uniqueAnswerField;
			}

			bool vinline() const
			{
				return m_current->is("inline") && !isWriterInline();
			}

			// special mark
			void makeMark(const char* prefix)
			{
				return;
				const char* s;
				switch (m_mode)
				{
					case modeGenReader:					s = "Reader"; break;
					case modeGenReaderBody:			s = "ReaderBody"; break;
					case modeGenReaderCalb:			s = "ReaderCalb"; break;
					case modeGenReaderCalbBody:	s = "ReaderCalbBody"; break;
					case modeGenWriter:					s = "Writer"; break;
					case modeGenWriterBody:			s = "WriterBody"; break;
					case modeGenWriterCalb:			s = "WriterCalb"; break;
					case modeGenWriterCalbBody:	s = "WriterCalbBody"; break;
				}
				stream() << "/*- " << prefix << " : '" << s << "' - */\n";
			}

			void clauseCreateRetData()
			{
				stream()
					<< "\tif (!handle_result(env, result))\n"
					<< "\t\treturn NULL;\n\n"

					<< "\tclazz = (*env)->FindClass(env, " << jvmClassForStruct() << ");\n"
					<< "\tif (clazz == NULL)\n"
					<< "\t\treturn NULL;\n"
					<< "\tcid = (*env)->GetMethodID(env, clazz, \"<init>\", \"()V\");\n"
					<< "\tif (cid == NULL)\n"
					<< "\t\treturn NULL;\n"
					<< "\tret_data = (*env)->NewObject(env, clazz, cid);\n"
					<< "\tif (ret_data == NULL)\n"
					<< "\t\treturn NULL;\n\n";
			}

			virtual void visitCommandPost (Command& command)
			{
				if (!m_current)
					return;

				if (m_current->calb && m_current->communicable() == Communicable::reader && vinline())
					throw ast_error("Cannot yet build JNI for inlined reader calibrated commands", m_current);

				CalibrationEnum::Type antiRequiredCalb = anyCalbMode() ? CalibrationEnum::normal : CalibrationEnum::calb;

				makeMark("post begin");

				// fully dataless command
				if (!command.withAnyFields())
				{
					if (m_mode == modeGenWriter)
					{
							stream() << ")\n{\n"
								<< "\tXIMC_UNUSED(cthis);\n"
								<< "\thandle_result(env, " <<
									helpers::getCommandName(*m_current, anyCalbMode(), true)
									<< "(id));\n"
								<< "}\n\n";
					}
					return;
				}

				if (shouldWriteHeader( &command ))
				{
					if (m_mode == modeGenReaderCalb || m_mode == modeGenWriterCalb)
						stream() << ", jobject calibration";

					stream()
						<< ")\n"
						<< "{\n"
						<< "\tresult_t result;\n";

					if (m_extraFeatures.withArrays())
					{
						if (vinline())
						{
							if (anyReaderMode())
							{
								EmitArrayCPreallocDecl v(stream(), antiRequiredCalb);
								m_current->accept(v);
							}
							else
							{
								EmitArrayJNIDecl v(stream(), antiRequiredCalb);
								m_current->accept(v);
							}
						}
					}

					if (anyCalbMode())
						stream()
							<< "\tcalibration_t c_calibration;\n";

					if (!vinline())
					{
						stream()
							<< "\t" << (anyCalbMode() ? m_current->structCalbName() : m_current->structName())
							<< "_t data;\n";

						stream() << "\n"
							<< "\tjclass clazz;\n"
							<< "\tjmethodID cid;\n"
							<< "\tjfieldID fid;\n";
						if (m_current->communicable() == Communicable::reader)
							stream()
								<< "\tjobject ret_data;\n";
						if (m_extraFeatures.withArrays())
							stream()
								<< "\tjobject ofield;\n";
						stream() << "\n"
							<< "\tXIMC_UNUSED(cid);\n"
							<< "\tXIMC_UNUSED(fid);\n";
					}

					// should occur a little later
					if (!( command.withAnswer() && (vinline() || isWriterInline()) && m_uniqueAnswerField && !m_extraFeatures.withArrays()))
						stream() << "\tXIMC_UNUSED(cthis);\n\n";

					if (anyCalbMode())
					{
						stream()
							<< "\n\tif (!fill_calibration(env, calibration, &c_calibration))\n"
							<< "\t\treturn"
							<< (anyReaderMode() ? " NULL" : "")
							<< ";\n\n";
					}
				}

				std::string optCheckInOverrun;

				std::string resultExpression;

				switch (m_mode)
				{
					case modeGenWriter:
					case modeGenWriterCalb:
						if (!vinline())
						{
							stream()
								<< "\tclazz = (*env)->FindClass(env, " << jvmClassForStruct() << ");\n"
								<< "\tif (clazz == NULL)\n"
								<< "\t\treturn;\n\n";
						}
						break;

					case modeGenReader:
					case modeGenReaderCalb:
						if (command.withAnswer())
						{
							if (vinline() || isWriterInline())
							{
								if (m_uniqueAnswerField)
								{
									// command contains only one value type in answer so we can return it as a retval
									if (!m_extraFeatures.withArrays())
										stream()
											<< "\t" << mapToNativeType(m_uniqueAnswerField.get()->type(), false) << " " << m_uniqueAnswerField.get()->declaration() << ";\n"
											<< "\tXIMC_UNUSED(cthis);\n\n";
									std::string varName = (m_extraFeatures.withArrays() ? "c_" : "") + m_uniqueAnswerField.get()->name();
									stream()
										<< "\n\tresult = " <<
										helpers::getCommandName(*m_current, anyCalbMode(), true)
										<< "(id, &" << varName << ");\n\n"

										<< "\tif (!handle_result(env, result))\n"
										<< "\t\treturn 0;\n\n";
								}
								else
								{
									stream()
										<< "\tresult = " <<
										helpers::getCommandName(*m_current, anyCalbMode(), true)
										<< "(id, &data."
										<< join(m_argsCollector.inlineArgs(), ", &data.")
										<< (anyCalbMode() ? ", &c_calibration" : "")
										<< ");\n\n";

									clauseCreateRetData();
								}
							}
							else
							{
								stream()
									<< "\tresult = " <<
									helpers::getCommandName(*m_current, anyCalbMode(), true)
									<< "(id, &data"
									<< (anyCalbMode() ? ", &c_calibration" : "")
									<< ");\n\n";

								clauseCreateRetData();
							}
						}
						break;

					case modeGenWriterBody:
					case modeGenWriterCalbBody:
						{
							if (vinline())
							{
								stream()
									<< "\n\tresult = " <<
									helpers::getCommandName(*m_current, anyCalbMode(), true)
									<< "(id, "
									<< join(m_argsCollector.inlineArgs(), ", ")
									<< (anyCalbMode() ? ", &c_calibration" : "")
									<< ");\n\n";
							}
							else
							{
								stream()
									<< "\n\t(*env)->DeleteLocalRef(env, clazz);\n"
									<< "\n\tresult = "
										<< helpers::getCommandName(*m_current, anyCalbMode(), true)
									<< "(id, &data"
									<< (anyCalbMode() ? ", &c_calibration" : "")
									<< ");\n\n";
							}
							if (m_extraFeatures.withArrays())
							{
								if (vinline())
								{
									EmitArrayDtor v(stream(), antiRequiredCalb);
									m_current->accept(v);
									stream() << "\n";
								}
							}
							stream()
								<< "\thandle_result(env, result);";
							break;
						}

					case modeGenReaderBody:
					case modeGenReaderCalbBody:
					{
						if (!(vinline() && m_uniqueAnswerField))
						{
							stream()
								<< "\n\t(*env)->DeleteLocalRef(env, clazz);\n\n"

								<< "\treturn ret_data;";
						}
						break;
					}
				}

				if (shouldWriteFooter( &command ))
				{
					stream() << "\n}\n\n";
				}

				makeMark("post end");

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
			void emitGetField (F& field)
			{
				if (field.type() == VariableEnum::Char)
					stream()
						<< "\tif (!get_string_field(env, clazz, " << m_current->structParameterName() << ", \"" << field.name() << "\", data."
						<< field.name() << ", sizeof(data." << field.name() << ")))\n"
						<< "\t\treturn;\n\n";
				else
					stream()
						<< "\tfid = (*env)->GetFieldID(env, clazz, \"" << field.name() << "\", " << jvmField(field.type(), false) << ");\n"
						<< "\tif (fid == NULL) return;\n"
						<< "\tdata." << field.name() << " = (*env)->Get" << mapToNativeJNICamelType(field.type(), true) << "Field(env, "
							<< m_current->structParameterName() << ", fid);\n\n";
			}

			template <class F>
			void emitSetField (F& field)
			{
				if (field.type() == VariableEnum::Char)
					stream()
						<< "\tif (!set_string_field(env, clazz, ret_data, \"" << field.name() << "\", data."
						<< field.name() << ", sizeof(data." << field.name() << ")))\n"
						<< "\t\treturn NULL;\n\n";
				else
					stream()
						<< "\tfid = (*env)->GetFieldID(env, clazz, \"" << field.name() << "\", " << jvmField(field.type(), false) << ");\n"
						<< "\tif (fid == NULL) return NULL;\n"
						<< "\t(*env)->Set" << mapToNativeJNICamelType(field.type(), true) << "Field(env, ret_data, fid, data." << field.name() << ");\n\n";
			}

			template <class F>
			void emitField (F& field, bool reader)
			{
				if (reader)
					emitSetField(field);
				else
					emitGetField(field);
			}

			template <class F>
			void emitGenericDataField (F& field)
			{
				if (!m_current)
					return;

				if (helpers::fieldPurpose( field ) != DataField::Purpose::normal)
					throw ast_error( "Unsupported service result/answer field for the JNI wrapper", m_current );

				Location requiredLocation = anyReaderMode() ? locationAnswer : locationRequest;
				CalibrationEnum::Type antiRequiredCalb = anyCalbMode() ? CalibrationEnum::normal : CalibrationEnum::calb;

				switch (m_mode)
				{
					case modeGenReader:
					case modeGenWriter:
					case modeGenReaderCalb:
					case modeGenWriterCalb:
						if (vinline() && shouldWriteHeader( m_current )
								&& (m_mode == modeGenWriter || !m_uniqueAnswerField || m_uniqueAnswerField.get() != &field)
								&& helpers::reducedCalibrationType( field ) != antiRequiredCalb)
						{
							stream() << ", " << mapToNativeJNIType( field.type() ) << " " << field.name();
						}
						break;

					case modeGenReaderBody:
					case modeGenWriterBody:
					case modeGenReaderCalbBody:
					case modeGenWriterCalbBody:
						if (m_location == requiredLocation &&
								(!m_current->calb || helpers::reducedCalibrationType( field ) != antiRequiredCalb))
						{
							if (vinline())
							{
								if (anyReaderMode() && m_uniqueAnswerField && m_uniqueAnswerField.get() == &field)
								{
									stream()
										<< "\treturn (" << mapToNativeJNIType(m_uniqueAnswerField.get()->type()) <<  ")" << m_uniqueAnswerField.get()->name() << ";";
								}
							}
							else
								emitField(field, anyReaderMode());
						}
						break;
				}
			}

			virtual void visitDataField (DataField& field)
			{
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

				Location requiredLocation = anyReaderMode() ? locationAnswer : locationRequest;
				CalibrationEnum::Type antiRequiredCalb = anyCalbMode() ? CalibrationEnum::normal : CalibrationEnum::calb;
				std::string returnClause = (anyReaderMode() ? "return NULL;\n" : "return;\n");


				// old weird comment: write function header only for writer if both available
				switch (m_mode)
				{
					case modeGenReader:
					case modeGenWriter:
					case modeGenReaderCalb:
					case modeGenWriterCalb:
						if (m_location == requiredLocation && vinline() &&
								field.calibrationType() != antiRequiredCalb &&
								(!anyReaderMode() || !m_uniqueAnswerField || m_uniqueAnswerField.get() != &field))
							stream() << ", " << mapToNativeJNIType( field.type()) << "Array " << field.name();
						break;

					case modeGenReaderBody:
					case modeGenReaderCalbBody:
						if (m_location == requiredLocation &&
								(!m_current->calb || helpers::reducedCalibrationType( field ) != antiRequiredCalb))
						{
							if (field.type() == VariableEnum::Char)
								emitField(field, anyReaderMode());
							else
							{
								if (vinline())
								{
									if (m_uniqueAnswerField && m_uniqueAnswerField.get() == &field)
									{
										stream()
											<< "\treturn (*env)->New" << mapToNativeJNICamelType(field.type(), true) << "Array(env, sizeof(c_"
												<< field.name() << ")/sizeof(c_" << field.name() << "[0]));";
									}
								}
								else
								{
									stream()
										<< "\tofield = (*env)->New" << mapToNativeJNICamelType(field.type(), true) << "Array(env, sizeof(data."
													<< field.name() << ")/sizeof(data." << field.name() << "[0]));\n"
										<< "\tif (!ofield) " << returnClause
										<< "\tfid = (*env)->GetFieldID(env, clazz, \"" << field.name() << "\", " << jvmField(field.type(), true) << ");\n"
										<< "\tif (fid == NULL) " << returnClause
										<< "\t(*env)->SetObjectField(env, ret_data, fid, ofield);\n\n";
								}
							}
						}
						break;

					case modeGenWriterBody:
					case modeGenWriterCalbBody:
						if (m_location == requiredLocation &&
								(!m_current->calb || helpers::reducedCalibrationType( field ) != antiRequiredCalb))
						{
							if (field.type() == VariableEnum::Char)
								emitField(field, anyReaderMode());
							else
							{
								if (vinline())
								{
									stream()
										<< "\tc_" << field.name() << " = (*env)->Get" << mapToNativeJNICamelType(field.type(), true)
											<< "ArrayElements(env, " << field.name() << ", NULL);\n"
										<< "\tif (!c_" << field.name() << ")\n"
										<< "\t\treturn;\n";
								}
								else
								{
									stream()
										<< "\tfid = (*env)->GetFieldID(env, clazz, \"" << field.name() << "\", " << jvmField(field.type(), true) << ");\n"
										<< "\tif (fid == NULL) " << returnClause
										<< "\tofield = (*env)->GetObjectField(env, " << m_current->structParameterName() << ", fid);\n"
										<< "\tif (!ofield) " << returnClause
										<< "\t(*env)->Get" << mapToNativeJNICamelType(field.type(), true) << "ArrayRegion(env, ofield,\n"
										<< "\t\t0, sizeof(data." << field.name() << ")/sizeof(data." << field.name() << "[0]), ("
											<< mapToNativeJNIType(field.type()) << "*)data." << field.name() << ");\n\n";
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
			helpers::FindUniqueAnswerField m_uniqueAnswerField;
			helpers::ExtraFeaturesVisitor m_extraFeatures;
			ArgsCollector m_argsCollector;

			std::ostream& stream()
			{
				return m_os;
			}

			void pushState(Command& command)
			{
				m_current = &command;

				// first of all detect command properties
				m_uniqueAnswerField.clear();
				m_current->accept(m_uniqueAnswerField);
				m_extraFeatures.clear();
				m_current->accept(m_extraFeatures);

				CalibrationEnum::Type antiRequiredCalb = anyCalbMode() ? CalibrationEnum::normal : CalibrationEnum::calb;
				m_argsCollector.setup(antiRequiredCalb, m_current->communicable() == Communicable::reader);
				m_current->accept(m_argsCollector);
			}

			void clear()
			{
			}

			std::string makeJNIFunctionName(std::string name)
			{
				replace_all(name, "_", "_1");
				return "Java_ru_ximc_libximc_JXimc_" + name;
			}

			std::string jvmField(VariableEnum::Type type, bool isArray)
			{
				std::string ref = mapToJVMSpec(type);
				if (type == VariableEnum::Char)
					ref = "L" + ref + ";";
				if (isArray)
					ref = "[" + ref;
				return "\"" + ref + "\"";
			}

			std::string jvmClassForStruct()
			{
				return "\"ru/ximc/libximc/JXimc$" +
								(anyCalbMode() ? m_current->structCalbName() : m_current->structName())
								+ "_t\"";
			}

			void emitFunctionHead (Command& command)
			{
				//std::string commandName = makeJNIFunctionName(anyCalbMode() ? command.functionCalbName() : command.functionName());
				std::string commandName = makeJNIFunctionName(helpers::getCommandName(command, anyCalbMode(), true));

				std::string retType = anyReaderMode()
					? ((vinline() && m_uniqueAnswerField)
							? mapToNativeJNIType(m_uniqueAnswerField.get()->type()) + (m_extraFeatures.withArrays() ? "Array" : "")
							: "jobject")
					: "void";

				stream() << "JNIEXPORT " << retType << " JNICALL " <<
					commandName <<
					"\n\t(JNIEnv *env, jclass cthis, jint id";

				if (!vinline() && command.withAnyFields() && !anyReaderMode())
				{
					stream() << ", jobject " << command.structParameterName();
				}
			}

			void doGenerate (Protocol* protocol, std::ostream* os)
			{
				m_mode = (Mode)0;
				m_current = NULL;

				protocol->accept( *this );

				echoBanner( "BEGIN OF GENERATED function definitions", os );

				*os << "\n" << m_os.str() << "\n\n";

				echoBanner( "END OF GENERATED CODE", os );
			}

			void echoBanner (std::string bannerText, std::ostream* os)
			{
				*os << "\n/*\n"
					<< " "		<< std::string( bannerText.size()+4, '-' ) << "\n"
					<< "   "	<< bannerText << "\n"
					<< " "		<< std::string( bannerText.size()+4, '-' ) << "\n"
					<< "*/\n";
			}

		public:

			JNIGenerator ()
				: m_current(NULL)
			{
			}

	};

}

#endif

/* vim: set ts=2 sw=2: */
