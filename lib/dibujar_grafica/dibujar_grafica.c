#include "stdio.h"

#include "dibujar_grafica.h"

const int anchurabarra = 5;
const int ypantallalimite = 87;

const unsigned char g_ucBarra[60] = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0xff, 0xff, 0xf0, 0x00, 0x00, 0x00
};

void dibujar_estadistica (unsigned long pesoActual){

	static int i = 0, contbarras = 0;
	int cont = 0, ylimite = 0, y_lim_pant;
	char str[20];

	y_lim_pant = ypantallalimite;
	ylimite = ypantallalimite - pesoActual + 20;
	if (contbarras > 18){ //más de 19 barras enteras no muestra bien
			RIT128x96x4Clear();
			contbarras = 0;
			i = 0;
			RIT128x96x4StringDraw("Mostrando peso:", 3, 5, 15);
			sprintf(str, "%lu", pesoActual);
			RIT128x96x4StringDraw(str, 95, 5, 15);
	}
	for (cont = ypantallalimite; cont >= ylimite; cont--){
		RIT128x96x4ImageDraw(g_ucBarra, anchurabarra + i, y_lim_pant--, 12, 10);
	}
	i = i + 6; //hacemos +6 porque la barra ocupa 5 pixeles y para que deje 1 pixel entre barras
	contbarras++;
}
