#ifndef MODEL_HH
#define MODEL_HH

#include "common.hh"
#include "visitor.hh"

/*
1. crc logic
If there is only 4-byte command in section (field or answer), don't add crc
If anything added (even reserved fields), add crc
But be careful to check everything

2. something weird with:
command "write_data1" writer "wdax" (142) reader "wdat" (4)
without public
with inline
fields:
	int8u datalen
	int8u data[128]
	reserved 8

datalen became a pointer!
*/

namespace xigen
{

	struct VariableEnum
	{
		typedef enum { Int64u, Int64s, Int32u, Int32s, Int16u, Int16s, Int8u, Int8s, Float, Double, Char, Byte, CFloat, CDFloat } Type;
	};

	template <>
	inline std::string toString<VariableEnum::Type>(const VariableEnum::Type& type)
	{
		switch (type)
		{
			case VariableEnum::Int64u:	return "int64u";
			case VariableEnum::Int64s:	return "int64s";
			case VariableEnum::Int32u:	return "int32u";
			case VariableEnum::Int32s:	return "int32s";
			case VariableEnum::Int16u:	return "int16u";
			case VariableEnum::Int16s:	return "int16s";
			case VariableEnum::Int8u:		return "int8u";
			case VariableEnum::Int8s:		return "int8s";
			case VariableEnum::Float:		return "float";
			case VariableEnum::Double:	return "double";
			case VariableEnum::Char:		return "char";
			case VariableEnum::Byte:		return "int8u";
			case VariableEnum::CFloat:		return "float";
			case VariableEnum::CDFloat:		return "float";
			default:										throw std::runtime_error( "unserializable type" );
		}
	}

	/* Purpose of trueArray parameter is to use actual types in array declarations
		(function inline arguments at least) */

	inline std::string mapToNativeType(const VariableEnum::Type& type, bool trueArray)
	{
		switch (type)
		{
			case VariableEnum::Int64u:	return "ulong_t";
			case VariableEnum::Int64s:	return "long_t";
			case VariableEnum::Int32u:	return "unsigned int";
			case VariableEnum::Int32s:	return "int";
			case VariableEnum::Int16u:	return "unsigned int";
			case VariableEnum::Int16s:	return "int";
			case VariableEnum::Int8u:		return trueArray ? "uint8_t" : "unsigned int";
			case VariableEnum::Int8s:		return trueArray ? "int8_t" : "int";
			case VariableEnum::Float:		return "float";
			case VariableEnum::Double:	return "double";
			case VariableEnum::Char:		return "char";
			case VariableEnum::Byte:		return "uint8_t";
			case VariableEnum::CFloat:		return "float";
			case VariableEnum::CDFloat:		return "float";
			default:										throw std::runtime_error( "unserializable type" );
		}
	}

	inline std::string mapToProtoSerializer(const VariableEnum::Type& type)
	{
		switch (type)
		{
			case VariableEnum::Int64u:	return "uint64";
			case VariableEnum::Int64s:	return "int64";
			case VariableEnum::Int32u:	return "uint32";
			case VariableEnum::Int32s:	return "int32";
			case VariableEnum::Int16u:	return "uint16";
			case VariableEnum::Int16s:	return "int16";
			case VariableEnum::Int8u:		return "uint8";
			case VariableEnum::Int8s:		return "int8";
			case VariableEnum::Float:		return "float";
			case VariableEnum::Double:	return "double";
			case VariableEnum::Byte:		return "uint8";
			case VariableEnum::CFloat:		return "float";
			case VariableEnum::CDFloat:		return "float";
			default:										throw std::runtime_error( "unserializable type" );
		}
	}

	inline std::string mapToNativeCSharpType(const VariableEnum::Type& type, bool trueArray)
	{
		switch (type)
		{
			case VariableEnum::Int64u:	return "ulong";
			case VariableEnum::Int64s:	return "long";
			case VariableEnum::Int32u:	return "uint";
			case VariableEnum::Int32s:	return "int";
			case VariableEnum::Int16u:	return "uint";
			case VariableEnum::Int16s:	return "int";
			case VariableEnum::Int8u:		return trueArray ? "byte"  : "uint";
			case VariableEnum::Int8s:		return trueArray ? "sbyte" : "int";
			case VariableEnum::Float:		return "float";
			case VariableEnum::Double:	return "double";
			case VariableEnum::Char:		return "string";
			case VariableEnum::Byte:		return "byte";
			case VariableEnum::CFloat:		return "float";
			case VariableEnum::CDFloat:		return "float";
			default:										throw std::runtime_error( "unserializable type" );
		}
	}

	inline std::string mapToNativeJavaType(const VariableEnum::Type& type)
	{
		switch (type)
		{
			case VariableEnum::Int64u:	return "long";
			case VariableEnum::Int64s:	return "long";
			case VariableEnum::Int32u:	return "int";
			case VariableEnum::Int32s:	return "int";
			case VariableEnum::Int16u:	return "short";
			case VariableEnum::Int16s:	return "short";
			case VariableEnum::Int8u:		return "byte";
			case VariableEnum::Int8s:		return "byte";
			case VariableEnum::Float:		return "float";
			case VariableEnum::Double:	return "double";
			case VariableEnum::Char:		return "String";
			case VariableEnum::Byte:		return "byte";
			case VariableEnum::CFloat:		return "float";
			case VariableEnum::CDFloat:		return "float";
			default:										throw std::runtime_error( "unserializable type" );
		}
	}

