#ifndef CANARIN_TIMEOUT_H_
#define CANARIN_TIMEOUT_H_

#define USB_CDC_ENABLED (Serial)

#include <iostream>
#include <functional>
#include <chrono>
#include <thread>


#include <mutex>
#include <condition_variable>

// Function wrapper with a timeout and a timeout handler for void functions with no arguments
bool callWithTimeout(
    std::function<void()> func,
    int timeout,
    std::function<void(char*)> timeoutHandler,
    char * signal
) {
/*
    bool timedOut = false;

    std::thread worker([&]() {
        func();
    });

    worker.detach(); // Detach the worker thread, it will run in the background

    // Wait for the specified timeout duration
    std::this_thread::sleep_for(std::chrono::seconds(timeout));

    // Check if the worker thread is still running
    if (worker.joinable()) {
        // If it's still running, it exceeded the timeout
        worker.detach(); // Detach the worker thread
        timedOut = true;
    }

    if (timedOut) {
        // Handle timeout error
        std::cerr << "Function timed out!" << std::endl;
        // Call the timeout handler function
        if (timeoutHandler) {
            timeoutHandler(signal);
        }
      
    return false;    
    }
    else {
      //worker.join();
      return true;
      }

      */
    #ifdef USB_CDC_ENABLED

    unsigned long time_start_timeout = millis();
    logger.println("Timeout START "+(String)time_start_timeout);

    #endif
    bool timedOut = false;
    std::mutex mutex;
    std::condition_variable cv;
    std::thread worker([&]() {
        func();
        cv.notify_one(); // Notify that the function has completed
    });
    // Wait for the specified timeout duration or function completion
    std::unique_lock<std::mutex> lock(mutex);
    if (cv.wait_for(lock, std::chrono::seconds(timeout)) == std::cv_status::timeout) {
        // Timeout occurred
        timedOut = true;
    }
    if (timedOut) {
        // Handle timeout error
        logger.println("Function timed out!");
        // Call the timeout handler function
        if (timeoutHandler) {
            timeoutHandler(signal);
        }
    } else {
        // Function completed within the timeout
        worker.join(); // Wait for the worker thread to finish
    }
    #ifdef USB_CDC_ENABLED

    unsigned long time_stop_timeout = millis();
    logger.println("Timeout STOP "+(String)time_stop_timeout);
    logger.println((String) (time_stop_timeout - time_start_timeout));

    #endif
    return !timedOut;

}

#endif
