#include <SoftwareSerial.h>
SoftwareSerial BTSerial(10, 11); //Rx, Tx
#include <Wire.h> 
#include "RTClib.h"
#include "DHT.h"
#include <EEPROM.h>
int i;  
char estadoB = 'g';         // inicia detenido

int hs;
int seg_min=EEPROM.read(14);  // 0=seg ... 1=min

 
RTC_DS1307 RTC;                   // inicializa el modulo RTC

DHT dht(10, DHT11);
int estado = 0;                  // estado del tibre off=0 , on=1
const int timbre = 2;             // Pin 7 encargado de activar el timbre, se conecta al Rele
const int riego_on=6;
const int luz=8;
const int piloto = 13;
int segundo;
int minuto;
int hora;
int tiempo_timbre=EEPROM.read(1);  // Tiempo continuo que dura el timbre sonando en minutos EEPROM
int hora_timbre=EEPROM.read(0);  // Hora a la que suena el timbre  EEPROM
int min_timbre=EEPROM.read(9);
int lun=EEPROM.read(2);
int mar=EEPROM.read(3);
int mie=EEPROM.read(4);
int jue=EEPROM.read(5);
int vie=EEPROM.read(6);
int sab=EEPROM.read(7);
int dom=EEPROM.read(8);
int seg_timbre_f=EEPROM.read(10);
int min_timbre_f=EEPROM.read(11);
int hora_timbre_f=EEPROM.read(12);
int limite_hs =EEPROM.read(13);       //***** valor limite de la humedad del suelo

int r_dia;
String stat;

// Hora en la que suena el timbre escrito  h1=Hora, m1=Minutos, s1=Segundos
int h1_c1=hora_timbre;     int m1_c1=min_timbre;     int s1_c1=0;
int h1_c1_f=hora_timbre_f;   int m1_c1_f=min_timbre_f; int s1_c1_f=seg_timbre_f;

