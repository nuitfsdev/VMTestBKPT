#include <jni.h>
#include <string>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include <pthread.h>
#include <android/log.h>
#include <sstream>
#include <stdio.h>

#define TAG "test"
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,TAG,__VA_ARGS__)


void handler_sigtrap(int signo) {
    exit(-1);
}

void handler_sigbus(int signo) {
    exit(-1);
}

int setupSigTrap() {
    // BKPT throws SIGTRAP on nexus 5 / oneplus one (and most devices)
    signal(SIGTRAP, handler_sigtrap);
    // BKPT throws SIGBUS on nexus 4
    signal(SIGBUS, handler_sigbus);
}

// This will cause a SIGSEGV on some QEMU or be properly respected
int tryBKPT() {
    __asm__ __volatile__("#255");
}

// Write C++ code here.
//
// Do not forget to dynamically load the C++ library into your application.
//
// For instance,
//
// In MainActivity.java:
//    static {
//       System.loadLibrary("vmtestbkpt");
//    }
//
// Or, in MainActivity.kt:
//    companion object {
//      init {
//         System.loadLibrary("vmtestbkpt")
//      }
//    }

extern "C"
JNIEXPORT jint JNICALL
Java_com_example_vmtestbkpt_MainActivity_qemuBkpt(JNIEnv *env, jobject jObject) {
    // TODO: implement qemuBkpt()
    pid_t child = fork();
    int child_status, status = 0;

    if(child == 0) {
        setupSigTrap();
        tryBKPT();
    } else if(child == -1) {
        status = -1;
    } else {

        int timeout = 0;
        int i = 0;
        while ( waitpid(child, &child_status, WNOHANG) == 0 ) {
            sleep(1);
            // Time could be adjusted here, though in my experience if the child has not returned instantly
            // then something has gone wrong and it is an emulated device
            if(i++ == 1) {
                timeout = 1;
                break;
            }
        }

        if(timeout == 1) {
            // Process timed out - likely an emulated device and child is frozen
            status = 1;
        }

        if ( WIFEXITED(child_status) ) {
            // Likely a real device
            status = 0;
        } else {
            // Didn't exit properly - very likely an emulator
            status = 2;
        }

        // Ensure child is dead
        kill(child, SIGKILL);
    }

    return status;
}