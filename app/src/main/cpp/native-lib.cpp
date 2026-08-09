#include <jni.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <string>
#include <cstring>

#include "node.h"

static int node_stdin_fd = -1;

static JavaVM *java_vm = nullptr;
static jobject activity_object = nullptr;
static jmethodID append_method = nullptr;

static pthread_t output_thread;
static bool output_thread_running = false;


// --------------------------------------------------
// Send stdout from Node.js to Android
// --------------------------------------------------

static void *output_reader(void *) {

    JNIEnv *env = nullptr;

    java_vm->AttachCurrentThread(&env, nullptr);

    char buffer[4096];

    while (output_thread_running) {

        ssize_t count = read(
                STDOUT_FILENO,
                buffer,
                sizeof(buffer) - 1
        );

        if (count <= 0) {
            break;
        }

        buffer[count] = '\0';

        jstring text =
                env->NewStringUTF(buffer);

        env->CallVoidMethod(
                activity_object,
                append_method,
                text
        );

        env->DeleteLocalRef(text);
    }

    java_vm->DetachCurrentThread();

    return nullptr;
}


// --------------------------------------------------
// Node thread
// --------------------------------------------------

static void *node_thread(void *) {

    const char *argv[] = {
            "node",
            "-i"
    };

    int argc = 2;

    node::Start(
            argc,
            const_cast<char **>(argv)
    );

    return nullptr;
}


// --------------------------------------------------
// JNI: start Node
// --------------------------------------------------

extern "C"
JNIEXPORT jint JNICALL
Java_com_example_androidnode_MainActivity_startNode(
        JNIEnv *env,
        jobject activity) {

    if (java_vm == nullptr) {
        env->GetJavaVM(&java_vm);
    }

    activity_object =
            env->NewGlobalRef(activity);

    jclass activity_class =
            env->GetObjectClass(activity);

    append_method =
            env->GetMethodID(
                    activity_class,
                    "appendTerminal",
                    "(Ljava/lang/String;)V"
            );

    // Create stdin pipe.
    int stdin_pipe[2];

    if (pipe(stdin_pipe) != 0) {
        return -1;
    }

    // Java -> Node stdin
    node_stdin_fd = stdin_pipe[1];

    dup2(stdin_pipe[0], STDIN_FILENO);

    close(stdin_pipe[0]);

    // Start stdout reader.
    output_thread_running = true;

    pthread_create(
            &output_thread,
            nullptr,
            output_reader,
            nullptr
    );

    // Start Node.
    pthread_t nodeThread;

    pthread_create(
            &nodeThread,
            nullptr,
            node_thread,
            nullptr
    );

    // Wait for Node.
    pthread_join(nodeThread, nullptr);

    output_thread_running = false;

    return 0;
}


// --------------------------------------------------
// JNI: send command to Node
// --------------------------------------------------

extern "C"
JNIEXPORT jint JNICALL
Java_com_example_androidnode_MainActivity_sendCommand(
        JNIEnv *env,
        jobject,
        jstring command) {

    if (node_stdin_fd < 0) {
        return -2;
    }

    const char *text =
            env->GetStringUTFChars(
                    command,
                    nullptr
            );

    std::string command_string(text);

    env->ReleaseStringUTFChars(
            command,
            text
    );

    command_string += "\n";

    ssize_t written =
            write(
                    node_stdin_fd,
                    command_string.c_str(),
                    command_string.length()
            );

    if (written < 0) {
        return -3;
    }

    return 0;
}
