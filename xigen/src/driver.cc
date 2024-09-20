#include "common.hh"
#include "parsercontext.hh"
#include "driver.hh"
#include "scanner.hh"

namespace yy {

	Driver::Driver(xigen::ParserContext& context)
		: trace_scanning(false),
		trace_parsing(false),
		trace_ast(false),
		trace_warnings(false),
		no_comments(false),
		m_context(context)
	{
	}

	bool Driver::parse_stream(std::istream& in, const std::string& sname)
	{
		m_context.prepare();

		streamname = sname;

		yy::Scanner scanner(&in);
		scanner.set_debug(trace_scanning);
		m_lexer = &scanner;

		parser the_parser(*this);
		the_parser.set_debug_level(trace_parsing);
		return (the_parser.parse() == 0);
	}

	bool Driver::parse_file(const std::string &filename)
	{
		std::ifstream in(filename.c_str());
		if (!in.good()) return false;
		return parse_stream(in, filename);
	}

	bool Driver::parse_string(const std::string &input, const std::string& sname)
	{
		std::istringstream iss(input);
		return parse_stream(iss, sname);
	}

	void Driver::report(const class location& l, const std::string& m)
	{
		std::cerr << l << ": " << m << std::endl;
	}

	void Driver::report(const std::string& m)
	{
		std::cerr << m << std::endl;
	}

} // namespace xigen
/* vim: set ts=2 sw=2: */
