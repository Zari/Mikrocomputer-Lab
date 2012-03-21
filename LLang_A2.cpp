/* Leo Lang		TEL09BAT		Aufgabe 2 inkl. Laborvorlesung
Version 1.3 -19.11.2011

- PI mit Waage:  Quadrat wiegt: 3,12g  Kreis wiegt 2,48g - Kante = Durchmesser
- PI via Monte Carlo mit srand, random, randomize

1. PI berechnen
2. BIOS extrensions
3. Konfiguration des Rechners mit Int86(0x11)
4. Ausgabe der Uhrzeit mit BPB, CMOS Ram, BIOS Call, DOS Call
5. Ist eine Maus vorhanden, aktivieren	S20
	Mausspur darstellen
	Koordinaten laufend ausgeben
	Abbruch, wenn beide Tasten gleichzeitig gedrckt werden

	Interuputs mit  int86(0x11, &cpu, &cpu)
	&cpu sind Union Struct komplexe

	Seite 17 im Skript
	zus. Seite 19 .3   S22

	IRQ soll im Weiten Interrupt oder Interruptanforderung abkürzen
*/
#include <time.h>	//für Ramdom Startwert
#include <conio.h>	//für kbhit()
#include <math.h>	//Für sqrt
#include <stdio.h>	//Für Ein-/Ausgabe
#include <stdlib.h>	//Für Random
#include <dos.h>	//Für Interrupts/Speicher

#define RMAX 32767
#define STEPS 429496

//Typendefinitionen
enum Fehlercodes {OK=0, NOK=1, NotImplemented=2};

//Funktionsprototypen
enum Fehlercodes color_screen(unsigned int, unsigned int);

//Glob Variablen
unsigned int last_x=0, last_y=0, x_grenze;
const unsigned int y_grenze=25;	//Glob Variablen als ersatz für ein Static Element

