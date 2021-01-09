/*********************************************************************
File Name: jni_server.c
Author: fuweicheng
mail: fu_huaicheng@163.com
Created Time: Fri 17 Aug 2018 02:52:11 PM CST
Description:
*************************************************************/
#include "com_fu_server_ServerLib.h"
#include "server.h"
#include "log.h"
#include "jni.h"
#include "crc.h"

char logStr[200];
JavaVM *gVM;
jobject gObj;

jstring charTojstring(JNIEnv* env, const char* pat) 
{
	jclass strClass = (*env)->FindClass(env,"Ljava/lang/String;");
	jmethodID ctorID = (*env)->GetMethodID(env,strClass, "<init>", "([BLjava/lang/String;)V");
	jbyteArray bytes = (*env)->NewByteArray(env,(jsize)strlen(pat));
	(*env)->SetByteArrayRegion(env,bytes, 0, (jsize)strlen(pat), (jbyte*) pat);
	return (jstring) (*env)->NewObject(env,strClass, ctorID, bytes);
}
/*
 * Class:     com_fu_server_ServerLib
 * Method:    starpServer
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_fu_server_ServerLib_starpServer
  (JNIEnv *env, jobject obj, jint port)
{
	gObj = (*env)->NewGlobalRef(env,obj);
	init_config(port);
	starp_server();
	jlog(1,"jni server init....");
}

/*
 * Class:     com_fu_server_ServerLib
 * Method:    closeServer
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_fu_server_ServerLib_closeServer
  (JNIEnv *env, jobject ojb)
{
	stop_server();
}

void rece_user_str(char *key,char *data,size_t len)
{
	int ret;
	JNIEnv *env;
	if(NULL != gVM)
	{
		ret = (*gVM)->AttachCurrentThread(gVM,(void **)&env,NULL);
		if(ret == 0 && NULL != env)
		{
			jclass cls = (*env)->GetObjectClass(env,gObj);
			jmethodID mid =(*env)->GetMethodID(env,cls,"receUserStr","(Ljava/lang/String;[BI)V");

			jbyteArray array = (*env)->NewByteArray(env,len);
			(*env)->SetByteArrayRegion(env,array,0,len,(jbyte *)data);

			(*env)->CallVoidMethod(env,gObj,mid,(*env)->NewStringUTF(env,key),array,len);
			(*gVM)->DetachCurrentThread(gVM);

		}
	}
}

void rece_user_data(char *key,char *data,size_t len)
{
	int ret;
	JNIEnv *env;
	if(NULL != gVM)
	{
		ret = (*gVM)->AttachCurrentThread(gVM,(void **)&env,NULL);
		if(ret == 0 && NULL != env)
		{
			jclass cls = (*env)->GetObjectClass(env,gObj);
			jmethodID mid =(*env)->GetMethodID(env,cls,"receUserData","(Ljava/lang/String;[BI)V");

			jbyteArray array = (*env)->NewByteArray(env,len);
			(*env)->SetByteArrayRegion(env,array,0,len,(jbyte *)data);

			(*env)->CallVoidMethod(env,gObj,mid,(*env)->NewStringUTF(env,key),array,len);
			(*gVM)->DetachCurrentThread(gVM);

		}
	}
}

void rece_user_transpond(char *key,char *tokey,char *data,size_t len)
{
	int ret;
	JNIEnv *env;
	if(NULL != gVM)
	{
		ret = (*gVM)->AttachCurrentThread(gVM,(void **)&env,NULL);
		if(ret == 0 && NULL != env)
		{
			jclass cls = (*env)->GetObjectClass(env,gObj);
			jmethodID mid =(*env)->GetMethodID(env,cls,"receUserTranspond","(Ljava/lang/String;Ljava/lang/String;[BI)V");

			jbyteArray array = (*env)->NewByteArray(env,len);
			(*env)->SetByteArrayRegion(env,array,0,len,(jbyte *)data);

			(*env)->CallVoidMethod(env,gObj,mid,(*env)->NewStringUTF(env,key),(*env)->NewStringUTF(env,tokey),array,len);
			(*gVM)->DetachCurrentThread(gVM);

		}
	}
}

void client_disconnect(int fd)
{
	int ret;
	JNIEnv *env;
	close(fd);
	if(NULL != gVM)
	{
		ret = (*gVM)->AttachCurrentThread(gVM,(void **)&env,NULL);
		if(ret == 0 && NULL != env)
		{
			jclass cls = (*env)->GetObjectClass(env,gObj);
			jmethodID mid =(*env)->GetMethodID(env,cls,"clientDisConnect","(I)V");
			(*env)->CallVoidMethod(env,gObj,mid,fd);
			
			(*gVM)->DetachCurrentThread(gVM);
		}
	}
}
/*
*新的用户连接
*fd:用户描述符
*/
void new_user_connect(int fd)
{
	int ret;
	JNIEnv *env;

	if(NULL != gVM)
	{
		ret = (*gVM)->AttachCurrentThread(gVM,(void **)&env,NULL);
		if(ret == 0 && NULL != env)
		{
			jclass cls = (*env)->GetObjectClass(env,gObj);
			jmethodID mid =(*env)->GetMethodID(env,cls,"newUserConnect","(I)V");
			(*env)->CallVoidMethod(env,gObj,mid,fd);
			
			(*gVM)->DetachCurrentThread(gVM);
		}
	}
}

