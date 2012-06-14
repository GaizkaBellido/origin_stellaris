/**
 * @addtogroup demos
 * Test de las comunicaciones Internet mediante el uso de un
 * servidor web y consulta a una página de Internet
 * @{
 ********************************************************************
 * @author		Eñaut Muxika <emuxika at mondragon dot edu>
 * @date		2011/7/10
 * @copyright	BSDL
 ********************************************************************
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>  
#include <ctype.h>
#include <libMU/serie.h>
#include <libMU/botones.h>
#include <libMU/pantalla.h>
#include <libMU/internet.h>
#include <libMU/cronometro.h>
#include <libMU/analogico_digital.h>
#include <libMU/temperatura.h>
#include <libMU/leds.h>
#include "uip.h"

/**
 * Funciones locales
 */
static void		ProgramaPrincipal(void *param);
//static void 	Internet_ProcesamientoDePagina( char* data, unsigned short int len ) ;
static char*	Internet_Procesamiento_control( const char* tag, int* len, char* name );
//static char*	Internet_Procesamiento_eventSub( const char* tag, int* len, char* name );

char response[350];

int power = 1;

/**
 * Variables locales (paginas Web del servidor)
 */
const char device_descriptor[] = 
	"<?xml version=\"1.0\" ?>"
	"<root xmlns=\"urn:schemas-upnp-org:device-1-0\">"
	"	<specVersion>"
	"		<major>1</major> "
	"		<minor>0</minor> "
	"	</specVersion>"
	"	<device>"
	"		<deviceType>urn:schemas-upnp-org:device:light:1</deviceType> "
	"		<friendlyName>CyberGarage Light Device</friendlyName> "
	"		<manufacturer>CyberGarage</manufacturer> "
	"		<manufacturerURL>http://www.cybergarage.org</manufacturerURL>" 
	"		<modelDescription>CyberUPnP Light Device</modelDescription> "
	"		<modelNumber>1.0</modelNumber> "
	"		<modelURL>http://www.cybergarage.org</modelURL> "
	"		<serialNumber>1234567890</serialNumber> "
	"		<UDN>uuid:cybergarageLightDevice</UDN> "
	"		<UPC>DOMOCONEX</UPC> "
	"		<iconList>"
	"			<icon>"
	"				<mimetype>image/gif</mimetype> "
	"				<width>48</width> "
	"				<height>32</height> "
	"				<depth>8</depth> "
	"				<url>icon.gif</url> "
	"			</icon>"
	"		</iconList>"
	"		<serviceList>"
	"			<service>"
	"				<serviceType>urn:schemas-upnp-org:service:power:1</serviceType>" 
	"				<serviceId>urn:schemas-upnp-org:serviceId:power:1</serviceId> "
	"				<SCPDURL>services.xml</SCPDURL>" 
	"				<controlURL>/service/power/control.xml</controlURL> "
	"				<eventSubURL>/service/power/event.xml</eventSubURL> "
	"			</service>"
	"		</serviceList>"
	"		<presentationURL>http://www.cybergarage.org</presentationURL> "
	"	</device>"
	"</root>\r\n\r\n";
	
const char services_descriptor[] = 
	"<?xml version=\"1.0\"?>"
	"<scpd xmlns=\"urn:schemas-upnp-org:service-1-0\" >"
	"	<specVersion>"
	"		<major>1</major>"
	"		<minor>0</minor>"
	"	</specVersion>"
	"	<actionList>"
	"		<action>"
	"			<name>SetPower</name>"
	"			<argumentList>"
	"				<argument>"
	"					<name>Power</name>"
	"					<relatedStateVariable>Power</relatedStateVariable>"
	"					<direction>in</direction>"
	"				</argument>"
	"				<argument>"
	"					<name>Result</name>"
	"					<relatedStateVariable>Result</relatedStateVariable>"
	"					<direction>out</direction>"
	"				</argument>"
	"			</argumentList>"
	"		</action>"
	"		<action>"
	"			<name>GetPower</name>"
	"			<argumentList>"
	"				<argument>"
	"					<name>Power</name>"
	"					<relatedStateVariable>Power</relatedStateVariable>"
	"					<direction>out</direction>"
	"				</argument>"
	"			</argumentList>"
	"		</action>"
	"	</actionList>"
	"	<serviceStateTable>"
	"		<stateVariable sendEvents=\"yes\">"
	"			<name>Power</name>"
	"			<dataType>boolean</dataType>"
	"			<allowedValueList>"
	"				<allowedValue>0</allowedValue>"
	"				<allowedValue>1</allowedValue>"
	"			</allowedValueList>"
	"			<allowedValueRange>"
	"				<maximum>123</maximum>"
	"				<minimum>19</minimum>"
	"				<step>1</step>"
	"			</allowedValueRange>"
	"		</stateVariable>"
	"		<stateVariable sendEvents=\"no\">"
	"			<name>Result</name>"
	"			<dataType>boolean</dataType>"
	"		</stateVariable>"
	"	</serviceStateTable>"
	"</scpd>\r\n\r\n";
	