	inline std::string mapToNativeJNIType(const VariableEnum::Type& type)
	{
		switch (type)
		{
			case VariableEnum::Int64u:	return "jlong";
			case VariableEnum::Int64s:	return "jlong";
			case VariableEnum::Int32u:	return "jint";
			case VariableEnum::Int32s:	return "jint";
			case VariableEnum::Int16u:	return "jshort";
			case VariableEnum::Int16s:	return "jshort";
			case VariableEnum::Int8u:		return "jbyte";
			case VariableEnum::Int8s:		return "jbyte";
			case VariableEnum::Float:		return "jfloat";
			case VariableEnum::Double:	return "jdouble";
			case VariableEnum::Char:		return "String";
			case VariableEnum::Byte:		return "jbyte";
			case VariableEnum::CFloat:		return "jfloat";
			case VariableEnum::CDFloat:		return "jfloat";
			default:										throw std::runtime_error( "unserializable type" );
		}
	}

	inline std::string mapToNativeJNICamelType(const VariableEnum::Type& type, bool trueArray)
	{
		switch (type)
		{
			case VariableEnum::Int64u:	return "Long";
			case VariableEnum::Int64s:	return "Long";
			case VariableEnum::Int32u:	return "Int";
			case VariableEnum::Int32s:	return "Int";
			case VariableEnum::Int16u:	return "Short";
			case VariableEnum::Int16s:	return "Short";
			case VariableEnum::Int8u:		return "Byte";
			case VariableEnum::Int8s:		return "Byte";
			case VariableEnum::Float:		return "Float";
			case VariableEnum::Double:	return "Double";
			case VariableEnum::Char:		return trueArray ? "String" : "Char";
			case VariableEnum::Byte:		return "Byte";
			case VariableEnum::CFloat:		return "Float";
			case VariableEnum::CDFloat:		return "Float";
			default:										throw std::runtime_error( "unserializable type" );
		}
	}

	inline std::string mapToJVMSpec(const VariableEnum::Type& type)
	{
		switch (type)
		{
			case VariableEnum::Int64u:	return "J";
			case VariableEnum::Int64s:	return "J";
			case VariableEnum::Int32u:	return "I";
			case VariableEnum::Int32s:	return "I";
			case VariableEnum::Int16u:	return "S";
			case VariableEnum::Int16s:	return "S";
			case VariableEnum::Int8u:		return "B";
			case VariableEnum::Int8s:		return "B";
			case VariableEnum::Float:		return "F";
			case VariableEnum::Double:	return "D";
			case VariableEnum::Char:		return "java/lang/String";
			case VariableEnum::Byte:		return "B";
			case VariableEnum::CFloat:		return "F";
			case VariableEnum::CDFloat:		return "F";
			default:										throw std::runtime_error( "unserializable type" );
		}
	}

	inline std::string mapToNativePascalType(const VariableEnum::Type& type, bool trueArray)
	{
		switch (type)
		{
			case VariableEnum::Int64u:	return "Int64"; // Delphi 7 does not know unsigned 64-bit type
			case VariableEnum::Int64s:	return "Int64";
			case VariableEnum::Int32u:	return "cardinal";
			case VariableEnum::Int32s:	return "integer";
			case VariableEnum::Int16u:	return "cardinal";
			case VariableEnum::Int16s:	return "integer";
			case VariableEnum::Int8u:		return trueArray ? "byte" : "cardinal";
			case VariableEnum::Int8s:		return trueArray ? "byte" : "integer";
			case VariableEnum::Float:		return "single";
			case VariableEnum::Double:	return "double";
			case VariableEnum::Char:		return "PAnsiChar"; // actually it will be used instead of char array
			case VariableEnum::Byte:		return "byte";
			case VariableEnum::CFloat:		return "single";
			case VariableEnum::CDFloat:		return "single";
			default:										throw std::runtime_error( "unserializable type" );
		}
	}

	inline std::string mapToNativeFwType(const VariableEnum::Type& type)
	{
		switch (type)
		{
			case VariableEnum::Int64u:	return "uint64_t";
			case VariableEnum::Int64s:	return "int64_t";
			case VariableEnum::Int32u:	return "uint32_t";
			case VariableEnum::Int32s:	return "int32_t";
			case VariableEnum::Int16u:	return "uint16_t";
			case VariableEnum::Int16s:	return "int16_t";
			case VariableEnum::Int8u:		return "uint8_t";
			case VariableEnum::Int8s:		return "int8_t";
			case VariableEnum::Float:		return "float";
			case VariableEnum::Double:	return "double";
			case VariableEnum::Char:		return "uint8_t";
			case VariableEnum::Byte:		return "uint8_t";
			case VariableEnum::CFloat:		return "float";
			case VariableEnum::CDFloat:		return "float";
			default:										throw std::runtime_error("unserializable type");
		}
	}

