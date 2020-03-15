//
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// Distributed under the GPL version 3 or any later version.
//
#include "nlprocexecsocket.h"

#include "processevent.h"
#include <linux/bpf_common.h>
#include <linux/cn_proc.h>
#include <linux/connector.h>
#include <linux/filter.h>
#include <linux/netlink.h>
#include <netinet/in.h>
#include <stddef.h>
#include <string.h>
#include <sys/socket.h>
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
  struct __attribute__((aligned(NLMSG_ALIGNTO)))
  {
    struct nlmsghdr nl_hdr;
    struct __attribute__((__packed__))
    {
      struct cn_msg cn_msg;
      struct proc_event proc_ev;
    } cn;
  } nlcn_msg;

  ssize_t const rc = recv(sockFd_.fd, &nlcn_msg, sizeof(nlcn_msg), 0);
  if (rc > 0) {
    switch (nlcn_msg.cn.proc_ev.what) {
      case proc_event::PROC_EVENT_EXEC:
        return ProcessEvent{ProcessEvent::Type::EXEC,
                            nlcn_msg.cn.proc_ev.event_data.exec.process_pid};

      case proc_event::PROC_EVENT_EXIT:
        return ProcessEvent{ProcessEvent::Type::EXIT,
                            nlcn_msg.cn.proc_ev.event_data.exec.process_pid};
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
  struct sock_fprog fprog;

  // rules to only accept exec and exit messages
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

      // accept exec message
      BPF_STMT(BPF_LD | BPF_W | BPF_ABS, NLMSG_LENGTH(0) +
                                         offsetof (struct cn_msg, data) +
                                         offsetof (struct proc_event, what)),
      BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, htonl(proc_event::PROC_EVENT_EXEC), 0, 1),
      BPF_STMT(BPF_RET | BPF_K, 0xffffffff),

      // accept exit message
      BPF_STMT(BPF_LD | BPF_W | BPF_ABS, NLMSG_LENGTH(0) +
                                         offsetof (struct cn_msg, data) +
                                         offsetof (struct proc_event, what)),
      BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, htonl(proc_event::PROC_EVENT_EXIT), 0, 1),
      BPF_STMT(BPF_RET|BPF_K, 0xffffffff),

      BPF_STMT(BPF_RET|BPF_K, 0x0), // drop other messages

      // clang-format on
  };

  memset(&fprog, 0, sizeof(fprog));
  fprog.filter = filter;
  fprog.len = sizeof(filter) / sizeof(*filter);

  return setsockopt(sockFd_.fd, SOL_SOCKET, SO_ATTACH_FILTER, &fprog,
                    sizeof(fprog));
}

int NLProcExecSocket::subscribeToProcEvents(bool subscribe) const
{
  struct __attribute__((aligned(NLMSG_ALIGNTO)))
  {
    struct nlmsghdr nl_hdr;
    struct __attribute__((__packed__))
    {
      struct cn_msg cn_msg;
      enum proc_cn_mcast_op cn_mcast;
    } cn;
  } nlcn_msg;

  memset(&nlcn_msg, 0, sizeof(nlcn_msg));
  nlcn_msg.nl_hdr.nlmsg_len = sizeof(nlcn_msg);
  nlcn_msg.nl_hdr.nlmsg_pid = 0;
  nlcn_msg.nl_hdr.nlmsg_type = NLMSG_DONE;

  nlcn_msg.cn.cn_msg.id.idx = CN_IDX_PROC;
  nlcn_msg.cn.cn_msg.id.val = CN_VAL_PROC;
  nlcn_msg.cn.cn_msg.len = sizeof(enum proc_cn_mcast_op);

  nlcn_msg.cn.cn_mcast = subscribe ? PROC_CN_MCAST_LISTEN : PROC_CN_MCAST_IGNORE;

  if (send(sockFd_.fd, &nlcn_msg, sizeof(nlcn_msg), 0) < 0)
    return -1;

  return 0;
}
