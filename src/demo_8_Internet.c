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
static void 	Internet_ProcesamientoDePagina( char* data, unsigned short int len ) ;
static char*	Internet_Procesamiento_control( const char* tag, int* len, char* name );
static char*	Internet_Procesamiento_eventSub( const char* tag, int* len, char* name );

/**
 * Variables locales (datos de la página leída)
 */
static int hora;
static int minutos;
static int bytes_recibidos;

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
	"				<controlURL>/service/power/control</controlURL> "
	"				<eventSubURL>/service/power/eventSub</eventSubURL> "
	"			</service>"
	"		</serviceList>"
	"		<presentationURL>http://www.cybergarage.org</presentationURL> "
	"	</device>"
	"</root>";
	
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
	"</scpd>";

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
static void ProgramaPrincipal(void *param)
{
	libMU_Stopwatch_Time_t t;
	IPAddress_t ip; int res = 0;
	char msg[50];
	static const char* mensajes[] = {
		"Esperando IP   ",
		"Esperando IP.  ",
		"Esperando IP.. ",
		"Esperando IP..."
	};

	/* Configuración del servidor WEB */
	libMU_Internet_Server_SetPage( "/device.xml", (char*)device_descriptor, sizeof(device_descriptor) );  
	libMU_Internet_Server_SetPage( "/services.xml", (char*)services_descriptor, sizeof(services_descriptor) );
	libMU_Internet_Server_SetCommandProcessingInfo( "/service/power/control", Internet_Procesamiento_control );
	libMU_Internet_Server_SetCommandProcessingInfo( "/service/power/eventSub", Internet_Procesamiento_eventSub );
	
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
//				ip = libMU_Internet_GetWiFiRouterIP();
//				if( ip != 0 ) {
//					snprintf( msg, sizeof(msg), "WF<%d.%d.%d.%d>    ",
//								libMU_IP_1( ip ), libMU_IP_2( ip ), 
//								libMU_IP_3( ip ), libMU_IP_4( ip ) );
//					libMU_Display_DrawString( msg, 0, 0, 15 );
//				}
//				libMU_Internet_DNS_resolution( "www.worldtimeserver.com", &ip, 10000 );
//				libMU_Display_DrawString( "worldtimeserver.com", 0, 8, 12 );
//				snprintf( msg, sizeof(msg), "  <%d.%d.%d.%d>",
//							libMU_IP_1( ip ), libMU_IP_2( ip ), 
//							libMU_IP_3( ip ), libMU_IP_4( ip ) );
//				libMU_Display_DrawString( msg, 0, 16, 15 );
//				libMU_Internet_Delay( 1000 );
//				res = libMU_Internet_GetPage ( 
//							"http://www.worldtimeserver.com/mobile/mobile_time_in_ES.aspx?city=Madrid",
//							Internet_ProcesamientoDePagina	/* Función llamada por cada trozo de página recibida */
//						);
//				if( !res ) {
//					libMU_Display_DrawString( "Error de conexion", 0, 24, 15 );
//					return;
//				}
//				libMU_Stopwatch_Start(&t);
//				while( !libMU_Internet_IsPageReadingFinished() ) {
//					snprintf( msg, sizeof(msg), "Bytes rcvd:% 6d", bytes_recibidos );
//					libMU_Display_DrawString( msg, 0, 24, 15 );
//					libMU_Internet_Delay( 100 );
//				}
//				snprintf( msg, sizeof(msg), "Hora = %d:%02d", hora, minutos );
//				libMU_Display_DrawString( msg, 0, 32, 15 );
//				
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


/**
 * Función para el tratamiento de formularios con metodo (GET/POST)
 * @param	cmd_param	Parametros del comando GET (empezando desde el caracter '?')
 * @param	len			Puntero a un entero donde se guarda la longitud de datos de la respuesta
 * @return				Puntero al texto con el que tiene que responder el servidor web
 * @note
 * - La longitud máxima del comando GET/POST es de 100 bytes
 * - Si se devuelve NULL el servidor no responde nada
 * @see libMU_Internet_Server_SetCommandProcessingInfo()
 */
static char* Internet_Procesamiento_control( const char* cmd_param, int* len, char* name )
{
	char * found;
	char params_copy[100];
	
	memcpy( params_copy, cmd_param, 21 ); 
	params_copy[21] = 0;
	libMU_Display_DrawString( params_copy, 0, 81, 10 );
	
	//length = *len;	
	//memcpy(params_copy, cmd_param, length);
	//libMU_Serial_SendString(cmd_param);
	//libMU_Serial_SendString("\r\n");
	//libMU_Serial_SendString(params_copy);
	/* Debug info */

	//libMU_Serial_SendString(cmd_param);
	/* Incorrectly formated cmd_param */
	//if( cmd_param == NULL || cmd_param[0] != '?' ) return NULL;

	/* Process cmd_param (NOTE: Case is NOT ignored)*/
/*	
	POST /service/power/control HTTP/1.1
	Content-Type: text/xml; charset="utf-8"
	HOST: 172.17.100.131:4004
	Content-Length: 304
	SOAPACTION: "urn:schemas-upnp-org:service:power:1#SetPower"
	Connection: close
	
	<?xml version="1.0" encoding="utf-8"?>
	<s:Envelope xmlns:s="http://schemas.xmlsoap.org/soap/envelope/" s:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/">
		<s:Body>
			<u:SetPower xmlns:u="urn:schemas-upnp-org:service:power:1">
				<Power>0</Power>
			</u:SetPower>
		</s:Body>
	</s:Envelope>
*/	


	/* Buscamos u:SetPower */
	found = strstr(cmd_param,"u:SetPower");
	if(found){
		/* Buscamos <Power> */
		found = strstr(found,"<Power>");
		if(found){
				found = found + 7;
//				libMU_Serial_SendString("Set Encontrado: ");
//				libMU_Serial_SendChar(found-1);
//				libMU_Serial_SendChar(found);
//				libMU_Serial_SendChar(found+1);
				libMU_LED_Toggle(LED_1);
				found[0] = '0';
		}
	}

	
	/* Buscamos u:SetPower */
	found = strstr(cmd_param,"u:GetPower");
	if(found){
		/* Buscamos <Power> */
		found = strstr(cmd_param,"<Power>");
		if(found){
				found = found + 5;
//				libMU_Serial_SendString("Get Encontrado: ");
//				libMU_Serial_SendChar(found-1);
//				libMU_Serial_SendChar(found);
//				libMU_Serial_SendChar(found+1);
		}	
	}

	*len = strlen(cmd_param);
	return (char*)cmd_param;
}

/**
 * Función para el tratamiento de formularios con metodo (GET/POST)
 * @param	cmd_param	Parametros del comando GET (empezando desde el caracter '?')
 * @param	len			Puntero a un entero donde se guarda la longitud de datos de la respuesta
 * @return				Puntero al texto con el que tiene que responder el servidor web
 * @note
 * - La longitud máxima del comando GET/POST es de 100 bytes
 * - Si se devuelve NULL el servidor no responde nada
 * @see libMU_Internet_Server_SetCommandProcessingInfo()
 */
static char* Internet_Procesamiento_eventSub( const char* cmd_param, int* len, char* name )
{
	/* Incorrectly formated cmd_param */
	if( cmd_param == NULL ) return NULL;

	*len = sizeof(services_descriptor);
	return (char*)services_descriptor;
}



/**
 * Función para el tratamiento de datos descargados de Internet.
 * @param	data	Puntero a los datos descargados
 * @param	len		Tamaño de los datos descargados
 * @note
 * Debido a limitaciones de memoria, la página descargada no se almacena y hay
 * que procesarla según llegan los datos.
 * Esta función se llama varias veces por cada página excepto para aquellas
 * páginas que tengan un tamaño menor de 1kB (aprox.).
 * @see libMU_Internet_GetPage()
 */
static void Internet_ProcesamientoDePagina( char* data, unsigned short int len ) 
{
	bytes_recibidos += len;
	data[len-1] = 0;
	
	// Find specific string in the received packet
	char *loc = strstr(data, "\"font7\"" );
	if( loc == NULL ) return;
	
	loc += 7; char *end = data + len;
	// Skip everything until we find digits
	while( !isdigit( *loc ) && loc < end ) loc++;
	// Convert hours to number
	hora = atoi( loc );
	// Skip digits 
	loc++; while( isdigit( *loc ) && loc < end ) loc++;
	// check if ':' present
	if( loc[0] != ':' ) { hora = 0; return; } 
	loc++;
	// Convert minutes to number
	minutos = atoi( loc );
}

/**
 * @}
 */
