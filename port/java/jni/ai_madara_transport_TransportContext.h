/*********************************************************************
 * Copyright (c) 2013-2015 Carnegie Mellon University. All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following acknowledgments and disclaimers.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The names "Carnegie Mellon University," "SEI" and/or
 * "Software Engineering Institute" shall not be used to endorse or promote
 * products derived from this software without prior written permission. For
 * written permission, please contact permission@sei.cmu.edu.
 *
 * 4. Products derived from this software may not be called "SEI" nor may "SEI"
 * appear in their names without prior written permission of
 * permission@sei.cmu.edu.
 *
 * 5. Redistributions of any form whatsoever must retain the following
 * acknowledgment:
 *
 * This material is based upon work funded and supported by the Department of
 * Defense under Contract No. FA8721-05-C-0003 with Carnegie Mellon University
 * for the operation of the Software Engineering Institute, a federally funded
 * research and development center. Any opinions, findings and conclusions or
 * recommendations expressed in this material are those of the author(s) and
 * do not necessarily reflect the views of the United States Department of
 * Defense.
 *
 * NO WARRANTY. THIS CARNEGIE MELLON UNIVERSITY AND SOFTWARE ENGINEERING
 * INSTITUTE MATERIAL IS FURNISHED ON AN "AS-IS" BASIS. CARNEGIE MELLON
 * UNIVERSITY MAKES NO WARRANTIES OF ANY KIND, EITHER EXPRESSED OR IMPLIED,
 * AS TO ANY MATTER INCLUDING, BUT NOT LIMITED TO, WARRANTY OF FITNESS FOR
 * PURPOSE OR MERCHANTABILITY, EXCLUSIVITY, OR RESULTS OBTAINED FROM USE OF THE
 * MATERIAL. CARNEGIE MELLON UNIVERSITY DOES NOT MAKE ANY WARRANTY OF ANY KIND
 * WITH RESPECT TO FREEDOM FROM PATENT, TRADEMARK, OR COPYRIGHT INFRINGEMENT.
 *
 * This material has been approved for public release and unlimited
 * distribution.
 *
 * @author James Edmondson <jedmondson@gmail.com>
 * @author James Root <james.root@gmail.com>
 *********************************************************************/

#ifndef _Included_ai_madara_transport_TransportContext
#define _Included_ai_madara_transport_TransportContext

#include "madara/MadaraExport.h"
#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     ai_madara_transport_TransportContext
 * Method:    jni_addRecord
 * Signature: (JLjava/lang/String;J)V
 */
MADARA_EXPORT void JNICALL
Java_ai_madara_transport_TransportContext_jni_1addRecord(
    JNIEnv*, jclass, jlong, jstring, jlong);

/*
 * Class:     ai_madara_transport_TransportContext
 * Method:    jni_clearRecords
 * Signature: (J)V
 */
MADARA_EXPORT void JNICALL
Java_ai_madara_transport_TransportContext_jni_1clearRecords(
    JNIEnv*, jclass, jlong);

/*
 * Class:     ai_madara_transport_TransportContext
 * Method:    jni_getCurrentTime
 * Signature: (J)J
 */
MADARA_EXPORT jlong JNICALL
Java_ai_madara_transport_TransportContext_jni_1getCurrentTime(
    JNIEnv*, jclass, jlong);

/*
 * Class:     ai_madara_transport_TransportContext
 * Method:    jni_getDomain
 * Signature: (J)Ljava/lang/String;
 */
MADARA_EXPORT jstring JNICALL
Java_ai_madara_transport_TransportContext_jni_1getDomain(
    JNIEnv*, jclass, jlong);

/*
 * Class:     ai_madara_transport_TransportContext
 * Method:    jni_getMessageTime
 * Signature: (J)J
 */
MADARA_EXPORT jlong JNICALL
Java_ai_madara_transport_TransportContext_jni_1getMessageTime(
    JNIEnv*, jclass, jlong);

/*
 * Class:     ai_madara_transport_TransportContext
 * Method:    jni_getOperation
 * Signature: (J)J
 */
MADARA_EXPORT jlong JNICALL
Java_ai_madara_transport_TransportContext_jni_1getOperation(
    JNIEnv*, jclass, jlong);

/*
 * Class:     ai_madara_transport_TransportContext
 * Method:    jni_getOriginator
 * Signature: (J)Ljava/lang/String;
 */
MADARA_EXPORT jstring JNICALL
Java_ai_madara_transport_TransportContext_jni_1getOriginator(
    JNIEnv*, jclass, jlong);

/*
 * Class:     ai_madara_transport_TransportContext
 * Method:    jni_getReceiveBandwidth
 * Signature: (J)J
 */
MADARA_EXPORT jlong JNICALL
Java_ai_madara_transport_TransportContext_jni_1getReceiveBandwidth(
    JNIEnv*, jclass, jlong);

/*
 * Class:     ai_madara_transport_TransportContext
 * Method:    jni_getRecords
 * Signature: (JLai/madara/transport/TransportContext/MapReturn;)V
 */
MADARA_EXPORT void JNICALL
Java_ai_madara_transport_TransportContext_jni_1getRecords(
    JNIEnv*, jclass, jlong, jobject);

/*
 * Class:     ai_madara_transport_TransportContext
 * Method:    jni_getSendBandwidth
 * Signature: (J)J
 */
MADARA_EXPORT jlong JNICALL
Java_ai_madara_transport_TransportContext_jni_1getSendBandwidth(
    JNIEnv*, jclass, jlong);

#ifdef __cplusplus
}
#endif
#endif