	inline std::string mapToNativeWikiType(const VariableEnum::Type& type)
	{
		switch (type)
		{
			case VariableEnum::Int64u:	return "INT64U";
			case VariableEnum::Int64s:	return "INT64S";
			case VariableEnum::Int32u:	return "INT32U";
			case VariableEnum::Int32s:	return "INT32S";
			case VariableEnum::Int16u:	return "INT16U";
			case VariableEnum::Int16s:	return "INT16S";
			case VariableEnum::Int8u:		return "INT8U";
			case VariableEnum::Int8s:		return "INT8S";
			case VariableEnum::Float:		return "FLT32";
			case VariableEnum::Double:	return "FLT64";
			case VariableEnum::Char:		return "CHAR";
			case VariableEnum::Byte:		return "INT8U";
			case VariableEnum::CFloat:		return "FLT32";
			case VariableEnum::CDFloat:		return "FLT32";
			default:										throw std::runtime_error( "unserializable type" );
		}
	}

	inline std::string mapToQtType(const VariableEnum::Type& type)
	{
		switch (type)
		{
			case VariableEnum::Int64u:	return "Integer";
			case VariableEnum::Int64s:	return "Integer";
			case VariableEnum::Int32u:	return "UInt32";
			case VariableEnum::Int32s:	return "Int32";
			case VariableEnum::Int16u:	return "UInt32";
			case VariableEnum::Int16s:	return "Int32";
			case VariableEnum::Int8u:		return "UInt32";
			case VariableEnum::Int8s:		return "Int32";
			case VariableEnum::Float:		return "Number";
			case VariableEnum::Double:	return "Number";
			case VariableEnum::Char:		return "String";
			case VariableEnum::Byte:		return "UInt32";
			case VariableEnum::CFloat:		return "Number";
			case VariableEnum::CDFloat:		return "Number";
			default:										throw std::runtime_error( "unserializable type" );
		}
	}

	// Be aware that some types must match native C types because Python ctypes must
	// match header types
	inline std::string mapToPythonType(const VariableEnum::Type& type, bool trueArray)
	{
		switch (type)
		{
			case VariableEnum::Int64u:	return "c_ulonglong";
			case VariableEnum::Int64s:	return "c_longlong";
			case VariableEnum::Int32u:	return "c_uint";
			case VariableEnum::Int32s:	return "c_int";
			case VariableEnum::Int16u:	return "c_uint";
			case VariableEnum::Int16s:	return "c_int";
			case VariableEnum::Int8u:		return trueArray ? "c_ubyte" : "c_uint";
			case VariableEnum::Int8s:		return trueArray ? "c_byte" : "c_int";
			case VariableEnum::Float:		return "c_float";
			case VariableEnum::Double:	return "c_double";
			case VariableEnum::Char:		return "c_char";
			case VariableEnum::Byte:		return "c_ubyte";
			case VariableEnum::CFloat:		return "c_float";
			case VariableEnum::CDFloat:		return "c_float";
			default:										throw std::runtime_error( "unserializable type" );
		}
	}

	inline size_t typeSize(const VariableEnum::Type& type)
	{
		switch (type)
		{
			case VariableEnum::Int64u:	return 8;
			case VariableEnum::Int64s:	return 8;
			case VariableEnum::Int32u:	return 4;
			case VariableEnum::Int32s:	return 4;
			case VariableEnum::Int16u:	return 2;
			case VariableEnum::Int16s:	return 2;
			case VariableEnum::Int8u:		return 1;
			case VariableEnum::Int8s:		return 1;
			case VariableEnum::Float:		return 4;
			case VariableEnum::Double:	return 8;
			case VariableEnum::Char:		return 1;
			case VariableEnum::Byte:		return 1;
			case VariableEnum::CFloat:		return 4;
			case VariableEnum::CDFloat:		return 4;
			default:										throw std::runtime_error( "unserializable type" );
		}
	}

	// AST types are mapped to native types in a ximc.h
	// Bindings must use exact native types and not the AST types
	// For example, int16u maps to int and it is int in c# not short
	inline VariableEnum::Type remapBindingType(const VariableEnum::Type& type)
	{
		//return type;
		switch (type)
		{
			case VariableEnum::Int16u: return VariableEnum::Int32u;
			case VariableEnum::Int16s: return VariableEnum::Int32s;
			case VariableEnum::Int8u:  return VariableEnum::Int32u;
			case VariableEnum::Int8s:  return VariableEnum::Int32s;
			default:
				return type;
		}
	}

	/* Calibrattion specifier */
	struct CalibrationEnum
	{
		typedef enum { none, normal, calb } Type;
	};


	class Comment : public Printable
	{
		public:
			typedef enum { doxyNone, doxyUnknown, doxySection,
				doxyRead, doxyWrite, doxyStruct,
				doxyReadCalb, doxyWriteCalb, doxyStructCalb
			} DoxyType;

			explicit Comment (bool isDoxygenComment, std::string aComment)
				: comment(aComment)
			{
				if (isDoxygenComment)
					preprocess();
				else
					doxyType = doxyNone;
			}

			Comment ()
				: doxyType(doxyNone)
			{
			}

			Comment (const Comment& rhs)
				: Printable(), comment(rhs.comment), doxyType(rhs.doxyType)
			{
			}

			Comment& operator= (const Comment& rhs) {
				comment = rhs.comment;
				doxyType = rhs.doxyType;
				return *this;
			}

			virtual ~Comment() { }

			std::string getComment() const { return comment; }
			DoxyType type() const { return doxyType; }

			virtual std::string toString() const
			{
				std::ostringstream os;
				os << "[comment"
					<< " type=" << doxyType
					<< " comment=" << comment << "]";
				return os.str();
			}

