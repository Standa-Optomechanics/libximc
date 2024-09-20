#include "ru_ximc_libximc_JXimc.h"
#include <string.h>
#include <stdlib.h>
#include <wchar.h>
#include "ximc.h"

#if defined(_MSC_VER)
/* msvc types burden */
typedef unsigned char bool;
#define true 1
#define false 0
#else
#include <stdbool.h>
#endif

#if defined(_MSC_VER)
/* MS vs POSIX */
char * strndup (char const *s, size_t n)
{
  size_t len = strnlen (s, n);
  char *new = malloc (len + 1);

  if (new == NULL)
    return NULL;

  new[len] = '\0';
  return memcpy (new, s, len);
}
#endif

/* -----------------
 * utility functions
 * -----------------
 */

#define STR_EXPAND(tok) #tok
#define STR(tok) STR_EXPAND(tok)

#define XIMC_UNUSED(x) (void)x;

/* Checks a result and throw an exception if it is not okay
 * Returns true if an exception hasn't been raised */
bool handle_result (JNIEnv *env, result_t result)
{
	jthrowable exc;
	jclass exc_class;
	const char* class_name;
	switch (result)
	{
		case result_ok:
			return true;
		case result_error:
			class_name = "ru/ximc/libximc/XimcError";
			break;
		case result_not_implemented:
			class_name = "ru/ximc/libximc/XimcNotImplemented";
			break;
		case result_value_error:
			class_name = "ru/ximc/libximc/XimcValueError";
			break;
		case result_nodevice:
			class_name = "ru/ximc/libximc/XimcNoDevice";
			break;
	}

	exc = (*env)->ExceptionOccurred(env);
	if (!exc) {
		exc_class = (*env)->FindClass(env, class_name);
		if (!exc_class) {
			exc_class = (*env)->FindClass(env, "java/lang/RuntimeException");
		}
		if (!exc_class) {
			return false;
		}
		if ((*env)->ThrowNew(env, exc_class, "") != 0)
			return false;
	  (*env)->DeleteLocalRef(env, exc_class);
	}
	return false;
}

bool set_string_field(JNIEnv *env, jclass clazz, jobject ret_data, const char* name, const char* value, size_t size)
{
	char* cstr;
	jstring jstr;
	jfieldID fid;

	fid = (*env)->GetFieldID(env, clazz, name, "Ljava/lang/String;");
	if (fid == NULL)
		return false;
	cstr = strndup(value, size);
	jstr = (*env)->NewStringUTF(env, cstr);
	if (!jstr)
		return false;
	free(cstr);
	(*env)->SetObjectField(env, ret_data, fid, jstr);
	return true;
}

bool get_string_field(JNIEnv *env, jclass clazz, jobject ret_data, const char* name, char* value, size_t size)
{
	const char* cstr;
	jstring jstr;
	jfieldID fid;

	fid = (*env)->GetFieldID(env, clazz, name, "Ljava/lang/String;");
	if (fid == NULL)
		return false;
	jstr = (*env)->GetObjectField(env, ret_data, fid);
	if (!jstr)
		return false;
	cstr = (*env)->GetStringUTFChars(env, jstr, NULL);
	if (!cstr)
		return false;

	strncpy(value, cstr, size);

	(*env)->ReleaseStringUTFChars(env, jstr, cstr);
	return true;
}

device_enumeration_t native_de(jlong jvm_de)
{
#if defined(_WIN64) || defined(__LP64__)
	return (device_enumeration_t)jvm_de;
#else
	return (device_enumeration_t)(jvm_de & 0xFFFFFFFF);
#endif
}

bool fill_calibration (JNIEnv *env, jobject calibration, calibration_t* c_calibration)
{
	jclass clazz;
	jfieldID fid;

	clazz = (*env)->FindClass(env, "ru/ximc/libximc/JXimc$calibration_t");
	if (clazz == NULL)
		return false;

	fid = (*env)->GetFieldID(env, clazz, "A", "D");
	if (fid == NULL) return false;
	c_calibration->A = (*env)->GetDoubleField(env, calibration, fid);

	fid = (*env)->GetFieldID(env, clazz, "MicrostepMode", "I");
	if (fid == NULL) return false;
	c_calibration->MicrostepMode = (*env)->GetIntField(env, calibration, fid);

	(*env)->DeleteLocalRef(env, clazz);

	return true;
}

