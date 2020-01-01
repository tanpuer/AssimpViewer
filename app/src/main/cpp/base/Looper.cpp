//
// Created by templechen on 2019/3/15.
//

#include <pthread.h>
#include "Looper.h"
#include "./native_log.h"

Looper::Looper() {
    head = nullptr;
    sem_init(&headdataavailable, 0, 0);
    sem_init(&headwriteprotect, 0, 1);
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_create(&worker_thread, &attr, trampoline, this);
    running = true;
}

Looper::~Looper() {
    if (running) {
        quit();
    }
}

void Looper::sendMessage(int what) {
    sendMessage(what, 0, 0, nullptr);
}

void Looper::sendMessage(int what, void *obj) {
    sendMessage(what, 0, 0, obj);
}

void Looper::sendMessage(int what, int arg1, int arg2) {
    sendMessage(what, arg1, arg2, nullptr);
}

void Looper::sendMessage(int what, int arg1, int arg2, void *obj) {
    auto *msg = new LooperMessage();
    msg->what = what;
    msg->arg1 = arg1;
    msg->arg2 = arg2;
    msg->obj = obj;
    msg->quit = false;
    addMessage(msg);
}

void Looper::quit() {
    auto *msg = new LooperMessage();
    msg->what = quitMessage;
    msg->arg1 = 0;
    msg->arg2 = 0;
    msg->obj = nullptr;
    msg->quit = true;
    addMessage(msg);
    pthread_join(worker_thread, nullptr);
    sem_destroy(&headwriteprotect);
    sem_destroy(&headdataavailable);
    running = false;
}

void Looper::addMessage(Looper::LooperMessage *msg) {
    sem_wait(&headwriteprotect);
    if (head == nullptr) {
        head = msg;
    } else {
        LooperMessage *h = head;
        while (h->next != nullptr) {
            h = h->next;
        }
        h->next = msg;
    }
    sem_post(&headwriteprotect);
    sem_post(&headdataavailable);
}

void *Looper::trampoline(void *p) {
    ((Looper *) p)->loop();
    ((Looper *) p)->pthreadExit();
    ALOGD("pthread exit");
    return nullptr;
}

void Looper::loop() {
    while (true) {
        sem_wait(&headdataavailable);
        sem_wait(&headwriteprotect);
        LooperMessage *msg = head;
        if (msg == nullptr) {
            sem_post(&headwriteprotect);
            continue;
        }
        head = msg->next;
        sem_post(&headwriteprotect);
        if (msg->quit) {
            delete msg;
            break;
        }
        handleMessage(msg);
        delete msg;
    }
}

void Looper::handleMessage(Looper::LooperMessage *msg) {

}

void Looper::pthreadExit() {

}
