#include <jni.h>
#include <string>

#include "node.h"

static JavaVM* g_vm = nullptr;
static jobject g_activity = nullptr;
static jmethodID g_append = nullptr;

extern "C"
JNIEXPORT jint JNICALL
Java_com_example_androidnode_MainActivity_startNode(
        JNIEnv* env,
        jobject activity) {

    env->GetJavaVM(&g_vm);

    if (g_activity != nullptr) {
        env->DeleteGlobalRef(g_activity);
    }

    g_activity = env->NewGlobalRef(activity);

    jclass cls = env->GetObjectClass(activity);

    g_append = env->GetMethodID(
            cls,
            "appendTerminal",
            "(Ljava/lang/String;)V"
    );

    if (g_append == nullptr) {
        return -10;
    }

    const char* argv[] = {
            "node",
            "-e",
            "console.log('Node.js started successfully');"
            "console.log('version:', process.version);"
            "console.log('platform:', process.platform);"
            "console.log('arch:', process.arch);"
    };

    int argc = 3;

    return node::Start(
            argc,
            const_cast<char**>(argv)
    );
}


extern "C"
JNIEXPORT jint JNICALL
Java_com_example_androidnode_MainActivity_sendCommand(
        JNIEnv* env,
        jobject activity,
        jstring command) {

    const char* cmd =
            env->GetStringUTFChars(command, nullptr);

    jclass cls =
            env->GetObjectClass(activity);

    jmethodID append =
            env->GetMethodID(
                    cls,
                    "appendTerminal",
                    "(Ljava/lang/String;)V"
            );

    if (append == nullptr) {
        env->ReleaseStringUTFChars(command, cmd);
        return -11;
    }

    std::string output =
            "\n[Command received] " +
            std::string(cmd) +
            "\n";

    jstring result =
            env->NewStringUTF(output.c_str());

    env->CallVoidMethod(
            activity,
            append,
            result
    );

    env->DeleteLocalRef(result);

    env->ReleaseStringUTFChars(
            command,
            cmd
    );

    return 0;
}
