#include <check.h>
#include "ximc.h"
#include "common.h"
#include "metadata.h"
#include "protosup.h"
#include "util.h"

START_TEST(test_pre)
{
	ck_assert_str_ne("Foo", "");
}
END_TEST

START_TEST(test_ximc_version)
{
	char buf[1024];
	ximc_version(buf);
	ck_assert(strlen(buf) > 3);
}
END_TEST

void test_uri_fail_impl(const char *name)
{
	char scheme[1024], host[1024], path[1024];
	int result = parse_uri(name, scheme, sizeof(scheme),
			host, sizeof(host), path, sizeof(path), NULL, 0, NULL, 0);
	ck_assert_int_ne(result, 0);
}

void test_uri_impl(const char *name, const char *need_scheme,
		const char *need_host, const char *need_path)
{
	char scheme[1024], host[1024], path[1024];
	int result = parse_uri(name, scheme, sizeof(scheme),
			host, sizeof(host), path, sizeof(path), NULL, 0, NULL, 0);
	fprintf(stderr, "%s: scheme '%s', host '%s', path '%s'\n",
			name, scheme, host, path);
	ck_assert_int_eq(result, 0);
	ck_assert_str_eq(scheme, need_scheme);
	ck_assert_str_eq(host, need_host);
	ck_assert_str_eq(path, need_path);
}

void test_uri_impl_param(const char *name, const char *need_scheme,
		const char *need_host, const char *need_path,
		const char *need_paramname, const char *need_paramvalue)
{
	char scheme[1024], host[1024], path[1024], paramname[1024], paramvalue[1024];
	int result = parse_uri(name, scheme, sizeof(scheme),
			host, sizeof(host), path, sizeof(path),
			paramname, sizeof(paramname), paramvalue, sizeof(paramvalue));
	fprintf(stderr, "%s: scheme '%s', host '%s', path '%s', param '%s'='%s'\n",
			name, scheme, host, path, paramname, paramvalue);
	ck_assert_int_eq(result, 0);
	ck_assert_str_eq(scheme, need_scheme);
	ck_assert_str_eq(host, need_host);
	ck_assert_str_eq(path, need_path);
	ck_assert_str_eq(paramname, need_paramname ? need_paramname : "");
	ck_assert_str_eq(paramvalue, need_paramvalue ? need_paramvalue : "");
}

void test_uri_encode_impl(const char *name, const char *encoded_name)
{
	char *encoded, *decoded;
	encoded = uri_encode(name);
	ck_assert_str_eq(encoded, encoded_name);
	decoded = uri_decode(encoded);
	ck_assert_str_eq(decoded, name);
	free(encoded);
	free(decoded);
}


START_TEST(test_uri)
{
	test_uri_impl("sch://COM1",
			"sch", "COM1", "");
	test_uri_impl("sch:///COM1",
			"sch", "", "COM1");
	test_uri_impl("sch://\\\\.\\COM12",
			"sch", "\\\\.\\COM12", "");
	test_uri_impl("sch:///dev/tty/ttyACM34",
			"sch", "", "dev/tty/ttyACM34");

	test_uri_impl("sch:///c:/temp/file",
			"sch", "", "c:/temp/file");
	test_uri_impl("sch://remote.ximc.ru/c:/temp/file",
			"sch", "remote.ximc.ru", "c:/temp/file");

	test_uri_impl("sch:virtual56",
			"sch", "", "virtual56");
	test_uri_impl("sch:/virtual56",
			"sch", "", "/virtual56");
	test_uri_impl("sch://virtual56",
			"sch", "virtual56", "");
	test_uri_impl("sch:///virtual56",
			"sch", "", "virtual56");

	test_uri_impl("sch://127.0.0.1/path",
			"sch", "127.0.0.1", "path");
	test_uri_impl("sch://remote.ximc.ru/path",
			"sch", "remote.ximc.ru", "path");

	test_uri_impl("sch:",
			"sch", "", "");
	test_uri_impl("sch:foobar",
			"sch", "", "foobar");
	test_uri_fail_impl("no-scheme-here/path");

	test_uri_impl("sch://remote.ximc.ru/path",
			"sch", "remote.ximc.ru", "path");
	test_uri_impl_param("sch://remote.ximc.ru/path?serial=123",
			"sch", "remote.ximc.ru", "path", "serial", "123");
	test_uri_impl_param("sch:///path?serial=123",
			"sch", "", "path", "serial", "123");
	test_uri_impl_param("sch://remote.ximc.ru/path",
			"sch", "remote.ximc.ru", "path", NULL, NULL);
	test_uri_impl_param("sch://remote.ximc.ru/c:/temp/file?serial=123",
			"sch", "remote.ximc.ru", "c:/temp/file", "serial", "123");
}
END_TEST

START_TEST(test_uri_encode)
{
	test_uri_encode_impl("", "");
	test_uri_encode_impl("foobar", "foobar");
	test_uri_encode_impl("foo bar", "foo+bar");
	test_uri_encode_impl("foo_bar\\baz", "foo_bar\%5Cbaz");
	test_uri_encode_impl("foo_bar\\\\baz", "foo_bar\%5C\%5Cbaz");
	test_uri_encode_impl("foo/bar/baz", "foo/bar/baz");
}
END_TEST

START_TEST(test_powi)
{
	ck_assert_int_eq(powi(1,3), 1);
	ck_assert_int_eq(powi(2,3), 8);
	ck_assert_int_eq(powi(2,4), 16);
	ck_assert_int_eq(powi(0,3), 0);
}
END_TEST

int main(void)
{
    SRunner *sr;
    Suite *s = suite_create("ximc");
    TCase *tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_pre);
    tcase_add_test(tc_core, test_ximc_version);
    tcase_add_test(tc_core, test_uri);
    tcase_add_test(tc_core, test_powi);
    tcase_add_test(tc_core, test_uri_encode);
    suite_add_tcase(s, tc_core);

    sr = srunner_create(s);
    srunner_run_all(sr, CK_NORMAL);
    return srunner_ntests_failed(sr) ? 1 : 0;
}