		private:
			std::string comment;
			DoxyType doxyType;

			void preprocess()
			{
				std::vector<std::string> strings;
				size_t x;
				std::string s;

				parseToList(comment, "\n", " *\r\t", strings);

				doxyType = doxyUnknown;

				if (strings.size() > 1 &&
						(x = strings[0].find("$XI")) != std::string::npos)
				{
					s = trim_copy(strings[0].substr(x+3), " \t");
					if (s == "S")  doxyType = Comment::doxyStruct;
					if (s == "R")  doxyType = Comment::doxyRead;
					if (s == "W")  doxyType = Comment::doxyWrite;
					if (s == "SC") doxyType = Comment::doxyStructCalb;
					if (s == "RC") doxyType = Comment::doxyReadCalb;
					if (s == "WC") doxyType = Comment::doxyWriteCalb;

					// Drop marker from the original raw string
					if (doxyType != Comment::doxyNone &&
							doxyType != Comment::doxyUnknown &&
							s.size() >= 1)
					{
						x = comment.find("$XI"+s);
						if (x != std::string::npos && x < 10)
							comment.erase(x, s.size() + 3);
					}
				}
			}
	};
	typedef std::vector<Comment> Comments;

	// ------------------------------------------------------------------

	class Field : public Printable, public Noncopyable, public IVisitable
	{
		public:
			Field()
				: m_comment(NULL)
			{
			}


			virtual ~Field()
			{
			}
			void setComment (Comment* comment) { m_comment = comment; }
			Comment* getComment () const { return m_comment; }

			virtual size_t getSize() const = 0;
		protected:
			Comment* m_comment;
	};

	class TypeableField : public Field
	{
	public:
		TypeableField(VariableEnum::Type atype, std::string aname)
			: m_name(aname), m_type(atype)
		{
		}

		std::string name() const { return m_name; }
		VariableEnum::Type type() const { return m_type; }

		virtual std::string declaration() const { return m_name; }
	protected:
		const std::string m_name;
		const VariableEnum::Type m_type;
	};

	class CalibrableTypeableField : public TypeableField
	{
	public:
		CalibrableTypeableField(VariableEnum::Type atype, std::string aname, CalibrationEnum::Type acalibrationType)
			: TypeableField(atype, aname), m_calibrationType(acalibrationType)
		{
		}

		CalibrationEnum::Type calibrationType() const { return m_calibrationType; }
		void setCalibrationType (CalibrationEnum::Type acalibrationType) { m_calibrationType = acalibrationType; }
	private:
		CalibrationEnum::Type m_calibrationType;
	};


	class DataField : public CalibrableTypeableField
	{
		public:

			struct Purpose
			{
				typedef enum { normal, answer, result } Type;
			};

			DataField(VariableEnum::Type atype, std::string aname, Purpose::Type apurpose, CalibrationEnum::Type acalibrationType)
				: CalibrableTypeableField(atype, aname, acalibrationType), m_purpose(apurpose)
			{
			}

			virtual std::string toString() const
			{
				std::ostringstream os;
				os << "[field-data"
					<< " type=" << m_type
					<< " name=" << m_name
					<< " purpose=" << (int)m_purpose;
				if (calibrationType() != CalibrationEnum::none)
					os << " calb=" << (int)calibrationType();
				os
					<< "]";
				return os.str();
			}

			void accept (IVisitor& visitor)
			{
				visitor.visitDataField( *this );
			}

			virtual size_t getSize() const
			{
				return typeSize( m_type );
			}

			Purpose::Type purpose() const { return m_purpose; }

		private:
			const Purpose::Type m_purpose;
	};

	class ConstantField : public TypeableField
	{
		public:
			ConstantField(VariableEnum::Type atype, std::string aname, int avalue)
				: TypeableField(atype, aname), m_value(avalue)
			{
			}

			virtual std::string toString() const
			{
				std::ostringstream os;
				os << "[field-constant"
					<< " type=" << m_type
					<< " name=" << m_name
					<< " value=" << m_value
					<< "]";
				return os.str();
			}

			void accept (IVisitor& visitor)
			{
				visitor.visitConstantField( *this );
			}

			virtual size_t getSize() const
			{
				return typeSize( m_type );
			}

			int value() const { return m_value; }

		private:
			const int m_value;
	};

	class ArrayField : public CalibrableTypeableField
	{
		public:
			ArrayField(VariableEnum::Type atype, std::string aname, unsigned int dim, bool metalen)
				: CalibrableTypeableField(atype, aname, CalibrationEnum::none), m_dynamic(false), m_vardim(""), m_dim(dim), m_metalen(metalen)
			{
			}

			ArrayField(VariableEnum::Type atype, std::string aname, std::string vardim)
				: CalibrableTypeableField(atype, aname, CalibrationEnum::none), m_dynamic(true), m_vardim(vardim), m_dim(0), m_metalen(false)
			{
			}

			virtual std::string toString() const
			{
				std::ostringstream os;
				os << "[field-array"
					<< " type=" << m_type
					<< " name=" << m_name
					<< " dynamic=" << (m_dynamic?"yes":"no")
					<< " metalen=" << (m_metalen?"yes":"no")
					<< " dim=" << dimExpression();
				if (calibrationType() != CalibrationEnum::none)
					os << " calb=" << (int)calibrationType();
				os << "]";
				return os.str();
			}