////////////////////////////////// Void Setup() ///////////
void setup () { 
 pinMode(timbre, OUTPUT);                    // Configura como salida el pin 12
 pinMode(piloto, OUTPUT); 
 Wire.begin();                               
 RTC.begin();                                // Inicia la comunicaci¢n con el RTC
 dht.begin();
// RTC.adjust(DateTime(__DATE__, __TIME__)); // Lee la fecha y hora del PC (Solo en la primera carga)
                                             // el anterior se usa solo en la configuracion inicial luego se pone como comentario
                                             // y se vuelve a cargar el programa sin esa linea.
 Serial.begin(9600);                         // Establece la velocidad de datos del puerto serie a 9600
 BTSerial.begin(9600);
 HS_prueba();

 
} 
////////////////////////////////// Void loop() ///////////////////////
void loop(){

 lecturaBlu();    
 DateTime now = RTC.now();          // Obtiene la fecha y hora del RT
 segundo=now.second();
 minuto=now.minute();
 hora=now.hour();

horario_1();
 
}   //////////////////////////////// Fin del void loop(), inician las Funciones/////////////////////////


 ///////////****************** Accion 01 ************************ 


 //********************************Lectura del modulo Blutooth ************************ 
 void lecturaBlu(){
  if(BTSerial.available()>0){        // lee el bluetooth y almacena en estado
    estadoB = BTSerial.read();
    Serial.print(estadoB);

  }
   if(estadoB=='1'){          // ENCENDER
     on_();
     estadoB = 'g';
  }
  if(estadoB=='0'){         // APAGAR
     off_();
    estadoB = 'g';
  }
  
  if(estadoB=='u'){           //  Tiempo de duracion
     set_tiempo_timbre();
     EEPROM.write(1,tiempo_timbre);
     estadoB = 'g';
  }    
  if(estadoB=='v'){   //  duracion minutos o segundos
    delay(30);
    while (BTSerial.available()){
    char ms = BTSerial.read();
    if(ms=='s')seg_min=0;
    if(ms=='m')seg_min=1;
    EEPROM.write(14,seg_min);
    
    estadoB = 'g';
    }
  }

  
  if(estadoB=='h'){         // Hora de Encendido
    set_hora_timbre();
    h1_c1 = hora_timbre; 
    EEPROM.write(0,hora_timbre); 
    estadoB = 'g';
  }
  if(estadoB=='n'){         // Minuto de Encendido
    set_min_timbre();
    m1_c1 = min_timbre; 
    EEPROM.write(9,min_timbre); 
    estadoB = 'g';
  }



if(estadoB=='A'){     //DIAS DE SEMANA
    lun=1; 
    EEPROM.write(2,lun);
    estadoB = 'g';
}
if(estadoB=='B'){     
    lun=0; 
    EEPROM.write(2,lun);
    estadoB = 'g';
}
if(estadoB=='C'){     
    mar=1; 
    EEPROM.write(3,mar);
    estadoB = 'g';
}
if(estadoB=='D'){     
    mar=0; 
    EEPROM.write(3,mar);
    estadoB = 'g';
}
if(estadoB=='E'){     
    mie=1; 
    EEPROM.write(4,mie);
    estadoB = 'g';
}
if(estadoB=='F'){     
    mie=0; 
    EEPROM.write(4,mie);
    estadoB = 'g';
}
if(estadoB=='G'){     
    jue=1; 
    EEPROM.write(5,jue);
    estadoB = 'g';
}
if(estadoB=='I'){     
    jue=0; 
    EEPROM.write(5,jue);
    estadoB = 'g';
}
if(estadoB=='J'){     
    vie=1; 
    EEPROM.write(6,vie);
    estadoB = 'g';
}
if(estadoB=='K'){     
    vie=0; 
    EEPROM.write(6,vie);
    estadoB = 'g';
}
if(estadoB=='M'){     
    sab=1; 
    EEPROM.write(7,sab);
    estadoB = 'g';
}
if(estadoB=='N'){     
    sab=0; 
    EEPROM.write(7,sab);
    estadoB = 'g';
}
if(estadoB=='O'){     
    dom=1; 
    EEPROM.write(8,dom);
    estadoB = 'g';
}
if(estadoB=='P'){     
    dom=0; 
    EEPROM.write(8,dom);
    estadoB = 'g';
}
                      //Acabaron dias de semana

  if(estadoB=='L'){         // Boton Datos
      
      delay(100);
      hs=analogRead(A0);        // Obtiene la Humedad del Suelo A0
      hs = map(hs, 0 , 1023 , 100 ,  0);
      if(hora_timbre<10) BTSerial.print(" ");
      BTSerial.print(hora_timbre);
      BTSerial.print(":");
      if(min_timbre<10) BTSerial.print("0");
      BTSerial.print(min_timbre);
      if(tiempo_timbre<10) BTSerial.print(" ");
      BTSerial.print(tiempo_timbre);
      if(seg_min==0)BTSerial.print("seg");
      else BTSerial.print("min");
      if(hs<100) BTSerial.print(" ");
      if(hs<10) BTSerial.print(" ");
      BTSerial.print(hs);
      BTSerial.print("%");
      if(lun==1)BTSerial.print("L");
      else BTSerial.print("N");
      if(mar==1)BTSerial.print("M");
      else BTSerial.print("N");
      if(mie==1)BTSerial.print("C");
      else BTSerial.print("N");
      if(jue==1)BTSerial.print("J");
      else BTSerial.print("N");
      if(vie==1)BTSerial.print("V");
      else BTSerial.print("N");
      if(sab==1)BTSerial.print("S");
      else BTSerial.print("N");
      if(dom==1)BTSerial.print("D");
      else BTSerial.print("N"); 
      if(estado==1)BTSerial.print("*"); 
      if(estado==0)BTSerial.print("-");
      BTSerial.print(hora); // Horas
      BTSerial.print(':');
      BTSerial.print(minuto); // Minutos
      BTSerial.print(':');
      BTSerial.print(segundo); // Segundos
      estadoB = 'g';
  }

if(estadoB=='H'){
    set_lim_humedad();
    EEPROM.write(13,limite_hs);
    estadoB = 'g';
  }
 
  if(estadoB=='Z'){
    ajuste_hora_final();
    estadoB = 'g';
  }

if(estadoB=='Q'){
    digitalWrite(luz,HIGH);
    estadoB = 'g';
  }
  if(estadoB=='R'){
    digitalWrite(luz,LOW);
    estadoB = 'g';
  }

}


 //********************************************** Fin Control Blu ************************ 
 

/////////////////////////////// Funcion que activa el Timbre //////////////////////////////////

/////////////////////////////// Calcula el dia de la Semana //////////////////////////////////
int dia_de_semana(){
 // Encuentar el dia de la semana de una fecha
 int n_dia;
 int n_mes;
 int t_mes;
 int n_anno;
 int d_anno;
 int t_siglo=6;

 DateTime now = RTC.now(); //fecha y hora del RTC


 n_anno=(now.year()-2000);
 d_anno=n_anno/4;
 n_dia=now.day();
 n_mes=now.month();

 switch (n_mes) {
    case 1:
      if(now.year()%4==0){
      t_mes=6;}
      else{
      t_mes=0;}    
      break;
    case 2:
      if(now.year()%4==0){
      t_mes=2;}
      else{
      t_mes=3;}
      break;
    case 3:
      t_mes=3;
      break;
    case 4:
      t_mes=6;
      break;
    case 5:
      t_mes=1;
      break;
    case 6:
      t_mes=4;
      break;
    case 7:
      t_mes=6;
      break;
    case 8:
      t_mes=2;
      break;
    case 9:
      t_mes=5;
      break;
    case 10:
      t_mes=0;
      break;
    case 11:
      t_mes=3;
      break;
    case 12:
      t_mes=5;
      break;
    default: 
      t_mes=t_mes;
    break;
 }

 r_dia=n_dia+t_mes+n_anno+d_anno+t_siglo;
 r_dia = r_dia % 7;
 return r_dia; 
}