/*
*接收新的用户
*fd:用户描述符
*key:用户ID
*/
void accept_new_user(int fd,char *key)
{
	int ret;
	JNIEnv *env;

	if(NULL != gVM)
	{
		ret = (*gVM)->AttachCurrentThread(gVM,(void **)&env,NULL);
		if(ret == 0 && NULL != env)
		{
			jclass cls = (*env)->GetObjectClass(env,gObj);
			jmethodID mid =(*env)->GetMethodID(env,cls,"acceptNewUser","(ILjava/lang/String;)V");
			(*env)->CallVoidMethod(env,gObj,mid,fd,(*env)->NewStringUTF(env,key));
			
			(*gVM)->DetachCurrentThread(gVM);
		}
	}
}

/*
*用户心跳
*fd:用户描述符
*key:用户ID
*/
void user_heartbeat(int fd,char *key)
{
	int ret;
	JNIEnv *env;

	if(NULL != gVM)
	{
		ret = (*gVM)->AttachCurrentThread(gVM,(void **)&env,NULL);
		if(ret == 0 && NULL != env)
		{
			jclass cls = (*env)->GetObjectClass(env,gObj);
			jmethodID mid =(*env)->GetMethodID(env,cls,"userHeartbeat","(ILjava/lang/String;)V");
			(*env)->CallVoidMethod(env,gObj,mid,fd,(*env)->NewStringUTF(env,key));
			
			(*gVM)->DetachCurrentThread(gVM);
		}
	}
}

void jlog(int type,char *ch)
{
	int ret;
	JNIEnv *env;

	if(NULL != gVM)
	{
		ret = (*gVM)->AttachCurrentThread(gVM,(void **)&env,NULL);
		if(ret == 0 && NULL != env)
		{
			jclass cls = (*env)->GetObjectClass(env,gObj);
			jmethodID mid =(*env)->GetMethodID(env,cls,"LOG","(ILjava/lang/String;)V");
			(*env)->CallVoidMethod(env,gObj,mid,type,(*env)->NewStringUTF(env,ch));
			
			(*gVM)->DetachCurrentThread(gVM);
		}
	}
}

/*
 * Class:     com_fu_server_ServerLib
 * Method:    sendData
 * Signature: (I[B)J
 */
JNIEXPORT jlong JNICALL Java_com_fu_server_ServerLib_sendData
  (JNIEnv *env, jobject obj, jbyte type,jint fd, jbyteArray array)
  {
    ssize_t ret = 0;
    
    int len = (*env)->GetArrayLength(env,array);
    jbyte *j_bytes = (*env)->GetByteArrayElements(env,array,0);
    jbyte data[len];
    memcpy(data,j_bytes,len);

    ret = send_data_pack(fd, type,(char *)data, len);

    if(j_bytes)
    {
      (*env)->ReleaseByteArrayElements(env,array, j_bytes, 0);
    }

    return ret;
  }

/*
 * Class:     com_fu_server_ServerLib
 * Method:    sendCmd
 * Signature: (IB)J
 */
JNIEXPORT jlong JNICALL Java_com_fu_server_ServerLib_sendCmd
  (JNIEnv *env, jobject obj, jint fd, jbyte cmd)
  {
	ssize_t ret;
    
    char *data = (char *)malloc(sizeof(char));
    memcpy(data, (char*)&cmd, 1);
    ret = send_data_pack(fd,MSG_TYPE_CMD,data,1);
    free(data);
    data = NULL;
    
    return ret;
  }

/*
 * Class:     com_fu_server_ServerLib
 * Method:    closeClient
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_fu_server_ServerLib_closeClient
  (JNIEnv *env, jobject obj, jint fd)
  {
	  close(fd);
  }

/*
 * Class:     com_fu_server_ServerLib
 * Method:    refreshFds
 * Signature: ([II)V
 */
JNIEXPORT void JNICALL Java_com_fu_server_ServerLib_refreshFds
  (JNIEnv *env, jobject obj, jintArray _fds, jint len)
  {
	if(len > 0)
	{
		(*env)->GetIntArrayRegion(env,_fds,0,len,fds);
		fds_cnt = len;
		for(int i =0; i < len; i++)
		{
			log_flush("%d:%d\r\n",i,fds[i]);
		}
	}
	else
	{
		memset(fds,-1,fds_cnt);
		fds_cnt = 0;
		
	}
	

  }
jint JNI_OnLoad(JavaVM *vm,void *reservd)
{
	gVM = vm;
	return JNI_VERSION_1_6;
}
