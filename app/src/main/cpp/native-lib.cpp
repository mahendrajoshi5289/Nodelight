#include <jni.h>
#include <cstdlib>
#include <cstring>

#include "node.h"

extern "C"
JNIEXPORT jint JNICALL
Java_com_example_androidnode_MainActivity_startNodeWithArguments(
        JNIEnv *env,
        jobject,
        jobjectArray arguments) {

    jsize argument_count = env->GetArrayLength(arguments);

    int c_arguments_size = 0;

    for (int i = 0; i < argument_count; i++) {
        jstring argument =
                (jstring) env->GetObjectArrayElement(arguments, i);

        const char *current_argument =
                env->GetStringUTFChars(argument, nullptr);

        c_arguments_size += strlen(current_argument) + 1;

        env->ReleaseStringUTFChars(argument, current_argument);
    }

    char *args_buffer =
            (char *) calloc(c_arguments_size, sizeof(char));

    char **argv =
            (char **) calloc(argument_count, sizeof(char *));

    char *current_position = args_buffer;

    for (int i = 0; i < argument_count; i++) {

        jstring argument =
                (jstring) env->GetObjectArrayElement(arguments, i);

        const char *current_argument =
                env->GetStringUTFChars(argument, nullptr);

        strcpy(current_position, current_argument);

        argv[i] = current_position;

        current_position += strlen(current_argument) + 1;

        env->ReleaseStringUTFChars(argument, current_argument);
    }

    int result = node::Start(argument_count, argv);

    free(argv);
    free(args_buffer);

    return result;
}
