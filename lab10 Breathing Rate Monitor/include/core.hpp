#include <Arduino.h>

namespace core {

const int TSAMP_MSEC = 100;
const int NUM_SAMPLES = 900;  // 3600;
const int NUM_SUBSAMPLES = 160;
const int DAC0 = 3, DAC1 = 4, DAC2 = 5, LM61 = A0, VDITH = A1;
const int V_REF = 5.0;
const int SPKR = 12;  // d12  PB4

volatile boolean sampleFlag = false;

const long DATA_FXPT = 1000;             // Scale value to convert from float to fixed
const float INV_FXPT = 1.0 / DATA_FXPT;  // division slow: precalculate

int nSmpl = 1, sample;

float xv, yv, yLF, yMF, yHF, stdLF, stdMF, stdHF;
float printArray[9];
int numValues = 0;

int loopTick = 0;
bool statsReset;
bool isToneEn = false;

unsigned long startUsec, endUsec, execUsec;

//  Define a structure to hold statistics values for each filter band
struct stats_t {
    int tick = 1;
    float mean, var, stdev;
} statsLF, statsMF, statsHF;

//******************************************************************
int AlarmCheck(float stdLF, float stdMF, float stdHF) {
    if (stdLF < 0.05 && stdMF < 0.05 && stdHF < 0.05)
        return 0;

    if (stdHF > 0.30 && stdHF > stdMF && stdHF > stdLF)
        return 3;

    if (stdLF > 0.30 && stdLF > stdMF && stdLF > stdHF)
        return 1;

    return 2;
}  // end AlarmCheck

//*******************************************************************
void getStats(float xv, stats_t& s, bool reset) {
    float oldMean, oldVar;

    if (reset == true) {
        s.stdev = sqrt(s.var / s.tick);
        s.tick = 1;
        s.mean = xv;
        s.var = 0.0;
    } else {
        oldMean = s.mean;
        s.mean = oldMean + (xv - oldMean) / (s.tick + 1);
        oldVar = s.var;
        s.var = oldVar + (xv - oldMean) * (xv - s.mean);
    }
    s.tick++;
}

//*******************************************************************
float analogReadDitherAve(void) {

    float sum = 0.0;
    for (int i = 0; i < NUM_SUBSAMPLES; i++) {
        digitalWrite(DAC0, (i & B00000001));  // LSB bit mask
        digitalWrite(DAC1, (i & B00000010));
        digitalWrite(DAC2, (i & B00000100));  // MSB bit mask
        sum += analogRead(LM61);
    }
    return sum / NUM_SUBSAMPLES;  // averaged subsamples
}

//*********************************************************************
void setAlarm(int aCode, boolean isToneEn) {

    if (!isToneEn) { noTone(SPKR); return; }

    static unsigned long lastToggle = 0;
    static bool highOn = false;

    switch (aCode)
    {
        case 0:
            tone(SPKR, 200);
            break;

        case 1:
            tone(SPKR, 400);
            break;

        case 2:
            noTone(SPKR);
            highOn = false;
            break;

        case 3:
        {
            unsigned long now = millis();
            if (now - lastToggle >= 1000)
            {
                lastToggle = now;
                highOn = !highOn;
                if (highOn) tone(SPKR, 1000);
                else noTone(SPKR);
            }
            break;
        }

        default:
            noTone(SPKR);
            break;

}  // setBreathRateAlarm()

//*************************************************************
float testVector(void) {
    // Variable rate sinusoidal input
    // Specify segment frequencies in bpm.
    // Test each frequency for nominally 60 seconds.
    // Adjust segment intervals for nearest integer cycle count.

    const int NUM_BAND = 6;
    const float CAL_FBPM = 10.0, CAL_AMP = 2.0;

    const float FBPM[NUM_BAND] = {5.0, 10.0, 15.0, 20.0, 30.0, 70.0};  // LPF test
    static float bandAmp[NUM_BAND] = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0};

    //  Determine the number of samples (around 600 ) that will give you an even
    //  number of full cycles of the sinewave.  This is done to avoid a large
    //  discontinuity between bands.  This forces the sinewave in each band to end
    //  near a value of zer

    static int bandTick = int(int(FBPM[0] + 0.5) * (600 / FBPM[0]));
    static int simTick = 0, band = 0;
    static float Fc = FBPM[0] / 600, cycleAmp = bandAmp[0];

    // for (int i = 0; i < NUM_BAND; i++) bandAmp[i] = CAL_AMP*(CAL_FBPM/FBPM[i]);

    //  Check to see if the simulation tick has exceeded the number of tick in
    //  each band. If it has then switch to the next frequency (band) again
    //  computing how many ticks to go through to end up at the end of a cycle.

    if ((simTick >= bandTick) && (FBPM[band] > 0.0)) {

        //  The simTick got to the end of the band cycle.  Go to the next frequency
        simTick = 0;
        band++;
        Fc = FBPM[band] / 600.0;
        cycleAmp = bandAmp[band];
        bandTick = int(int(FBPM[band] + 0.5) * (600 / FBPM[band]));
    }

    float degC = 0.0;  // DC offset
    degC += cycleAmp * sin(TWO_PI * Fc * simTick++);
    // degC += 1.0*(tick/100.0); // drift: degC / 10sec
    // degC += 0.1*((random(0,101)-50.0)/29.0); // stdev scaled from 1.0
    return degC;
}

//*******************************************************************
void configureArduino(void) {
    pinMode(DAC0, OUTPUT);
    digitalWrite(DAC0, LOW);
    pinMode(DAC1, OUTPUT);
    digitalWrite(DAC1, LOW);
    pinMode(DAC2, OUTPUT);
    digitalWrite(DAC2, LOW);

    pinMode(SPKR, OUTPUT);
    digitalWrite(SPKR, LOW);

    analogReference(INTERNAL);  // DEFAULT, INTERNAL
    analogRead(LM61);           // read and discard to prime ADC registers
    Serial.begin(115200);       // 11 char/msec
}

//**********************************************************************
void WriteToSerial(int numValues, float dataArray[]) {

    int index = 0;
    for (index = 0; index < numValues; index++) {
        if (index > 0) {
            Serial.print('\t');
        }
        Serial.print(dataArray[index], DEC);
    }

    Serial.print('\n');
    delay(20);

}  // end WriteToMATLAB

////**********************************************************************
float ReadFromMATLAB() {
    int charCount;
    bool readComplete = false;
    char inputString[80], inChar;

    // Wait for the serial port

    readComplete = false;
    charCount = 0;
    while (!readComplete) {
        while (Serial.available() <= 0)
            ;
        inChar = Serial.read();

        if (inChar == '\n') {
            readComplete = true;
        } else {
            inputString[charCount++] = inChar;
        }
    }
    inputString[charCount] = 0;
    return atof(inputString);

}  // end ReadFromMATLAB

//*******************************************************************
void syncSample(void) {
    while (sampleFlag == false)
        ;                // spin until ISR trigger
    sampleFlag = false;  // disarm flag: enforce dwell
}

//**********************************************************************
void ISR_Sample() { sampleFlag = true; }

}  // namespace core