typedef struct
{
	unsigned int Major;
	unsigned int Minor;
	unsigned int Release;
} bootloader_version_t;

typedef struct
{
	unsigned int Major;
	unsigned int Minor;
	unsigned int Release;
} firmware_version_t;

/* @@GENERATED_CODE@@ */


/* ----------------------
 * hand-crafted functions
 * ----------------------
 */

/*
 * Class:     ru_ximc_libximc_JXimc
 * Method:    open_device
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_ru_ximc_libximc_JXimc_open_1device
  (JNIEnv *env, jclass cthis, jstring name)
{
	device_t device_id;
	const char* str_name;
	XIMC_UNUSED(cthis);
	str_name = (*env)->GetStringUTFChars(env, name, NULL);
	if (!str_name)
		return ru_ximc_libximc_JXimc_DEVICE_UNDEFINED;

	device_id = open_device(str_name);

  (*env)->ReleaseStringUTFChars(env, name, str_name);
	return device_id;
}

/*
 * Class:     ru_ximc_libximc_JXimc
 * Method:    close_device
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_ru_ximc_libximc_JXimc_close_1device
  (JNIEnv *env, jclass cthis, jint id)
{
	int device_id = id;
	XIMC_UNUSED(cthis);
	handle_result(env, close_device(&device_id));
}

/*
 * Class:     ru_ximc_libximc_JXimc
 * Method:    probe_device
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_ru_ximc_libximc_JXimc_probe_1device
  (JNIEnv *env, jclass cthis, jstring name)
{
	result_t result;
	const char* str_name;
	XIMC_UNUSED(cthis);
	str_name = (*env)->GetStringUTFChars(env, name, NULL);
	if (!str_name)
		return;

	result = probe_device(str_name);

  (*env)->ReleaseStringUTFChars(env, name, str_name);
	handle_result(env, result);
}

/*
 * Class:     ru_ximc_libximc_JXimc
 * Method:    reset_locks
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_ru_ximc_libximc_JXimc_reset_1locks
  (JNIEnv *env, jclass cthis)
{
	XIMC_UNUSED(cthis);
	handle_result(env, reset_locks());
}

/*
 * Class:     ru_ximc_libximc_JXimc
 * Method:    ximc_fix_usbser_sys
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_ru_ximc_libximc_JXimc_ximc_1fix_1usbser_1sys
  (JNIEnv *env, jclass cthis, jstring name)
{
	result_t result;
	const char* str_name;
	XIMC_UNUSED(cthis);
	str_name = (*env)->GetStringUTFChars(env, name, NULL);
	if (!str_name)
		return;

	result = ximc_fix_usbser_sys(str_name);

  (*env)->ReleaseStringUTFChars(env, name, str_name);
	handle_result(env, result);
}

/*
 * Class:     ru_ximc_libximc_JXimc
 * Method:    goto_firmware
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL Java_ru_ximc_libximc_JXimc_goto_1firmware
  (JNIEnv *env, jclass cthis, jint id)
{
	result_t result;
	uint8_t ret;
	XIMC_UNUSED(cthis);

	result = goto_firmware(id, &ret);

	if (!handle_result(env, result))
		return false;

	return ret != 0;
}

/*
 * Class:     ru_ximc_libximc_JXimc
 * Method:    has_firmware
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_ru_ximc_libximc_JXimc_has_1firmware
  (JNIEnv *env, jclass cthis, jstring name)
{
	result_t result;
	uint8_t ret;
	const char* str_name;
	XIMC_UNUSED(cthis);

	str_name = (*env)->GetStringUTFChars(env, name, NULL);
	if (!str_name)
		return false;

	result = has_firmware(str_name, &ret);

	(*env)->ReleaseStringUTFChars(env, name, str_name);

	if (!handle_result(env, result))
		return false;

	return ret != 0;
}

/*
 * Class:     ru_ximc_libximc_JXimc
 * Method:    write_key
 * Signature: (Ljava/lang/String;[B)V
 */
