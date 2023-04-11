# include <IRremote.h>
IRrecv IR(2);

void setup() {
  Serial.begin(9600);
  IR.enableIRIn();
}

void loop() {
  if(IR.decode()){
    Serial.println(IR.decodedIRData.decodedRawData, HEX);
    delay(1500);
    IR.resume();
  }
}