///////////////////// on_  //////////////////////////////////
void on_(){ 
  estado=1;
  //analogWrite(timbre,110);
  digitalWrite(timbre,HIGH);
  digitalWrite(riego_on,HIGH);
}

///////////////////// off_  //////////////////////////////////
void off_(){ 
  estado=0;
  digitalWrite(timbre,LOW);
  digitalWrite(riego_on,LOW);
}

/////////////////////Condicional del Horario 1  //////////////////////////////////
void horario_1(){ 
 dia_de_semana();
 if ((hora==h1_c1) && (minuto==m1_c1) && (segundo==s1_c1)) {
      digitalWrite(piloto,HIGH);
      delay(100);
      hs=analogRead(A0);        // Obtiene la Humedad del Suelo A0
      hs = map(hs, 0 , 1023 , 100 ,  0);
      delay(10);
      digitalWrite(piloto,LOW);
      if(hs <= limite_hs){
        if(lun==1 && r_dia==1) on_();
        if(mar==1 && r_dia==2) on_();
        if(mie==1 && r_dia==3) on_();
        if(jue==1 && r_dia==4) on_();
        if(vie==1 && r_dia==5) on_();
        if(sab==1 && r_dia==6) on_();
        if(dom==1 && r_dia==0) on_();
      }   
 }
 
if ((hora==h1_c1_f) && (minuto==m1_c1_f) && (segundo==s1_c1_f)) { 
     if(lun==1 && r_dia==1) off_();
     if(mar==1 && r_dia==2) off_();
     if(mie==1 && r_dia==3) off_();
     if(jue==1 && r_dia==4) off_();
     if(vie==1 && r_dia==5) off_();
     if(sab==1 && r_dia==6) off_();
     if(dom==1 && r_dia==0) off_();
}  
if ((hora==h1_c1) && (minuto>=m1_c1)&&(minuto<=m1_c1_f)&&(segundo==1)&&(estado==1)) { // prueba de humedad del suelo cada minuto 
                                                                          // durante la activacion programada
        if(lun==1 && r_dia==1) HS_prueba();
        if(mar==1 && r_dia==2) HS_prueba();
        if(mie==1 && r_dia==3) HS_prueba();
        if(jue==1 && r_dia==4) HS_prueba();
        if(vie==1 && r_dia==5) HS_prueba();
        if(sab==1 && r_dia==6) HS_prueba();
        if(dom==1 && r_dia==0) HS_prueba();
      }
}
/////////////////////Prueba de Humedad de Suelo y apaga el sistema///////////
void HS_prueba(){ 
      hs=analogRead(A0);        // Obtiene la Humedad del Suelo A0
      hs = map(hs, 0 , 1023 , 100 ,  0);
      delay(10);
      digitalWrite(piloto,LOW);
      if (hs>=limite_hs){
          off_();
      }  
}
/////////////////////Intro_0 //////////////////////////////


////////////////////ajuste hora final timbre  //////////////////////////////////
void ajuste_hora_final(){ 
  if(seg_min==0){ 
    s1_c1_f = s1_c1+tiempo_timbre;
    m1_c1_f=m1_c1;
    seg_timbre_f=s1_c1_f;
    min_timbre_f=m1_c1_f;
  }
  else{
    s1_c1_f = s1_c1;
    m1_c1_f=m1_c1+tiempo_timbre;
    seg_timbre_f=s1_c1_f;
    min_timbre_f=m1_c1_f;
  }

  if(m1_c1_f>=60){
    h1_c1_f=h1_c1+1;
    hora_timbre_f=h1_c1_f;
    m1_c1_f=m1_c1_f-60;
  }
  else{
    h1_c1_f=h1_c1;
    hora_timbre_f=h1_c1_f;
  }
EEPROM.write(10,seg_timbre_f);
EEPROM.write(11,min_timbre_f);
EEPROM.write(12,hora_timbre_f);
}

void set_tiempo_timbre(){
delay(30);
  while (BTSerial.available()){
    char c = BTSerial.read();
    Serial.print(c);
    stat += c;
    
  }
  if(stat.length() > 0){
    tiempo_timbre=stat.toInt();
    stat = "";
  }
}

void set_hora_timbre(){
delay(30);
  while (BTSerial.available()){
    char c = BTSerial.read();
    Serial.print(c);
    stat += c;
    
  }
  if(stat.length() > 0){
    hora_timbre=stat.toInt();
    stat = "";
  }
}

void set_min_timbre(){
delay(30);
  while (BTSerial.available()){
    char c = BTSerial.read();
    Serial.print(c);
    stat += c;
    
  }
  if(stat.length() > 0){
    min_timbre=stat.toInt();
    stat = "";
  }
}


void set_lim_humedad(){
delay(30);
  while (BTSerial.available()){
    char c = BTSerial.read();
    Serial.print(c);
    stat += c;
    
  }
  if(stat.length() > 0){
    limite_hs=stat.toInt();
    stat = "";
  }
}
