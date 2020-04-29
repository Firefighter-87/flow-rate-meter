//sensore di flusso aria con moduli ultrasuoni HC RS04
const int triggerPort1 = 7;
const int echoPort1 = 8;
const int triggerPort2 = 9;
const int echoPort2 = 10;
bool calibrato = false;
unsigned long t1, t2, d1, d2, D1T2, D2T1;
float angleDeg = 45;
float angleRad = (angleDeg * 71) / 4068.0;
const float coeffVel = (1/(2*cos(angleRad))); //parte costante della formula della velocità
const float ro = 1.184; //kg/m^3 a 20 gradi celsius
const float internalR = ((21+21.7)/2)/2; //valore medio misurato col calibro in mm
const float superficie = internalR*internalR*PI/1000000;//da mm^2 a m^2
float VF, spinta, flusso;
 
void setup() {
pinMode(triggerPort1, OUTPUT);
pinMode(echoPort1, INPUT);
pinMode(triggerPort2, OUTPUT);
pinMode(echoPort2, INPUT);
Serial.begin(9600);
Serial.println( "flusso istantaneo");
Serial.print("sezione condotta area in cm^2 :");
Serial.println(superficie*10000, 6); //trasformo in cm^2 e tengo 6 cifre significative
//Serial.print("sezione condotta area in m^2 :");
//Serial.println(superficie, 6); //trasformo in cm^2 e tengo 6 cifre significative
}

void loop() {
  if (!calibrato){
    Serial.println("calibrazione in corso, non soffiare nel tubo");
    //porta bassa l'uscita del trigger1
    digitalWrite( triggerPort1, LOW );
    //invia un impulso di 10microsec su trigger1
    digitalWrite( triggerPort1, HIGH );
    delayMicroseconds( 10 );
    digitalWrite( triggerPort1, LOW );
    t1 = pulseIn( echoPort1, HIGH ); //rilevo i microsecondi...
    digitalWrite( echoPort1, LOW );
    d1 = 346 * t1; //in micrometri (346 sono i metri percorsi dal suono in 1 secondo a 25°C; t1 è espresso in microsecondi). Dovremo poi dividere per 1.000.000 per passare da micrometri a metri nel risultato finale.
    // lo facciamo nel risultato finale per non perdere fin da ora cifre significative usando subito un float (che ha solo 7/8 cifre significative)
    
    delay(200);
    
    //porta bassa l'uscita del trigger2
    digitalWrite( triggerPort2, LOW );
    //invia un impulso di 10microsec su trigger1
    digitalWrite( triggerPort2, HIGH );
    delayMicroseconds( 10 );
    digitalWrite( triggerPort2, LOW );
    t2 = pulseIn( echoPort2, HIGH ); //rilevo i microsecondi...
    digitalWrite( echoPort2, LOW );
    d2 = 346 * t2; //anche qui dovremo dividere per 1.000.000 per passare da micrometri a metri nel risultato finale
    Serial.println("calibrazione terminata, adesso è possibile soffiare nel tubo");
    calibrato = true;
  }
//Serial.print("coeffVel: ");
//Serial.println(coeffVel);

//Serial.println(d1);
//Serial.println(d2);

//porta bassa l'uscita del trigger1
digitalWrite( triggerPort1, LOW );
//invia un impulso di 10microsec su trigger1
digitalWrite( triggerPort1, HIGH );
delayMicroseconds( 10 );
digitalWrite( triggerPort1, LOW );
t1 = pulseIn( echoPort1, HIGH ); //rilevo i microsecondi (anche qui dovremo dividere per 1.000.000 per passare da microsecondi a secondi nel risultato finale)
digitalWrite( echoPort1, LOW );
//Serial.print("t1: "); 
//Serial.println(t1); 

delay(150); //questo delay è necessario per assestare Arduino, prima di dare potenza all'altro sensore (non scendere al di sotto di 150)

//porta bassa l'uscita del trigger2
digitalWrite( triggerPort2, LOW );
//invia un impulso di 10microsec su trigger1
digitalWrite( triggerPort2, HIGH );
delayMicroseconds( 10 );
digitalWrite( triggerPort2, LOW );
t2 = pulseIn( echoPort2, HIGH ); //idem, dovrò poi ricordanrmi di andare a dividere per 1.000.000 per passare da microsecondi a secondi nel risultato finale
digitalWrite( echoPort2, LOW );
//Serial.print("t2: ");
//Serial.println(t2); 


D1T2 = d1*t2; //questo valore è in micrometri*microsecondi
//Serial.print("D1T2: "); 
//Serial.println(D1T2); 

D2T1 = d2*t1; //e così anche questo
//Serial.print("D2T1: "); 
//Serial.println(D2T1);

//calcolo grandezze
VF = coeffVel*(abs(D1T2-D2T1))/(t1*t2);
//questo valore è in micrometri/microsecondi.
//è arrivato il momento di dividere micrometri e microsecondi per 1.000.000. 
//Numericamente il risultato non cambia, perché equivale a moltiplicarlo per 1.000.000/1.000.000, ma l'unità di misura diventa finalmente m/s.
//Convertendo alla fine anziché all'inzizio abbiamo conservato il maggior numero possibile di cifre significative.

flusso = (VF*superficie)*60*1000; //in m^3/s a L/min
spinta = (superficie*VF*VF*ro); //N (or kg*m/s^s)

//velocità
Serial.println("");
Serial.print("velocità: ");
Serial.print(VF, 6); //tengo 6 cifre significative
Serial.println(" m/s");

//flusso
Serial.print("flusso: ");
Serial.print(flusso, 6); //tengo 6 cifre significative
Serial.print(" L/min o ");
Serial.print(flusso/60000, 12);
Serial.println(" ml/s");

//spinta (not really required for ventilation)
Serial.print("spinta: ");
Serial.print(spinta);
Serial.println(" N");


//Aspetta non scendere al di sotto di 150 microsecondi
delay(2000);
}
