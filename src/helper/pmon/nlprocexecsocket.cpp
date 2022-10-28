// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "nlprocexecsocket.h"

#include "processevent.h"
#include <cstdint>
#include <linux/bpf_common.h>
#include <linux/cn_proc.h>
#include <linux/connector.h>
#include <linux/filter.h>
#include <linux/netlink.h>
#include <netinet/in.h>
#include <stddef.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

NLProcExecSocket::FDHandle::~FDHandle()
{
  if (fd >= 0)
    close(fd);
}

NLProcExecSocket::NLProcExecSocket()
{
  sockFd_.fd = createSocket();
  if (sockFd_.fd < 0)
    throw std::runtime_error("Cannot create netlink socket");

  if (setTimeout(5) < 0)
    throw std::runtime_error("Cannot set socket timeout");

  if (installSocketFilter() < 0)
    throw std::runtime_error("Cannot install socket filters");

  if (bindToSocket() < 0)
    throw BindError("Cannot bind to socket");

  if (subscribeToProcEvents(true) < 0)
    throw std::runtime_error("Cannot subscribe to proc events");
}

NLProcExecSocket::~NLProcExecSocket()
{
  subscribeToProcEvents(false);
}

ProcessEvent NLProcExecSocket::waitForEvent() const
{
  struct nlcn_msg_t
  {
    struct nlmsghdr nl_hdr __attribute__((aligned(NLMSG_ALIGNTO)));
    struct cn_msg cn_msg;
  };

  auto const msg_size =
      NLMSG_LENGTH(sizeof(struct cn_msg) + sizeof(struct proc_event));
  std::uint8_t msg_buffer[msg_size] = {0};
  auto msg = (struct nlcn_msg_t *)msg_buffer;

  ssize_t const rc = recv(sockFd_.fd, msg, msg_size, 0);
  if (rc > 0) {
    auto const event = ((struct proc_event *)msg->cn_msg.data);
    switch (event->what) {
      case proc_event::PROC_EVENT_EXEC:
        return ProcessEvent{ProcessEvent::Type::EXEC,
                            event->event_data.exec.process_pid};

      case proc_event::PROC_EVENT_EXIT:
        return ProcessEvent{ProcessEvent::Type::EXIT,
                            event->event_data.exit.process_pid};
      default:
        return ProcessEvent{ProcessEvent::Type::IGNORE, -1};
    }
  }

  return ProcessEvent{ProcessEvent::Type::IGNORE, -1};
}

inline int NLProcExecSocket::createSocket() const
{
  return socket(PF_NETLINK, SOCK_DGRAM, NETLINK_CONNECTOR);
}

int NLProcExecSocket::setTimeout(unsigned int seconds) const
{
  struct timeval duration;
  duration.tv_sec = seconds;
  duration.tv_usec = 0;

  return setsockopt(sockFd_.fd, SOL_SOCKET, SO_RCVTIMEO, &duration,
                    sizeof(duration));
}

int NLProcExecSocket::bindToSocket() const
{
  struct sockaddr_nl sa_nl;
  sa_nl.nl_family = AF_NETLINK;
  sa_nl.nl_groups = CN_IDX_PROC;
  sa_nl.nl_pid = 0; // 0 = lets the kernel to handle nl_pid

  return bind(sockFd_.fd, reinterpret_cast<struct sockaddr *>(&sa_nl),
              sizeof(sa_nl));
}

