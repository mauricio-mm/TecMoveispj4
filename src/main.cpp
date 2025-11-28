#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#define DHTPIN 4
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);


int batimentos = 75;  // Valor inicial de batimentos cardíacos
int saturacao = 98;   // Saturação de oxigênio (SpO2)

// Variáveis de BLE
BLECharacteristic *pCharacteristicBatimentos;
BLECharacteristic *pCharacteristicSpO2;

BLEServer *pServer;
BLEService *pServiceBatimentos;
BLEService *pServiceSpO2;

void setup() {
  Serial.begin(9600);

  dht.begin();
  pinMode(DHTPIN, INPUT);

  Serial.println("Starting BLE work!");

  // Inicializa o dispositivo BLE
  BLEDevice::init("TecMoveis");

  // Cria o servidor BLE
  pServer = BLEDevice::createServer();

  // Cria o serviço de Batimentos Cardíacos (Heart Rate - UUID 0x180D)
  // Neste caso está sendo usado UUID padrão de 16bits e não o de 128 bits.
  // tanto para batimentos como SpO2.
  pServiceBatimentos = pServer->createService(BLEUUID((uint16_t)0x180D));
  pCharacteristicBatimentos = pServiceBatimentos->createCharacteristic(
                                  BLEUUID((uint16_t)0x2A37),  // Heart Rate Measurement
                                  BLECharacteristic::PROPERTY_READ |
                                  BLECharacteristic::PROPERTY_NOTIFY
                              );

  // Cria o serviço de Saturação de Oxigênio (SpO2)
  pServiceSpO2 = pServer->createService(BLEUUID((uint16_t)0x1810)); // UUID customizado
  pCharacteristicSpO2 = pServiceSpO2->createCharacteristic(
                                  BLEUUID((uint16_t)0x2A5F),  // Saturation (SpO2)
                                  BLECharacteristic::PROPERTY_READ |
                                  BLECharacteristic::PROPERTY_NOTIFY
                              );

  // Inicia os serviços
  pServiceBatimentos->start();
  pServiceSpO2->start();

  // Inicia a publicidade BLE
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(BLEUUID((uint16_t)0x180D)); // Heart Rate Service
  pAdvertising->addServiceUUID(BLEUUID((uint16_t)0x1810)); // SpO2 Service

  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // Ajusta para melhorar a conectividade com dispositivos
  BLEDevice::startAdvertising();
  Serial.println("BLE services and characteristics configuradas!");

}

void loop() {      

  Serial.println(
    String(dht.readTemperature()) + ':' +
    String(dht.readHumidity())
  );

  delay(10);

  
  // Simula a alteração dos dados
  batimentos = (batimentos + 1) % 120;  // Batimentos entre 1 e 119
  saturacao = 90 + (rand() % 11);       // Saturação de 90 a 100%

  // Atualiza a característica de batimentos cardíacos
  uint8_t heartRateValue = batimentos;
  uint8_t heartRateData[4] = {0x06, heartRateValue}; // Flags e valor de batimento
  pCharacteristicBatimentos->setValue(heartRateData, sizeof(heartRateData));
  pCharacteristicBatimentos->notify();

  // Atualiza a característica de Saturação (SpO2)
  uint8_t SpO2Data[2] = {0x06, saturacao}; // Flags e valor de SpO2
  pCharacteristicSpO2->setValue(SpO2Data, sizeof(SpO2Data));
  pCharacteristicSpO2->notify();

  // Exibe os dados no serial monitor
  Serial.print("Heart Rate: ");
  Serial.println(batimentos);
  Serial.print("SpO2: ");
  Serial.println(saturacao);

  delay(2000);  // Espera 2 segundos antes de enviar o próximo valor

}