			void accept (IVisitor& visitor)
			{
				visitor.visitArrayField( *this );
			}

			std::string dimExpression(int delta = 0) const
			{
				return m_dynamic
					? m_vardim + (delta ? "+"+xigen::toString(delta) : std::string(""))
					: xigen::toString( m_dim+delta );
			}

			bool isDynamic() const { return m_dynamic; }
			bool isMetalen() const { return m_metalen; }

			size_t getDim() const
			{
				if (m_dynamic)
					throw std::runtime_error( "Dynamic dim is not supported to be read" );
				return m_dim;
			}

			virtual size_t getSize() const
			{
				if (m_dynamic)
					return (size_t)-1;
				return typeSize( m_type ) * m_dim ;
			}

			virtual std::string declaration() const
			{
				return m_name + "[" + dimExpression() + "]";
			}

		private:
			const bool m_dynamic;
			const std::string m_vardim;
			const unsigned int m_dim;
			const bool m_metalen;
	};

	class FlagField : public TypeableField
	{
		public:
			FlagField(VariableEnum::Type atype, std::string aname)
				: TypeableField(atype, aname)
			{
			}

			FlagField(VariableEnum::Type atype, std::string aname, std::string aflagset)
				: TypeableField(atype, aname), m_flagset(aflagset)
			{
			}

			virtual std::string toString() const
			{
				std::ostringstream os;
				os << "[field-flag"
					<< " type=" << m_type
					<< " name=" << m_name;
				if (!m_flagset.empty())
					os << " flagset=" << m_flagset;
				os << "]";
				return os.str();
			}

			void accept (IVisitor& visitor)
			{
				visitor.visitFlagField( *this );
			}

			virtual size_t getSize() const
			{
				return typeSize( m_type );
			}

			std::string flagset() const { return m_flagset; }

		private:
			const std::string m_flagset;
	};

	class ReservedField : public Field
	{
		public:
			ReservedField(unsigned int amount)
				: m_amount(amount)
			{
			}

			virtual std::string toString() const
			{
				std::ostringstream os;
				os << "[field-reserved"
					<< " amount=" << m_amount
					<< "]";
				return os.str();
			}

			void accept (IVisitor& visitor)
			{
				visitor.visitReservedField( *this );
			}

			virtual size_t getSize() const
			{
				return (size_t)m_amount;
			}

		private:
			const unsigned int m_amount;
	};

	// ------------------------------------------------------------------
	//

	class FieldNest : public Printable, public Noncopyable
	{
	public:
		FieldNest()
			: fieldCalb(NULL), fieldNormal(NULL), fieldMicro(NULL)
		{
		};

		virtual std::string toString() const
		{
			std::ostringstream os;
			os << "[fieldnest";
			if (fieldCalb) os << " calb=" << *fieldCalb;
			if (fieldNormal) os << " normal=" << *fieldNormal;
			if (fieldMicro) os << " micro=" << *fieldMicro;
			os << " ] ";
			return os.str();
		}

		CalibrableTypeableField* fieldCalb;
		TypeableField* fieldNormal;
		TypeableField* fieldMicro;
	};

	typedef std::vector<FieldNest*> FieldNests;

	// ------------------------------------------------------------------
	//

	typedef std::vector<Field*> Fields;

	typedef std::vector<ConstantField*> Flags;

	class Flagset : public Printable, public Noncopyable, public IVisitable
	{
	public:

		explicit Flagset (const std::string& aname, Comment* acomment)
			: m_name(aname), m_comment(acomment)
		{
		}

		void addFlag (ConstantField* flag) { m_flags.push_back(flag); }

		virtual std::string toString() const
		{
			std::ostringstream os;
			os << "[flagset"
				<< " name=" << m_name
				<< " comment=" << (m_comment ? m_comment->getComment() : std::string())
				<< " [flags";
			for (Flags::const_iterator it = m_flags.begin(); it != m_flags.end(); ++it)
				os << " " << **it;
			os << " ]"
				<< " ] ";
			return os.str();
		}

		void accept (IVisitor& visitor)
		{
			visitor.visitFlagset( *this );
			for (Flags::iterator it = m_flags.begin(); it != m_flags.end(); ++it)
				(*it)->accept( visitor );
			visitor.visitFlagsetPost( *this );
		}

		std::string name() const { return m_name; }
		Comment* comment() const { return m_comment; }
		const Flags& flags() const { return m_flags; }

	private:
		const std::string m_name;
		Comment* m_comment;
		Flags m_flags;
	};

	typedef std::vector<Flagset*> Flagsets;

	// ------------------------------------------------------------------

	struct Communicable
	{
		typedef enum { none, reader, writer, both } Type;
	};

	class Communicator
	{
		public:
			explicit Communicator(unsigned int acode, unsigned int asize)
				: isString(false), name(""), code(acode), size(asize)
			{
				throw std::runtime_error( "Communicators with codes are not supported" );
			}

			explicit Communicator(std::string aname, unsigned int asize)
					: isString(true), name(aname), code(0), size(asize)
			{
			}

			explicit Communicator(const Communicator& rhs)
					: isString(rhs.isString), name(rhs.name), code(rhs.code), size(rhs.size)
			{
			}

			bool equalName(const Communicator& rhs) const
			{
				if (isString != rhs.isString)
					return false;
				return isString
					? (name == rhs.name)
					: (code == rhs.code);
			}

