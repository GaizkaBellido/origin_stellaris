#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <libMU/serie.h>
#include <libMU/pantalla.h>
#include "ssdp.h"
#include "uip.h"


struct uip_udp_conn *upd_conex;
unsigned int		Contador_Envio_SSDP;

void ssdp_udpinit()
{
	uip_ipaddr_t addr;
	uip_ipaddr(&addr,239,255,255,250);
	upd_conex = uip_udp_new(&addr, HTONS(SSDP_PORT));
  	
  	if(upd_conex != NULL) {
		uip_udp_bind(upd_conex, HTONS(SSDP_PORT));
	}else{
		libMU_Serial_SendString("Error0: imposible abrir conexion!");
  	}
	
	Contador_Envio_SSDP = 0;
	
	return;
}

void ssdp_udpapp()
{
	char *ssdp;
	
	if(uip_newdata()) {
		//libMU_Serial_SendString("Evento uip_newdata()\n");
		ssdp = (char*) uip_appdata;
		if(strncmp(ssdp, "M-SEARCH", 8) == 0) {
			libMU_Display_DrawString("M-SEARCH *",10,10,15);
			ssdp_advertise();
		}
	}
	if( uip_poll() ) {
		//libMU_Serial_SendString("Evento uip_poll()\n");
		//ssdp_advertise();
		Contador_Envio_SSDP++;	// Cada 500ms
		if( Contador_Envio_SSDP == 20 ) {
			Contador_Envio_SSDP = 0;
			ssdp_advertise();
		}
	}
}

void ssdp_advertise(void)
{
	unsigned int len;
	char ssdp[250];

	//ssdp = (char *) &uip_appdata;
  	len = sprintf(ssdp, "NOTIFY * HTTP/1.0\r\n"
				"Host: %i.%i.%i.%i:%i\r\n"
				"Cach-Control: max-age=900\r\n"
				"Location: http://%i.%i.%i.%i/device.xml\r\n"
				"NT: upnp:rootdevice\r\n"
				"USN: uuid:%s::upnp:rootdevice\r\n"
				"NTS: ssdp:alive\r\n"
				"Server: SMART TV 1.0 UPnP/1.0 \r\n\r\n",
				((SSDP_MULTICAST>>0)&0xff),
				((SSDP_MULTICAST>>8)&0xff),
				((SSDP_MULTICAST>>16)&0xff),
				((SSDP_MULTICAST>>24)&0xff),
				SSDP_PORT,
				((uip_hostaddr[0]>>0)&0xff),
				((uip_hostaddr[0]>>8)&0xff),
				((uip_hostaddr[1]>>0)&0xff),
				((uip_hostaddr[1]>>8)&0xff),
				"06227070-4286-0000-00");
               
	if(upd_conex != NULL) {
		//libMU_Serial_SendString("\nEnviando ssdp_advertise()\n");
		//libMU_Serial_SendString(ssdp);
		uip_send(ssdp, len);
	}else{
		//libMU_Serial_SendString("Error1: imposible abrir conexion!\n");
	}
	return;
}
