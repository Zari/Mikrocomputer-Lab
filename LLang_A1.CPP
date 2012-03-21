/* WS2011 Aufgabe 1: BIOS, & CMOS		TEL09BAT		Leo Lang

Version 1.3 - Stand: 15.11.2011 (final-3)

Erster Aufgabensatz:
Bestimmen sie die Art des Rechners						- Done
Geben sie das Datum des BIOS aus						- Done
Sind laut BPB LED's auf der Tastatur gesetzt? Wenn ja welche?			- Done
Lesen sie die Uhrzeit aus dem CMOS Ram aus!					- Done
Geben sie die Uhrzeit aus dem 32Bit Zaehler des BPB aus (Fortlaufend hh:mm:ss)	- Done
*/

//Benötigte Libarys/Header
#include <dos.h>
#include <stdio.h>
#include <conio.h>

int main(){		//Einsprungpunkt bei Programmstart
	clrscr();	//Sauberer Bildschirm zum Start

	printf("Programm vom Leo Lang, \t TEL09BAT\n\n");

{	//Ermittlung des PC Types
	unsigned char x = peekb(0xF000, 0xFFFE);
	printf("-------------------------------------------------\n");
	printf("Der PC Typ ist: ");
	switch ( x ){
		case 0xFC:
			printf("AT-Kompatibler PC\n");
			break;
		case 0xDFD:
			printf("PC-Junior\n");
			break;
		case 0xFE:
			printf("Tragbarer PC oder XT\n");
			break;
		case 0xFF:
			printf("Einfacher PC\n");
			break;
		default:
			printf("Unbekannter Typ: %x\n", x);
	}//switch
}
{	//Datum des BIOS
	unsigned short i;
	printf("\n\n-------------------------------------------------\nDas BIOS Datum ist (Amerikanisches Format): ");
	for(i=0xFFF5; i<= 0xFFFC; i++)	//8 Zeichen in folge ausgeben / sind im Speicher
		printf("%c", peekb(0xF000, i));
	printf("\n");
}
{	//LED Status
	unsigned char y = peekb(0x0040, 0x0017);	//1 Byte aus Speicher
	printf("\n\n-------------------------------------------------\nZustand der LEDs auf der Tastatur: \n");

	if(y & 0x40)		//Auf Tastenbits testen
		printf("CAPS-Lock an\n");
	if(y & 0x20)
		printf("NUM-Lock an\n");
	if(y & 0X10)
		printf("SCROLL-Lock an\n");
}
{	//Zeit aus CMOS
	unsigned short i;
	unsigned char Bit7;
	printf("\n-------------------------------------------------\nCMOS Zeit: ");
	_disable();	//IRQ aus
	for(i=4; i<=4; i-=2){			//Gehe die einzelnen Elemente durch Läuft: i=4, i=2, i=0
		Bit7 = inportb(0x70) & 0x80;	//Rettet das 7. Bit vom Port 70 -> Siehe Datenblatt
		outportb(0x70, (i | Bit7));
		printf("%x:", inportb(0x71));	//Ausgabe
	}
	printf("\b \n\n");
	_enable();	//IRQ an
}
{	//Ausgabe der BIOS Zeit
	unsigned long zeit, zeit2;
	printf("\n-------------------------------------------------\n");
	while (!kbhit()) {  //Endlosschaleife bis kbhit
		//Nach allen Operationen werden mögliche Fehlerquellen ausgeschlossen (siehe z.B. & 0x0000FFFF)
		zeit = (( (unsigned long)peek(0x0040, 0x006E) << 16 ) & 0xFFFF0000) +
					 ( (unsigned long)peek(0x0040, 0x006C) & 0x0000FFFF ); //2x16Bit aus Speicher Masken da bei peek in long ungenutzte Bits 1 werden.
		zeit /= 18.2064819336;		//Zeit in Sekunden umwandeln
		printf("BIOS Zeit: %02lu:%02lu:%02lu\r", (zeit / 3600) % 24, (zeit / 60) % 60, zeit % 60);	//Ausgabe, getrennt nach hh:mm:ss
		delay(200); //Verzögere - mehr als 5 Zeitaktualisierungen pro Sekunde sind unnötig
	}
}//Ende Bios Zeit

return 0; //Beenden, keine Fehler
}