			std::string asHexString() const
			{
				char buf[32];
				snprintf(buf, sizeof(buf)-1, "0x%.2x%.2x%.2x%.2x",
						::tolower(name[3]),
						::tolower(name[2]),
						::tolower(name[1]),
						::tolower(name[0])
						);
				return buf;
			}

			std::string asHexStringUpper() const
			{
				char buf[32];
				snprintf(buf, sizeof(buf)-1, "0x%.2X%.2X%.2X%.2X",
						::tolower(name[3]),
						::tolower(name[2]),
						::tolower(name[1]),
						::tolower(name[0])
						);
				return buf;
			}

			const bool isString;
			const std::string name;
			const unsigned int code;
			const unsigned int size;
	};

	inline Communicator* cloneOrNull(Communicator* pcomm)
	{
		return pcomm ? new Communicator(*pcomm) : NULL;
	}


	typedef std::map<std::string, bool> Features;

	/* Visitor to check if a command contains a calb field */
	class CheckIsCalbCommand : public DefaultVisitor
	{
		bool m_calb;
	public:
		CheckIsCalbCommand() : m_calb(false) { }
		virtual void visitDataField (DataField& field)   { m_calb = m_calb || (field.calibrationType() == CalibrationEnum::calb); }
		virtual void visitArrayField (ArrayField& field) { m_calb = m_calb || (field.calibrationType() == CalibrationEnum::calb); }
		bool isCalb() const { return m_calb; }
	};

	class Command : public Printable, public IVisitable
	{
		public:
			explicit Command(std::string aName)
				: paired(false),
					master(true),
					unsynced(false),
					calb(false),
					pairedCommand(NULL),
					communicatorUniversal(NULL),
					communicatorReader(NULL),
					communicatorWriter(NULL),
					m_name(aName)
			{
			}

			Command (const Command& rhs)
				: Printable(), IVisitable(),
					paired(rhs.paired),
					master(rhs.master),
					unsynced(rhs.unsynced),
					calb(rhs.calb),
					pairedCommand(NULL),
					doxyComments(rhs.doxyComments),
					communicatorUniversal(cloneOrNull(rhs.communicatorUniversal)),
					communicatorReader(cloneOrNull(rhs.communicatorReader)),
					communicatorWriter(cloneOrNull(rhs.communicatorWriter)),
					m_name(rhs.m_name),
					m_features(rhs.m_features),
					m_fields(rhs.m_fields),
					m_answer(rhs.m_answer)
			{
			}

			virtual ~Command()
			{
				// do not delete fields! they can be shared between paired commands
				if (communicatorUniversal) delete communicatorUniversal;
				if (communicatorReader) delete communicatorReader;
				if (communicatorWriter) delete communicatorWriter;
			}

			virtual std::string toString() const
			{
				std::ostringstream os;
				os << "[command-base"
					<< " name=" << m_name
					<< " comment=" << doxyComments.size();
				if (unsynced)
					os << " unsynced=true";
				if (calb)
					os << " calb=true";
				if (communicatorUniversal)
					os << " communicatorUniversal=[" << communicatorUniversal->name
							<< " " << communicatorUniversal->size << "]";
				if (communicatorReader)
					os << " communicatorReader=[" << communicatorReader->name
							<< " " << communicatorReader->size << "]";
				if (communicatorWriter)
					os << " communicatorWriter=[" << communicatorWriter->name
							<< " " << communicatorWriter->size << "]";
				os << " features=" << m_features
					<< " [fields request";
				for (Fields::const_iterator it = m_fields.begin(); it != m_fields.end(); ++it)
					os << " " << **it;
				os << " ]"
					<< " [fields answer";
				for (Fields::const_iterator it = m_answer.begin(); it != m_answer.end(); ++it)
					os << " " << **it;
				os << " ]"
				 << " [fieldnests";
				for (FieldNests::const_iterator it = m_nests.begin(); it != m_nests.end(); ++it)
					os << " " << **it;
				os << " ]";
				os << " ] ";
				return os.str();
			}

			void addFieldRequest (Field* field) { m_fields.push_back(field); }
			void addFieldAnswer (Field* field) { m_answer.push_back(field); }

			void addFieldNest (FieldNest* nest) { m_nests.push_back(nest); }

			bool withAnyFields() const { return !m_fields.empty() || !m_answer.empty(); }
			bool withFields() const { return !m_fields.empty(); }
			bool withAnswer() const { return !m_answer.empty(); }

			void setDefaultFeatures (const Features& defaultFeatures)
			{
				m_features = defaultFeatures;
			}

			void addFeatures (const Features& addedFeatures)
			{
				for (Features::const_iterator it = addedFeatures.begin(); it != addedFeatures.end(); ++it)
						m_features[it->first] = it->second;
			}

			bool is(const std::string feature) const
			{
				Features::const_iterator it = m_features.find( feature );
				return it != m_features.end() && it->second;
			}

			void setComments (const Comments& comments)
			{
				transformComments( comments );
			}

			std::string name() const
			{
				return m_name;
			}

			std::string functionName() const
			{
				if (withHead( name(), "command_" ) ||
						withHead( name(), "service_command_" ) ||
						withHead( name(), "get_" ) ||
						withHead( name(), "set_" ))
					return name();

				if (!withAnyFields() && communicatorWriter && !communicatorReader)
					return "command_" + name();
				else if (communicatorReader)
					return "get_" + name();
				else if (communicatorWriter)
					return "set_" + name();
				throw std::runtime_error("Can't determine name of the command " + name());
			}