JNIEXPORT void JNICALL Java_ru_ximc_libximc_JXimc_write_1key
  (JNIEnv *env, jclass cthis, jstring name, jbyteArray key)
{
	result_t result;
	jbyte* arr;
	const char* str_name;
	XIMC_UNUSED(cthis);

	str_name = (*env)->GetStringUTFChars(env, name, NULL);
	if (!str_name)
		return;

	arr = (*env)->GetByteArrayElements(env, key, NULL);
	if (!arr)
		return;

	result = write_key(str_name, (uint8_t*)arr);

	(*env)->ReleaseStringUTFChars(env, name, str_name);
	(*env)->ReleaseByteArrayElements(env, key, arr, 0);

	handle_result(env, result);
}

/*
 * Class:     ru_ximc_libximc_JXimc
 * Method:    command_reset
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_ru_ximc_libximc_JXimc_command_1reset
  (JNIEnv *env, jclass cthis, jint id)
{
	XIMC_UNUSED(cthis);
	handle_result(env, command_reset(id));
}

/*
 * Class:     ru_ximc_libximc_JXimc
 * Method:    command_update_firmware
 * Signature: (Ljava/lang/String;[BI)V
 */
JNIEXPORT void JNICALL Java_ru_ximc_libximc_JXimc_command_1update_1firmware
  (JNIEnv *env, jclass cthis, jstring name, jbyteArray firmware, jint data_size)
{
	result_t result;
	jbyte* arr;
	const char* str_name;
	XIMC_UNUSED(cthis);

	str_name = (*env)->GetStringUTFChars(env, name, NULL);
	if (!str_name)
		return;

	arr = (*env)->GetByteArrayElements(env, firmware, NULL);
	if (!arr)
		return;

	result = command_update_firmware(str_name, (const uint8_t*)arr, data_size);

	(*env)->ReleaseStringUTFChars(env, name, str_name);
	(*env)->ReleaseByteArrayElements(env, firmware, arr, 0);

	handle_result(env, result);
}

/*
 * Class:     ru_ximc_libximc_JXimc
 * Method:    msec_sleep
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_ru_ximc_libximc_JXimc_msec_1sleep
  (JNIEnv *env, jclass cthis, jint id)
{
	XIMC_UNUSED(env);
	XIMC_UNUSED(cthis);
	msec_sleep(id);
}

/*
 * Class:     ru_ximc_libximc_JXimc
 * Method:    ximc_version
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_ru_ximc_libximc_JXimc_ximc_1version
  (JNIEnv *env, jclass cthis)
{
	char cstr[32];
	jstring jstr;
	XIMC_UNUSED(cthis);

	ximc_version(cstr);

	jstr = (*env)->NewStringUTF(env, cstr);
	if (!jstr)
		return NULL;
	return jstr;
}

/*
 * Class:     ru_ximc_libximc_JXimc
 * Method:    enumerate_devices
 * Signature: (I)J
 */
JNIEXPORT jlong JNICALL Java_ru_ximc_libximc_JXimc_enumerate_1devices
  (JNIEnv *env, jclass cthis, jint enumerate_flags, jstring hints)
{
	jlong device_enumeration;
	const char* str_hints;
	XIMC_UNUSED(env);
	XIMC_UNUSED(cthis);

	str_hints = (*env)->GetStringUTFChars(env, hints, NULL);
	if (!str_hints)
		return 0;

	device_enumeration = (jlong)enumerate_devices(enumerate_flags, str_hints);

	(*env)->ReleaseStringUTFChars(env, hints, str_hints);

	return device_enumeration;
}

