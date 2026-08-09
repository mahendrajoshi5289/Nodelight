#include <jni.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

#include "node.h"

static JavaVM* g_vm = nullptr;
static jobject g_activity = nullptr;
static jmethodID g_append = nullptr;

static int stdout_pipe[2];


static void appendText(JNIEnv* env, const char* text)
{
    jstring value = env->NewStringUTF(text);

    env->CallVoidMethod(
        g_activity,
        g_append,
        value
    );

    env->DeleteLocalRef(value);
}


static void* readNodeOutput(void*)
{
    JNIEnv* env = nullptr;

    g_vm->AttachCurrentThread(
        &env,
        nullptr
    );

    char buffer[1024];

    while (true)
    {
        int count = read(
            stdout_pipe[0],
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

    appendText(
        env,
        "JNI: redirecting stdout...\n"
    );

    /*
     * Create pipe:
     *
     * stdout_pipe[1] = Node writes here
     * stdout_pipe[0] = Android reads here
     */
    if (pipe(stdout_pipe) != 0)
    {
        appendText(
            env,
            "ERROR: pipe() failed\n"
        );

        return -1;
    }

    /*
     * Redirect stdout.
     */
    dup2(
        stdout_pipe[1],
        STDOUT_FILENO
    );

    /*
     * Redirect stderr too.
     */
    dup2(
        stdout_pipe[1],
        STDERR_FILENO
    );

    /*
     * Start Android reader.
     */
    pthread_t thread;

    pthread_create(
        &thread,
        nullptr,
        readNodeOutput,
        nullptr
    );


    appendText(
        env,
        "JNI: starting Node...\n"
    );


    /*
     * Node executes this JavaScript.
     */
    const char* argv[] =
    {
        "node",
        "-e",
        "console.log('HELLO_FROM_NODE');"
        "console.log('version:', process.version);"
        "console.log('platform:', process.platform);"
        "console.log('architecture:', process.arch);"
    };


    int result =
        node::Start(
            3,
            const_cast<char**>(argv)
        );


    /*
     * This is Android-side output,
     * not Node stdout.
     */
    char message[100];

    snprintf(
        message,
        sizeof(message),
        "\nNode exited: %d\n",
        result
    );

    appendText(
        env,
        message
    );

    return result;
}