const char response_set_power[] =
"<?xml version=\"1.0\" encoding=\"utf-8\"?>"
"<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">\r\n"
"	<s:Body>\r\n"
"		<u:SetPowerResponse xmlns:u=\"urn:schemas-upnp-org:service:power:1\">\r\n"
"			<Result>%d</Result>\r\n"
"		</u:SetPowerResponse>\r\n"
"	</s:Body>\r\n"
"</s:Envelope>\r\n";

const char response_get_power[] =
"<?xml version=\"1.0\" encoding=\"utf-8\"?>"
"<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">\r\n"
"	<s:Body>\r\n"
"		<u:GetPowerResponse xmlns:u=\"urn:schemas-upnp-org:service:power:1\">\r\n"
"			<Power>%d</Power>\r\n"
"		</u:GetPowerResponse>\r\n"
"	</s:Body>\r\n"
"</s:Envelope>\r\n";

/**
 * Programa principal
 */
int main(void)
{
	libMU_Display_Initialize();
	libMU_Serial_Initialize(115200);
	libMU_Stopwatch_Initialize();
	libMU_AD_Initialize();
	libMU_LED_Initialize();
	libMU_Button_Initialize();
		
	libMU_Display_DrawString	( "Guten morgen!!", 0, 0, 15 );

	// Esta libreria es diferente, arranca un modo ejecución en paralelo
	// para poder procesar las tramas de internet en paralelo con el programa principal
	// y nunca debería volver
	libMU_Internet_Initialize(ProgramaPrincipal);

	return 0;	// Nunca llega aquí
}

/**
 * Una vez que se inicializa internet el resto del programa sigue aqui
 */
static void ProgramaPrincipal(void *param) {
	IPAddress_t ip; int res = 0;
	char msg[50];
	static const char* mensajes[] = {
		"Esperando IP   ",
		"Esperando IP.  ",
		"Esperando IP.. ",
		"Esperando IP..."
	};

	/* Configuracion del servidor WEB */
	libMU_Internet_Server_SetPage( "/device.xml", (char*)device_descriptor, sizeof(device_descriptor)-1 );  
	libMU_Internet_Server_SetPage( "/services.xml", (char*)services_descriptor, sizeof(services_descriptor) -1 );
	libMU_Internet_Server_SetCommandProcessingInfo( "/service/power/control.xml", Internet_Procesamiento_control );
	
	for(;;) {
		switch( libMU_Internet_GetStatus() ) {
			case NETWORK_IP_MISSING:
				res &= 3; //if( res > 3 ) res = 0;
				libMU_Display_DrawString( mensajes[res++], 0, 0, 15 );
				libMU_Internet_Delay( 250 );
				break;
				
			case NETWORK_IP_OK:
				ip = libMU_Internet_GetDeviceIP();
				snprintf( msg, sizeof(msg), "IP<%d.%d.%d.%d>    ",
							libMU_IP_1( ip ), libMU_IP_2( ip ), 
							libMU_IP_3( ip ), libMU_IP_4( ip ) );
				libMU_Display_DrawString( msg, 0, 0, 12 );

				// Deja solo el servidor web en marcha
				for(;;) {
					libMU_Internet_Delay( 100 );
					// Mostrar
					snprintf( msg, sizeof(msg), "R/S:%d/%d", 
						libMU_Internet_GetNumberOfFramesReceived(), 
						libMU_Internet_GetNumberOfFramesSent() );
					libMU_Display_DrawString( msg, 0, 40, 15 );
					int t = libMU_Temperature_Get(); 
					snprintf( msg, sizeof(msg), "T:%d.%d  ", 
						 (t/10), (t%10) );
					libMU_Display_DrawString( msg, 0, 48, 15 );
					
					snprintf( msg, sizeof(msg), "P:%d, %d/%d C:%d", uip_conns[0].rport, 
							  uip_conns[0].appstate.state,
							  uip_conns[0].appstate.state_post, 
							  uip_conns[0].appstate.count );
					libMU_Display_DrawString( msg, 0, 8, 12 );
					snprintf( msg, sizeof(msg), "P:%d, %d/%d C:%d", uip_conns[1].rport, 
							  uip_conns[1].appstate.state,
							  uip_conns[1].appstate.state_post, 
							  uip_conns[1].appstate.count );
					libMU_Display_DrawString( msg, 0, 16, 12 );
				}
				
			case NETWORK_ERROR:
			default:
				libMU_Display_DrawString( "Error!!!   ", 0, 8, 15 );
				break;
		}		
	}
}


static char* Internet_Procesamiento_control( const char* cmd_param, int* len, char* name )
{
	char * found;
	
	if( cmd_param == NULL) return NULL;

	/* Buscamos u:SetPower */
	found = strstr(cmd_param,"u:SetPower");
	if(found){
		/* Buscamos <Power> */
		found = strstr(found,"<Power>");
		if(found){
				found = found + 7;
				libMU_LED_Toggle(LED_1);
				if(power){
					power = 0;
				}else{
					power = 1;
				}
				sprintf(response, response_set_power, power);
		}
	}

	
	/* Buscamos u:GetPower */
	found = strstr(cmd_param,"u:GetPower");
	if(found){
		sprintf(response, response_get_power, power);
	}

	*len = strlen(response);
	return (char*)response;
}

