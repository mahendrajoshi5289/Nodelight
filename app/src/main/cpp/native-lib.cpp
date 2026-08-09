#include <jni.h>
#include <string>

#include "node.h"

static void append(JNIEnv* env, jobject activity, const char* text) {
    jclass cls = env->GetObjectClass(activity);

    jmethodID method = env->GetMethodID(
        cls,
        "appendTerminal",
        "(Ljava/lang/String;)V"
    );

    if (method == nullptr) {
        return;
    }

    jstring value = env->NewStringUTF(text);

    env->CallVoidMethod(
        activity,
        method,
        value
    );

    env->DeleteLocalRef(value);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_example_androidnode_MainActivity_startNode(
        JNIEnv* env,
        jobject activity) {

    append(env, activity, "\nJNI: startNode() called\n");

    const char* argv[] = {
        "node",
        "-e",
        "console.log('HELLO_FROM_NODE');"
    };

    append(env, activity, "JNI: calling node::Start()\n");

    int result = node::Start(
        3,
        const_cast<char**>(argv)
    );

    append(env, activity, "\nJNI: node::Start() returned\n");

    if (result == 0) {
        append(env, activity, "JNI: return code = 0\n");
    } else {
        append(env, activity, "JNI: Node returned an error\n");
    }

    return result;
}


extern "C"
JNIEXPORT jint JNICALL
Java_com_example_androidnode_MainActivity_sendCommand(
        JNIEnv* env,
        jobject activity,
        jstring command) {

    const char* cmd =
        env->GetStringUTFChars(command, nullptr);

    std::string message =
        "\n[Command received] " +
        std::string(cmd) +
        "\n";

    append(env, activity, message.c_str());

    env->ReleaseStringUTFChars(
        command,
        cmd
    );

    return 0;
}
