# Breathing Rate Monitor

## Description

- Build a breathing rate monitor to warn of a potential acute respiratory problem (pneumonia) in a child age 11 months to 5 years.

- In children, a breathing rate of greater than 50 breaths per minute can indicate pneumonia.

- Fewer than 12 breaths per minute can also indicate an abnormal condition.

## Requirements

- Must be done entirely in Arduino.

- Monitor shall detect if the breathing rate is greater than 40 breaths per minute.

- Monitor shall detect a breathing rate below 12 breaths per minute.

  - May indicate that the sensor is disconnected, or other abnormal condition is occurring.

- Either condition shall be detected and alert the caregiver within 2 minutes of its occurrence.

- A warning shall be sounded for a breathing rate greater than 40 breaths per minute.

- A warning shall be sounded for a breathing rate less than 12 breaths per minute.

- TThe warning for low breathing rate shall be different from that of the warning for high breathing rate

## Design Process

- Using filtering to remove noise

- using filtering to separate the frequnecy content of signals.

- Computing statistics efficiently in computing systems.

- What number systems should I use and why?

  - Integers
  - Longs
  - Floating point

- What types of filters should I use and where?

  - FIR
  - Recursive filters
  - LPF, HPF, BPF, BSF

## Constraints

- Program memory limits

- Data memory limits

- Processor execution times

- What drives these factors?

- What are my tradeoffs
  - Execution speed / Data precision
  - Memory usage / Data precision