int main(){
	double x, y, PI;
	unsigned long i, inKreis=0;
	unsigned short j;
	unsigned char Bit7;
	union REGS cpu_reg;
	
	clrscr();			//Alles vom Bild entfernen;
	randomize();		//Zufallsgenerator initialisieren

	printf("Aufgabe 2\t\tLeo Lang \t\tTEL09BAT");

//Berechnung von PI nach Monte Carlo
	for(i=0; i < STEPS; i++) {
		x = ((double) random(RMAX)) / (double) (RMAX-1);
		y = ((double) random(RMAX)) / (double) (RMAX-1);
		x = sqrt(x*x + y*y);
		if(x <= 1.0)	//Wenn im Kreis, dann zähle hoch
			inKreis++;
		if(x == 1.0)	//Zeige Treffer des Randes (ca. 1 Event unter 4 Millionen)
			printf("=1.0 Event\n");
	}
	PI = (4 *( (double)inKreis))/i;		//Errechne PI mit den gesammelten Daten
	printf("PI = %lf\nGenauigkeit: %lf%% \n\n", PI, 100-(100*PI/M_PI)); //Ausgabe und Ermittlung von Abweichung

//BIOS Extensions
	for(i=0xC000; i<=0xF5FF; i += 0x0080 ){	//In 2kb Schritten durchgehen -> 0x80 Schritte im Segment
		if(peek(i,0x0000 ) == 0xAA55)
			printf("Gefunden: Lage: 0x%lx \t Groese: %lu\n",i , 512 * (unsigned long)peekb(i, 0x0002));
	}

//Konfiguration des Rechners
	//Reuse von i
	int86(0x11, &cpu_reg, &cpu_reg);	//Interrupt ausführen, keine Parameter
	if(cpu_reg.x.ax & 0x0001)		//Diskettenlaufwerk
		//Ausgabe der Anzahl durch Maskieren und verschieben der signifikaten Bits nach rechts	
		printf("Anzahl der Diskettenlaufwerke: %1u\n", 1+((cpu_reg.x.ax & 0x00C0) >> 6)); 
	if(cpu_reg.x.ax & 0x0002)
		printf("Ein Co-Prozessor ist vorhanden.\n");
	switch(cpu_reg.x.ax & 0x0030){
	case 0x0010:	//40x25 Color
		x_grenze = 40;
		printf("Videomodus: 40x25 Color\n");
		break;
	case 0x0020:	//80x25 Color
		x_grenze = 80;
		printf("Videomodus: 80x25 Color\n");
		break;
	case 0x0030:	//80x25 Monochrom
		x_grenze = 80;
		printf("Videomodus: 80x25 Monochrom\n");
		break;
	default:
		printf("Ungültiger Eintrag\n");
	}
	printf("Der Computer besitzt %u RS232C Schnittstellen\n", (cpu_reg.x.ax & 0x0E00) >> 9);
	printf("Der Computer besitzt %u parallele Schnittstellen\n\n", (cpu_reg.x.ax & 0xC000) >> 14);

//Ausgabe der Uhrzeit mit BPB
	//Reuse of i
	i = (( (unsigned long)peek(0x0040, 0x006E) << 16 ) & 0xFFFF0000) +
				( (unsigned long)peek(0x0040, 0x006C) & 0x0000FFFF ); //2x16Bit aus Speicher Masken da bei peek in long ungenutzte Bits 1 werden.
	i /= 18.2064819336;		//Zeit in Sekunden umwandeln
	printf("BIOS Zeit (BPB): %02lu:%02lu:%02lu\n\n", (i / 3600) % 24, (i / 60) % 60, i % 60);	//Ausgabe, getrennt nach hh:mm:ss
	
//Zeit aus CMOS
	printf("CMOS Zeit: ");
	_disable();	//IRQ aus
	for(j=4; j<=4; j-=2){			//Gehe die einzelnen Elemente durch Läuft: i=4, i=2, i=0
		Bit7 = inportb(0x70) & 0x80;	//Rettet das 7. Bit vom Port 70 -> Siehe Datenblatt
		outportb(0x70, (j | Bit7));		//Fordert an
		printf("%x:", inportb(0x71));	//Ausgabe
	}
	printf("\b \n\n");
	_enable();	//IRQ an

//Zeit aus BIOS (Interrupt)
	//reuse of cpu_reg
	cpu_reg.h.ah = 0x02;	//Auswählen der Richtigen IRQ Funktion -> AH=0x1A: Zeit
	int86(0x1A, &cpu_reg, &cpu_reg);	//IRQ ausführen
	printf("BIOS Interrupt Zeit: %02x:%02x:%02x\n", cpu_reg.h.ch, cpu_reg.h.cl, cpu_reg.h.dh); //Zeit ausgeben
	if(cpu_reg.x.flags & 1)	//Batterie NOK
		printf("Die Batterie ist leer, daher ist die Genauigkeit der Uhr nicht gesichert.\n");

//Zeit aus DOS Interrupt
	cpu_reg.h.ah = 0x2C;	//Auswählen der Richtigen IRQ Funktion -> AH=0x2C: Zeit
	int86(0x21, &cpu_reg, &cpu_reg);	//IRQ ausführen
	printf("DOS Interrupt Zeit: %02d:%02d:%02d.%02d\n", cpu_reg.h.ch, cpu_reg.h.cl, cpu_reg.h.dh, cpu_reg.h.dl);

//Mausfunktionen (Aufgabenteil 5)
	//Reuse of cpu_reg
	cpu_reg.x.ax = 0x00;	//IRQ vorbereiten
	int86(0x33, &cpu_reg, &cpu_reg);	//Maus Status abfragen
	if(cpu_reg.x.ax == 0)
		printf("Keine Maus am PC vorhanden\n");
	else {
		printf("Die Maus besitzt %u Tasten.\n", cpu_reg.x.bx);
		cpu_reg.x.ax=0x01;	//Vorbereiten zum Maus aktivieren
		int86(0x33, &cpu_reg, &cpu_reg); //Maus aktivieren
		
		cpu_reg.x.ax = 0x03;	//IRQ Mausstatus vorbereiten
		int86(0x33, &cpu_reg, &cpu_reg); //IRQ	- Vor Schleife, damit Laufbed. erfüllt 

		while((cpu_reg.x.bx & 0x03)!= 0x03) { //Solange nicht beide tasten Gedrückt
			printf("Mauskoordinaten: X: %0u \tY:%0u      \r", cpu_reg.x.cx, cpu_reg.x.dx);	//Ausgabe der Mauskoordinaten
			color_screen(cpu_reg.x.cx, cpu_reg.x.dx);
			//Bereite nächsten Durchlauf vor
			cpu_reg.x.ax = 0x03;	//IRQ Mausstatus vorbereiten
			int86(0x33, &cpu_reg, &cpu_reg);	//IRQ
		}//while
	}//else
	return 0;	//Beenden
}//main

//Funktionsdefinition "color screen" Färbt eine übergebene Kachel.
enum Fehlercodes color_screen(unsigned int x, unsigned int y){	//Übergabe: Pos x,y
	x /= 8;		//Bilde Kachel ab.
	y /= 8; 
	if(x > (x_grenze*8) || y > (y_grenze*8))	//Auserhalb des Bereichs
		return NOK;								//NOK beenden, führt dazu das nichts passiert
	if(x != last_x || y != last_y){				//Nur wenn sich was geändert hat
		pokeb(0xB800, 1+ 2*(x + x_grenze*y), 0x17 );	//neues Färben
		pokeb(0xB800, 1+ 2*(last_x + x_grenze * last_y), 0x07 );	//Letztes Zurückfärben
		last_x = x;
		last_y = y;
	}
	return OK;
};