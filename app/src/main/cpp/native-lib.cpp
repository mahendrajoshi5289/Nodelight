#include <jni.h>
#include <unistd.h>
#include <pthread.h>
#include <string>
#include <cstring>
#include <cstdio>

#include "node.h"

static JavaVM* g_vm = nullptr;
static jobject g_activity = nullptr;
static jmethodID g_append = nullptr;

static int node_stdin_pipe[2];
static int node_stdout_pipe[2];

static pthread_t output_thread;


/*
 * Send text to Android TextView
 */
static void appendText(JNIEnv* env, const char* text)
{
    jstring value =
        env->NewStringUTF(text);

    env->CallVoidMethod(
        g_activity,
        g_append,
        value
    );

    env->DeleteLocalRef(value);
}


/*
 * Read Node stdout/stderr.
 */
static void* outputReader(void*)
{
    JNIEnv* env = nullptr;

    g_vm->AttachCurrentThread(
        &env,
        nullptr
    );

    char buffer[4096];

    while (true)
    {
        ssize_t count =
            read(
                node_stdout_pipe[0],
                buffer,
                sizeof(buffer) - 1
            );

        if (count <= 0)
            break;

        buffer[count] = '\0';

        appendText(
            env,
            buffer
        );
    }

    g_vm->DetachCurrentThread();

    return nullptr;
}


/*
 * Start Node.js.
 */
extern "C"
JNIEXPORT jint JNICALL
Java_com_example_androidnode_MainActivity_startNode(
    JNIEnv* env,
    jobject activity)
{
    env->GetJavaVM(&g_vm);

    g_activity =
        env->NewGlobalRef(activity);

    jclass cls =
        env->GetObjectClass(activity);

    g_append =
        env->GetMethodID(
            cls,
            "appendTerminal",
            "(Ljava/lang/String;)V"
        );

    if (g_append == nullptr)
        return -10;


    /*
     * Android -> Node
     */
    if (pipe(node_stdin_pipe) != 0)
        return -11;


    /*
     * Node -> Android
     */
    if (pipe(node_stdout_pipe) != 0)
        return -12;


    /*
     * Connect Node's stdin.
     */
    dup2(
        node_stdin_pipe[0],
        STDIN_FILENO
    );


    /*
     * Connect Node's stdout.
     */
    dup2(
        node_stdout_pipe[1],
        STDOUT_FILENO
    );


    /*
     * Connect Node's stderr.
     */
    dup2(
        node_stdout_pipe[1],
        STDERR_FILENO
    );


    /*
     * Start output reader.
     */
    pthread_create(
        &output_thread,
        nullptr,
        outputReader,
        nullptr
    );


    /*
     * Start Node with main.js.
     */
    const char* argv[] =
    {
        "node",
        "/data/data/com.example.androidnode/files/main.js"
    };

    chdir("/data/data/com.example.androidnode/files");
    int result =
        node::Start(
            2,
            const_cast<char**>(argv)
        );


    return result;
}


/*
 * Send a command from Android to Node.
 */
extern "C"
JNIEXPORT jint JNICALL
Java_com_example_androidnode_MainActivity_sendCommand(
    JNIEnv* env,
    jobject,
    jstring command)
{
    if (node_stdin_pipe[1] < 0)
        return -20;


    const char* text =
        env->GetStringUTFChars(
            command,
            nullptr
        );


    std::string commandString(text);

    env->ReleaseStringUTFChars(
        command,
        text
    );


    commandString += "\n";


    ssize_t result =
        write(
            node_stdin_pipe[1],
            commandString.c_str(),
            commandString.length()
        );


    if (result < 0)
        return -21;


    return 0;
}
