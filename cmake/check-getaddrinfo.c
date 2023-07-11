#include <stdio.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(void)
{
  int passive, gaierr, inet4 = 0, inet6 = 0;
  struct addrinfo hints, *ai, *aitop;
  char straddr[INET6_ADDRSTRLEN], strport[16];

  for (passive = 0; passive <= 1; passive++) {
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_flags = passive ? AI_PASSIVE : 0;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    if ((gaierr = getaddrinfo(NULL, "54321", &hints, &aitop)) != 0) {
      (void)gai_strerror(gaierr);
      goto bad;
    }
    for (ai = aitop; ai; ai = ai->ai_next) {
      if (ai->ai_addr == NULL ||
          ai->ai_addrlen == 0 ||
          getnameinfo(ai->ai_addr, ai->ai_addrlen,
                      straddr, sizeof(straddr), strport, sizeof(strport),
                      NI_NUMERICHOST|NI_NUMERICSERV) != 0) {
        goto bad;
      }
      switch (ai->ai_family) {
      case AF_INET:
        if (strcmp(strport, "54321") != 0) {
          goto bad;
        }
        if (passive) {
          if (strcmp(straddr, "0.0.0.0") != 0) {
            goto bad;
          }
        } else {
          if (strcmp(straddr, "127.0.0.1") != 0) {
            goto bad;
          }
        }
        inet4++;
        break;
      case AF_INET6:
        if (strcmp(strport, "54321") != 0) {
          goto bad;
        }
        if (passive) {
          if (strcmp(straddr, "::") != 0) {
            goto bad;
          }
        } else {
          if (strcmp(straddr, "::1") != 0) {
            goto bad;
          }
        }
        inet6++;
        break;
      case AF_UNSPEC:
        goto bad;
        break;
      default:
        /* another family support? */
        break;
      }
    }
    freeaddrinfo(aitop);
    aitop = NULL;
  }

  if (!(inet4 == 0 || inet4 == 2))
    goto bad;
  if (!(inet6 == 0 || inet6 == 2))
    goto bad;

  if (aitop)
    freeaddrinfo(aitop);
  return 0;

 bad:
  if (aitop)
    freeaddrinfo(aitop);
  return 1;
}
