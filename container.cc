// Copyright (c) 2015, Baidu.com, Inc. All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: yanshiguang02@baidu.com

#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mount.h>
#include <stdio.h>
#include <sched.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define CLONE_NEWUTS                    0x04000000                /* New utsname group? */
#define CLONE_NEWIPC                    0x08000000                /* New ipcs */
#define CLONE_NEWUSER                    0x10000000                /* New user namespace */
#define CLONE_NEWPID                    0x20000000                /* New pid namespace */
#define CLONE_NEWNET                    0x40000000                /* New network namespace */
#define CLONE_IO                            0x80000000                /* Clone io context */

/* 定义一个给 clone 用的栈，栈大小1M */
#define STACK_SIZE (1024 * 1024)
static char container_stack[STACK_SIZE];

char container_cmd[] = "/bin/bash";
char* container_args[] = {
    container_cmd,
    NULL
};

int container_main(void* arg)
{
    printf("Container [%5d] - inside the container!\n", getpid());
    sethostname("container", 10);

    // proc
    if (mount("proc", "rootfs/proc", "proc", 0, NULL) !=0 ) {
        perror("proc");
    }
    // sysfs
    if (mount("sysfs", "rootfs/sys", "sysfs", 0, NULL)!=0) {
        perror("sys");
    }
    // tmpfs
    if (mount("none", "rootfs/tmp", "tmpfs", 0, NULL)!=0) {
        perror("tmp");
    }
    // udev
    if (mount("udev", "rootfs/dev", "devtmpfs", 0, NULL)!=0) {
        perror("dev");
    }
    // devpts
    if (mount("devpts", "rootfs/dev/pts", "devpts", 0, NULL)!=0) {
        perror("dev/pts");
    }
    // shm
    if (mount("shm", "rootfs/dev/shm", "tmpfs", 0, NULL)!=0) {
        perror("dev/shm");
    }
    // run
    if (mount("tmpfs", "rootfs/run", "tmpfs", 0, NULL)!=0) {
        perror("run");
    }
    // chroot
    if ( chdir("./rootfs") != 0 || chroot("./") != 0 ){
        perror("chdir/chroot");
    }
    execv(container_args[0], container_args); 
    perror("exec");
    printf("Something's wrong!\n");
    return 1;
}

int main()
{
    printf("Parent - start a container!\n");
    // clone
    int container_pid = clone(container_main, container_stack+STACK_SIZE, CLONE_NEWNET | CLONE_NEWPID | CLONE_NEWNS | CLONE_NEWUTS | SIGCHLD, NULL);

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
