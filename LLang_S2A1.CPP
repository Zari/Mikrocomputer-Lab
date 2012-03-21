/* Leo Lang 		TEL09BAT		5.3.2012

Ursprünglich: Darstellen eines X-es auf dem Display (wird nicht angezeigt, siehe erster Codeabschnitt)

Dann: Kreis nach brute Force Methode (aukommentiert

Kreidarstellung mit Sin und Cos.

Weitere Aufgabe:
Bild zyklisch anzeigen und Phase dabei erhöhen. (Bei einer Phasenverschiebung sollte das Bild daraufhin wandern.
Steuerung der Geschwindigkeit soll durch die Tasten + und - möglich sein. Verlassen mit q
*/

#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include <math.h>
#include <iostream.h>

int Kreis(double radius, double time, double x_freq, double y_freq, double x_phase, double y_phase, char zeichen);

int main(){
	unsigned int x;
	unsigned int y, z;
	double m;
	unsigned int spalten, zeilen;
	float r=0;

	//Starte mit sauberem Bildschirm
	clrscr();

	//Ermittle die Fenstergröße
	spalten = peekb(0x40, 0x4A);
	zeilen = peek(0x40, 0x4C)/(2*spalten);

	//Kreuz
	m = (double)zeilen / (double) spalten;
	for(x=0; x<=spalten; x++){
		y = (unsigned int)((double)x*( m ));
		pokeb(0xb800, 1+2*(x+y*spalten), 0x17);
		z = (unsigned int)((double)x* (-m)  +(double)zeilen);
		pokeb(0xb800, 1+2*(x+z*spalten), 0x17);
	}

/*	//Kreis  nach Brute Force
	int x2, y2;
	for(y2=0; y2 <= zeilen; y2++){
		for(x2=0; x2 <= spalten; x2++){
			zw = sqrt(pow(x2-((double)spalten/2),2)+pow(y2-((double)zeilen/2),2));
			if( zw <= 11.4 ){
				if( zw >= 8.0){
					pokeb(0xb800, 2*(x2+y2*spalten), '0');
				}
			}
		}
	}*/
	clrscr();

	//Variable zur Steuerung
	r = 12;
	const int waittime= 10;
	double timestep = 2*3.14;
	double t=0.0;
	char eingabe;
	short abbruch=0;
	double x_phase=3.14/4, y_phase=0;
	double x_freq=1, y_freq=1.01;
		 do{
			//Zeichnet die Lissajon Figuren

			if(kbhit()){
				eingabe = getch();
				switch (eingabe){
					case 'q':
						abbruch = 1;
					break;
					case '+':
						timestep *= 2;
					break;
					case '-':
						timestep /= 2;
					break;
					case 'w':
						x_freq *= 2;
					break;
					case 'e':
						x_freq /= 2;
				}
			}

			Kreis(r, t, x_freq, y_freq, x_phase, y_phase, '#');
			delay(waittime);	//Verzoegere das Neuzeichnen
			Kreis(r, t, x_freq, y_freq, x_phase, y_phase, ' ');
			t += timestep;			//Naechster Zeitschritt
		} while(abbruch==0);
	return 0;
}

//Funktion die einen Kreis mit den angegebenen parametern zeichnet
//Rückgabe 0 wenn im Bild
int Kreis(double radius, double startphi, double x_freq, double y_freq, double x_phase, double y_phase, char zeichen){
	unsigned int spalten, zeilen;
	int x, y;
	spalten = peekb(0x40, 0x4A);
	zeilen = peek(0x40, 0x4C)/(2*spalten);

	if(radius > spalten || radius > zeilen)
		return 1;
	for(double phi=startphi; phi < startphi + 2*3.14; phi+=0.02){
		x = (int)(radius * cos(phi * x_freq + x_phase)) + spalten/2;
		y = (int)(radius * sin(phi * y_freq + y_phase)) + zeilen/2;
		pokeb(0xb800, 2*(x+y*spalten), zeichen);
	}
	return 0;
}