#ifndef _SSDP_H_
#define _SSDP_H_


//----- DEFINES -----
#define SSDP_PORT                      (1900)
#define SSDP_MULTICAST                 ((239UL<<0)|(255UL<<8)|(255UL<<16)|(250UL<<24))
#define ETH_HEADERLEN                  (14)

//Proto: SSDP (Simple Service Discovery Protocol)
#define SSDP_OFFSET                    (ETH_HEADERLEN+IP_HEADERLEN+UDP_HEADERLEN)


//----- PROTOTYPES -----
void									ssdp_advertise(void);
void									ssdp_udpapp(void);
void									ssdp_udpinit(void);


#endif //_SSDP_H_
