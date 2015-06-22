/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
#include "madara/MADARA_export.h"
/* Header for class com_madara_logger_Logger */

#ifndef _Included_com_madara_logger_Logger
#define _Included_com_madara_logger_Logger
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_madara_logger_Logger
 * Method:    jni_Logger
 * Signature: ()J
 */
MADARA_Export jlong JNICALL Java_com_madara_logger_Logger_jni_1Logger__
  (JNIEnv *, jobject);

/*
 * Class:     com_madara_logger_Logger
 * Method:    jni_freeLogger
 * Signature: (J)V
 */
MADARA_Export void JNICALL Java_com_madara_logger_Logger_jni_1freeLogger
  (JNIEnv *, jclass, jlong);

/*
 * Class:     com_madara_logger_Logger
 * Method:    jni_setLevel
 * Signature: (JI)V
 */
MADARA_Export void JNICALL Java_com_madara_logger_Logger_jni_1setLevel
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     com_madara_logger_Logger
 * Method:    jni_getLevel
 * Signature: (J)I
 */
MADARA_Export jint JNICALL Java_com_madara_logger_Logger_jni_1getLevel
  (JNIEnv *, jobject, jlong);

/*
 * Class:     com_madara_logger_Logger
 * Method:    jni_getTag
 * Signature: (J)Ljava/lang/String;
 */
MADARA_Export jstring JNICALL Java_com_madara_logger_Logger_jni_1getTag
  (JNIEnv *, jobject, jlong);

/*
 * Class:     com_madara_logger_Logger
 * Method:    jni_setTag
 * Signature: (JLjava/lang/String;)V
 */
MADARA_Export void JNICALL Java_com_madara_logger_Logger_jni_1setTag
  (JNIEnv *, jobject, jlong, jstring);

/*
 * Class:     com_madara_logger_Logger
 * Method:    jni_addTerm
 * Signature: (J)V
 */
MADARA_Export void JNICALL Java_com_madara_logger_Logger_jni_1addTerm
  (JNIEnv *, jobject, jlong);

/*
 * Class:     com_madara_logger_Logger
 * Method:    jni_addSyslog
 * Signature: (J)V
 */
MADARA_Export void JNICALL Java_com_madara_logger_Logger_jni_1addSyslog
  (JNIEnv *, jobject, jlong);

/*
 * Class:     com_madara_logger_Logger
 * Method:    jni_clear
 * Signature: (J)J
 */
MADARA_Export void JNICALL Java_com_madara_logger_Logger_jni_1clear
  (JNIEnv *, jobject, jlong);

/*
 * Class:     com_madara_logger_Logger
 * Method:    jni_addFile
 * Signature: (JLjava/lang/String;)V
 */
MADARA_Export void JNICALL Java_com_madara_logger_Logger_jni_1addFile
  (JNIEnv *, jobject, jlong, jstring);

/*
 * Class:     com_madara_logger_Logger
 * Method:    jni_log
 * Signature: (JLjava/lang/String;)V
 */
MADARA_Export void JNICALL Java_com_madara_logger_Logger_jni_1log
(JNIEnv *, jobject, jlong, jint, jstring);

#ifdef __cplusplus
}
#endif
#endif