			std::string functionCalbName() const
			{
				return functionName() + "_calb";
			}

			/*std::string exportFunctionName() const
			{
				return dropTail( dropTail( functionName(), "_unsynced" ), "_impl" );
			}

			std::string exportFunctionCalbName() const
			{
				return dropTail( dropTail( functionCalbName(), "_unsynced" ), "_impl" ) + "_calb";
			}*/

			std::string structName() const
			{
				return dropTail( dropTail( name(), "_unsynced" ), "_impl" );
			}

			std::string structCalbName() const
			{
				return dropTail( dropTail( name(), "_unsynced" ), "_impl" ) + "_calb";
			}

			std::string structParameterName() const
			{
				std::string s = structName();
				return s == functionName() ? "the_" + s : s;
			}

			std::string structParameterCalbName() const
			{
				std::string s = structCalbName();
				return s == functionCalbName() ? "the_" + s : s;
			}

			std::string ioSyncSuffix() const
			{
				return unsynced ? "_unsynced" : "";
			}

			void setName (std::string aname)
			{
				m_name = aname;
			}

			Communicable::Type communicable() const
			{
				if (communicatorUniversal)
					return Communicable::both;
				else if (communicatorWriter && communicatorReader)
					return Communicable::both;
				else if (!communicatorWriter && communicatorReader)
					return Communicable::reader;
				else if (communicatorWriter && !communicatorReader)
					return Communicable::writer;
				else
					return Communicable::none;
			}

			// finish parsing
			void finish()
			{
				// non-default value handling
				if (is("public"))
				{
					m_features["publicstruct"] = true;
				}
				else
				{
					if (m_features.find("publicstruct") == m_features.end())
						m_features["publicstruct"] = false;
				}

				CheckIsCalbCommand visitor;
				accept(visitor);
				calb = visitor.isCalb();
			}

			// --- fields
			bool paired;
			bool master;
			bool unsynced;
			bool calb;
			Command* pairedCommand;

			std::map<Comment::DoxyType, Comment> doxyComments;

			Communicator* communicatorUniversal;
			Communicator* communicatorReader;
			Communicator* communicatorWriter;

			void accept (IVisitor& visitor)
			{
				size_t cookie = 0;
				while (visitor.visitCommand( *this, cookie++ ))
				{
					visitor.visitRequest();
					if (withFields())
					{
						for (Fields::const_iterator it = m_fields.begin(); it != m_fields.end(); ++it)
							(*it)->accept( visitor );
					}
					visitor.visitAnswer();
					if (withAnswer())
					{
						for (Fields::const_iterator it = m_answer.begin(); it != m_answer.end(); ++it)
							(*it)->accept( visitor );
					}

					visitor.visitCommandPost( *this );
				}
			}

			Command* forkUniversal()
			{
				if (!communicatorUniversal)
					throw std::runtime_error("Wrong command.communicator configuration");

				// myself became reader, forked became writer
				Command* forked = new Command(*this);

				paired = true;
				master = true;
				forked->paired = true;
				forked->master = false;
				pairedCommand = forked;
				forked->pairedCommand = this;

				communicatorReader = new Communicator("g"+communicatorUniversal->name,
						communicatorUniversal->size);

				forked->communicatorWriter = new Communicator("s"+communicatorUniversal->name,
						communicatorUniversal->size);

				communicatorUniversal = NULL;
				forked->communicatorUniversal = NULL;

				// RWS
				doxyComments.erase(Comment::doxyWrite);
				forked->doxyComments.erase(Comment::doxyRead);

				return forked;
			}

			Command* forkDualSync()
			{
				// myself became synced, forked became unsynced
				Command* forked = new Command(*this);

				forked->unsynced = true;
				forked->setName( forked->name() + "_unsynced" );
				forked->doxyComments.clear();

				return forked;
			}

			void rebaseFields()
			{
				if (communicatorReader && !communicatorWriter && !m_fields.empty() && m_answer.empty())
				{
					XI_ASSERT(!communicatorWriter);
					XI_ASSERT(!m_fields.empty());
					XI_ASSERT(m_answer.empty());
					std::swap(m_fields, m_answer);
				}
			}

			const FieldNest* findNestByField (const Field* field)
			{
				for (FieldNests::const_iterator itNest = m_nests.begin(); itNest != m_nests.end(); ++itNest)
				{
					const FieldNest* nest = *itNest;
					if (nest->fieldCalb == field || nest->fieldNormal == field || nest->fieldMicro == field)
						return nest;
				}
				throw std::runtime_error( "Cannot find field nest for the field " + field->toString() + " command " + toString() );
			}

			const FieldNests allNests() const { return m_nests; }

		private:

			void transformComments (const Comments& comments)
			{
				// Collect only comments adjacent to the command
				// There are maybe block doxygen comments before the 1-2-3 adjacent comments, they are dropped temporarily
				doxyComments.clear();

				Comments::const_reverse_iterator citSection = comments.rbegin();
				Comments onlyDoxy;
				for (; citSection != comments.rend() && citSection->type() != Comment::doxyNone; ++citSection)
					onlyDoxy.push_back( *citSection );

				// Collect section comments
				std::string doxySectionComment;
				for (; citSection != comments.rend(); ++citSection)
					doxySectionComment = "\n" + citSection->getComment() + "\n" + doxySectionComment;

				if (!doxySectionComment.empty())
					doxyComments[Comment::doxySection] = Comment( false, doxySectionComment );

				Comments::iterator cit = onlyDoxy.begin(), cit_end = onlyDoxy.end();

				if (cit == cit_end)
				{
					if (is("public"))
						std::cerr << "WARN undocumented command " << name() << std::endl;
				}
				else
				{
					bool isThereAnyMarker = false;
					for (Comments::iterator it = cit; it != cit_end; ++it)
					{
						isThereAnyMarker = isThereAnyMarker ||
							(it->type() != Comment::doxyNone && it->type() != Comment::doxySection && it->type() != Comment::doxyUnknown);
					}

					if (!isThereAnyMarker)
						throw std::runtime_error( "Please port the documentation to the new tagged format, " + m_name );

					// Parse markers
					for (; cit != cit_end; ++cit)
					{
						if (cit->type() != Comment::doxyNone && cit->type() != Comment::doxySection && cit->type() != Comment::doxyUnknown)
						{
							doxyComments[cit->type()] = *cit;
						}
						else
							throw std::runtime_error( "Unexpected doxygen comment for command " + name());
					}

					// inline commands needs only read/write, struct is inline
					// full commands needs read/write/struct
					// XXX abstraction leak in condition
					if (communicatorUniversal || (communicatorReader && communicatorWriter && !communicatorReader->equalName( *communicatorWriter )))
					{
						if (is("inline"))
						{
							if (doxyComments.find(Comment::doxyWrite) == doxyComments.end() ||
									doxyComments.find(Comment::doxyRead) == doxyComments.end())
								throw std::runtime_error( "Wrong doxygen layout for inline universal command " + name() );
						}
						else
						{
							if (doxyComments.find(Comment::doxyWrite) == doxyComments.end() ||
									doxyComments.find(Comment::doxyRead) == doxyComments.end() ||
									doxyComments.find(Comment::doxyStruct) == doxyComments.end())
								throw std::runtime_error( "Wrong doxygen layout for full universal command " + name() );
						}
					} else if (!communicatorUniversal && communicatorReader && communicatorWriter && communicatorReader->equalName( *communicatorWriter ))
					{
						// it is special command like 'rest'
						if ((doxyComments.find(Comment::doxyWrite) == doxyComments.end()) ==
								(doxyComments.find(Comment::doxyRead) == doxyComments.end()))
							throw std::runtime_error( "Wrong doxygen layout for inline reader/writer command " + name() );
					} else if (communicatorReader)
					{
						if (is("inline") || !withAnyFields())
						{
							if (doxyComments.find(Comment::doxyRead) == doxyComments.end())
								throw std::runtime_error( "Wrong doxygen layout for inline reader command " + name() );
						}
						else
						{
							if (doxyComments.find(Comment::doxyRead) == doxyComments.end() ||
									doxyComments.find(Comment::doxyStruct) == doxyComments.end())
								throw std::runtime_error( "Wrong doxygen layout for full reader command " + name() );
						}
					} else if (communicatorWriter)
					{
						if (is("inline") || !withAnyFields())
						{
							if (doxyComments.find(Comment::doxyWrite) == doxyComments.end())
								throw std::runtime_error( "Wrong doxygen layout for inline writer command " + name() );
						}
						else
						{
							if (doxyComments.find(Comment::doxyWrite) == doxyComments.end() ||
									doxyComments.find(Comment::doxyStruct) == doxyComments.end())
								throw std::runtime_error( "Wrong doxygen layout for full writer command" + name() );
						}
					}
				}

			}

			Command& operator= (const Command& rhs);

			std::string m_name;

			Features m_features;

			//Comments m_comments;
			Fields m_fields;
			Fields m_answer;

			FieldNests m_nests;

	};


	typedef std::vector<Command*> Commands;


	// ------------------------------------------------------------------

	class Protocol : public IVisitable
	{
		public:
			Protocol() { };

			~Protocol()
			{
				clear_ptr_container( commands );
			}

			Commands commands;
			Flagsets flagsets;
			Features features;
			std::string version;

			void registerDefaults(const Features& someFeatures)
			{
				features = someFeatures;
				registerDefault( "crc", true );
				registerDefault( "answer", true );
				registerDefault( "inline", false );
				registerDefault( "public", true );
				/* no default value - registerDefault( "publicstruct", true );*/
				registerDefault( "lock", true );
				registerDefault( "dualsync", false );
			}

			virtual std::string toString() const
			{
				std::ostringstream os;
				os << "[protocol"
					<< " version=" << version
					<< " features=" << features
					<< " ] ";
				return os.str();
			}

			void accept (IVisitor& visitor)
			{
				visitor.visitProtocol( *this );
				for (Flagsets::iterator it = flagsets.begin(); it != flagsets.end(); ++it)
					(*it)->accept( visitor );
				visitor.visitCommands();
				for (Commands::iterator it = commands.begin(); it != commands.end(); ++it)
					(*it)->accept( visitor );
			}

		private:

			void registerDefault (const std::string& name, bool value)
			{
				if (features.find(name) == features.end())
					features[name] = value;
			}
	};

}

#endif


/* vim: set ts=2 sw=2: */
