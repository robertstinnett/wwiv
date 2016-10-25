/**************************************************************************/
/*                                                                        */
/*                              WWIV Version 5.0x                         */
/*             Copyright (C)1998-2016, WWIV Software Services             */
/*                                                                        */
/*    Licensed  under the  Apache License, Version  2.0 (the "License");  */
/*    you may not use this  file  except in compliance with the License.  */
/*    You may obtain a copy of the License at                             */
/*                                                                        */
/*                http://www.apache.org/licenses/LICENSE-2.0              */
/*                                                                        */
/*    Unless  required  by  applicable  law  or agreed to  in  writing,   */
/*    software  distributed  under  the  License  is  distributed on an   */
/*    "AS IS"  BASIS, WITHOUT  WARRANTIES  OR  CONDITIONS OF ANY  KIND,   */
/*    either  express  or implied.  See  the  License for  the specific   */
/*    language governing permissions and limitations under the License.   */
/*                                                                        */
/**************************************************************************/
#include <sys/wait.h>
#include <unistd.h>

#include "bbs/bbs.h"
#include "bbs/fcns.h"
#include "bbs/vars.h"
#include "bbs/remote_io.h"

#include "core/log.h"

static int UnixSpawn(const std::string& cmd, char* environ[]) {
  if (cmd.empty()) {
    return 1;
  }
  int pid = fork();
  if (pid == -1) {
    return -1;
  }
  if (pid == 0) {
    const char* argv[4] = { "/bin/sh", "-c", cmd.c_str(), 0 };
    execv("/bin/sh", const_cast<char ** const>(argv));
    exit(127);
  }

  for (;;) {
    int nStatusCode = 1;
    if (waitpid(pid, &nStatusCode, 0) == -1) {
      if (errno != EINTR) {
        return -1;
      }
    } else {
      return nStatusCode;
    }
  }

  // Should never happen.
  return -1;
}

int ExecExternalProgram(const std::string cmdline, int flags) {
  if (flags & EFLAG_FOSSIL) {
    LOG(ERROR) << "EFLAG_FOSSIL is not supported on UNIX";
  }
  if (flags & EFLAG_COMIO) {
    LOG(ERROR) << "EFLAG_COMIO is not supported on UNIX";
  }

  if (ok_modem_stuff) {
    session()->remoteIO()->close(true);
  }

  int i = UnixSpawn(cmdline, NULL);

  // reengage comm stuff
  if (ok_modem_stuff) {
    session()->remoteIO()->open();
  }
  return i;
}


