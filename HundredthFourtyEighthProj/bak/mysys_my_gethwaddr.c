/* Copyright (C) 2004 MySQL AB

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA */

/* get hardware address for an interface */
/* if there are many available, any non-zero one can be used */

#include "mysys_priv.h"
#include <libmysql_m_string.h>

#ifndef MAIN

#if defined(__FreeBSD__) || defined(__linux__)
static my_bool memcpy_and_test(uchar *to, uchar *from, uint len)
{
  uint i, res=1;

  for (i=0; i < len; i++)
    if ((*to++= *from++))
      res=0;
  return res;
}
#endif   /* FreeBSD || linux */

#ifdef __FreeBSD__

#include <net/ethernet.h>
#include <sys/sysctl.h>
#include <net/route.h>
#include <net/if.h>
#include <net/if_dl.h>

my_bool my_gethwaddr(uchar *to)
{
  size_t len;
  uchar  *buf, *next, *end, *addr;
  struct if_msghdr *ifm;
  struct sockaddr_dl *sdl;
  int i, res=1, mib[6]={CTL_NET, AF_ROUTE, 0, AF_LINK, NET_RT_IFLIST, 0};

  if (sysctl(mib, 6, NULL, &len, NULL, 0) == -1)
    goto err;
  if (!(buf = alloca(len)))
    goto err;
  if (sysctl(mib, 6, buf, &len, NULL, 0) < 0)
    goto err;

  end = buf + len;

  for (next = buf ; res && next < end ; next += ifm->ifm_msglen)
  {
    ifm = (struct if_msghdr *)next;
    if (ifm->ifm_type == RTM_IFINFO)
    {
      sdl = (struct sockaddr_dl *)(ifm + 1);
      addr=LLADDR(sdl);
      res=memcpy_and_test(to, addr, ETHER_ADDR_LEN);
    }
  }

err:
  return res;
}

#elif __linux__

#include <net/if.h>
#include <sys/ioctl.h>
#include <net/ethernet.h>

my_bool my_gethwaddr(uchar *to)
{
  int fd, res=1;
  struct ifreq ifr;

  fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (fd < 0)
    goto err;

  bzero(&ifr, sizeof(ifr));
  strnmov(ifr.ifr_name, "eth0", sizeof(ifr.ifr_name) - 1);

  do {
    if (ioctl(fd, SIOCGIFHWADDR, &ifr) >= 0)
      res=memcpy_and_test(to, (uchar *)&ifr.ifr_hwaddr.sa_data, ETHER_ADDR_LEN);
  } while (res && (errno == 0 || errno == ENODEV) && ifr.ifr_name[3]++ < '6');

  close(fd);
err:
  return res;
}

#elif defined(__WIN__)
#include <iphlpapi.h>

/* 
  The following typedef is for dynamically loading 
  iphlpapi.dll / GetAdaptersAddresses.  Dynamic loading is 
  used because GetAdaptersAddresses is not available on Windows 2000 
  which MySQL still supports.  Static linking would cause an unresolved export.
*/
typedef DWORD (WINAPI *pfnGetAdaptersAddresses)(IN ULONG Family,
    IN DWORD Flags,IN PVOID Reserved,
    OUT PIP_ADAPTER_ADDRESSES pAdapterAddresses, 
    IN OUT PULONG pOutBufLen);

/*
  my_gethwaddr - Windows version  

  @brief Retrieve MAC address from network hardware
 
  @param[out]  to MAC address exactly six bytes
 
  @return Operation status
    @retval 0       OK
    @retval <>0     FAILED  
*/
my_bool my_gethwaddr(uchar *to)
{                       
  PIP_ADAPTER_ADDRESSES pAdapterAddresses;
  PIP_ADAPTER_ADDRESSES pCurrAddresses;
  IP_ADAPTER_ADDRESSES  adapterAddresses;
  ULONG                 address_len;
  my_bool               return_val= 1;     
  static pfnGetAdaptersAddresses fnGetAdaptersAddresses= 
                                (pfnGetAdaptersAddresses)-1; 

  if(fnGetAdaptersAddresses == (pfnGetAdaptersAddresses)-1)
  {
    /* Get the function from the DLL */
    fnGetAdaptersAddresses= (pfnGetAdaptersAddresses)
                            GetProcAddress(LoadLibrary("iphlpapi.dll"),
                                          "GetAdaptersAddresses");
  }
  if (!fnGetAdaptersAddresses)
    return 1;                                   /* failed to get function */
  address_len= sizeof (IP_ADAPTER_ADDRESSES);

  /* Get the required size for the address data. */
  if (fnGetAdaptersAddresses(AF_UNSPEC, 0, 0, &adapterAddresses, &address_len) 
      == ERROR_BUFFER_OVERFLOW)
  {
    pAdapterAddresses= my_malloc(address_len, 0);
    if (!pAdapterAddresses)
      return 1;                                   /* error, alloc failed */
  }
  else
    pAdapterAddresses= &adapterAddresses;         /* one is enough don't alloc */

  /* Get the hardware info. */
  if (fnGetAdaptersAddresses(AF_UNSPEC, 0, 0, pAdapterAddresses, &address_len) 
      == NO_ERROR)
  {
    pCurrAddresses= pAdapterAddresses;

    while (pCurrAddresses) 
    {
      /* Look for ethernet cards. */
      if (pCurrAddresses->IfType == IF_TYPE_ETHERNET_CSMACD)
      {
        /* check for a good address */
        if (pCurrAddresses->PhysicalAddressLength < 6)
            continue;                           /* bad address */          

        /* save 6 bytes of the address in the 'to' parameter */
        memcpy(to, pCurrAddresses->PhysicalAddress, 6);

        /* Network card found, we're done. */
        return_val= 0;
        break;                                  
      }
      pCurrAddresses= pCurrAddresses->Next;
    }  
  }

  /* Clean up memory allocation. */
  if (pAdapterAddresses != &adapterAddresses)
    my_free(pAdapterAddresses, 0);

  return return_val;
}

#else /* __FreeBSD__ || __linux__ || __WIN__ */
/* just fail */
my_bool my_gethwaddr(uchar *to __attribute__((unused)))
{
  return 1;
}
#endif

#else /* MAIN */
int main(int argc __attribute__((unused)),char **argv)
{
  uchar mac[6];
  uint i;
  MY_INIT(argv[0]);
  if (my_gethwaddr(mac))
  {
    printf("my_gethwaddr failed with errno %d\n", errno);
    exit(1);
  }
  for (i=0; i < sizeof(mac); i++)
  {
    if (i) printf(":");
    printf("%02x", mac[i]);
  }
  printf("\n");
  return 0;
}
#endif

