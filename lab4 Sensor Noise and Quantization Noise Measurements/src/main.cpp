// OPEN NEW ARDUINO SKETCH.
// CLICK IN THIS TEXT BOX. CTRL-A, CTRL-C.
// CLICK IN SKETCH. CTRL-A, CTRL-V.

// Lab2_handout.ino
// created by: Clark Hochgraf 2015
// modified by: David Orlicki, October 1, 2017
// modified by: David Orlicki, September 7, 2019
// modified by: Mark Thompson, September 8, 2020

// Outputs 3 bit DAC resistor network drive.
// Reads/displays ADC samples of LM61 sensor output.
// Reads/displays averaged subsamples of LM61 sensor output.
// Reads/displays dithered-averaged subsamples of LM61 sensor output.
// Calculates running mean and standard deviation of samples.

#include <manager.hpp>

const int TSAMP_MSEC = 100; // senor: 100, sim: 10
const int LM61 = A0, VDITH = A1;
const int NUM_SAMPLES = 256;

extern int tick;
extern float sample, mean, stdev;

//*******************************************************************
void setup() {
  configureArduino();
  
  // This line tells MATLAB that the Arduino is ready to accept data
  Serial.println("%Arduino Ready");
  
  // Serial.println("Enter 'g' to go .....");

  // Wait until MATLAB sends a 'g' to start sending data
  while (Serial.read() != 'g'); // spin until 'g' entry  
  
  MsTimer2::set(TSAMP_MSEC, ISR_Sample); // Set sample msec, ISR name
  MsTimer2::start(); // start running the Timer   
} // setup()

//*******************************************************************
void loop()
{
  syncSample(); 
  
//  sample = analogRead(LM61);
  // sample = analogReadAve();
  // sample = testDAC(tick);
  // sample = testDAC(random(256));
  sample = analogReadDitherAve(tick);
  // sample = 185-0*tick/32.0+2*sin((tick/32.0)*TWO_PI);

  // recurseStats1(sample, mean, stdev);
  // recurseStats2(sample, mean, stdev);
  
  displayData();
  if (++tick >= NUM_SAMPLES) while(true); // spin forever
} // loop()