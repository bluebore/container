// Copyright (c) 2015, Baidu.com, Inc. All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: yanshiguang02@baidu.com

#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <sched.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define CLONE_NEWUTS                    0x04000000                /* New utsname group? */
#define CLONE_NEWIPC                    0x08000000                /* New ipcs */
#define CLONE_NEWUSER                    0x10000000                /* New user namespace */
#define CLONE_NEWPID                    0x20000000                /* New pid namespace */
#define CLONE_NEWNET                    0x40000000                /* New network namespace */
#define CLONE_IO                            0x80000000                /* Clone io context */

/* 定义一个给 clone 用的栈，栈大小1M */
#define STACK_SIZE (1024 * 1024)
static char container_stack[STACK_SIZE];

char* const container_args[] = {
    "/bin/bash",
    NULL
};

int container_main(void* arg)
{
    printf("Container - inside the container!\n");

    sethostname("container", 10);
    execv(container_args[0], container_args); 
    printf("Something's wrong!\n");
    return 1;
}

int main()
{
    printf("Parent - start a container!\n");
    // clone
    int container_pid = clone(container_main, container_stack+STACK_SIZE, CLONE_NEWUTS | SIGCHLD, NULL);

    if (container_pid == -1) {
        fprintf(stderr, "Clone fail: %s\n", strerror(errno));
        return 1;
    }
    // waitpid
    waitpid(container_pid, NULL, 0);
    printf("Parent - container stopped!\n");
    return 0;
}

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