/*
 * Class:     ru_ximc_libximc_JXimc
 * Method:    free_enumerate_devices
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_ru_ximc_libximc_JXimc_free_1enumerate_1devices
  (JNIEnv *env, jclass cthis, jlong device_enumeration)
{
	XIMC_UNUSED(cthis);
	handle_result(env, free_enumerate_devices(native_de(device_enumeration)));
}

/*
 * Class:     ru_ximc_libximc_JXimc
 * Method:    get_device_count
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_ru_ximc_libximc_JXimc_get_1device_1count
  (JNIEnv *env, jclass cthis, jlong device_enumeration)
{
	jint ret_data;
	XIMC_UNUSED(env);
	XIMC_UNUSED(cthis);

	ret_data = get_device_count(native_de(device_enumeration));

	return ret_data;
}

/*
 * Class:     ru_ximc_libximc_JXimc
 * Method:    get_device_name
 * Signature: (JI)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_ru_ximc_libximc_JXimc_get_1device_1name
  (JNIEnv *env, jclass cthis, jlong device_enumeration, jint device_index)
{
	jstring jstr;
	char* device_name;
	XIMC_UNUSED(cthis);

	device_name = get_device_name(native_de(device_enumeration), device_index);
	if (!device_name)
		return NULL;

	jstr = (*env)->NewStringUTF(env, device_name);
	if (!jstr)
		return NULL;
	return jstr;
}

/*
 * Class:     ru_ximc_libximc_JXimc
 * Method:    get_enumerate_device_information
 * Signature: (JI)Lru/ximc/libximc/JXimc/device_information_t;
 */
JNIEXPORT jobject JNICALL Java_ru_ximc_libximc_JXimc_get_1enumerate_1device_1information
  (JNIEnv *env, jclass cthis, jlong device_enumeration, jint device_index)
{
	result_t result;
	device_information_t data;

	jclass clazz;
	jobject ret_data;
	jmethodID cid;
	XIMC_UNUSED(cthis);

	result = get_enumerate_device_information(native_de(device_enumeration), device_index, &data);

	if (!handle_result(env, result))
		return NULL;

	clazz = (*env)->FindClass(env, "ru/ximc/libximc/JXimc$device_information_t");
	if (clazz == NULL)
		return NULL;
	cid = (*env)->GetMethodID(env, clazz, "<init>", "()V");
	if (cid == NULL)
		return NULL;
	ret_data = (*env)->NewObject(env, clazz, cid);

	if (!set_string_field(env, clazz, ret_data, "Manufacturer", data.Manufacturer, sizeof(data.Manufacturer)))
		return NULL;
	if (!set_string_field(env, clazz, ret_data, "ManufacturerId", data.ManufacturerId, sizeof(data.ManufacturerId)))
		return NULL;
	if (!set_string_field(env, clazz, ret_data, "ProductDescription", data.ProductDescription, sizeof(data.ProductDescription)))
		return NULL;

	(*env)->DeleteLocalRef(env, clazz);

	return ret_data;
}

/*
 * Class:     ru_ximc_libximc_JXimc
 * Method:    get_enumerate_device_serial
 * Signature: (JI)J
 */
JNIEXPORT jlong JNICALL Java_ru_ximc_libximc_JXimc_get_1enumerate_1device_1serial
  (JNIEnv *env, jclass cthis, jlong device_enumeration, jint device_index)
{
	result_t result;
	uint32_t serial;
	XIMC_UNUSED(cthis);

	result = get_enumerate_device_serial(native_de(device_enumeration), device_index, &serial);

	handle_result(env, result);

	return (jlong)serial;
}


/*
 * Class:     ru_ximc_libximc_JXimc
 * Method:    command_wait_for_stop
 * Signature: 
 */
JNIEXPORT void JNICALL Java_ru_ximc_libximc_JXimc_command_1wait_1for_1stop
  (JNIEnv *env, jclass cthis, jint id, jint wait_interval)
{
	result_t result;
	XIMC_UNUSED(cthis);

	result = command_wait_for_stop(id, wait_interval);
	handle_result(env, result);
}

/*
 * Class:     ru_ximc_libximc_JXimc
 * Method:    set_bindy_key
 * Signature: 
 */
JNIEXPORT void JNICALL Java_ru_ximc_libximc_JXimc_set_1bindy_1key
  (JNIEnv *env, jclass cthis, jstring keyfilepath)
{
	result_t result;
	const char* str_path = (*env)->GetStringUTFChars(env, keyfilepath, NULL);;
	XIMC_UNUSED(cthis);

	result = set_bindy_key(str_path);
	handle_result(env, result);
}