int NLProcExecSocket::installSocketFilter() const
{
  struct sock_filter filter[] = {
      // clang-format off

      // check message from kernel
      BPF_STMT(BPF_LD | BPF_W | BPF_ABS, offsetof(struct nlmsghdr, nlmsg_pid)),
      BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0, 1, 0),
      BPF_STMT(BPF_RET | BPF_K, 0x0), // drop message

      // check message type NLMSG_DONE
      BPF_STMT(BPF_LD | BPF_H | BPF_ABS, offsetof(struct nlmsghdr, nlmsg_type)),
      BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, htons(NLMSG_DONE), 1, 0),
      BPF_STMT(BPF_RET | BPF_K, 0x0), // drop message

      // check proc connector event CN_IDX_PROC
      BPF_STMT(BPF_LD | BPF_W | BPF_ABS, NLMSG_LENGTH(0) +
                                         offsetof(struct cn_msg, id) +
                                         offsetof(struct cb_id, idx)),
      BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, htonl(CN_IDX_PROC), 1, 0),
      BPF_STMT(BPF_RET | BPF_K, 0x0), // drop message

      // check proc connector event CN_VAL_PROC
      BPF_STMT(BPF_LD | BPF_W | BPF_ABS, NLMSG_LENGTH(0) +
                                         offsetof(struct cn_msg, id) +
                                         offsetof(struct cb_id, val)),
      BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, htonl(CN_VAL_PROC), 1, 0),
      BPF_STMT(BPF_RET | BPF_K, 0x0), // drop message

      // accept exec messages from processes
      BPF_STMT(BPF_LD | BPF_W | BPF_ABS, NLMSG_LENGTH(0) +
                                         offsetof(struct cn_msg, data) +
                                         offsetof(struct proc_event, what)),
      BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, htonl(proc_event::PROC_EVENT_EXEC), 0, 6),

      // processes have process_pid == process_tgid (thread group leaders)
      BPF_STMT(BPF_LD | BPF_W | BPF_ABS, NLMSG_LENGTH(0) +
                                         offsetof(struct cn_msg, data) +
                                         offsetof(struct proc_event, event_data.exec.process_pid)),
      BPF_STMT(BPF_ST, 0),
      BPF_STMT(BPF_LDX | BPF_W | BPF_MEM, 0),
      BPF_STMT(BPF_LD | BPF_W | BPF_ABS, NLMSG_LENGTH(0) +
                                         offsetof(struct cn_msg, data) +
                                         offsetof(struct proc_event, event_data.exec.process_tgid)),
      BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_X, 0, 0, 1),
      BPF_STMT(BPF_RET | BPF_K, 0xffffffff),

      // accept exit messages from processes
      BPF_STMT(BPF_LD | BPF_W | BPF_ABS, NLMSG_LENGTH(0) +
                                         offsetof(struct cn_msg, data) +
                                         offsetof(struct proc_event, what)),
      BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, htonl(proc_event::PROC_EVENT_EXIT), 0, 6),

      // processes have process_pid == process_tgid
      BPF_STMT(BPF_LD | BPF_W | BPF_ABS, NLMSG_LENGTH(0) +
                                         offsetof(struct cn_msg, data) +
                                         offsetof(struct proc_event, event_data.exit.process_pid)),
      BPF_STMT(BPF_ST, 0),
      BPF_STMT(BPF_LDX | BPF_W | BPF_MEM, 0),
      BPF_STMT(BPF_LD | BPF_W | BPF_ABS, NLMSG_LENGTH(0) +
                                         offsetof(struct cn_msg, data) +
                                         offsetof(struct proc_event, event_data.exit.process_tgid)),
      BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_X, 0, 0, 1),
      BPF_STMT(BPF_RET | BPF_K, 0xffffffff),

      // drop any other messages
      BPF_STMT(BPF_RET | BPF_K, 0x0),

      // clang-format on
  };

  struct sock_fprog fprog;
  memset(&fprog, 0, sizeof(fprog));
  fprog.filter = filter;
  fprog.len = sizeof(filter) / sizeof(*filter);

  return setsockopt(sockFd_.fd, SOL_SOCKET, SO_ATTACH_FILTER, &fprog,
                    sizeof(fprog));
}

int NLProcExecSocket::subscribeToProcEvents(bool subscribe) const
{
  struct nlcn_msg_t
  {
    struct nlmsghdr nl_hdr __attribute__((aligned(NLMSG_ALIGNTO)));
    struct cn_msg cn_msg;
  };

  auto const msg_size =
      NLMSG_LENGTH(sizeof(struct cn_msg) + sizeof(enum proc_cn_mcast_op));
  std::uint8_t msg_buffer[msg_size] = {0};
  auto msg = (struct nlcn_msg_t *)msg_buffer;

  msg->nl_hdr.nlmsg_len = msg_size;
  msg->nl_hdr.nlmsg_pid = 0;
  msg->nl_hdr.nlmsg_type = NLMSG_DONE;

  msg->cn_msg.id.idx = CN_IDX_PROC;
  msg->cn_msg.id.val = CN_VAL_PROC;
  msg->cn_msg.len = sizeof(enum proc_cn_mcast_op);

  auto mcast = (enum proc_cn_mcast_op *)msg->cn_msg.data;
  *mcast = subscribe ? PROC_CN_MCAST_LISTEN : PROC_CN_MCAST_IGNORE;

  if (send(sockFd_.fd, msg, msg_size, 0) < 0)
    return -1;

  return 0;
}
